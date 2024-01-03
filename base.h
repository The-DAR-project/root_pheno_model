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
        int RunID;

        TString fileName;

        TData(TString infilename, int inRunID, double Tamb, double heatEndTime, int rebHeat, double coolStarTime, int rebCool){
            //======== SetUp =============
            double TemperError = 1.0;          // Estimate, needs more attention
            double WinError    = 0.1;
            int rebin[2] = {rebHeat, rebCool};          // rebinning 100 for cooling 30 for heating
            RunID = inRunID;

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

                if(RunID == 509)
                    myfile >> pwr >> tvd02 >> tvd06 >> pt1000 >> clock;
                else
                    myfile >> pwr >> tvd02 >> tvd06 >> tvd03 >> pt1000 >> clock;

                pwr *= 1./800; // kW/800

                grAll->AddPoint(clock, tvd02); //for bounder check;

                if(clock <= heatEndTime) {
                    // Rebining
                    if(countLines<rebin[0]) {
                        sumTemp += tvd02; // - Tamb;
                        sumTime += clock; //*(tvd02 - Tamb);
                        sumPWR  += pwr;
                        countLines++;
                    } else {
                        averTemp = sumTemp/rebin[0];
                        averTime = sumTime/rebin[0];
                        averPWR = sumPWR/rebin[0];

                        chartHeatingReb->AddPoint(averTime, averTemp - Tamb);
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
                        chartCoolingReb->AddPoint(averTime - coolStarTime, averTemp - Tamb);
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
        int GetRunID(){return RunID;}

};

class TSplitCan {
    
    public:
    
        TCanvas *can[10];
        TPad *mpad, *subMpad[2], *pad;
        TPaveText *pt;
        TAxis *ax, *ay;

        TSplitCan(int CID, int split=0){

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
            
            if(split==0){
                mpad->SetLeftMargin(0.15);
                mpad->SetBottomMargin(0.15);
                mpad->SetTopMargin(0.06);
                mpad->SetRightMargin(0.06);
            }

            if(split==1){
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