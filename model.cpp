
class THeatFcn {

    double timeWin[2], timeStep;

    public:
        THeatFcn(double FitRange0, double FitRange1) {
            timeWin[0]=FitRange0;
            timeWin[1]=FitRange1;
            timeStep = (timeWin[1]-timeWin[0])/1000.; //1000 points
        }

        double Evaluate(double *x, double *p){
            double timeOffset = p[0];
            double kin        = p[1];
            double Qloss      = p[2];
            double MeanT      = p[3];

            double time = x[0]-timeOffset;

            double Temperature = 0;
            double tempHeat=0, tempLoss=0;
            for(int it=timeWin[0]; (it<time && it<timeWin[1]); it+=timeStep){
                tempHeat = kin*timeStep;
                tempLoss = Qloss*( exp(Temperature/MeanT) - 1 ) * timeStep;
                Temperature += tempHeat - tempLoss;

                //cout<< "it="<< it << " time=" << time <<endl; 
                //cout<<"Heat = "<< tempHeat <<" tempLoss = " << tempLoss << " Temp=" << Temperature <<endl<<endl;; 
                //if(it>5*timeStep) exit(0);

            }
            return Temperature;
        }

        double GetFitRangeLow(){return timeWin[0];}
        double GetFitRangeHigh(){return timeWin[1];}

};


class TData {

    public:

        TGraph       *grAll;
        TGraphErrors *chartHeatingReb;
        TGraphErrors *chartCoolingReb;
        TGraphErrors *chartWin;

        TString fileName;

        TData(TString infilename, double TambH, double TambC, double heatEndTime, double coolStarTime){
            //======== SetUp =============
            double TemperError = 1.0;          // Estimate, needs more attention
            double WinError    = 1.0;
            int rebin[2] = {30, 100};          // rebinning 100 for cooling 30 for heating

            fileName = infilename;
            ifstream myfile(fileName);
            if(myfile.fail()) {
                cout << "File " << fileName << " does not exists" << endl;
                exit(0);
            }

            char dummyText[1024];
            myfile.getline(dummyText, 1024);

            double pwr, tvd02, tvd06, tvd03, pt1000, clock;

            grAll           = new TGraph();
            chartHeatingReb = new TGraphErrors();
            chartCoolingReb = new TGraphErrors();
            chartWin = new TGraphErrors();

            int count = 0;
            int countLines = 0;
            double sumTemp = 0, sumTime = 0, sumPWR=0, averTemp = 0, averTime = 0, averPWR = 0;

            while(!myfile.eof() && count <10000) { //cutoff Carefull !!!!!!!!!!!!!!!!

                myfile >> pwr >> tvd02 >> tvd06 >> tvd03 >> pt1000 >> clock;
                //myfile >> pwr >> tvd02 >> tvd06 >> pt1000 >> clock;
                pwr *= 1./800; // kW/800

                grAll->AddPoint(clock, tvd02); //for bounder check;

                if(clock <= heatEndTime) {
                    // Rebining
                    if(countLines<rebin[0]) {
                        sumTemp += tvd02; // - TambH;
                        sumTime += clock; //*(tvd02 - TambH);
                        sumPWR  += pwr;
                        countLines++;
                    } else {
                        averTemp = sumTemp/rebin[0];
                        averTime = sumTime/rebin[0];
                        averPWR = sumPWR/rebin[0];

                        chartHeatingReb->AddPoint(averTime, averTemp - TambH);
                        chartHeatingReb->SetPointError(chartHeatingReb->GetN()-1, 0, 2*TemperError);

                        chartWin->AddPoint(averTime, averPWR);
                        chartWin->SetPointError(chartWin->GetN()-1, 0, WinError);

                        countLines = 0;
                        sumTemp = 0;
                        sumTime = 0;
                        sumPWR  = 0;
                    }
                } else if(clock > coolStarTime && count < 10000) { //cutoff Carefull !!!!!!!!!!!!!!!!
                                                                   // Rebining
                    if(countLines<rebin[1]) {
                        sumTemp += tvd02; // - tempAmbHeating;
                        sumTime += clock; //*(tvd02 - tempAmbHeating);
                        countLines++;
                    } else {
                        averTemp = sumTemp/rebin[1];
                        averTime = sumTime/rebin[1];
                        chartCoolingReb->AddPoint(averTime - coolStarTime, averTemp - TambC);
                        chartCoolingReb->SetPointError(chartCoolingReb->GetN()-1, 0, TemperError);
                        countLines = 0;
                        sumTemp = 0;
                        sumTime = 0;
                    }   
                }

                count++;
            }
            cout << "Lines: " << count << endl;
        } //end of constructor

