#include "WeightProcessor.h"

#include <string>
#include <iostream>

using namespace std;

// CONSTRUCTORS //

// empty
WeightProcessor::WeightProcessor(): scale(0), offset(0), samp_rate(1), force(nullptr), weight(nullptr), vertical_velocity(nullptr), estimatedWeight(-1), sig_length(0){}

// full
WeightProcessor::WeightProcessor(long* lc_sig,double s, double o,int s_r, int s_l) : scale(s), offset(o), samp_rate(s_r), vertical_velocity(nullptr), estimatedWeight(-1), sig_length(s_l){
    // instantiate dynamic force and weight arrays
    weight = new double[sig_length];
    force = new double[sig_length];

    // assign relevaant values for force and weight signals
    for(int i = 0; i < sig_length;i++){
        weight[i] = (lc_sig[i] - offset )/ scale;
        force[i] = ((lc_sig[i] - offset )/ scale) * 9.8;
    }
}

// DECONSTRUCTOR
WeightProcessor::~WeightProcessor(){
    if (vertical_velocity != nullptr){delete [] vertical_velocity;}
    if (weight != nullptr){delete [] weight;}
    if (force != nullptr){delete [] force;}
}

// GETTERS //

// SETTERS //
void WeightProcessor::setForceWeight(long* lc_sig){
    // delete previous force or weight if previously set
    if (force != nullptr){delete [] force;}
    if (weight != nullptr){delete [] weight;}
    
    // instantiate dynamic force and weight arrays
    weight = new double[sig_length];
    force = new double[sig_length];

    // assign relevaant values for force and weight signals
    for(int i = 0; i < sig_length;i++){
        weight[i] = (lc_sig[i] - offset )/ scale;
        force[i] = (((lc_sig[i] - offset )/ scale)/1000) * 9.8;
    }
}


// UTILITY FUNCTIONS //


// MAIN FUNCTION //