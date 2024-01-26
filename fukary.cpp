
class TFukary {
    
    public:

        TGraphErrors *grTRaw[3][2], *grTemp[3], *grW[3];
        double offsetT[3];
        TString fname, path, label, fullName;
        TF1 *fitf;
        
        double Tlim, eTlim;
        double Tamp, eTamp;
        double Tbeta, eTbeta;
        int lw;

        TFukary(TString inpath, TString infname, TString inLabel){

            lw = 2;

            path       = inpath;
            fname      = infname;
            fullName   = path+fname+".csv";

            ifstream myfile(fullName);

            if(myfile.fail()){
                cout <<"file: "<< fullName <<" doesn't exist" <<endl;
                exit(0);
            }
            label = inLabel;

            gSystem->Exec("cat "+fullName+"|wc -l >l.txt");
            ifstream linefile("l.txt");
            int NoLines = 0;
            linefile >> NoLines;

            for(int i=0; i<3; i++){
                grTRaw[i][0] = new TGraphErrors(NoLines); 
                grTRaw[i][1] = new TGraphErrors(NoLines); 
                grTemp[i] = new TGraphErrors(NoLines); 
                grW[i] = new TGraphErrors(NoLines); 
            }

            char dummyText[1024];
            myfile.getline(dummyText, 1024);

            double sec, TempIn[3], TempOut[3], Win;
            int i=0;
            while(!myfile.eof()){
                myfile >> TempIn[0] >> TempIn[1] >> TempIn[2] >> TempOut[0] >> TempOut[1] >> TempOut[2] >> sec >> Win;
                //myfile >> sec >> TempIn[0] >> TempIn[1] >> TempIn[2] >> TempOut[0] >> TempOut[1] >> TempOut[2] >> Win;
                //cout << "in s=" << sec <<" W="<< Win <<" \t "<< TempOut[0]-TempIn[0] <<" \t "<< TempOut[1]-TempIn[1] <<" \t "<< TempOut[2]-TempIn[2] <<endl; 

                if(i==0) for(int id=0; id<3; id++){offsetT[id]=TempOut[id]-TempIn[id];}

                for(int id=0; id<3; id++){ 
                    grTRaw[id][0]->SetPoint(i, sec, TempOut[id]);
                    grTRaw[id][1]->SetPoint(i, sec, TempIn[id]);
                    //grTRaw[id][0]->SetPointError(i, 0, 1.0);
                    //grTRaw[id][1]->SetPointError(i, 0, 1.0);

                    grTemp[id]->SetPoint(i, sec, TempOut[id]-TempIn[id] - offsetT[id]);
                    grTemp[id]->SetPointError(i, 0, 1.0);

                    grW[id]->SetPoint(i, Win, TempOut[id]-TempIn[id] - offsetT[id]);
                    grW[id]->SetPointError(i, 0, 1.0);
                    //if(id==0) cout<< i <<" "<< sec << " " << Win <<" "<< TempOut[id]-TempIn[id] - offsetT <<endl; 
                }
                i++;
                //if(i>10) exit(0);
            }
        }

        TString GetFileName(){ return fullName;}
        TString GetDir(){ return path; }
        TString GetLabel(){ return label; }

        double GetOffsetT(int ID){ return offsetT[ID];}

        void DrawGrdW(int FID, int col, TLegend *leg){
            gr = grW[FID];
            fitf = new TF1("fitf","[0]-[1]*exp(-[2]*x)", 0, 5);
            fitf->SetParameters(66, 87, 1);
            fitf->SetLineWidth(lw); fitf->SetLineColor(1); fitf->SetLineStyle(2);
            gr->Fit(fitf,"","R+",0.5, 4);
            gr->SetMarkerStyle(21); gr->SetMarkerSize(0.3); gr->SetMarkerColor(1); gr->SetFillColor(col); gr->Draw("e3"); gr->Draw("X");
            Tlim  = fitf->GetParameter(0);  eTlim = fitf->GetParError(0);
            Tamp  = fitf->GetParameter(1);  eTamp = fitf->GetParError(1);
            Tbeta  = fitf->GetParameter(2); eTbeta = fitf->GetParError(2);
            leg->AddEntry(gr, label+Form(" T_{L}=%2.1f#pm%1.1f #circC", Tlim, eTlim), "f");
        }

};

