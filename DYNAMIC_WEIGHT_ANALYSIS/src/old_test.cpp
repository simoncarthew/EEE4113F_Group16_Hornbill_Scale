#include "WeightProcessor.h"
#include "WP_Utility.cpp"

// std includes
#include <string>
#include <vector>
#include <filesystem>

using namespace std;

int main(){
    // path to signals
    string get_directory = "test_signals/real_sim/sig_files";
    string save_directory = "test_results/real_sim/sig_files";

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
        vector<float>* moving = processor.movingAverage(7,processor.getWeightSig());
        write_signal(moving,save_path,processor,meta.second,meta.first,"Moving Average");       
        cout << path_to_name(sig_path) << endl;
        delete moving;
        // cout << "got here" << endl;

        /// EXP MOVING AVERAGE FILTER ////
        save_path = save_directory + "/" + path_to_name(sig_path) + "_exp_move_ave.txt";
        vector<float>* exp_moving = processor.expMovingAverage(7,processor.getWeightSig());
        write_signal(exp_moving,save_path,processor,meta.second,meta.first,"Exp Moving Average");       
        cout << path_to_name(sig_path) << endl;
        delete exp_moving;

        /// VELOCITY CALCULATOR ////
        // save_path = save_directory + "/" + path_to_name(sig_path) + "_velocity.txt";
        // float* velocity = processor.forceToVertVelocity();
        // write_signal(velocity,save_path,processor,meta.second,meta.first,"Velocity");       
        // cout << path_to_name(sig_path) << endl;
        // delete [] velocity;
    }

    return 0;
}