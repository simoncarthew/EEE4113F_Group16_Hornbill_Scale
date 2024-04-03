#ifndef WEIGHT_PROCESSOR_H
#define WEIGHT_PROCESSOR_H

// std includes
#include <string>

class WeightProcessor{
    public:
        int samp_rate; // sampling rate
        double* load_cell_force; // force measured at the load cell
        double* bird_force; // force exerted by the bird
        double* vertical_velocity; // vertical velocity of the bird
    private:
        // BIG 6
        WeightProcessor(std::string force_file); // constructor [file]
        WeightProcessor(double* lc_force); // constructor [force signal array]

        // SIGNAL CONVERSION
        double* getBirdForce();
        double* forceToVertVelocity();

        // ZERO VELCOITY METHOD
        std::pair<double,double> getMinVelocity();
        
        // MEAN METHOD
        double Mean(double* signal);

        // FIRST AND SECOND WEIGHT APPROX METHOD
        double VelcoityTrend();
        double FirstApprox();
        double SecondApprox();

        // MAIN FUNCTION
        int estimateWeight();
};
#endif