void CalcRatios(TFukary *mD, TFukary *mP){
        double Wref = 4.0; //kWh
        double Tref = mP->Tlim-mP->Tamp*exp(-mP->Tbeta*Wref);
        double Wcomp = -1/mD->Tbeta*log((mD->Tlim-Tref)/mD->Tamp);
        double Tcomp = mD->Tlim-mD->Tamp*exp(-mD->Tbeta*Wcomp);
        //cout<< Wref <<" "<< Tref <<" "<< Wcomp <<" "<< Tcomp <<endl; 
        TLine *l;
        l = new TLine(Wref, Tref, Wcomp, Tcomp); l->SetLineStyle(2); l->SetLineColor(1); l->Draw(); 
        l = new TLine(Wref, Tref, Wref, 0); l->SetLineStyle(2);      l->SetLineColor(1); l->Draw(); 
        l = new TLine(Wcomp, Tcomp, Wcomp, 0); l->SetLineStyle(2);   l->SetLineColor(1); l->Draw(); 
        TLatex *tex = new TLatex((Wref+Wcomp)/2., Tref+2,Form("T_{ref}=%2.1f#circC",Tref)); tex->SetTextSize(0.03); tex->Draw();

        TPad *pad = new TPad("pad", " ",0.2, 0.93, 0.9, 1.00);
        pad->Draw(); pad->cd();  pad->Range(0,0,1,1);
        pad->SetFillColor(33);pad->SetBorderMode(0); pad->SetBorderSize(0);
        TPaveText *pt = new TPaveText(0., 0, 1., 1.,"br");

        double Ratio = mD->Tlim/mP->Tlim;
        double eRatio = 1/pow(mP->Tlim,2)*sqrt(pow(mP->Tlim*mD->eTlim,2)+pow(mD->Tlim*mP->eTlim,2));
        pt->AddText(Form("T^{D}_{L}/T^{ND}_{L} = %3.1f#pm%1.1f%%; S_{rel}=%3.1f%%", Ratio*100, eRatio*100, Wcomp/Wref*100)); pt->Draw();
}




