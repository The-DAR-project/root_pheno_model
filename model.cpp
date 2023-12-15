
double sHeat(double *xvar, double *par){

    double timeWin[2] = {par[0], par[1]};
    double timeStep = (timeWin[1]-timeWin[0])/1000.;
    
    double timeOffset = par[2];
    double kin        = par[3];
    double Qloss      = par[4];
    double MeanT      = par[5];

    double time = xvar[0]-timeOffset;

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



void model() {
    // Measurement 503
    // double tempAmbHeating = 20.69;
    // double tempAmbCooling = 38.93;
    // double coolingStartTime = 5180.0;
    // int coolingStartIndex = 1030;
    // int heatingEndIndex = 970;
    // TString filename = "measurement_503.csv";

    // Measurement 509
    double tempAmbHeating = 21.63;
    double tempAmbCooling = 32.0;
    double coolingStartTime = 14205.0;
    int coolingStartIndex = 2823;
    int heatingEndIndex = 1449;
    TString filename = "measurement_509.csv";

    //======== SetUp =============
    double TemperError = 1.0;  // Estimate, needs more attention
    int rebin[2] = {30, 100};          // rebinning 100 for cooling 30 for heating
    //============================

    ifstream myfile(filename);
    if(myfile.fail()) {
        cout << "File does not exists" << endl;
        exit(0);
    }

    char dummyText[1024];
    myfile.getline(dummyText, 1024);

    double pwr, tvd02, tvd06, tvd03, pt1000, clock;

    TGraphErrors *chartHeating = new TGraphErrors();
    TGraphErrors *chartCooling = new TGraphErrors();
    TGraphErrors *chartHeatingReb = new TGraphErrors();
    TGraphErrors *chartCoolingReb = new TGraphErrors();
    TGraphErrors *chartWin = new TGraphErrors();
    TGraph *gr;


    cout << "Starting to read!" << endl;
    int count = 0;
    int countLines = 0;
    double sumTemp = 0, sumTime = 0, averTemp = 0, averTime = 0;
    while(!myfile.eof()) {
        //myfile >> pwr >> tvd02 >> tvd06 >> tvd03 >> pt1000 >> clock;
        myfile >> pwr >> tvd02 >> tvd06 >> pt1000 >> clock;

        // Chart W_in
        chartWin->AddPoint(clock, pwr);
        cout << clock << " " << pwr << endl;

        if(count <= heatingEndIndex) {
            chartHeating->AddPoint(clock, tvd02 - tempAmbHeating);
            // Rebining
            if(countLines<rebin[0]) {
                sumTemp += tvd02; // - tempAmbHeating;
                sumTime += clock; //*(tvd02 - tempAmbHeating);
                countLines++;
            } else {
                averTemp = sumTemp/rebin[0];
                averTime = sumTime/rebin[0];
                chartHeatingReb->AddPoint(averTime, averTemp - tempAmbHeating);
                chartHeatingReb->SetPointError(chartHeatingReb->GetN()-1, 0, 2*TemperError);
                countLines = 0;
                sumTemp = 0;
                sumTime = 0;
            }
        } else if(count > coolingStartIndex && count < 10000) {
            chartCooling->AddPoint(clock - coolingStartTime, tvd02 - tempAmbCooling);
            chartCooling->SetPointError(chartCooling->GetN()-1, 0, TemperError);
            // Rebining
            if(countLines<rebin[1]) {
                sumTemp += tvd02; // - tempAmbHeating;
                sumTime += clock; //*(tvd02 - tempAmbHeating);
                countLines++;
            } else {
                averTemp = sumTemp/rebin[1];
                averTime = sumTime/rebin[1];
                chartCoolingReb->AddPoint(averTime - coolingStartTime, averTemp - tempAmbCooling);
                chartCoolingReb->SetPointError(chartCoolingReb->GetN()-1, 0, TemperError);
                countLines = 0;
                sumTemp = 0;
                sumTime = 0;
            }   
        }

        count++;
    }

    cout << "Read count: " << count << endl;

    TH2F *hfr;
    double Xrange[2], Yrange[2];
    double fitRange[2]={5000, 30000};


    // Heating chart ======================================================
    // rebin 30

    mc(2, 1.5);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetMarkerSize(1.6);
    mpad->SetLogx(0);
    mpad->SetGridx(0);
    mpad->SetLogy(0);
    mpad->SetGridy(0);
    gPad->SetLeftMargin(0.17);

    gr = chartHeatingReb;
    Xrange[0] = 0*gr->GetXaxis()->GetXmin();
    Xrange[1] = gr->GetXaxis()->GetXmax();
    Yrange[0] = gr->GetYaxis()->GetXmin();
    Yrange[1] = gr->GetYaxis()->GetXmax();
    //cout << Xrange[0]  <<" "<< Xrange[1] <<" "<< Yrange[0] <<" "<< Yrange[1] <<endl; 
    //return;

    hfr = new TH2F("hfr"," ", 10, Xrange[0], Xrange[1], 10, Yrange[0], Yrange[1]); 
    hset( *hfr, "Time [hour:min]","Temp [deg]");
    //hfr->GetXaxis()->SetTimeDisplay(1);
    //hfr->GetXaxis()->SetTimeFormat("%H:%M");
    hfr->Draw();

    gr->SetMarkerStyle(33); 
    gr->SetMarkerColor(4); 
    gr->SetMarkerSize(1.2); 
    gr->Draw("PZ");

    fitRange[0]=150;
    fitRange[1]=7000; //7e3;
 
    TF1 *fHeat = new TF1("fheat", sHeat, fitRange[0], fitRange[1], 6);

    fHeat->SetParameters(
            fitRange[0], // 0 time interval to fit, FIXED
            fitRange[1], // 1 FIXED
            150,       // 2 time offset
            1.8e-2,    // 3 kin (heating = kin*Pin]
            7.9e3,   // 4 Qloss
            3.3e7      // 5 Spadova teplota
            );
    fHeat->SetParName(0,"timeR[0]");
    fHeat->SetParName(1,"timeR[1]");
    fHeat->SetParName(2,"timeOFST");
    fHeat->SetParName(3,"kin");
    fHeat->SetParName(4,"Qloss");
    fHeat->SetParName(5,"MeanT");

    fHeat->FixParameter(0, Xrange[0]);
    fHeat->FixParameter(1, Xrange[1]);
    //fHeat->FixParameter(2, 150);
    //fHeat->FixParameter(3, 1.8e-2);
    //fHeat->FixParameter(4, 0);
    //fHeat->FixParameter(5, 1);
    
    //fHeat->Draw("same"); return;

    gr->Fit("fheat"," ","R", fitRange[0], fitRange[1]);
    fHeat->SetParameter(4,0);
    fHeat->SetLineStyle(2); fHeat->Draw("same");
    //return;


    // Cooling chart =======================================================
    // rebin 100

    mc(3,1.5);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetMarkerSize(1.6);
    mpad->SetLogx(0);
    mpad->SetGridx(0);
    mpad->SetLogy(0);
    mpad->SetGridy(0);
    gPad->SetLeftMargin(0.17);

    gr = chartCoolingReb;

    Xrange[0] = 0*gr->GetXaxis()->GetXmin();
    Xrange[1] = gr->GetXaxis()->GetXmax();
    Yrange[0] = gr->GetYaxis()->GetXmin();
    Yrange[1] = gr->GetYaxis()->GetXmax();

    TF1 *fexp = new TF1("fexp","[0]*exp(-x*[1])", Xrange[0], Xrange[1]);
    fexp->SetParameters(3.85653, 6.28243e-05);
    fexp->SetLineWidth(3);
    fexp->SetLineColor(2);
    fitRange[0]=5e3;
    fitRange[1]=3e4;

    hfr = new TH2F("hfr"," ", 10, Xrange[0], Xrange[1], 10, Yrange[0], Yrange[1]); 
    hset( *hfr, "Time [hour:min]","Temp [deg]");
    hfr->GetXaxis()->SetTimeDisplay(1);
    hfr->GetXaxis()->SetTimeFormat("%H:%M");
    hfr->Draw();

    //-----------------
    gr->SetMarkerStyle(33); 
    gr->SetMarkerColor(4); 
    gr->SetMarkerSize(1.2); 
    //gr->SetLineColor(2);
    gr->Fit("fexp"," ","R", fitRange[0], fitRange[1]);
    fexp->Draw("same");

    gr->Draw("PZ");

    TLegend *leg;
    leg = new TLegend(0.5,0.7,0.7,0.9,"Sauna - cooling","brNDC");
    leg->SetFillStyle(0);leg->SetBorderSize(0);leg->SetTextSize(0.04);
    leg->AddEntry(gr, "measurement","p");
    leg->AddEntry(fexp, "expo. fit", "l");
    leg->AddEntry((TObject*)0, Form("T_{0}=%2.1f #circC", fexp->GetParameter(0)),"");
    leg->AddEntry((TObject*)0, Form("#LTt#GT=%4.0f min", 1/fexp->GetParameter(1)/60),"");
    leg->Draw(); 

    TLine *l;
    l = new TLine(fitRange[0], Yrange[0], fitRange[0], Yrange[1]); l->SetLineStyle(2); l->Draw(); 
    l = new TLine(fitRange[1], Yrange[0], fitRange[1], Yrange[1]); l->SetLineStyle(2); l->Draw(); 

    pPrint("../../figs/cooling","c3");

    // Cumulative W_in =======================================================

    mc(4,1.5);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetMarkerSize(1.6);
    mpad->SetLogx(0);
    mpad->SetGridx(0);
    mpad->SetLogy(0);
    mpad->SetGridy(0);
    gPad->SetLeftMargin(0.17);

    gr = chartWin;

    //-----------------
    gr->SetMarkerStyle(33); 
    gr->SetMarkerColor(4); 
    gr->SetMarkerSize(1.2); 
    gr->SetLineColor(2);
    gr->Draw();
}
