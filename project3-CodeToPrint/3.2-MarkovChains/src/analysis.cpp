#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

#define DICTIONARY_PATH "../asset/dictionary.txt"
const std::string ASSET_PATH_PREFIX = "../asset/";
const std::string CONFIG_PATH_PREFIX ="../configuration/";

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

int main(int argc, char* argv[]) {
    // Open the dictionary
    std::ifstream dictFile(DICTIONARY_PATH);
    if(!dictFile.good()) {
        std::cerr << "Failed to open dictionary at '" << DICTIONARY_PATH << "'." << std::endl;
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
    std::ifstream readerList(CONFIG_PATH_PREFIX + "filesToAnalyze.txt");
    if(!readerList.good()) {
        std::cerr << "Failed to open file at path '" << CONFIG_PATH_PREFIX << "/filesToAnalyze.txt' for reading." << std::endl;
        return 1;
    }

    std::string suffix;
    while(std::getline(readerList, suffix)) {
        if(suffix.at(suffix.length() - 1) == '\n') {
            suffix.erase(suffix.end() - 1);
        }
        std::string path = ASSET_PATH_PREFIX + suffix;
        analyzeFile(path);
    }

    readerList.close();

    writeRelations("../out/analysis.model");

    dictFile.close();

    return 0;
}