void fukary(){


    TFukary *mP = new TFukary("2022-09-06-Fukary/","Fukary-DAL-2R-25cm", "Dal_2R_25cm_20uF");
    //TFukary *mP = new TFukary("2022-09-06-Fukary/","Fukary-DIR-3R-25cm", "DIR_3R_25cm");
    //TFukary *mD = new TFukary("2022-09-06-Fukary/","Fukary-DAL-2R-15cm", "Dal_2R_15cm");
    //TFukary *mP = new TFukary("2022-09-06-Fukary/","Fukary-DIR-3R-15cm", "Dir_3R_15cm");

    //TFukary *mD = new TFukary("2022-09-06-Fukary/","F-DAL-2R-15cm-2", "DAL_2R_15cm");
    //TFukary *mP = new TFukary("2022-09-06-Fukary/","F-DIR-2R-15cm", "DIR_2R_15cm");
    //TFukary *mP = new TFukary("2022-09-06-Fukary/","F-GRE-2R-15cm", "GRE_2R_15cm");

    TFukary *mD = new TFukary("2022-09-06-Fukary/","F-DAL-2R-25cm-40uF-LM", "Dal_2R_25cm_40uF");
    //TFukary *mP = new TFukary("2022-09-06-Fukary/","F-GRE-2R-25cm-LM", "Gre_2R_25cm");

    TFukary *mP3 = new TFukary("2022-09-12-Fukary/","F-DIR-3R-25cm-2vs3comp", "Dir_3R_25cm");
    TFukary *mP2 = new TFukary("2022-09-12-Fukary/","F-DIR-2R-25cm-2vs3comp", "Dir_2R_25cm");
    //TFukary *mP=mP3;

    int TID=0, calc=1;

    TGraphErrors *gr;
    TFukary *data;
    TPad *pad;
    TPaveText *pt;
    TLine *l;
    TLatex *tex;

    int lw = 2;

    //-------------------------------------------
    mc(2,1.8);
    gStyle->SetOptStat(0);gStyle->SetOptTitle(0);
    gStyle->SetMarkerSize(1.6);
    mpad->SetLogx(0);mpad->SetGridx(0);
    mpad->SetLogy(0);mpad->SetGridy(1);
    gPad->SetLeftMargin(0.17);
    TH2F *hfr = new TH2F("hfr"," ", 10, 0, 1050, 10, 0, 110);
    hset( *hfr, "time  [s]","T [deg]");
    hfr->Draw();

    TLegend *leg = new TLegend(0.2,0.7,0.6,0.95," ","brNDC");
    leg->SetFillStyle(0);leg->SetBorderSize(0);leg->SetTextSize(0.03);

    data = mD;
    for(int id=0; id<3; id++){
        gr = data->grTRaw[id][0];
        //cout << gr <<" "<< data <<endl; return;
        gr->SetLineColor(id+1); gr->SetLineWidth(lw); gr->Draw("l");
        leg->AddEntry(gr, Form("DALYN 2R id=%d",id), "l");
        gr = data->grTRaw[id][1];
        gr->SetLineColor(id+1);  gr->SetLineWidth(lw); gr->Draw("L");
    } 
    data = mP;
    for(int id=0; id<3; id++){
        gr = data->grTRaw[id][0];
        gr->SetLineColor(id+1); gr->SetLineWidth(lw); gr->Draw("l");
        leg->AddEntry(gr, Form("DALYN 2R id=%d",id), "l");
        gr = data->grTRaw[id][1];
        gr->SetLineColor(id+1); gr->SetLineWidth(lw); gr->Draw("L");
    } 

    leg->Draw();


    //-------------------------------------------
    mc(3,1.8);
    gStyle->SetOptStat(0);gStyle->SetOptTitle(0);
    gStyle->SetMarkerSize(1.6);
    mpad->SetLogx(0);mpad->SetGridx(0);
    mpad->SetLogy(0);mpad->SetGridy(1);
    gPad->SetLeftMargin(0.17);
    hfr = new TH2F("hfr"," ", 10, 0, 1050, 10, 0, 80);
    hset( *hfr, "time  [s]","#DeltaT [deg]");
    hfr->Draw();

    leg = new TLegend(0.2,0.8,0.6,0.95," ","brNDC");
    leg->SetFillStyle(0);leg->SetBorderSize(0);leg->SetTextSize(0.03);

    data = mD;
    gr = data->grTemp[TID];
    //cout<< gr <<" "<< data->grTemp[TID] << endl; return;
    gr->SetLineColor(1); gr->SetLineWidth(lw); gr->SetFillColor(kRed-10); gr->Draw("e3"); gr->Draw("LX");
    leg->AddEntry(gr, data->label, "f");

    data = mP;
    gr = data->grTemp[TID];
    gr->SetLineColor(1); gr->SetLineWidth(lw); gr->SetFillColor(kBlue-10); gr->Draw("e3"); gr->Draw("LX");
    leg->AddEntry(gr, data->label, "f");

    leg->Draw(); 


    //-------------------------------------------
    mc(4,1.8);
    gStyle->SetOptStat(0);gStyle->SetOptTitle(0);
    gStyle->SetMarkerSize(1.6);
    mpad->SetLogx(0);mpad->SetGridx(0);
    mpad->SetLogy(0);mpad->SetGridy(1);
    gPad->SetLeftMargin(0.17);
    hfr = new TH2F("hfr"," ", 10, 0, 5, 10, 0, 80);
    //hfr = new TH2F("hfr"," ", 10, 0, 0.5, 10, -5, 5);
    hset( *hfr, "W_{in} [kWh]","#DeltaT [deg]");
    hfr->Draw();

    leg = new TLegend(0.2,0.8,0.6,0.95," ","brNDC");
    leg->SetFillStyle(0);leg->SetBorderSize(0);leg->SetTextSize(0.03);

    mD->DrawGrdW(0, kRed-10, leg);
    mP->DrawGrdW(0, kBlue-10, leg);
    mP3->DrawGrdW(0, kYellow-10, leg);
    mP2->DrawGrdW(0, kGreen-10, leg);
    leg->Draw(); 

    if(calc==1) CalcRatios(mD, mP);

    pPrint(mD->path+"figs/F-dTt-"+mD->label+mP->label,"c3");
    pPrint(mD->path+"figs/F-dTW-"+mD->label+mP->label,"c4");
}

