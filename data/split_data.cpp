#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    string inputFile;
    double heatingEndClock = -1.0; // Default to -1.0, indicating not set
    double coolingStartClock = -1.0; // Default to -1.0, indicating not set
    bool hasHeatingData = false;
    bool hasCoolingData = false;

    // Parsing command line arguments
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-i") {
            if (i + 1 < argc) { inputFile = argv[++i]; }
            else { cerr << "No input file specified after -i\n"; return 1; }
        }
        else if (arg == "-he") {
            if (i + 1 < argc) { heatingEndClock = stod(argv[++i]); }
            else { cerr << "No heating end clock specified after -he\n"; return 1; }
        }
        else if (arg == "-cs") {
            if (i + 1 < argc) { coolingStartClock = stod(argv[++i]); }
            else { cerr << "No cooling start clock specified after -cs\n"; return 1; }
        }
        else if (arg == "--heating") {
            hasHeatingData = true;
        }
        else if (arg == "--cooling") {
            hasCoolingData = true;
        }
    }

    // Check if required arguments are provided
    if (inputFile.empty()) {
        cerr << "Usage: " << argv[0] << " -i <input_file> [-he <heating_end_clock>] [-cs <cooling_start_clock>] [--heating] [--cooling]\n";
        return 1;
    }

    ifstream inFile(inputFile);
    if (!inFile) {
        cerr << "Error: Unable to open input file " << inputFile << endl;
        return 1;
    }

    ofstream heatingFile("heating.csv");
    ofstream coolingFile("cooling.csv");

    string line;
    while (getline(inFile, line)) {
        istringstream iss(line);
        vector<string> tokens;
        string token;

        // Splitting the line into tokens
        while (getline(iss, token, ' ')) { // Adjust delimiter if needed
            tokens.push_back(token);
        }

        // Check if the line has at least 3 columns (for the clock value)
        if (tokens.size() >= 3) {
            double currentClock = stod(tokens[2]); // Convert clock value to double

            if (hasHeatingData && (heatingEndClock > 0 && currentClock <= heatingEndClock)) {
                heatingFile << line << endl;
            }
            if (hasCoolingData && (coolingStartClock > 0 && currentClock >= coolingStartClock)) {
                coolingFile << line << endl;
            }
        }
    }

    inFile.close();
    heatingFile.close();
    coolingFile.close();

    return 0;
}
