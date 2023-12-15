

void test(){

     TH1F *hid = new TH1F("Test","h",3000,0.,20000.);
     hid->GetXaxis()->SetTimeDisplay(1);
     //hid->GetXaxis()->SetTimeOffset(inputData->startTime.Convert());
     hid->GetXaxis()->SetTimeFormat("%H:%M");

     //TDatime da(2003,02,28,12,00,00);
     //gStyle->SetTimeOffset(da.Convert());

     hid->Draw();
}
