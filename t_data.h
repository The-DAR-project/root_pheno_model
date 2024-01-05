class TData {

    public:

        TGraph       *grAll;
        TGraphErrors *chartHeatingReb;
        TGraphErrors *chartCoolingReb;
        TGraphErrors *chartWin;

        TData(TString fileName, TString dataType, double Tamb, int rebin) {
            double TemperError = 1.0;          // Estimate, needs more attention
            double WinError    = 0.1;

            ifstream myfile(fileName);
            if(myfile.fail()) {
                cout << "File " << fileName << " does not exists" << endl;
                exit(0);
            }

            double pwr, temp, clock;

            grAll           = new TGraph();
            chartHeatingReb = new TGraphErrors();
            chartCoolingReb = new TGraphErrors();
            chartWin = new TGraphErrors();

            int count = 0;
            int countLines = 0;
            double sumTemp = 0, sumTime = 0, sumPWR=0, averTemp = 0, averTime = 0, averPWR = 0;

            while(!myfile.eof()) {
                // Reads file
                myfile >> pwr >> temp >> clock;

                // 800 impuls are 1 kW
                pwr *= 1./800; // kW/800

                grAll->AddPoint(clock, temp); //for bounder check;

                if(dataType == "heating") {
                    // Rebining
                    if(countLines<rebin) {
                        sumTemp += temp; // - Tamb;
                        sumTime += clock; //*(temp - Tamb);
                        sumPWR  += pwr;
                        countLines++;
                    } else {
                        averTemp = sumTemp/rebin;
                        averTime = sumTime/rebin;
                        averPWR = sumPWR/rebin;

                        chartHeatingReb->AddPoint(averTime, averTemp - Tamb);
                        chartHeatingReb->SetPointError(chartHeatingReb->GetN()-1, 0, 2*TemperError);

                        chartWin->AddPoint(averTime, averPWR);
                        chartWin->SetPointError(chartWin->GetN()-1, 0, WinError);

                        countLines = 0;
                        sumTemp = 0;
                        sumTime = 0;
                        sumPWR  = 0;
                    }
                } else if(dataType == "cooling") {
                    if(countLines<rebin) {
                        sumTemp += temp; // - tempAmbHeating;
                        sumTime += clock; //*(temp - tempAmbHeating);
                        countLines++;
                    } else {
                        averTemp = sumTemp/rebin;
                        averTime = sumTime/rebin;
                        chartCoolingReb->AddPoint(averTime - 13500, averTemp - Tamb);
                        chartCoolingReb->SetPointError(chartCoolingReb->GetN()-1, 0, TemperError);
                        countLines = 0;
                        sumTemp = 0;
                        sumTime = 0;
                    }   
                }

                count++;
            }
            cout << "Lines: " << count << endl;
        }

        void DrawTemp() {
            grAll->Draw("AL");
        }
};