        void DrawTemp(){
            grAll->Draw("AL");
        }

        TString GetFileName(){return fileName;}

};

class TSplitCan {
    
    public:
    
        TCanvas *can[10];
        TPad *mpad, *subMpad[2], *pad;
        TPaveText *pt;
        TAxis *ax, *ay;

        TSplitCan(int CID){

            gStyle->SetOptStat(0);
            gStyle->SetOptTitle(0);

            TString canName = Form("c%i",CID);
            can[CID] = new TCanvas(canName, canName, 10, 50, 700, 700); 
            can[CID]->Range(0,0,1,1);
            can[CID]->SetFillColor(0); can[CID]->SetBorderMode(0); can[CID]->SetBorderSize(2);
            can[CID]->SetTicky(1); can[CID]->SetFrameBorderMode(0);

            mpad = new TPad("mpad", "mpad",0.01,0.01,0.99,0.99);
            mpad->Draw();
            mpad->cd();
            mpad->Range(0,0,1,1);
            
            //pad division----------------------------------------
            double divPosY=0.3, spaceBetweenDiv=9e-4; //Y division at 30% 

            subMpad[0] = new TPad("mpad_1", "mpad_1", 0.01, divPosY-spaceBetweenDiv, 0.99, 0.99);
            subMpad[0]->SetBottomMargin(0);
            subMpad[0]->Draw();

            mpad->cd();
            subMpad[1] = new TPad("mpad_2", "mpad_2", 0.01, 0.01, 0.99, divPosY);
            subMpad[1]->SetTopMargin(0.0);
            subMpad[1]->SetBottomMargin(0.3);
            subMpad[1]->Draw();
            subMpad[1]->cd();

        }

        void SetGrid(int padID, int gx=0, int gy=0){
            subMpad[padID]->SetGridx(gx);
            subMpad[padID]->SetGridy(gy);
        }

        void SetUpperFramePars(TH1 &hid){
            ax=hid.GetXaxis();      ay=hid.GetYaxis(); 
            ax->SetTimeDisplay(1);  ax->SetTimeFormat("%H:%M"); ax->SetTimeOffset(-3600);
            ax->SetTitleSize(0.055); ax->SetTitleOffset(0.88); 
            ax->SetTitleSize(0.055); ay->SetTitleOffset(0.6); 
        }

        void SetLowerFramePars(TH1 &hid){
            ax=hid.GetXaxis();      ay=hid.GetYaxis(); 
            ax->SetTimeDisplay(1);  ax->SetTimeFormat("%H:%M"); ax->SetTimeOffset(-3600);
            ax->SetTitleSize(0.15); ax->SetTitleOffset(0.8); 
            ay->SetTitleSize(0.14); ay->SetTitleOffset(0.25);
            ax->SetLabelSize(0.11); ax->SetLabelOffset(0.001);
            ay->SetLabelSize(0.11); ay->SetLabelOffset(0.001);
            ax->SetTickSize(0.08);
        }

        void PrintName(TString name){
            pad = new TPad("pad", " ",0.3, 0.9, 0.7, 0.95);
            pad->Draw(); pad->cd();  pad->Range(0,0,1,1);
            pad->SetFillColor(33);pad->SetBorderMode(0); pad->SetBorderSize(0);
            pt = new TPaveText(0., 0, 1., 1.,"br");
            pt->AddText( name ); pt->Draw();
        }

};

TGraphErrors* DeltaGrFcn( TGraphErrors *gr, TF1 *func, double ErrorSc = 0.1){
    TGraphErrors *grDiff = new TGraphErrors();
    int NP = gr->GetN();
    for(int ip=0; ip<NP; ip++){
        grDiff->AddPoint( gr->GetPointX(ip), gr->GetPointY(ip) - func->Eval(gr->GetPointX(ip)) );
        grDiff->SetPointError(ip, 0, gr->GetErrorY(ip)*ErrorSc );
        //cout<< ip <<" "<< gr->GetPointX(ip) <<" "<< gr->GetPointY(ip) <<" "<< fHeat->Eval(gr->GetPointX(ip)) <<endl; 
    }
    return grDiff;
}


