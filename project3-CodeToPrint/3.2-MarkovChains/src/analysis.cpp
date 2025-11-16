#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

std::string cfg_dictPath = "";
std::string cfg_analysisOrderPath = "";
std::string cfg_modelOutputPath = "";

std::unordered_set<std::string> dictionary;
std::unordered_map<std::string, std::vector<std::pair<std::string, long>>> relations;

bool isInDictionary(std::string token) {
    return dictionary.count(token) == 1;
}

void relate(std::string leftToken, std::string rightToken) {
    if(leftToken == rightToken) return;
    // iterate through the leftToken's set of pairs.
    // if the rightToken is found, iterate the counter.
    // if not, add it.
    std::vector<std::pair<std::string, long>>& pairSet = relations[leftToken];
    // if it's found, iterate
    for(auto& pair : pairSet) {
        if(pair.first == rightToken) {
            pair.second++;
            return;
        }
    }

    // not found, put it in.
    
    relations[leftToken].push_back(std::pair<std::string, long>(rightToken, 1));
}

void writeRelations(std::string path) {
    std::ofstream outFile(path);
    if(!outFile.good()) {
        std::cerr << "Failed to open file '" + path + "' for writing." << std::endl;
        return;
    }

    for(auto[token, relVector] : relations) {
        for(auto pair : relVector) {
            outFile << token << " " << pair.first << " " << pair.second << std::endl;
        }
    }

    outFile.close();
}

void clean(std::string& token) {
    auto it = token.end() - 1;
    while(it != token.begin() - 1 && !isalnum(*it)) {
        token.erase(it);
        it--;
    }
    
    if(!token.empty()) {
        auto b = token.begin();
        while(it != token.end() && !isalnum(*b)) {
            token.erase(b);
            b++;
        }
    }

    for(auto i = token.begin(); i != token.end(); i++) {
        *i = tolower(*i);
    }
}

void analyzeFile(std::string path) {
    std::ifstream document(path);
    if(!document.good()) {
        std::cerr << "Failed to open document at path '" << path << "'." << std::endl;
        return;
    }

    // break up the file into tokens
    std::vector<std::string> tokens;
    while(!document.eof()) {
        std::string token;
        document >> token;
        clean(token);
        if(isInDictionary(token)) {
            tokens.push_back(token);
        }
    }

    // analyze
    for(auto current = tokens.begin(); current != tokens.end() - 1; current++) { // end() - 1 because we're gonna look at one after.
        relate(*current, *(current + 1));
    }

    document.close();

    std::cout << "[SUCCESS] " << path << " (" << tokens.size() << " tokens)" << std::endl;
    
    return;
}

void printUsage() {
    std::cout 
    << "Markov chain generator" << std::endl
    << "Usage:" << std::endl
        << "\t--out <path>" << std::endl
            << "\t\tpath to write the final model file to." << std::endl
            << "\t\tdefault: analysis.model" << std::endl
        << "\t--dictionary <path>" << std::endl
        << "\t--dataset <path>" << std::endl
            << "\t\tpath to file containing paths to the documents included in the dataset." << std::endl;
}

// Arguments:
//  --out <path> (default: analysis.model)
//  --dictionary <path> (required)
//  --dataset <path> (required)
//  --help
void parseArguments(int argc, char* argv[]) {
    if(argc == 1) {
        printUsage();
        exit(1);
    }
    for(int i = 1; i < argc; i++) {
        std::string currentFlag = std::string(argv[i]);
        if(currentFlag == "--help") {
            printUsage();
            exit(0);
        } else if(currentFlag == "--out") {
            if(i + 1 >= argc) {
                std::cerr << "ERROR: --out must be followed by a path." << std::endl;
            }
            cfg_modelOutputPath = argv[i+1];
            i++;
        } else if(currentFlag == "--dictionary") {
            if(i + 1 >= argc) {
                std::cerr << "ERROR: --dictionary must be followed by a path." << std::endl;
            }
            cfg_dictPath = argv[i+1];
            i++;
        } else if(currentFlag == "--dataset") {
            if(i + 1 >= argc) {
                std::cerr << "ERROR: --dataset must be followed by a path." << std::endl;
            }
            cfg_analysisOrderPath = argv[i+1];
            i++;
        }
    }
}

int main(int argc, char* argv[]) {
    parseArguments(argc, argv);

    // Check required arguments.
    if(cfg_dictPath.empty() || cfg_analysisOrderPath.empty()) {
        std::cerr << "ERROR: --dictionary and --dataset must be defined as arguments." << std::endl;
        return 1;
    }

    // Open the dictionary
    std::ifstream dictFile(cfg_dictPath);
    if(!dictFile.good()) {
        std::cerr << "Failed to open dictionary at '" << cfg_dictPath << "'." << std::endl;
        return 1;
    }

    // Read the dictionary in
    while(!dictFile.eof()) {
        std::string x;
        dictFile >> x;
        for(char& c : x) {
            c = tolower(c);
        }
        dictionary.insert(x);
        
    }

    if(isInDictionary("")) {
        dictionary.erase("");
    }
    /*
    for(std::string x : dictionary) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    */

    // Begin reading in and analyzing real bodies of text
    std::ifstream readerList(cfg_analysisOrderPath);
    if(!readerList.good()) {
        std::cerr << "Failed to open file at path '" << cfg_analysisOrderPath << "' for reading." << std::endl;
        return 1;
    }

    std::string assetPath;
    while(std::getline(readerList, assetPath)) {
        if(assetPath.at(assetPath.length() - 1) == '\n') {
            assetPath.erase(assetPath.end() - 1);
        }
        analyzeFile(assetPath);
    }

    readerList.close();

    if(cfg_modelOutputPath.empty()) {
        std::cerr << "WARNING: model output path not defined, defaulting to ./analysis.model" << std::endl;
        cfg_modelOutputPath = "./analysis.model";
    }

    writeRelations(cfg_modelOutputPath);

    dictFile.close();

    return 0;
}