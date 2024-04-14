#ifndef WEIGHT_PROCESSOR_H
#define WEIGHT_PROCESSOR_H

// std includes
#include <string>

class WeightProcessor{
    private:
        int samp_rate; // sampling rate
        float scale; // scales raw readings based on calibration
        float offset; // removed offset from tare

        float* force; // force measured at the load cell
        float* weight; // force exerted by the bird
        float* vertical_velocity; // vertical velocity of the bird
        float* time;

        std::pair<int,int> unpad_int; // interval where signal is non_zero

        float estimatedWeight;
        int sig_length;
    public:
        // BIG 6
        // constructors
        WeightProcessor();
        WeightProcessor(long* lc_sig,float s, float o,int s_r,int s_l); // constructor [force signal array]

        // deconstructor
        ~WeightProcessor();

        // setters
        void setScale(float s){scale = s;};
        void setOffset(float o){offset = o;};
        void setSamplingRate(int s_r){samp_rate = s_r;};
        void setForceWeight(long* lc_sig);
        void setLength(int s_l){sig_length = s_l;}
        void setTime();
        void setUnpadInt(std::pair<int,int> intervals){unpad_int = intervals;};
        
        // getters
        float getScale(){return scale;};
        float getOffset(){return offset;};
        int getSamplingRate(){return samp_rate;};
        int getLength(){return sig_length;};
        float* getWeightSig(){return weight;}
        float* getForceSig(){return force;}
        std::pair<int,int> getUnpadInt(){return unpad_int;}

        // UTILITY
        float* forceToVertVelocity();
        std::pair<int,int> calculateUnpadInt(int thresh, float* sig);

        // 1. LEAST VELCOITY METHOD
        float leastVelocity(int interval);
        
        // 2. MEAN METHOD
        float Mean(float* signal);

        // 3. FIRST AND SECOND WEIGHT APPROX METHOD
        float VelcoityTrend();
        float FirstApprox();
        float SecondApprox();

        // 4. KALMAN FILTER
        float* kalmanFilter();

        // 5. MOVING AVERAGE FILTER
        float* movingAverage(int window, float* sig);

        // 6. EXPONENTIAL MOVING AVERAGE
        float* expMovingAverage(int window, float* sig);

        // 7. MEDIAN FILTER
        float* medianFilter();

        // 8. LOW PASS
        float* lowPass();

        // 9. WAVELET TRANFORM
        float* waveletTransform();

        // MAIN FUNCTION
        int estimateWeight();
};
#endif