#include <utility>
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>

std::unordered_map<std::string, std::vector<std::pair<std::string, long>>> relations;
std::stringstream finalString;

std::string cfg_model;
int cfg_floor = 0;

std::string cfg_prompt = "";

std::string cfg_output = "chain.txt";

std::string cfg_firstWord;
bool cfg_wantsPrune = true;
int cfg_wordsToGenerate = -1;

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
    finalString << cfg_firstWord << " ";
    std::string currentWord = cfg_firstWord;
    for(int words = 1; words < cfg_wordsToGenerate; words++) {
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
            else if(vec.at(i).second < cfg_floor) {
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

void printUsage() {
    std::cout 
    << "Markov chain generator" << std::endl
    << "Usage:" << std::endl
        << "\t--firstWord <word>" << std::endl
        << "\t--model <path>" << std::endl
        << "\t--no-prune" << std::endl
        << "\t--floor <int >= 0>" << std::endl
        << "\t--prompt <string>" << std::endl
        << "\t--out <path>" << std::endl
        << "\t--help" << std::endl;
}

// Arguments:
//  --firstWord <word> (required)
//  --model <path> (required)
//  --length <int>
//  --no-prune
//  --floor <int>
//  --prompt <prompt>
//  --out <path>
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
        } else if(currentFlag == "--firstWord") {
            if(i + 1 >= argc) {
                std::cerr << "ERROR: --firstWord flag must be followed by an argument." << std::endl;
                exit(1);
            }
            cfg_firstWord = argv[i+1];
            i++;
        } else if(currentFlag == "--no-prune") {
            cfg_wantsPrune = false;
        } else if(currentFlag == "--floor") {
            if(i + 1 >= argc) {
                std::cerr << "ERROR: --floor flag must be followed by an integer." << std::endl;
                exit(1);
            }
            try {
                cfg_floor = std::stoi(std::string(argv[i+1]));
            } catch(std::invalid_argument err) {
                std::cerr << "ERROR: --floor flag argument is not an integer." << std::endl;
                exit(1);
            } catch(std::out_of_range oErr) {
                std::cerr << "ERROR: --floor flag argument is too big." << std::endl;
                exit(1);
            }
            i++;
        } else if(currentFlag == "--prompt") {
            if(i + 1 >= argc) {
                std::cerr << "ERROR: --prompt flag must be followed by a string." << std::endl;
                exit(1);
            }

            cfg_prompt = std::string(argv[i+1]);
            i++;
        } else if(currentFlag == "--out") {
            if(i + 1 >= argc) {
                std::cerr << "ERROR: --out flag must be followed by a string." << std::endl;
                exit(1);
            }

            cfg_output = std::string(argv[i+1]);
            i++;
        } else if(currentFlag == "--model") {
            if(i + 1 >= argc) {
                std::cerr << "ERROR: --model flag must be followed by a path leading to the model file." << std::endl;
                exit(1);
            }

            cfg_model = std::string(argv[i+1]);
            i++;
        } else if(currentFlag == "--length") {
            if(i + 1 >= argc) {
                std::cerr << "ERROR: --wordsToGenerate flag must be followed by an integer." << std::endl;
                exit(1);
            }
            try {
                cfg_wordsToGenerate = std::stoi(std::string(argv[i+1]));
            } catch(std::invalid_argument err) {
                std::cerr << "ERROR: --wordsToGenerate flag argument is not an integer." << std::endl;
                exit(1);
            } catch(std::out_of_range oErr) {
                std::cerr << "ERROR: --wordsToGenerate flag argument is too big." << std::endl;
                exit(1);
            }
            i++;
        } else {
            std::cerr << "WARNING: unrecognized flag '" << currentFlag << "'. Ignoring." << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    // Parse CLI arguments
    parseArguments(argc, argv);

    // Check required values
    if(cfg_firstWord.empty() || cfg_model.empty() || cfg_wordsToGenerate == -1) {
        std::cerr << "ERROR: --firstWord, --model, & --length flags are required to be defined." << std::endl;
        exit(1);
    }

    // Read in the relations from the provided model
    std::ifstream model(cfg_model);
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
    std::cout << "Got model";

    if(cfg_wantsPrune) {
        std::cout << ", pruning..." << std::endl;
        pruneForFeasibility();
    } else {
        std::cout << "." << std::endl;
    }

    // Seed the random number generator based on a prompt
    std::cout << "Seeding with prompt.." << std::endl;

    seed(cfg_prompt);

    // Create the chain
    std::cout << "Creating chain..." << std::endl;

    if(relations.count(cfg_firstWord) == 0) {
        std::cerr << "'" << cfg_firstWord << "' does not appear in the model." << std::endl;
        return 1;
    }

    auto tStart = std::chrono::system_clock::now();
    createString();
    auto tEnd = std::chrono::system_clock::now();

    auto duration = std::chrono::duration<double>(tEnd - tStart);
    std::cout << "Finished in " << duration.count() << "s." << std::endl;

    model.close();

    // Write the chain to a file.
    std::ofstream out(cfg_output);
    if(!out.good()) {
        std::cerr << "Failed to write to path '" << cfg_output << "'." << std::endl;
        return 1;
    }
    out << finalString.str() << std::endl;

    out.close();

    return 0;
}