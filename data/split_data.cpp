#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    std::string inputFile;
    double heatingEndClock = -1.0; // Default to -1.0, indicating not set
    double coolingStartClock = -1.0; // Default to -1.0, indicating not set
    bool hasHeatingData = false;
    bool hasCoolingData = false;

    // Parsing command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-i") {
            if (i + 1 < argc) { inputFile = argv[++i]; }
            else { std::cerr << "No input file specified after -i\n"; return 1; }
        }
        else if (arg == "-he") {
            if (i + 1 < argc) { heatingEndClock = std::stod(argv[++i]); }
            else { std::cerr << "No heating end clock specified after -he\n"; return 1; }
        }
        else if (arg == "-cs") {
            if (i + 1 < argc) { coolingStartClock = std::stod(argv[++i]); }
            else { std::cerr << "No cooling start clock specified after -cs\n"; return 1; }
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
        std::cerr << "Usage: " << argv[0] << " -i <input_file> [-he <heating_end_clock>] [-cs <cooling_start_clock>] [--heating] [--cooling]\n";
        return 1;
    }

    std::ifstream inFile(inputFile);
    if (!inFile) {
        std::cerr << "Error: Unable to open input file " << inputFile << std::endl;
        return 1;
    }

    std::ofstream heatingFile("heating.csv");
    std::ofstream coolingFile("cooling.csv");

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        // Splitting the line into tokens
        while (std::getline(iss, token, ' ')) { // Adjust delimiter if needed
            tokens.push_back(token);
        }

        // Check if the line has at least 3 columns (for the clock value)
        if (tokens.size() >= 3) {
            double currentClock = std::stod(tokens[2]); // Convert clock value to double

            if (hasHeatingData && (heatingEndClock < 0 || currentClock <= heatingEndClock)) {
                heatingFile << line << std::endl;
            }
            if (hasCoolingData && (coolingStartClock < 0 || currentClock >= coolingStartClock)) {
                coolingFile << line << std::endl;
            }
        }
    }

    inFile.close();
    heatingFile.close();
    coolingFile.close();

    return 0;
}
