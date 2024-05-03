#include "WeightProcessor.h"
#include "WP_Utility.cpp"

// std includes
#include <string>
#include <vector>
#include <filesystem>

using namespace std;

// utility functions
void writeVectorToFile(const std::vector<float>& vec, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& value : vec) {
            file << value << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

void writeValidationsToFile(const std::vector<std::string>& strings, const std::vector<int>& ints, const std::string& filename) {
    std::ofstream file(filename, std::ios::trunc);
    if (file.is_open()) {
        // Make sure both vectors have the same size
        size_t size = std::min(strings.size(), ints.size());
        for (size_t i = 0; i < size; ++i) {
            file << strings[i] << "," << ints[i] << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

void writeEstimationsToFile(float estimation, float perc_error, int start, int end, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << std::fixed << std::setprecision(6) << estimation << std::endl;
        file << std::fixed << std::setprecision(6) << perc_error << std::endl;
        file << start << std::endl;
        file << end << std::endl;
        file.close();
    } else {
        std::cerr << "Unable to open file." << std::endl;
    }
}

// sim functions
void conversion(){
    // read in the signal
    WeightProcessor processor = WeightProcessor();
    read_sig("simulation/sig_files/conversion.txt",processor);
    
    // get the converted force and weight signal
    vector<float>* force = processor.getForceSig();
    vector<float>* weight = processor.getWeightSig();

    // write converted signals to output
    writeVectorToFile(*force, "simulation/outputs/conv_force.txt");
    writeVectorToFile(*weight, "simulation/outputs/conv_weight.txt");
}

void validating(){
    std::vector<std::string> signal_files = {"simulation/sig_files/conversion.txt", "simulation/sig_files/invalid_len.txt", "simulation/sig_files/invalid_min.txt","simulation/sig_files/invalid_max.txt"};
    vector<int> validations;

    for(string sig_path : signal_files){
        // read in the signals
        WeightProcessor processor = WeightProcessor();
        read_sig(sig_path,processor);
        
        // get the weight signal
        vector<float>* weight = processor.getWeightSig();

        // validate weight signal
        int valid = processor.validateSignal(500,1500,0.4,1.5,weight);

        validations.push_back(valid);
    }
    
    // write outputs to a text file
    writeValidationsToFile(signal_files,validations,"simulation/outputs/validations.txt");
}

void filtering(){
    std::vector<std::string> signal_files = {"simulation/sig_files/filter_0.txt","simulation/sig_files/filter_1.txt"};

    for(string sig_path : signal_files){
        // read in the signals
        WeightProcessor processor = WeightProcessor();
        read_sig(sig_path,processor);

        // filter the signal
        vector<float>* weight = processor.getWeightSig();
        vector<float>* exp_moving = processor.expMovingAverage(7,weight);
        vector<float>* moving = processor.movingAverage(7,weight);
    
        // write outputs to a text file
        write_signal(weight,"simulation/outputs/" + path_to_name(sig_path) + "_orig" + ".txt",processor,1000," " + path_to_name(sig_path),"Original");
        write_signal(exp_moving,"simulation/outputs/" + path_to_name(sig_path) + "_exp_mov" + ".txt",processor,1000," " + path_to_name(sig_path),"Exponential Moving Average");
        write_signal(moving,"simulation/outputs/" + path_to_name(sig_path) + "_mov" + ".txt",processor,1000," " + path_to_name(sig_path),"Moving Average");

        // delete the filtered signals
        delete exp_moving;
        delete moving;
    }
}

void weight_estimate(){
    // set the path to the estimation file
    string signal_file = "simulation/sig_files/weight_est.txt";

    // create the weight processor and read in the signal
    WeightProcessor processor = WeightProcessor();
    read_sig(signal_file,processor);

    // estimate weight and get stable intervals
    pair<float,float> est = processor.estimateWeight(500, 1500, 0.4, 1.5, 0.3, "",0,true);
    pair<int,int> intervals = processor.mostStableInterval(processor.getWeightSig(),0.3);

    // write the results to a text file
    writeEstimationsToFile(est.first,est.second,intervals.first,intervals.second,"simulation/outputs/estimation.txt");

    // write estimation signal to outputs
    write_signal(processor.getWeightSig(),"simulation/outputs/est_sig.txt",processor,1000," Estimation Signal","none");
}

int main(){
    conversion();
    cout << "Conversion Simulation completed." << endl;

    validating();
    cout << "Validating Simulation completed." << endl;

    filtering();
    cout << "Filtering Simulation completed." << endl;

    weight_estimate();
    cout << "Weight Estimation Simulation completed." << endl;

    system("python3 simulation/plot_sim.py");

    return 0;
}