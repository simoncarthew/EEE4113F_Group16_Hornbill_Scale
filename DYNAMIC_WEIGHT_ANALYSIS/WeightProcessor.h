#include <string>

class WeightProcessor{
    public:
        int samp_rate; // sampling rate
        double* load_cell_force; // force measured at the load cell
        double* weight_signal; // weight exerted by the bird
        double* vertical_velocity; // vertical velocity of the bird
    private:
        // BIG 6
        WeightProcessor(std::string force_file); // constructor [file]
        WeightProcessor(double* lc_force); // constructor [force signal array]

        // SIGNAL CONVERSION
        double* forceToWeight();
        double* forceToVertVelocity();

        // UTILITY FUNCTIONS
        std::pair<double,double> getMinVelocity();
        double getMean(double* signal);

        // MAIN FUNCTION
        int estimateWeight();
};