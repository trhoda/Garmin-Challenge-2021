#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "calc.h"

int main(int argc, char **argv)
{
    double **data, *freqs;
    int i, *rates;
    
    if (argc < 3) {
            printf("Not enough wave files.\n");
            return 1;
    }
    /*  
        original signal: data[0]
        javalin signal:  data[1]
        shotput signal:  data[2]
    */
    data = (double**)malloc(3 * sizeof(double*));
    rates = (int*)malloc(3*sizeof(int));
    freqs = (double*)malloc(3*sizeof(double));
    
    for (i = 1; i < 4; i++) {
        data[i-1] = get_file_data(argv[i], &rates[i-1]);
        freqs[i-1] = get_freq(data[i-1], rates[i-1]);    
    }

    printf("The velocity of the javelin is %f m/s\n", calc_speed(freqs[0], freqs[1]));
    printf("The velocity of the shot-put is %f m/s\n", calc_speed(freqs[0], freqs[2]));

    /* cleanup */
    for (i=0; i < 3; i++) free(data[i]);
    free(data);
    free(rates);
    free(freqs);

    return 0;
}