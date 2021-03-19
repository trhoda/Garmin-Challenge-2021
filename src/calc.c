#include <math.h>
#include <stdlib.h>
#include "calc.h"
#include "helper.h"

double calc_speed(double f_o, double f_s)
{
    double f_d, c;
    f_d = f_o-f_s;
    c = 340.29;
    return f_d/f_o*c/2;
}


double get_freq(double *data, long rate)
{
    int n, i, max_i;

    double *spectrum, freq, max;

    n = *(data++);  /* move off the count value */

    spectrum = get_spectrum(data, n);

    /* find max etc. */
    max = spectrum[1];
    max_i = 1;

    for (i = 1; i <= n/2; i++) {
        (spectrum[i] > max) ? update(&max, &max_i, spectrum[i], i):NULL;
    }
    
    freq = (max_i * rate / (double)n);
    
    free(spectrum);
    
    return freq;
}


double window(double val, long n, int index)
{
    return val * (0.54 - 0.46 * cos( 6.28 * index / (n - 1) ));
}


double* magnitude(int m, fftw_complex *spectrum) 
{
    int i;
    double *magnitude, im, re;

    magnitude = (double*)malloc(m*sizeof(double));
    for (i = 0; i < m; i++) {
        re = spectrum[i][0];
        im = spectrum[i][1];
        magnitude[i] = sqrt(re*re+im*im);
    }

    return magnitude;
}


double* get_spectrum(double *data, int n) 
{
    fftw_complex *out_cpx;
    fftw_plan fft;
    double *spectrum, *work;
    int i, size;

    size = n/2+1;
    work = (double*)malloc(n*sizeof(double));
    out_cpx = (fftw_complex*)fftw_malloc(size*sizeof(fftw_complex));
    
    for (i = 0; i < n; i++) {
        work[i] = window(data[i], n, i);
    }
    

    fft = fftw_plan_dft_r2c_1d(n, work, out_cpx, FFTW_ESTIMATE);
    fftw_execute(fft);

    spectrum = magnitude(size, out_cpx);
    
    fftw_destroy_plan(fft);
    fftw_free(out_cpx);
    fftw_cleanup();
    free(work);

    return spectrum;
}