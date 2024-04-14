#include "WeightProcessor.h"

// std includes
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
    vector<float> lc_sig_vec;
    
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
    processor.setTime();
    processor.setUnpadInt(processor.calculateUnpadInt(10,processor.getWeightSig()));

    // Close the file
    file.close();
    
    // create output
    pair<string,float> out(title,weight);

    delete [] lc_sig;

    return out;
}

void write_signal(float* sig, string sig_file, WeightProcessor& processor,float weight, string title, string filt_type){
    
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
    string get_directory = "test_signals/real_sim/sig_files";
    string save_directory = "results/real_sim/sig_files";

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
        // read in the signal
        WeightProcessor processor = WeightProcessor();
        pair<string,float> meta = read_sig(sig_path,processor);

        // save path init
        string save_path;

        /// MOVING AVERAGE FILTER ////
        save_path = save_directory + "/" + path_to_name(sig_path) + "_move_ave.txt";
        float* moving = processor.movingAverage(7,processor.getWeightSig());
        write_signal(moving,save_path,processor,meta.second,meta.first,"Moving Average");       
        cout << path_to_name(sig_path) << endl;
        delete [] moving;

        /// EXXP MOVING AVERAGE FILTER ////
        save_path = save_directory + "/" + path_to_name(sig_path) + "_exp_move_ave.txt";
        float* exp_moving = processor.expMovingAverage(7,processor.getWeightSig());
        write_signal(exp_moving,save_path,processor,meta.second,meta.first,"Exp Moving Average");       
        cout << path_to_name(sig_path) << endl;
        delete [] exp_moving;

        /// VELOCITY CALCULATOR ////
        save_path = save_directory + "/" + path_to_name(sig_path) + "_velocity.txt";
        float* velocity = processor.forceToVertVelocity();
        write_signal(velocity,save_path,processor,meta.second,meta.first,"Velocity");       
        cout << path_to_name(sig_path) << endl;
        delete [] velocity;
    }

    // plot the results using python
    system("python3 results/plot_res.py");

    return 0;
}