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