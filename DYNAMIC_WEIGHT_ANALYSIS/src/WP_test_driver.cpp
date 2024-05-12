#include "WeightProcessor.h"
#include "WP_Utility.cpp"

// std includes
#include <string>
#include <vector>
#include <filesystem>

using namespace std;

void writeResults(const std::vector<std::vector<std::string>>& data, const std::string& filename) {
    std::ofstream outfile(filename);

    if (!outfile.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
        return;
    }

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            outfile << row[i];
            if (i < row.size() - 1) {
                outfile << ',';
            }
        }
        outfile << std::endl;
    }

    outfile.close();
}

int main(){
    // path to signals
    string get_directory = "test_signals/real_sim/sig_files";
    string result_path = "test_results/filter_testing.csv";

    // initialize results set
    vector<vector<string>> results = {{"filter_type","filter_parameter", "stable_period", "average_est","ave_perc_error"}};

    // ensure directory to signals exists
    if (!filesystem::exists(get_directory)) {
        std::cerr << "Error: Directory does not exist." << std::endl;
        return 1;
    }

    // tested parameters
    vector<string> win_filters = {"mov_ave","exp_mov_ave","med"};
    vector<string> non_win_filters = {"","kal"};
    string whit = "whit";
    vector<int> windows = {4,5,6,7,8,9,10};
    vector<float> lambdas = {0.5,1,5,10,20};
    vector<float> stable_periods = {0.2,0.3,0.4};

    // get signal paths
    vector<string> sig_paths;
    for (const auto& entry : filesystem::directory_iterator(get_directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            sig_paths.push_back(entry.path());
        }
    }

    // test window filters
    for (string filt_type : win_filters){ // iterate over window filters
        for (float stable_period : stable_periods){
            for (int window : windows){ // iterate over window files
                float total_perc_error = 0.0f;
                float num_valid_signals = 0.0f;
                float total_est_weight = 0.0f;
                for(string sig_path : sig_paths){ // interate over signals
                    // initialize weight processor and read in signal
                    WeightProcessor processor = WeightProcessor();
                    read_sig(sig_path,processor);

                    // make the weight estimation
                    pair<float,float> est = processor.estimateWeight(500,1500,0.4,1.5,stable_period,filt_type,window,0,true);

                    // if valid signal record results
                    if (est.first > 0.0){
                        total_perc_error += abs((est.first-1000)/1000) * 100;
                        num_valid_signals += 1.0;
                        total_est_weight += est.first;
                    }
                }
                // calculate the averge percentage error
                // float ave_perc_error = total_perc_error/num_valid_signals;
                float ave_est = total_est_weight/num_valid_signals;
                float ave_perc_error = abs((ave_est-1000)/1000) * 100;

                // save results to the results set
                vector<string> res = {filt_type, to_string(window) ,to_string(stable_period).substr(0, 3), to_string(ave_est), to_string(ave_perc_error)};
                results.push_back(res);
            }
        }
    }

    // test non-window filters
    for (string filt_type : non_win_filters){ // iterate over window filters
        for (float stable_period : stable_periods){
                float total_perc_error = 0.0f;
                float num_valid_signals = 0.0f;
                float total_est_weight = 0.0f;
                for(string sig_path : sig_paths){ // interate over signals
                    // initialize weight processor and read in signal
                    WeightProcessor processor = WeightProcessor();
                    read_sig(sig_path,processor);

                    // make the weight estimation
                    pair<float,float> est = processor.estimateWeight(500,1500,0.4,1.5,stable_period,filt_type,0,0,true);

                    // if valid signal record results
                    if (est.first > 0.0){
                        total_perc_error += abs((est.first-1000)/1000) * 100;
                        num_valid_signals += 1.0;
                        total_est_weight += est.first;
                    }
                }
                // calculate the averge percentage error
                // float ave_perc_error = total_perc_error/num_valid_signals;
                float ave_est = total_est_weight/num_valid_signals;
                float ave_perc_error = abs((ave_est-1000)/1000) * 100;

                // save results to the results set
                string filt_out_type=filt_type;
                if (filt_type == ""){filt_out_type = "none";}
                vector<string> res = {filt_out_type, "none" ,to_string(stable_period).substr(0, 4), to_string(ave_est), to_string(ave_perc_error)};
                results.push_back(res);
        }
    }

    // test whitaker
    for (float stable_period : stable_periods){
        for (float lambda : lambdas){ // iterate over window files
            float total_perc_error = 0.0f;
            float total_est_weight = 0.0f;
            float num_valid_signals = 0.0f;
            for(string sig_path : sig_paths){ // interate over signals
                // initialize weight processor and read in signal
                WeightProcessor processor = WeightProcessor();
                read_sig(sig_path,processor);

                // make the weight estimation
                pair<float,float> est = processor.estimateWeight(500,1500,0.4,1.5,stable_period,whit,0,lambda,true);

                // if valid signal record results
                if (est.first > 0.0){
                    total_perc_error += abs((est.first-1000)/1000) * 100;
                    total_est_weight += est.first;
                    num_valid_signals += 1.0;
                }
            }
            // calculate the averge percentage error
            // float ave_perc_error = total_perc_error/num_valid_signals;
            float ave_est = total_est_weight/num_valid_signals;
            float ave_perc_error = abs((ave_est-1000)/1000) * 100;

            // save results to the results set
            vector<string> res = {whit, to_string(lambda).substr(0,4) ,to_string(stable_period).substr(0, 4), to_string(ave_est), to_string(ave_perc_error)};
            results.push_back(res);
        }
    }
    
    writeResults(results,result_path);

    system("python3 test_results/sort_res.py");

    return 0;
}