#include <utility>
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>

std::unordered_map<std::string, std::vector<std::pair<std::string, long>>> relations;
std::stringstream finalString;

#define MODEL_PATH "../out/analysis.model"
#define MODEL_FEASIBILITY_FLOOR 5

#define PROMPT_PATH "../configuration/prompt.txt"

#define OUTPUT_PATH "../out/chain.txt"

#define FIRST_WORD "i"
#define WORDS_TO_GENERATE 210

void loadRelation(std::string line) {
    std::stringstream lineStream(line);
    std::string key, value, instances;

    lineStream >> key;
    lineStream >> value;
    lineStream >> instances;

    long ins = atol(instances.c_str());

    relations[key].push_back(std::pair<std::string, long>(value, ins));
}

long countTotalRelationOccurances(std::string word) {
    if(relations.count(word) == 0) return 0;

    std::vector<std::pair<std::string, long>> relationVector = relations[word];
    long count = 0;
    
    for(auto pairing : relationVector) {
        count += pairing.second;
    }

    return count;
}

void nextWord(std::string left, std::string& right) {
    // get the word's relation vector

    std::random_device rd;
    std::mt19937 generator(rd());

    std::vector<std::pair<std::string, long>> relationVector;
    if(relations.count(left) == 1) {
        relationVector = relations[left];
    } else {
        // TODO: do this
        relationVector = relations[left];
    }

    // get the word associations and give it a rounded percentage
    long denom = countTotalRelationOccurances(left);
    if(denom == 0) return; // TODO: handle this better

    double rand = std::generate_canonical<double, sizeof(int) * 2>(generator);
    long dist = (long)(rand * (double)denom);

    long x = 0;
    std::string lp;
    for(auto[k,v] : relationVector) {
        x += v;
        lp = k;
        if(x >= dist) { // we found our word!
            right = k;
            return;
        }
    }
    right = lp;
    
}

void createString() {
    finalString << FIRST_WORD << " ";
    std::string currentWord = FIRST_WORD;
    for(int words = 1; words < WORDS_TO_GENERATE; words++) {
        std::string nwOutput;
        nextWord(currentWord, nwOutput);
        finalString << nwOutput << " ";
        currentWord = nwOutput;
    }
}

void pruneForFeasibility() {
    int count = 0;
    for(auto[k,vec] : relations) {
        for(size_t i = 0; i < vec.size(); i++) {
            if(vec.at(i).first == k) {
                std::cout << "removed duplicate " << k << std::endl;
                vec.erase(vec.begin() + i);
                i--;
                count++;
            }
            else if(vec.at(i).second < MODEL_FEASIBILITY_FLOOR) {
                vec.erase(vec.begin() + i);
                i--;
                count++;
            }
        }
    }
    std::cout << "Pruned " << count << " relations." << std::endl;
}

void seed(std::string usingStr) {
    unsigned col = 0;
    int count = 0;
    for(char c : usingStr) {
        col += count % 5 == 0 ? -c : c;
        count++;
    }

    std::srand(col);
}

int main() {
    // Read in the relations from the provided model
    std::ifstream model(MODEL_PATH);
    if(!model.good()) {
        std::cerr << "Failed to open the model." << std::endl;
        return 1;
    }

    std::string line;
    while(getline(model, line)) {
        if(line.at(line.length() - 1) == '\n') {
            line.erase(line.end() - 1);
        }

        loadRelation(line);
    }

    // Prune the model based on a floor of relations
    std::cout << "Got model, pruning..." << std::endl;

    pruneForFeasibility();

    // Seed the random number generator based on a prompt
    std::cout << "Loading prompt..." << std::endl;

    std::ifstream promptFile(PROMPT_PATH);
    if(!promptFile.good()) {
        std::cerr << "WARNING: couldn't open prompt file. proceeding without a prompt." << std::endl;
    }

    std::stringstream prompt;
    std::string pl;
    while(std::getline(promptFile, pl)) {
        prompt << pl;
    }

    seed(prompt.str());

    // Create the chain
    std::cout << "Creating chain..." << std::endl;

    if(relations.count(FIRST_WORD) == 0) {
        std::cerr << "'" << FIRST_WORD << "' does not appear in the model." << std::endl;
        return 1;
    }

    createString();

    model.close();

    // Write the chain to a file.
    std::ofstream out(OUTPUT_PATH);
    if(!out.good()) {
        std::cerr << "Failed to write to path '" << OUTPUT_PATH << "'." << std::endl;
        return 1;
    }
    out << finalString.str() << std::endl;

    out.close();

    return 0;
}