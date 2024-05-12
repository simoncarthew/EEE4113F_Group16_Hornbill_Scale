#include "WeightProcessor.h"

#include <string>
#include <iostream>
#include <cmath>

using namespace std;

// CONSTRUCTORS //

// empty
WeightProcessor::WeightProcessor(): scale(0), offset(0), samp_rate(1), time(nullptr), weight(nullptr), vertical_velocity(nullptr), estimatedWeight(-1), sig_length(0){}

// full
WeightProcessor::WeightProcessor(std::vector<long>* lc_sig,float s, float o,int s_r) : scale(s), offset(o), samp_rate(s_r), vertical_velocity(nullptr), estimatedWeight(-1), sig_length(lc_sig->size()){
    setForceWeight(lc_sig);
    // setTime();
    setUnpadInt(calculateUnpadInt(50,weight));
}

// DECONSTRUCTOR
WeightProcessor::~WeightProcessor(){
    // if (weight != nullptr){delete weight;}
    // if (time != nullptr){delete time;}
}

// GETTERS //

// SETTERS //
void WeightProcessor::setForceWeight(std::vector<long>* lc_sig){
    
    // instantiate dynamic force and weight arrays
    weight = new std::vector<float>;

    // assign relevaant values for force and weight signals
    for(int i = 0; i < lc_sig->size();i++){
        weight->push_back(((*lc_sig)[i] - offset )/ scale);
    }

    delete lc_sig;
}

void WeightProcessor::setTime(){
    // delete previous force or weight if previously set
    if (time != nullptr){delete [] time;}

    // calculate the sampling period and initialize time
    float samp_per = 1/samp_rate;
    time = new std::vector<float>();

    // assign relevaant values for force, weight and time signals
    for(int i = 0; i < sig_length;i++){
        time->push_back(i * samp_per);
    }
}

// UTILITY

float WeightProcessor::Mean(std::vector<float>* signal, int start, int end) {
    float sum = 0;
    for(int i = start; i < end; i++) {
        sum += (*signal)[i];
    }
    return sum / static_cast<float>(end - start);
}

float WeightProcessor::Variance(std::vector<float>* signal,int start, int end) {
    float variance = 0.0;

    // Calculate the mean
    float mean = Mean(signal,start,end);

    // Calculate the variance
    for (int i = start; i < end; i++) {
        variance += pow((*signal)[i] - mean, 2);
    }
    variance /= (end - start);

    return variance;
}

float WeightProcessor::Median(std::vector<float>* signal,int start, int end) {
    // create sub array
    vector<float> subarray;
    for (int i = start; i < end; i ++){
        subarray.push_back((*signal)[i]);
    }

    // sort the subarray
    int size = subarray.size();
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (subarray[j] > subarray[j + 1]) {
                // Swap values
                float temp = subarray[j];
                subarray[j] = subarray[j + 1];
                subarray[j + 1] = temp;
            }
        }
    }
    
    // find the middle value
    if (size % 2 == 0) {
        return (subarray[size / 2 - 1] + subarray[size / 2]) / 2.0f;
    } else {
        return subarray[size / 2];
    }
}

pair<int,int> WeightProcessor::calculateUnpadInt(int thresh, std::vector<float>* sig){
    pair<int,int> out(0,sig_length);
    if (sig == nullptr){return out;}
    bool found_start = false;
    bool found_end = false;
    for(int i = 0; i < sig->size(); i++){
        if (abs((*sig)[i]) < thresh & found_start == false){
            out.first++;
        }else{
            found_start = true;
        }
        if (abs((*sig)[sig->size()-1-i]) < thresh & found_end == false){
            out.second--;
        }else{
            found_end = true;
        }
    }

    return out;
}


//// SIGNAL VALIDATION ////