void model() {

    // TData("fileName:, T_Amb_Heating, T_Amb_Cooling, Heat_End_Time, Cooling_Start_Time);

    // Measurement 503
    //TData *data503 = new TData("measurement_503.csv", 20.69, 38.93, 1030, 970);
    //data503->DrawTemp(); return;

    // Measurement 509
    //TData *data509 = new TData("measurement_509.csv", 21.63, 32.0, 7200, 13500);
    //data509->DrawTemp(); return;


    // Measurement 513
    TData *data513 = new TData("measurement_513.csv", 20.98, 26.44, 5143, 5264);
    //data513->DrawTemp(); return;

    TGraphErrors *gr, *grDiff;
    TH2F *hfr;
    TLegend *leg;
    TSplitCan *splitCan;

    //-----------------------
    TData *data = data513;
    //-----------------------

    double Xrange[2], Yrange[2];


    //======================================
    // Heating chart =======================
    //======================================

    splitCan = new TSplitCan(2);
    splitCan->SetGrid(0,1,0);
    splitCan->SetGrid(1,1,1);
    splitCan->subMpad[0]->cd();

    gr = data->chartHeatingReb;
    Xrange[0] = 0*gr->GetXaxis()->GetXmin();
    Xrange[1] = gr->GetXaxis()->GetXmax();
    Yrange[0] = gr->GetYaxis()->GetXmin();
    Yrange[1] = gr->GetYaxis()->GetXmax();

    hfr = new TH2F("hfr"," ", 10, Xrange[0], Xrange[1], 10, Yrange[0], Yrange[1]); 
    hset( *hfr, "Time [h:m]","Temp [deg]");
    splitCan->SetUpperFramePars(*hfr);
    hfr->Draw();

    gr->SetMarkerStyle(33); 
    gr->SetMarkerColor(4); 
    gr->SetMarkerSize(1.2); 
    gr->Draw("PZ");


    THeatFcn *ptrHeatFcn = new THeatFcn(150, 7000);

    TF1 *fHeat = new TF1("fheat", ptrHeatFcn, &THeatFcn::Evaluate, 
            ptrHeatFcn->GetFitRangeLow(), ptrHeatFcn->GetFitRangeHigh(), 4);

    double timeOfst = 0;      //0 time offset
    double kin      = 1.729e-2;   //1 kin (heating = kin*Pin]
    double Qloss    = 4.958e3;    //2 Qloss
    double MeanT    = 2.225e7;    //3 Spadova teplota

    fHeat->SetParameters(timeOfst, kin, Qloss, MeanT);
    fHeat->SetParNames("timeOfst", "kin", "Qloss", "MeanT");

    fHeat->FixParameter(0, timeOfst);
    //fHeat->FixParameter(1, kin);
    //fHeat->FixParameter(2, Qloss);
    //fHeat->FixParameter(3, MeanT);
    //fHeat->Draw("same"); return;

    //---- fit ------------------
    gr->Fit("fheat"," ","R", ptrHeatFcn->GetFitRangeLow(), ptrHeatFcn->GetFitRangeHigh());
    //---------------------------

    timeOfst   = fHeat->GetParameter(0);
    kin        = fHeat->GetParameter(1);
    Qloss      = fHeat->GetParameter(2);
    MeanT      = fHeat->GetParameter(3);
    double Tmax = MeanT*log(kin/Qloss+1);


    Qloss = fHeat->GetParameter(2);
    fHeat->SetParameter(2,0); // Pin
    fHeat->SetLineStyle(2); fHeat->DrawCopy("same");
    cout<<"Temp at 2h should be ="<<fHeat->Eval(2*3600) <<" deg"<<endl; 
    fHeat->SetParameter(2,Qloss); // Pin

    leg = new TLegend(0.6,0.2,0.9,0.5," ","brNDC");
    leg->SetFillStyle(0);leg->SetBorderSize(0);leg->SetTextSize(0.04);
    leg->AddEntry(gr, "measurement","p");
    auto *faux = new TF1("faux", "x", 0,1);
    leg->AddEntry(faux, "model heating","l");
    leg->AddEntry(fHeat, "no loss heating","l");
    leg->AddEntry((TObject*)0, Form("Tmax = %2.1f #circC",Tmax),"");
    leg->Draw(); 

    splitCan->PrintName(data->GetFileName());

    //======================================
    //Difference ===========================
    //======================================
    splitCan->subMpad[1]->cd();

    grDiff = DeltaGrFcn(gr, fHeat, 0.1);

    hfr = new TH2F("hfr"," ", 10, Xrange[0], Xrange[1], 10, 2.0*grDiff->GetYaxis()->GetXmin(), 2.0*grDiff->GetYaxis()->GetXmax() ); 
    hset( *hfr, "Time [h:m]","#DeltaTemp [deg]");
    splitCan->SetLowerFramePars(*hfr);
    hfr->Draw();

    grDiff->SetMarkerStyle(33); 
    grDiff->SetMarkerColor(4); 
    grDiff->SetMarkerSize(1.2); 
    grDiff->Draw("PZ");


    //======================================
    // Cooling chart =======================
    //======================================
    splitCan = new TSplitCan(3);
    splitCan->SetGrid(1,0,1);
    splitCan->subMpad[0]->cd();

    gr = data->chartCoolingReb;

    Xrange[0] = 0*gr->GetXaxis()->GetXmin();
    Xrange[1] = gr->GetXaxis()->GetXmax();
    Yrange[0] = gr->GetYaxis()->GetXmin();
    Yrange[1] = gr->GetYaxis()->GetXmax();

    TF1 *fexp = new TF1("fexp","[0]*exp(-x*[1])", Xrange[0], Xrange[1]);
    fexp->SetParameters(3.85653, 6.28243e-05);
    fexp->SetLineWidth(3);
    fexp->SetLineColor(2);
    double fitRange[2] = {5e3, 3e4};

    hfr = new TH2F("hfr"," ", 10, Xrange[0], Xrange[1], 10, Yrange[0], Yrange[1]); 
    hset( *hfr, "Time [h:m]","Temp [deg]");
    splitCan->SetUpperFramePars(*hfr);
    hfr->Draw();

    //-----------------
    gr->SetMarkerStyle(33); 
    gr->SetMarkerColor(4); 
    gr->SetMarkerSize(1.2); 
    //gr->SetLineColor(2);
    gr->Fit("fexp"," ","R", fitRange[0], fitRange[1]);
    fexp->Draw("same");

    gr->Draw("PZ");

    leg;
    leg = new TLegend(0.5,0.6,0.7,0.9,"Sauna - cooling","brNDC");
    leg->SetFillStyle(0);leg->SetBorderSize(0);leg->SetTextSize(0.04);
    leg->AddEntry(gr, "measurement","p");
    leg->AddEntry(fexp, "expo. fit", "l");
    leg->AddEntry((TObject*)0, Form("T_{0}=%2.1f #circC", fexp->GetParameter(0)),"");
    leg->AddEntry((TObject*)0, Form("#LTt#GT=%4.0f min", 1/fexp->GetParameter(1)/60),"");
    leg->Draw(); 

    TLine *l;
    l = new TLine(fitRange[0], Yrange[0], fitRange[0], Yrange[1]); l->SetLineStyle(2); l->Draw(); 
    l = new TLine(fitRange[1], Yrange[0], fitRange[1], Yrange[1]); l->SetLineStyle(2); l->Draw(); 

    splitCan->PrintName(data->GetFileName());

    //======================================
    //Difference ===========================
    //======================================
    splitCan->subMpad[1]->cd();

    grDiff = new TGraphErrors();
    int NP = gr->GetN();
    for(int ip=0; ip<NP; ip++){
        grDiff->AddPoint( gr->GetPointX(ip), gr->GetPointY(ip) - fexp->Eval(gr->GetPointX(ip)) );
        grDiff->SetPointError(ip, 0, gr->GetErrorY(ip)*0.1 );
        //cout<< ip <<" "<< gr->GetPointX(ip) <<" "<< gr->GetPointY(ip) <<" "<< fHeat->Eval(gr->GetPointX(ip)) <<endl; 
    }

    hfr = new TH2F("hfr"," ", 10, Xrange[0], Xrange[1], 10, 2.0*grDiff->GetYaxis()->GetXmin(), 2.0*grDiff->GetYaxis()->GetXmax() ); 
    hset( *hfr, "Time [h:m]","#DeltaTemp [deg]");
    splitCan->SetLowerFramePars(*hfr);
    hfr->Draw();

    grDiff->SetMarkerStyle(33); 
    grDiff->SetMarkerColor(4); 
    grDiff->SetMarkerSize(1.2); 
    grDiff->Draw("PZ");

    pPrint("../../figs/heating","c2");
    pPrint("../../figs/cooling","c3");
    return;

    // Cumulative W_in =======================================================
    mc(4,1.5);
    mpad->SetLogx(0);  mpad->SetLogy(0);
    mpad->SetGridx(0); mpad->SetGridy(0);
    gPad->SetLeftMargin(0.17);

    gr = data->chartWin;

    hfr = new TH2F("hfr"," ", 
            10, gr->GetXaxis()->GetXmin(), gr->GetXaxis()->GetXmax(), 
            10, gr->GetYaxis()->GetXmin(), gr->GetYaxis()->GetXmax());

    hset( *hfr, "Time [hour:min]","P_{in} [Ws]");
    //hfr->GetXaxis()->SetTimeDisplay(1);
    //hfr->GetXaxis()->SetTimeFormat("%H:%M");
    hfr->Draw();

    //-----------------
    gr->SetMarkerStyle(33); 
    gr->SetMarkerColor(4); 
    gr->SetMarkerSize(1.2); 
    gr->SetLineColor(2);

    gr->Fit("pol1");
    gr->Draw("psame");

}
