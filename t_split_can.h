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