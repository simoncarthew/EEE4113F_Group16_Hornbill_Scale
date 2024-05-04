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
            file << path_to_name(strings[i]) << "," << ints[i] << std::endl;
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

void writeFullSystemToFile(const std::vector<std::string>& strings, const std::vector<int>& floats, const std::string& filename) {
    std::ofstream file(filename, std::ios::trunc);
    if (file.is_open()) {
        // Make sure both vectors have the same size
        size_t size = std::min(strings.size(), floats.size());
        for (size_t i = 0; i < size; ++i) {
            file << path_to_name(strings[i]) << "," << std::fixed << std::setprecision(6) << floats[i] << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

void validating(){
    std::vector<std::string> signal_files = {"simulation/sig_files/conversion.txt", "simulation/sig_files/invalid_len.txt", "simulation/sig_files/invalid_min.txt","simulation/sig_files/invalid_max.txt"};
    vector<int> validations;

    for(string sig_path : signal_files){
        // read in the signals
        WeightProcessor processor = WeightProcessor();
        pair<string,float> meta = read_sig(sig_path,processor);
        
        // get the weight signal
        vector<float>* weight = processor.getWeightSig();

        // validate weight signal
        int valid = processor.validateSignal(500,1500,0.4,1.5,weight);

        validations.push_back(valid);

        write_signal(weight,"simulation/outputs/" + path_to_name(sig_path) + "_val.txt",processor,-1," " + meta.first,"");
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
        vector<float>* median = processor.medianFilter(10,weight);
        vector<float>* whit = processor.whitEielers(40,40,weight);
        vector<float>* kal = processor.kalmanFilter(weight);
    
        // write outputs to a text file
        write_signal(weight,"simulation/outputs/" + path_to_name(sig_path) + "_orig" + ".txt",processor,1000," " + path_to_name(sig_path),"Original");
        write_signal(exp_moving,"simulation/outputs/" + path_to_name(sig_path) + "_exp_mov" + ".txt",processor,1000," " + path_to_name(sig_path),"Exponential Moving Average");
        write_signal(moving,"simulation/outputs/" + path_to_name(sig_path) + "_mov" + ".txt",processor,1000," " + path_to_name(sig_path),"Moving Average");
        write_signal(median,"simulation/outputs/" + path_to_name(sig_path) + "_med" + ".txt",processor,1000," " + path_to_name(sig_path),"Median");
        write_signal(whit,"simulation/outputs/" + path_to_name(sig_path) + "_whit" + ".txt",processor,1000," " + path_to_name(sig_path),"Whittaker Eilers");
        write_signal(kal,"simulation/outputs/" + path_to_name(sig_path) + "_kal" + ".txt",processor,1000," " + path_to_name(sig_path),"Kalman Filter");

        // delete the filtered signals
        delete exp_moving;
        delete moving;
        delete median;
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

void full_system(){
    // set the path to the estimation file
    string signal_file = "simulation/sig_files/full_sys_sig.txt";

    // create the processors
    WeightProcessor processor = WeightProcessor();
    read_sig(signal_file,processor);

    // estimate weight and get stable intervals
    pair<float,float> est_mov = processor.estimateWeight(500, 1500, 0.4, 1.5, 0.3, "mov_ave",7,-1,true);
    pair<float,float> est_exp_mov = processor.estimateWeight(500, 1500, 0.4, 1.5, 0.3, "exp_mov_ave",7,-1,true);
    pair<float,float> est_med = processor.estimateWeight(500, 1500, 0.4, 1.5, 0.3, "med",7,-1,true);
    pair<float,float> est_whit = processor.estimateWeight(500, 1500, 0.4, 1.5, 0.3, "whit",7,10,true);
    pair<float,float> est_kal = processor.estimateWeight(500, 1500, 0.4, 1.5, 0.3, "kal",7,-1,true);

    // get filterd signals for plotting
    vector<float>* mov = processor.movingAverage(7,processor.getWeightSig());
    vector<float>* exp_mov = processor.expMovingAverage(7,processor.getWeightSig());
    vector<float>* med = processor.medianFilter(7,processor.getWeightSig());
    vector<float>* whit = processor.whitEielers(10,40,processor.getWeightSig());
    vector<float>* kal = processor.kalmanFilter(processor.getWeightSig());

    // get the intervals
    pair<int,int> mov_intervals = processor.mostStableInterval(mov,0.3);
    pair<int,int> exp_mov_intervals = processor.mostStableInterval(exp_mov,0.3);
    pair<int,int> med_intervals = processor.mostStableInterval(med,0.3);
    pair<int,int> whit_intervals = processor.mostStableInterval(whit,0.3);
    pair<int,int> kal_intervals = processor.mostStableInterval(kal,0.3);

    // write the results to text files
    writeEstimationsToFile(est_mov.first,est_mov.second,mov_intervals.first,mov_intervals.second,"simulation/outputs/full_sys_est_mov.txt");
    writeEstimationsToFile(est_exp_mov.first,est_exp_mov.second,exp_mov_intervals.first,exp_mov_intervals.second,"simulation/outputs/full_sys_est_exp_mov.txt");
    writeEstimationsToFile(est_med.first,est_med.second,med_intervals.first,med_intervals.second,"simulation/outputs/full_sys_est_med.txt");
    writeEstimationsToFile(est_whit.first,est_whit.second,whit_intervals.first,whit_intervals.second,"simulation/outputs/full_sys_est_whit.txt");
    writeEstimationsToFile(est_kal.first,est_kal.second,kal_intervals.first,kal_intervals.second,"simulation/outputs/full_sys_est_kal.txt");

    // write signals test file
    write_signal(processor.getWeightSig(),"simulation/outputs/full_sys_orig_sig.txt",processor,1000," Original Weight Signal","Original");
    write_signal(mov,"simulation/outputs/full_sys_mov_ave_sig.txt",processor,1000," Weight Estimation with Moving Average","Moving Average Filter");
    write_signal(exp_mov,"simulation/outputs/full_sys_exp_mov_ave_sig.txt",processor,1000," Weight Estimation with Exponential Moving Average","Exponential Moving Average");
    write_signal(med,"simulation/outputs/full_sys_med_sig.txt",processor,1000," Weight Estimation with Median Filter","Median");
    write_signal(whit,"simulation/outputs/full_sys_whit_sig.txt",processor,1000," Weight Estimation with Whittaker Eilers Filter","Whittaker Eilers");
    write_signal(kal,"simulation/outputs/full_sys_kal_sig.txt",processor,1000," Weight Estimation with Kalman Filter","Kalman Filter");

    // delete filtered signals
    delete mov;
    delete exp_mov;
    delete med;
    delete kal;
    delete whit;
}

int main(){
    validating();
    cout << "Validating Simulation completed." << endl;

    filtering();
    cout << "Filtering Simulation completed." << endl;

    weight_estimate();
    cout << "Weight Estimation Simulation completed." << endl;

    full_system();
    cout << "Full System Simulation completed." << endl;

    system("python3 simulation/plot_sim.py");

    return 0;
}