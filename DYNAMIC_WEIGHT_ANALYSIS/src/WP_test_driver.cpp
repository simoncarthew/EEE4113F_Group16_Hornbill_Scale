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
    string result_path = "test_results/real_sim/sig_files";

    // initialize results set
    vector<vector<string>> results = {{"filter_type","filter_parameter","ave_perc_error"}};

    // ensure directory to signals exists
    if (!filesystem::exists(get_directory)) {
        std::cerr << "Error: Directory does not exist." << std::endl;
        return 1;
    }

    // tested parameters
    vector<string> win_filters = {"mov_ave","exp_mov_ave","med", "whit", "kal"};
    vector<string> non_win_filters = {"","kal"};
    string whit = "whit";
    vector<int> windows = {4,5,6,7,8,9,10};
    vector<float> alphas = {0.5,1,5,10,20};

    // get signal paths
    vector<string> sig_paths;
    for (const auto& entry : filesystem::directory_iterator(get_directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            sig_paths.push_back(entry.path());
        }
    }

    // read in all signals
    for (string filt_type : win_filters){ // iterate over window filters
        for (int window : windows){ // iterate over window files
            for(string sig_path : sig_paths){ // interate over signals
                // initialize weight processor and read in signal
                WeightProcessor processor = WeightProcessor();
                read_sig(sig_path,processor);

                // 
            }
        }
    }

    

    return 0;
}