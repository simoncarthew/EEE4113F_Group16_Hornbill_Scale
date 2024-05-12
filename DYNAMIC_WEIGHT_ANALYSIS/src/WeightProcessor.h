#ifndef WEIGHT_PROCESSOR_H
#define WEIGHT_PROCESSOR_H

// std includes
#include <vector>
#include <string>

class WeightProcessor{
    private:
        int samp_rate; // sampling rate
        float scale; // scales raw readings based on calibration
        float offset; // removed offset from tare

        std::vector<float>* weight; // force exerted by the bird
        std::vector<float>* vertical_velocity; // vertical velocity of the bird
        std::vector<float>* time;

        std::pair<int,int> unpad_int; // interval where signal is non_zero

        float estimatedWeight;
        int sig_length;
    public:
        // BIG 6
        // constructors
        WeightProcessor();
        WeightProcessor(std::vector<long>* lc_sig,float s, float o,int s_r); // constructor [force signal array]

        // deconstructor
        ~WeightProcessor();

        // setters
        void setScale(float s){scale = s;};
        void setOffset(float o){offset = o;};
        void setSamplingRate(int s_r){samp_rate = s_r;};
        void setForceWeight(std::vector<long>* lc_sig);
        void setLength(int s_l){sig_length = s_l;}
        void setTime();
        void setUnpadInt(std::pair<int,int> intervals){unpad_int = intervals;};
        
        // getters
        float getScale(){return scale;};
        float getOffset(){return offset;};
        int getSamplingRate(){return samp_rate;};
        int getLength(){return sig_length;};
        std::vector<float>* getWeightSig(){return weight;}
        std::pair<int,int> getUnpadInt(){return unpad_int;}

        // UTILITY
        std::vector<float>* forceToVertVelocity();
        std::pair<int,int> calculateUnpadInt(int thresh, std::vector<float>* sig);
        float maxValue(std::vector<float>* sig);
        float Mean(std::vector<float>* signal, int start, int end);
        float Median(std::vector<float>* signal, int start, int end);
        float Variance(std::vector<float>* signal,int start, int end);

        // SIGNAL VALIDATION
        int ValuePeriod(float value, bool above_below, std::vector<float>* sig);
        int validateSignal(float min, float max, float sec_above_below, float min_total_sec, std::vector<float>* sig);

        // FILTERS
        std::vector<float>* kalmanFilter(std::vector<float>* sig, float initial_estimate = 0.0f, float initial_error = 100.0f, float process_noise = 0.1f, float measurement_noise = 10.0f);
        std::vector<float>* movingAverage(int window, std::vector<float>* sig);
        std::vector<float>* expMovingAverage(int window, std::vector<float>* sig);
        std::vector<float>* medianFilter(int window, std::vector<float>* sig);
        std::vector<float>* whitEielers(float lambda, int iterations, std::vector<float>* sig);

        // STABLE LOCALIZATION
        std::pair<int,int> mostStableInterval(std::vector<float>* sig, float interval_len);

        // MAIN FUNCTION
        std::pair<float,float> estimateWeight(int min_weight = 500, int max_weight = 1500, float threshold_time = 0.4, float total_time = 1.5, float stable_interval_len = 0.3, std::string filter = "mov_ave", int window = 4, float lambda = 10, bool validate = true);
        

        // ALTERNATIVE METHODS
        float leastVelocity(int interval);
        float VelcoityTrend();
        float FirstApprox();
        float SecondApprox();
};
#endif