int WeightProcessor::ValuePeriod(float value, bool above_below, std::vector<float>* sig){
    int max_no_above_below = -1;
    int no_above_below = 0;
    for(int i = 0; i < sig->size(); i++){
        if (((*sig)[i] < value && above_below == false) || ((*sig)[i] > value && above_below == true)){
            no_above_below++;
        } else if (((*sig)[i] > value && above_below == false) || ((*sig)[i] < value && above_below == true)){
            if (no_above_below > max_no_above_below){
                max_no_above_below = no_above_below;
                no_above_below = 0;
            }
            if (no_above_below < max_no_above_below){
                no_above_below = 0;
            }
        }
    }

    // if the signal never dips below minimum
    if (max_no_above_below == -1){
        max_no_above_below = no_above_below;
    }

    return max_no_above_below;
}

int WeightProcessor::validateSignal(float min, float max, float sec_above_below, float min_sec_total, std::vector<float>* sig){
    // initialize to a valid signal
    int valid = 0;
    
    // caluclate the sampling period and number of samples for second parameters
    float samp_per = 1.0f/samp_rate;
    int samp_above_below = static_cast<int>(sec_above_below / samp_per);
    int min_samp_total = static_cast<int>(min_sec_total / samp_per);

    // validate that the signal is adequate length
    int valid_sig_len = unpad_int.second - unpad_int.first;
    if (valid_sig_len < min_samp_total){
        valid = 1; // indicates inadequate sample length
        return valid;
    }

    // validate that it is greater than the min value for specified period
    int val_per = ValuePeriod(min,true,sig);
    if (val_per < samp_above_below){
        valid = 2; // signal
        return valid;
    }

    // validate that it is not greater than the min value for specified period
    val_per = ValuePeriod(max,true,sig);
    if (val_per > samp_above_below){
        valid = 3; // signal
        return valid;
    }

    return valid;
    
}

//// FILTERS ////

std::vector<float>* WeightProcessor::movingAverage(int window, std::vector<float>* sig){
    // initialize filtered signal
    std::vector<float>* filtered = new std::vector<float>;

    int skip = window / 2;

    for(int i = 0; i < sig_length; i++){
        int sum = 0;
        int count = 0;
        for (int n = -skip; n <= skip; n++) {
            int index = i + n;
            if (index >= 0 && index < sig_length) {
                sum += (*sig)[index];
                count++;
            }
        }
        filtered->push_back(sum/count);
    }

    return filtered;
}

std::vector<float>* WeightProcessor::expMovingAverage(int window, std::vector<float>* sig){
    // initialize filtered signal
    std::vector<float>* filtered = new std::vector<float>;
    filtered->push_back((*sig)[0]);

    // smoothing factor
    float smooth = 2.0f / (window + 1);

    for(int i = 1; i < sig_length; i++){
        filtered->push_back((1-smooth) * (*filtered)[i-1] + smooth * (*sig)[i]);
    }

    return filtered;
}

std::vector<float>* WeightProcessor::medianFilter(int window, std::vector<float>* sig){
    std::vector<float>* filtered = new std::vector<float>;

    int skip = window / 2;

    for (int i = 0; i < sig->size(); i++) {
        int start_index = i - skip;
        int end_index = i + skip;

        if (start_index < 0) {
            start_index = 0;
        }
        if (end_index >= sig->size()) {
            end_index = sig->size() - 1;
        }

        float median = Median(sig, start_index, end_index);

        filtered->push_back(median);
    }

    return filtered;
}

std::vector<float>* WeightProcessor::whitEielers(float lambda, int iterations, std::vector<float>* sig) {
    std::vector<float>* filtered = new vector<float>;
    int size = sig->size();

    // Initialize smoothed data with input data
    for(float value : (*sig)){
        filtered->push_back(value);
    }

    // Iterate to solve the optimization problem
    for (int iter = 0; iter < iterations; ++iter) {
        std::vector<float> d2(size);

        // Calculate the second derivative
        for (int i = 1; i < size - 1; ++i) {
            d2[i] = (*filtered)[i - 1] - 2 * (*filtered)[i] + (*filtered)[i + 1];
        }

        // Update smoothed data
        for (int i = 1; i < size - 1; ++i) {
            double weight = 1.0 / (1.0 + lambda * d2[i] * d2[i]);
            (*filtered)[i] = ((*sig)[i] + lambda * ((*filtered)[i - 1] + (*filtered)[i + 1])) / (1 + 2 * lambda);
        }
    }

    return filtered;
}

