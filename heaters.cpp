#include "t_heat_func.h"
#include "t_data.h"
#include "t_split_can.h"

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

void drawHeatChart(TData *data, TString chartTitle) {
    TGraphErrors *gr, *grDiff;
    TH2F *hfr;
    TLegend *leg;
    TSplitCan *splitCan;

    double Xrange[2], Yrange[2];

    // calculate P_in
    data->chartWin->Fit("pol1");
    double Pin = data->chartWin->GetFunction("pol1")->GetParameter(1)*3600; //kW

    splitCan = new TSplitCan(2,1);
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


    // Defines fit params "out of hand".
    double timeOfst = 0;            // 0 time offset
    double kin      = 1.73183e-2;   // 1 kin (heating = kin*Pin)
    double Qloss    = 4.96247e3;    // 2 Qloss
    double MeanT    = 2.223e7;      // 3 Spadova teplota

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
    fHeat->SetLineStyle(2);
    fHeat->DrawCopy("same");

    cout << "Temp at 2h should be =" << fHeat->Eval(300) <<" deg" << endl;
    cout << "Warmrate is " << fHeat->Eval(360) - fHeat->Eval(300) << " deg / min" << endl;

    fHeat->SetParameter(2,Qloss); // Pin

    leg = new TLegend(0.4,0.2,0.9,0.5," ","brNDC");
    leg->SetFillStyle(0);leg->SetBorderSize(0);leg->SetTextSize(0.04);
    leg->AddEntry(gr, "data","p");
    auto *faux = new TF1("faux", "x", 0,1);
    leg->AddEntry(faux, "model heating","l");
    leg->AddEntry(fHeat, "no loss heating","l");
    leg->AddEntry((TObject*)0, Form("Tmax (P_{in}=%1.2fkW) = %2.1f #circC", Pin, Tmax),"");
    leg->Draw(); 

    splitCan->PrintName(chartTitle);

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
}

void heaters() {
    //TData *heatingData1 = new TData("data/fukary/heating_2R_DIR.csv", "heating", 21.6, 30);
    TData *heatingData2 = new TData("data/fukary/heating_3R_DIR.csv", "heating", 21.6, 30);
    TData *heatingData3 = new TData("data/fukary/heating_2R_DAL.csv", "heating", 21.6, 30);

    //TData *coolingData = new TData("data/cooling.csv", "cooling", 21.6, 100);

    //drawHeatChart(heatingData1, "Heating");
    drawHeatChart(heatingData2, "Heating");
    drawHeatChart(heatingData3, "Heating");
}