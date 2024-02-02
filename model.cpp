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
    double Tmax = MeanT*log(kin/Qloss+1) + data->TAmbient;

    fHeat->SetParameter(2,0); // Pin
    fHeat->SetLineStyle(2);
    fHeat->DrawCopy("same");

    double lineTime[2]={1000,3600};
    cout << "Temp should be =" << fHeat->Eval(1800) <<" deg" << endl;
    cout << "Temp should be =" << fHeat->Eval(3600) <<" deg" << endl;
    double linWarmRate = (fHeat->Eval(lineTime[1]) - fHeat->Eval(lineTime[0]))/(lineTime[1] - lineTime[0])*60.;
    cout << "Warmrate is " << linWarmRate << " deg / min" << endl;

    fHeat->SetParameter(2,Qloss); // Pin

    leg = new TLegend(0.4,0.2,0.9,0.5," ","brNDC");
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(gr, "data","p");
    auto *faux = new TF1("faux", "x", 0,1);
    leg->AddEntry(faux, "model heating","l");
    leg->AddEntry(fHeat, "no loss heating","l");
    leg->AddEntry((TObject*)0, Form("Tmax (P_{in}=%1.2fkW) = %2.1f #circC", Pin, Tmax),"");
    leg->AddEntry((TObject*)0, Form("NoLossWarmRate=%1.2f #circC/min", linWarmRate),"");
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

void drawCoolingChart(TData *data, TString chartTitle) {
    TGraphErrors *gr, *grDiff;
    TH2F *hfr;
    TLegend *leg;
    TSplitCan *splitCan;

    double Xrange[2], Yrange[2];

    splitCan = new TSplitCan(3,1);
    splitCan->SetGrid(1,0,1);
    splitCan->subMpad[0]->cd();

    gr = data->chartCoolingReb;

    Xrange[0] = 0*gr->GetXaxis()->GetXmin();
    Xrange[1] = gr->GetXaxis()->GetXmax();
    Yrange[0] = gr->GetYaxis()->GetXmin();
    Yrange[1] = gr->GetYaxis()->GetXmax();

    TF1 *fexp = new TF1("fexp","[0]*exp(-x*[1])+[2]", Xrange[0], Xrange[1]);
    fexp->SetParameters(3.85653, 6.28243e-05, 20);
    fexp->SetLineWidth(3);
    fexp->SetLineColor(2);
    //double fitRange[2] = {2e3, 3.5e4};
    double fitRange[2] = {2e3, 4.4e4};

    hfr = new TH2F("hfr"," ", 10, Xrange[0], Xrange[1], 10, Yrange[0], Yrange[1]); 
    hset( *hfr, "Time [h:m]","Temp [deg]");
    splitCan->SetUpperFramePars(*hfr);
    hfr->Draw();

    //-----------------
    gr->SetMarkerStyle(33); 
    gr->SetMarkerColor(4); 
    gr->SetMarkerSize(1.2); 
    //gr->SetLineColor(2);
    //gr->Fit("fexp"," ","R", fitRange[0], fitRange[1]);
    gr->Fit("fexp");
    fexp->Draw("same");

    gr->Draw("PZ");

    leg = new TLegend(0.5,0.6,0.7,0.9,"Sauna - cooling","brNDC");
    leg->SetFillStyle(0);leg->SetBorderSize(0);leg->SetTextSize(0.04);
    leg->AddEntry(gr, "data","p");
    leg->AddEntry(fexp, "expo. fit", "l");
    leg->AddEntry((TObject*)0, Form("T_{0}=%2.1f #circC", fexp->GetParameter(0)),"");
    leg->AddEntry((TObject*)0, Form("#LTt#GT=%4.0f min", 1/fexp->GetParameter(1)/60),"");
    leg->Draw();

    //TLine *l;
    //l = new TLine(fitRange[0], Yrange[0], fitRange[0], Yrange[1]); l->SetLineStyle(2); l->Draw(); 
    //l = new TLine(fitRange[1], Yrange[0], fitRange[1], Yrange[1]); l->SetLineStyle(2); l->Draw(); 

    splitCan->PrintName(chartTitle);

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

    //pPrint(Form("../../figs/heating_%i",data->GetRunID()),"c2");
    //pPrint(Form("../../figs/cooling_%i",data->GetRunID()),"c3");
}

void drawCumulativeWinChart(TData *data) {
    TGraphErrors *gr, *grDiff;
    TH2F *hfr;
    TLegend *leg;
    TSplitCan *splitCan;
    splitCan = new TSplitCan(4,0);

    gr = data->chartWin;
    hfr = new TH2F("hfr"," ", 
            10, gr->GetXaxis()->GetXmin(), gr->GetXaxis()->GetXmax(), 
            10, gr->GetYaxis()->GetXmin(), gr->GetYaxis()->GetXmax());

    hset( *hfr, "Time [hour:min]","P_{in} [kWh]");
    hfr->GetXaxis()->SetTimeDisplay(1);
    hfr->GetXaxis()->SetTimeFormat("%H:%M");
    hfr->GetXaxis()->SetTimeOffset(-3600);
    hfr->Draw();

    //-----------------
    gr->SetMarkerStyle(33);
    gr->SetMarkerColor(4);
    gr->SetMarkerSize(1.2);
    gr->SetLineColor(4);

    //gr->Fit("pol1");
    gr->Draw("psame");

    leg = new TLegend(0.2,0.2,0.72,0.4," ","brNDC");
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.04);
    leg->AddEntry((TObject*)0, Form("P_{in} = %3.2f kW", gr->GetFunction("pol1")->GetParameter(1)*3600 ),"p");
    leg->Draw();
}

void model() {
    // Params: fileName, dataType, Tamb, rebin
    TData *heatingData = new TData("csv_data/509/heating.csv", "heating", 21.6, 30);
    TData *coolingData = new TData("csv_data/509/cooling.csv", "cooling", 21.6, 100);

    // Params: TData var, Chart title
    drawHeatChart(heatingData, "Heating");
    drawCoolingChart(coolingData, "Cooling");
    drawCumulativeWinChart(heatingData);
}