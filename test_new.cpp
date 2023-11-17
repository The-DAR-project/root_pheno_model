using namespace std;

class PhenomenologicalModel {

public:
	TString filename = "data.csv";
	string csvHeader;
	double pwr, tvd02, tvd06, tvd03, pt1000, time;
	double heatingCsvData[1000][2];
	double coolingCsvData[10000][2];
	double tempAmbHeating = 0.0;
	double tempAmbCooling = 0.0;
	double timeStartCooling = 0.0;
	int coolingDataIndex = 1030;

	void loadCsv() {
		ifstream myfile(filename);
		if(myfile.fail()) {
			cout << "File does not exists" << endl;
			exit(0);
		}

		cout << "Starting to read!" << endl;

		int count = 0;
		while(!myfile.eof()) {
			myfile >> pwr >> tvd02 >> tvd06 >> tvd03 >> pt1000 >> time;
			//cout << pwr << " " << tvd02 << endl;
			if(count <= 970) {
				if(count == 0) {
					tempAmbHeating = tvd02;
				}
				heatingCsvData[count][0] = time;
				heatingCsvData[count][1] = tvd02;
			}
			count++;
		}

		cout << "End of reading!" << endl;
	}

	void drawCharts() {
		TF1 *f;
		TH2F *hfr;
		TF1 *fout = new TF1("fout","[0]", 0, 40);

		TGraph *chartHeating = new TGraph();
		TGraph *chartCooling = new TGraph();
		TGraph *gr;

	    fout->SetLineColor(1); fout->SetLineStyle(2); fout->Draw("same");

	    for (int i = 0; i < sizeof(heatingCsvData); ++i) {
			chartHeating->AddPoint(heatingCsvData[i][0], heatingCsvData[i][1] - tempAmbHeating);
	    }

	    for (int i = 0; i < sizeof(coolingCsvData); ++i) {
			chartCooling->AddPoint(heatingCsvData[i][0], heatingCsvData[i][1] - tempAmbCooling);
	    }

	    // First heating chart
		mc(2,1.8, 300, 100, 10, 2.);
	    gStyle->SetOptStat(0);gStyle->SetOptTitle(0);
	    gStyle->SetMarkerSize(1.6);
	    mpad->SetLogx(0);mpad->SetGridx(0);
	    mpad->SetLogy(0);mpad->SetGridy(0);
	    gPad->SetLeftMargin(0.17);

	    hfr = new TH2F("hfr"," ", 10, 0, 20, 10, 0, 100);
	    hset( *hfr, "Time [s]","Temp [deg]");
	    hfr->Draw();

	    gr=chartHeating;
	    gr->SetMarkerStyle(20); gr->SetMarkerColor(4); gr->SetMarkerSize(0.4); gr->SetLineWidth(4); gr->SetLineColor(4); 
	    gr->Draw();

	    // First cooling chart
		mc(3,1.8, 300, 100, 10, 2.);
	    gStyle->SetOptStat(0);gStyle->SetOptTitle(0);
	    gStyle->SetMarkerSize(1.6);
	    mpad->SetLogx(0);mpad->SetGridx(0);
	    mpad->SetLogy(0);mpad->SetGridy(0);
	    gPad->SetLeftMargin(0.17);

	    hfr = new TH2F("hfr"," ", 10, 0, 20, 10, 0, 100);
	    hset( *hfr, "Time [s]","Temp [deg]");
	    hfr->Draw();

	    gr=chartCooling;
	    gr->SetMarkerStyle(20); gr->SetMarkerColor(2); gr->SetMarkerSize(0.4); gr->SetLineWidth(4); gr->SetLineColor(2); 
	    gr->Draw();

	    f = fout;
	    f->SetLineColor(1); 
	    f->SetLineStyle(2); 
	    f->Draw("same");
	}
};

void test() {
	PhenomenologicalModel model;
	model.loadCsv();
	model.drawCharts();
}

int main() {
	test();
	return 0;
}

// void test() {
// 	TF1 *f;
// 	TF1 *fout = new TF1("fout","[0]", 0, 40);

// 	TString filename = "data.csv";
// 	ifstream myfile(filename);
// 	if(myfile.fail()) {
// 		cout << "File does not exists" << endl;
// 		exit(0);
// 	}



//     TGraph *chartHeating = new TGraph();
// 	TGraph *chartCooling = new TGraph();
// 	TGraph *gr;

//     fout->SetLineColor(1); fout->SetLineStyle(2); fout->Draw("same");

//     double tempAmbHeating = 20.69;
// 	double tempAmbCooling = 38.93;

// 	cout << "Starting to read!" << endl;
// 	int count = 0;
// 	while(!myfile.eof()) {
// 		myfile >> pwr >> tvd02 >> tvd06 >> tvd03 >> pt1000 >> clock;
// 		//cout << pwr << " " << tvd02 << endl;
// 		if(count <= 970) {
// 			if(count == 0) tempAmbHeating = tvd02;
// 			chartHeating->AddPoint(clock, tvd02 - tempAmbHeating);
// 		} else if(count > 1030) {
// 			chartCooling->AddPoint(clock, tvd02 - tempAmbCooling);
// 		}
// 		count++;
// 	}

// 	cout << "Read count: " << count << endl;

// 	TH2F *hfr;

//     // First heating chart
// 	mc(2,1.8, 300, 100, 10, 2.);
//     gStyle->SetOptStat(0);gStyle->SetOptTitle(0);
//     gStyle->SetMarkerSize(1.6);
//     mpad->SetLogx(0);mpad->SetGridx(0);
//     mpad->SetLogy(0);mpad->SetGridy(0);
//     gPad->SetLeftMargin(0.17);

//     hfr = new TH2F("hfr"," ", 10, 0, 20, 10, 0, 100);
//     hset( *hfr, "Time [s]","Temp [deg]");
//     hfr->Draw();

//     gr=chartHeating;
//     gr->SetMarkerStyle(20); gr->SetMarkerColor(4); gr->SetMarkerSize(0.4); gr->SetLineWidth(4); gr->SetLineColor(4); 
//     gr->Draw();

//     // First cooling chart
// 	mc(3,1.8, 300, 100, 10, 2.);
//     gStyle->SetOptStat(0);gStyle->SetOptTitle(0);
//     gStyle->SetMarkerSize(1.6);
//     mpad->SetLogx(0);mpad->SetGridx(0);
//     mpad->SetLogy(0);mpad->SetGridy(0);
//     gPad->SetLeftMargin(0.17);

//     hfr = new TH2F("hfr"," ", 10, 0, 20, 10, 0, 100);
//     hset( *hfr, "Time [s]","Temp [deg]");
//     hfr->Draw();

//     gr=chartCooling;
//     gr->SetMarkerStyle(20); gr->SetMarkerColor(2); gr->SetMarkerSize(0.4); gr->SetLineWidth(4); gr->SetLineColor(2); 
//     gr->Draw();

//     f = fout;
//     f->SetLineColor(1); 
//     f->SetLineStyle(2); 
//     f->Draw("same");
// }