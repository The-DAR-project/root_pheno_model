#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <string>

int main(int argc, char* argv[]) {
    std::string inputFile, outputFile;
    std::set<int> columnsToExport;
    int startOffset = 1; // Default value
    int cutoffIndex = -1; // Default value, -1 signifies no cutoff

    // Parsing command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-i") {
            if (i + 1 < argc) { inputFile = argv[++i]; }
            else { std::cerr << "No input file specified after -i\n"; return 1; }
        }
        else if (arg == "-o") {
            if (i + 1 < argc) { outputFile = argv[++i]; }
            else { std::cerr << "No output file specified after -o\n"; return 1; }
        }
        else if (arg == "-c") {
            if (i + 1 < argc) {
                std::istringstream iss(argv[++i]);
                std::string index;
                while (std::getline(iss, index, ',')) {
                    columnsToExport.insert(std::stoi(index));
                }
            }
            else { std::cerr << "No column indices specified after -c\n"; return 1; }
        }
        // Start index
        else if (arg == "-si") {
            if (i + 1 < argc) { startOffset = std::stoi(argv[++i]); }
            else { std::cerr << "No start offset specified after -si\n"; return 1; }
        }
        // Cutoff index
        else if (arg == "-ci") {
            if (i + 1 < argc) { cutoffIndex = std::stoi(argv[++i]); }
            else { std::cerr << "No cutoff index specified after -ci\n"; return 1; }
        }
    }

    // Check if required arguments are provided
    if (inputFile.empty() || outputFile.empty() || columnsToExport.empty()) {
        std::cerr << "Usage: " << argv[0] << " -i <input_file> -o <output_file> -c <column_indices> [--start <offset>] [--cutoff <index>]\n";
        return 1;
    }

    std::ifstream inFile(inputFile);
    if (!inFile) {
        std::cerr << "Error: Unable to open input file " << inputFile << std::endl;
        return 1;
    }

    std::ofstream outFile(outputFile);
    if (!outFile) {
        std::cerr << "Error: Unable to open output file " << outputFile << std::endl;
        return 1;
    }

    std::string line;
    int currentLine = 0;
    while (std::getline(inFile, line)) {
        if (currentLine >= startOffset && (cutoffIndex == -1 || currentLine <= cutoffIndex)) {
            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::string token;

            while (std::getline(iss, token, ' ')) { // Adjust delimiter if needed
                tokens.push_back(token);
            }

            bool firstColumn = true;
            for (int i = 0; i < tokens.size(); ++i) {
                if (columnsToExport.find(i) != columnsToExport.end()) {
                    if (!firstColumn) {
                        outFile << ",";
                    }
                    outFile << tokens[i];
                    firstColumn = false;
                }
            }
            outFile << std::endl;
        }
        currentLine++;
        if (cutoffIndex != -1 && currentLine > cutoffIndex) {
            break;
        }
    }

    inFile.close();
    outFile.close();

    return 0;
}
