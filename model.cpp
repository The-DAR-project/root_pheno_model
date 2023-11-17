void model() {
	// Measurement 503
	// double tempAmbHeating = 20.69;
	// double tempAmbCooling = 38.93;
	// double coolingStartTime = 5180.0;
	// int coolingStartIndex = 1030;
	// int heatingEndIndex = 970;
	// TString filename = "measurement_503.csv";

	// Measurement 509
	double tempAmbHeating = 21.63;
	double tempAmbCooling = 32.0;
	double coolingStartTime = 14205.0;
	int coolingStartIndex = 2823;
	int heatingEndIndex = 1449;
	TString filename = "measurement_509.csv";

	TF1 *f;
	TF1 *fout = new TF1("fout","[0]", 0, 40);

	ifstream myfile(filename);
	if(myfile.fail()) {
		cout << "File does not exists" << endl;
		exit(0);
	}

	char dummyText[1024];
	myfile.getline(dummyText, 1024);

	double pwr, tvd02, tvd06, tvd03, pt1000, clock;

    TGraph *chartHeating = new TGraph();
	TGraph *chartCooling = new TGraph();
	TGraph *gr;

    fout->SetLineColor(1); fout->SetLineStyle(2); fout->Draw("same");

	cout << "Starting to read!" << endl;
	int count = 0;
	while(!myfile.eof()) {
		//myfile >> pwr >> tvd02 >> tvd06 >> tvd03 >> pt1000 >> clock;
		myfile >> pwr >> tvd02 >> tvd06 >> pt1000 >> clock;

		if(count <= heatingEndIndex) {
			chartHeating->AddPoint(clock, tvd02 - tempAmbHeating);
		} else if(count > coolingStartIndex && count < 10000) {
			chartCooling->AddPoint(clock - coolingStartTime, tvd02 - tempAmbCooling);
		}
		
		count++;
	}

	cout << "Read count: " << count << endl;

	TH2F *hfr;

    // First heating chart
	mc(2,1.8, 300, 100, 10, 2.);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetMarkerSize(1.6);
    mpad->SetLogx(0);
    mpad->SetGridx(0);
    mpad->SetLogy(0);
    mpad->SetGridy(0);
    gPad->SetLeftMargin(0.17);

    hfr = new TH2F("hfr"," ", 10, 0, 20, 10, 0, 100);
    hset( *hfr, "Time [s]","Temp [deg]");
    hfr->Draw();

    gr=chartHeating;
    gr->SetMarkerStyle(20); gr->SetMarkerColor(2); gr->SetMarkerSize(0.4); gr->SetLineWidth(4); gr->SetLineColor(2); 
    gr->Draw();

    // First cooling chart
	mc(3,1.8, 300, 100, 10, 2.);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);
    gStyle->SetMarkerSize(1.6);
    mpad->SetLogx(0);
    mpad->SetGridx(0);
    mpad->SetLogy(0);
    mpad->SetGridy(0);
    gPad->SetLeftMargin(0.17);

    hfr = new TH2F("hfr"," ", 10, 0, 20, 10, 0, 100);
    hset( *hfr, "Time [s]","Temp [deg]");
    hfr->Draw();

    gr=chartCooling;
    gr->SetMarkerStyle(20); 
    gr->SetMarkerColor(4); 
    gr->SetMarkerSize(0.4); 
    gr->SetLineWidth(4); 
    gr->SetLineColor(4); 
    gr->Draw();

    // TF1 *fexp = new TF1("fexp","[0]+[1]*exp(-x[2])", 100, 2500);
    // fexp->SetParameters(1.444, 5.2, 0.0041);
    // TLine *l = new TLine(100,1.444,2500,1.444); l->SetLineWidth(2); l->SetLineColor(4); l->SetLineStyle(2); l->Draw();
    // fexp->Draw("same");
    // gr->Fit("fexp");
    // gr->SetMarkerStyle(43); 
    // gr->SetMarkerSize(1.6); 
    // gr->Draw();

    f = fout;
    f->SetLineColor(1); 
    f->SetLineStyle(2); 
    f->Draw("same");
}