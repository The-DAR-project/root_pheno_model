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