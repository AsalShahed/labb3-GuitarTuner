#ifndef SAMPLER_H_
#define SAMPLER_H_
/**
* Starts sampling on a given pin with a given fequency.
* Parameter freq: the sampling frequency
*/
void startSampling();
/**
* Stops the sampling process.
*/
void stopSampling();

/**
* Computes the average frequency of the signal that has been (or is being) sampled.
*/
float getFrequency();

/** 
 * Resets frequency detection without stopping the sampling process. 
 */ 
void resetSampling(); 
#endif