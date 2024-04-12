#include "WeightProcessor.h"

#include <string>
#include <iostream>
#include <cmath>

using namespace std;

// OTHER FUNCTIONS //
float absolute(float x) {
    return (x < 0) ? -x : x;
}

// CONSTRUCTORS //

// empty
WeightProcessor::WeightProcessor(): scale(0), offset(0), samp_rate(1), time(nullptr), force(nullptr), weight(nullptr), vertical_velocity(nullptr), estimatedWeight(-1), sig_length(0){}

// full
WeightProcessor::WeightProcessor(long* lc_sig,float s, float o,int s_r, int s_l) : scale(s), offset(o), samp_rate(s_r), vertical_velocity(nullptr), estimatedWeight(-1), sig_length(s_l){
    setForceWeight(lc_sig);
    setTime();
    setUnpadInt(calculateUnpadInt(20,weight));
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
    weight = new float[sig_length];
    force = new float[sig_length];

    // assign relevaant values for force and weight signals
    for(int i = 0; i < sig_length;i++){
        weight[i] = (lc_sig[i] - offset )/ scale;
        force[i] = (((lc_sig[i] - offset )/ scale)/1000) * 9.8;
    }
}

void WeightProcessor::setTime(){
    // delete previous force or weight if previously set
    if (time != nullptr){delete [] time;}

    // calculate the sampling period and initialize time
    float samp_per = 1/samp_rate;
    time = new float[sig_length];

    // assign relevaant values for force, weight and time signals
    for(int i = 0; i < sig_length;i++){
        time[i] = i * samp_per;
    }
}

// UTILITY
// float* forceToVertVelocity(){};

pair<int,int> WeightProcessor::calculateUnpadInt(int thresh, float* sig){
    pair<int,int> out(0,sig_length);
    if (weight == nullptr){return out;}
    
    bool found_start = false;
    bool found_end = false;
    for(int i = 0; i < sig_length; i++){
        if (absolute(weight[i]) < thresh & found_start == false){
            out.first++;
        }else{
            found_start = true;
        }
        if (absolute(weight[sig_length-1-i]) < thresh & found_end == false){
            out.second--;
        }else{
            found_end = true;
        }
    }

    return out;
}

// FIRST & SECOND //
// float FirstApprox(){

// };

float VelcoityTrend();
float SecondApprox();

// MAIN FUNCTION //