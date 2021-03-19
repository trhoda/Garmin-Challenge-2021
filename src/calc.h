#ifndef CALC_H
#define CALC_H

#include <fftw3.h>

/**
 * @brief window function for values 
 * 
 * @param val value
 * @param n number of samples
 * @param index index of value
 * @return double the new value
 */
double window(double val, long n, int index);

/**
 * @brief Get the spectrum from the wav values
 * 
 * @param data [in] ptr to values read from wav file
 * @param n number of samples
 * @return double* the sectrum size n/2 of data passed through fft
 */
double* get_spectrum(double *data, int n);

/**
 * @brief function to get the magnitude list (used in get_spectrum)
 *  from the fft list
 * 
 * @param m size of fft (n/2)
 * @param spectrum the fft list from the sample
 * @return double* magnitude list
 */
double* magnitude(int m, fftw_complex *spectrum);

/**
 * @brief Get the frequency of file data
 * 
 * @param sample the sample data
 * @param rate the sample rate
 * @return double frequency
 */
double get_freq(double *sample, long rate);

/**
 * @brief calculate the speed of object based of signal and 
 * reflaction frequeny.
 * 
 * @param f_o original signal
 * @param f_s reflected signal
 * @return double speed of object in m/s
 */
double calc_speed(double f_o, double f_s);

#endif