std::vector<float>* WeightProcessor::kalmanFilter(std::vector<float>* sig, float initial_estimate, float initial_error, float process_noise, float measurement_noise) {
    // Initialize state estimate
    float x_hat = initial_estimate;

    // Initialize error covariance matrix
    float P = initial_error;

    std::vector<float>* filtered = new vector<float>;

    // Iterate over measurements
    for (float value : (*sig)) {
        // Prediction step (no control input assumed)
        x_hat += 0.0;

        // Update error covariance matrix
        P += process_noise;

        // Calculate Kalman gain
        double K = P / (P + measurement_noise);

        // Update state estimate based on measurement
        x_hat += K * (value - x_hat);

        // Update error covariance matrix
        P = (1 - K) * P;

        // Store filtered estimate
        filtered->push_back(x_hat);
    }

    return filtered;
}

//// STABLE LOCATION ////

std::pair<int,int> WeightProcessor::mostStableInterval(std::vector<float>* sig, float interval_len){
    float samp_per = 1.0f/samp_rate;
    int samps_in_interval = static_cast<int>(interval_len / samp_per);

    if (sig->empty() || samps_in_interval <= 0 || samps_in_interval > unpad_int.second - unpad_int.first) {
        return make_pair(0.0f, 0.0f); // Return invalid interval if input is invalid
    }

    pair<float, float> mostStableInterval;
    float minVariance = 0;
    for (size_t i = unpad_int.first; i <= unpad_int.second - samps_in_interval; ++i) {
        float variance = Variance(sig,i,i+samps_in_interval);
        if (i == unpad_int.first){
            minVariance = variance;
            mostStableInterval.first = i;
            mostStableInterval.second = i+samps_in_interval;
        }else if (variance < minVariance) {
            minVariance = variance;
            mostStableInterval.first = i;
            mostStableInterval.second = i+samps_in_interval;
        }
    }

    return mostStableInterval;
};

//// WEIGHT ESTIMATION ////

std::pair<float,float> WeightProcessor::estimateWeight(int min_weight, int max_weight, float threshold_time, float total_time, float stable_interval_len, string filter, int window, float lambda, bool validate){
    // initalize output pair
    pair<float,float> out(-1.0f,-1.0f);

    // validate the signal
    if (validate){
        int valid = validateSignal(min_weight,max_weight,threshold_time,total_time,weight);
        if (valid != 0){
            out.first = -1.0f * valid;
            return out;
        }
    }

    // filter the signal
    vector<float>* filtered;
    if (filter != "") {
        if (filter == "mov_ave") {
            filtered = movingAverage(window,weight);
        } else if (filter == "exp_mov_ave") {
            filtered = expMovingAverage(window,weight);
        } else if (filter == "med"){
            filtered = medianFilter(window,weight);
        } else if (filter == "whit"){
            filtered = whitEielers(lambda,40,weight);
        } else if (filter == "kal"){
            filtered = kalmanFilter(weight);
        }
    }

    // find most stable intervals
    pair<float,float> stable_intervals;
    if (filter == ""){
        stable_intervals = mostStableInterval(weight,stable_interval_len);
    }else{
        stable_intervals = mostStableInterval(filtered,stable_interval_len);
    }

    // calculate the average of the stable readings and variance
    float estimate;
    float variance;
    if (filter == ""){
        estimate = Mean(weight,stable_intervals.first,stable_intervals.second);
        variance = Variance(weight,stable_intervals.first,stable_intervals.second);
    }else{
        estimate = Mean(filtered,stable_intervals.first,stable_intervals.second);
        variance = Variance(filtered,stable_intervals.first,stable_intervals.second);
    }

    // calculate the percentage error
    float perc_error = (variance / estimate) * 100 + 0.05;

    // save calculations to output
    out.first = estimate;
    out.second = perc_error;

    // delete filtered if filtered
    if (filter != "") {
        delete filtered;
    }

    return out;
};