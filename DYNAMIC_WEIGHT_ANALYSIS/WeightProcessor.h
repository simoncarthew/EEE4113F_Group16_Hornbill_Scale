#ifndef WEIGHT_PROCESSOR_H
#define WEIGHT_PROCESSOR_H

// std includes
#include <string>

class WeightProcessor{
    private:
        int samp_rate; // sampling rate
        double scale; // scales raw readings based on calibration
        double offset; // removed offset from tare
        double* force; // force measured at the load cell
        double* weight; // force exerted by the bird
        double* vertical_velocity; // vertical velocity of the bird
        double estimatedWeight;
        int sig_length;
    public:
        // BIG 6
        // constructors
        WeightProcessor();
        WeightProcessor(long* lc_sig,double s, double o,int s_r,int s_l); // constructor [force signal array]

        // deconstructor
        ~WeightProcessor();

        // setters
        void setScale(double s){scale = s;};
        void setOffset(double o){offset = o;};
        void setSamplingRate(int s_r){samp_rate = s_r;};
        void setForceWeight(long* lc_sig);
        void setLength(int s_l){sig_length = s_l;}
        
        // getters
        double getScale(){return scale;};
        double getOffset(){return offset;};
        int getSamplingRate(){return samp_rate;};
        int getLength(){return sig_length;};
        double* getWeightSig(){return weight;}


        // SIGNAL CONVERSION
        double* forceToVertVelocity();

        // 1. LEAST VELCOITY METHOD
        double leastVelocity(int interval);
        
        // 2. MEAN METHOD
        double Mean(double* signal);

        // 3. FIRST AND SECOND WEIGHT APPROX METHOD
        double VelcoityTrend();
        double FirstApprox();
        double SecondApprox();

        // 4. KALMAN FILTER
        double* kalmanFilter();

        // 5. MOVING AVERAGE FILTER
        double* movingAverage();

        // 6. MEDIAN FILTER
        double* medianFilter();

        // 7. LOW PASS
        double* lowPass();

        // 8. WAVELET TRANFORM
        double* waveletTransform();

        // MAIN FUNCTION
        int estimateWeight();
};
#endif