

class  TFcn {
    
    public:
        TFcn(){
            cout<<"ikjkk " <<endl;
        }



        double Evaluate(double *x, double *p) {
            return p[0] +x[0]*p[1];
        }
};



void test(){


    TFcn *fptr = new TFcn();  // create the user function class

    int npar = 2;
    auto f = new TF1("f", fptr, &TFcn::Evaluate, 0, 1, npar, "TFcn", "Evaluate");   // create TF1 class.
    f->SetParameters(1,1);
    f->Draw();
}

