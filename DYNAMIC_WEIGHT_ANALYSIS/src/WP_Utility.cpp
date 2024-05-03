#include "WeightProcessor.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

using namespace std;

pair<string,float> read_sig(string sig_file, WeightProcessor& processor){
    // initialise variables
    string title;
    int weight;
    int samp_rate;
    float scale;
    float offset;
    vector<long>* lc_sig_vec = new vector<long>;
    
    // open the input file
    std::ifstream file(sig_file);
    
    // check if the file opened successfully
    if (!file) {
        std::cerr << "Error opening file." << std::endl;
        exit(1);
    }
    
    // read file line by line
    std::string line;
    while (getline(file, line)) {
        // find the pos of ":"
        size_t pos = line.find(":");
        
        // if the ":" is in the line then check what the word is
        if (pos != std::string::npos){
            string word = line.substr(0, pos);

            if (word == "Title"){  
                title = line.substr(pos + 1);
            }else if (word == "Sampling Frequency"){
                samp_rate = stoi(line.substr(pos + 1));
            }else if (word == "Weight") {
                try {
                    weight = stoi(line.substr(pos + 1));
                } catch (...) {
                    weight = -1;
                }
            }else if (word == "Offset"){
                offset = stod(line.substr(pos + 1));
            }else if (word == "Scale"){
                scale = stod(line.substr(pos + 1));
            }
        }else{ // save lc sig value to vector
            lc_sig_vec->push_back(stod(line.substr(pos + 1)));
        }
    }
    
    // asssign values to weight processor
    processor.setSamplingRate(samp_rate);
    processor.setScale(scale);
    processor.setOffset(offset);
    processor.setLength(lc_sig_vec->size());
    processor.setForceWeight(lc_sig_vec);
    processor.setTime();
    processor.setUnpadInt(processor.calculateUnpadInt(25,processor.getWeightSig()));

    // Close the file
    file.close();
    
    // create output
    pair<string,float> out(title,weight);

    delete lc_sig_vec;

    return out;
}

void write_signal(vector<float>* sig, string sig_file, WeightProcessor& processor,float weight, string title, string filt_type){
    
    // open the input file
    std::ofstream file(sig_file);
    
    // check if the file opened successfully
    if (!file) {
        std::cerr << "Error opening file." << std::endl;
        exit(1);
    }
    
    // write the paramters
    file << "Title:" + title + "\n";
    file << "Sampling Frequency: " + to_string(processor.getSamplingRate()) + "\n";
    file << "Filter Type: " + filt_type << "\n";
    file << "Weight: " + to_string(weight) + "\n";

    // get the signal values and print them
    for(int i = 0; i < processor.getLength(); i++){
        file << to_string((*sig)[i]) << "\n";
    }

    // Close the file
    file.close();
}

string path_to_name(string path){
    size_t lastSlashPos = path.find_last_of('/');
    string fileName = path.substr(lastSlashPos + 1);
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos) {
        fileName = fileName.substr(0, dotPos);
    }
    return fileName;
}