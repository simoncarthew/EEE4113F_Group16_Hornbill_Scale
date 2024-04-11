#include "WeightProcessor.h"

// std includes
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

using namespace std;

pair<string,double> read_sig(string sig_file, WeightProcessor& processor){
    // initialise variables
    string title;
    int weight;
    int samp_rate;
    double scale;
    double offset;
    vector<double> lc_sig_vec;
    
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
            }else if (word == "Weight"){
                weight = stoi(line.substr(pos + 1));
            }else if (word == "Offset"){
                offset = stod(line.substr(pos + 1));
            }else if (word == "Scale"){
                scale = stod(line.substr(pos + 1));
            }
        }else{ // save lc sig value to vector
            lc_sig_vec.push_back(stod(line.substr(pos + 1)));
        }
    }

    // transfer the values in vector to dynamic array
    long* lc_sig = new long[lc_sig_vec.size()];
    for(int i = 0; i < lc_sig_vec.size(); i++){
        lc_sig[i] = lc_sig_vec[i];
    }
    
    // asssign values to weight processor
    processor.setSamplingRate(samp_rate);
    processor.setScale(scale);
    processor.setOffset(offset);
    processor.setLength(lc_sig_vec.size());
    processor.setForceWeight(lc_sig);

    // Close the file
    file.close();
    
    // create output
    pair<string,double> out(title,weight);

    delete [] lc_sig;

    return out;
}

void write_signal(double* sig, string sig_file, WeightProcessor& processor,double weight, string title){
    
    // open the input file
    std::ofstream file(sig_file);
    
    // check if the file opened successfully
    if (!file) {
        std::cerr << "Error opening file." << std::endl;
        exit(1);
    }
    
    // write the paramters
    file << "Title: " + title + " filtered\n";
    file << "Sampling Frequency: " + to_string(processor.getSamplingRate()) + "\n";
    file << "Weight: " + to_string(weight) + "\n";

    // get the signal values and print them
    for(int i = 0; i < processor.getLength(); i++){
        file << to_string(sig[i]) << "\n";
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

int main(){
    // path to signals
    string get_directory = "test_signals/real_sim";
    string save_directory = "results/filtered_sigs";

    // ensure directory to signals exists
    if (!filesystem::exists(get_directory)) {
        std::cerr << "Error: Directory does not exist." << std::endl;
        return 1;
    }

    // get signal paths
    vector<string> sig_paths;
    for (const auto& entry : filesystem::directory_iterator(get_directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            // Print the path of the .txt file
            sig_paths.push_back(entry.path());
        }
    }

    // interate over signals
    for(string sig_path : sig_paths){
        // read in teh signal
        WeightProcessor processor = WeightProcessor();
        pair<string,double> meta = read_sig(sig_path,processor);

        /// FILTER ////

        // write the signal
        string save_path = save_directory + "/" + path_to_name(sig_path) + "_filtered.txt";
        write_signal(processor.getWeightSig(),save_path,processor,meta.second,meta.first);       
        cout << path_to_name(sig_path) << endl;
    }

    return 0;
}