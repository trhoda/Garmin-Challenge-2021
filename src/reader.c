#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include "wave.h"
#include "reader.h"
#define TRUE 1
#define FALSE 0

unsigned char buffer4[4];
unsigned char buffer2[2];

double* get_file_data(char *name, int *rate)
{
    FILE *ptr;
    char *filename, cwd[1024];
    struct HEADER header;
    double *d, data_in_channel;
    int read, offset;
    unsigned int  xchannels;
    long low_limit, high_limit, bytes_in_each_channel, num_samples, \
        size_of_each_sample, i;
    char format_name[10] = "";
    int  size_is_correct = TRUE;

    filename = (char*) malloc(sizeof(char) * 1024);
    if (filename == NULL) {
        printf("Error in malloc\n");
        exit(1);
    }
    /* get file path */
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        strcpy(filename, cwd);
        /* get filename from command line */
        strcat(filename, "/");
        strcat(filename, name);
    }

    /* open file */
    ptr = fopen(filename, "rb");
    if (ptr == NULL) {
        printf("Error opening file\n");
        exit(1);
    }

    read = 0;
    /* read header parts */
    read = fread(header.riff, sizeof(header.riff), 1, ptr);
    read = fread(buffer4, sizeof(buffer4), 1, ptr);
    /* convert little endian to big endian 4 byte int */
    header.overall_size  = buffer4[0] | (buffer4[1]<<8) |
                        (buffer4[2]<<16) | (buffer4[3]<<24);
    read = fread(header.wave, sizeof(header.wave), 1, ptr);
    read = fread(header.fmt_chunk_marker, sizeof(header.fmt_chunk_marker), 1, ptr);
    read = fread(buffer4, sizeof(buffer4), 1, ptr);
    /* convert little endian to big endian 4 byte integer */
    header.length_of_fmt = buffer4[0] | (buffer4[1] << 8) |
                        (buffer4[2] << 16) | (buffer4[3] << 24);
    read = fread(buffer2, sizeof(buffer2), 1, ptr);
    header.format_type = buffer2[0] | (buffer2[1] << 8);
    
    if (header.format_type == 1)
        strcpy(format_name,"PCM");
    else if (header.format_type == 6)
        strcpy(format_name, "A-law");
    else if (header.format_type == 7)
        strcpy(format_name, "Mu-law");
    read = fread(buffer2, sizeof(buffer2), 1, ptr);
    header.channels = buffer2[0] | (buffer2[1] << 8);
    read = fread(buffer4, sizeof(buffer4), 1, ptr);
    header.sample_rate = buffer4[0] | (buffer4[1] << 8) |
                    (buffer4[2] << 16) | (buffer4[3] << 24);
    read = fread(buffer4, sizeof(buffer4), 1, ptr);
    header.byterate  = buffer4[0] | (buffer4[1] << 8) |
                    (buffer4[2] << 16) | (buffer4[3] << 24);
    read = fread(buffer2, sizeof(buffer2), 1, ptr);
    header.block_align = buffer2[0] |
        (buffer2[1] << 8);
    read = fread(buffer2, sizeof(buffer2), 1, ptr);
    header.bits_per_sample = buffer2[0] |
        (buffer2[1] << 8);
    read = fread(header.data_chunk_header, sizeof(header.data_chunk_header), 1, ptr);
    read = fread(buffer4, sizeof(buffer4), 1, ptr);
    header.data_size = buffer4[0] | (buffer4[1] << 8) |
				(buffer4[2] << 16) | (buffer4[3] << 24 );

    *rate = header.sample_rate;
    /* calculate no.of samples */
    num_samples = (8 * header.data_size) / (header.channels * header.bits_per_sample);
    size_of_each_sample = (header.channels * header.bits_per_sample) / 8;

    d = (double*)malloc((num_samples+1)*sizeof(double));
    d[0] = (int)num_samples;

    /* read each sample from data chunk if PCM */
    if (header.format_type == 1) { /* PCM */
        i =0;
        char data_buffer[size_of_each_sample];
        size_is_correct = TRUE;

        /* make sure that the bytes-per-sample is completely divisible by num.of channels */
        bytes_in_each_channel = (size_of_each_sample / header.channels);
        if ((bytes_in_each_channel  * header.channels) != size_of_each_sample) {
            printf("Error: %ld x %ud <> %ldn", bytes_in_each_channel, header.channels, size_of_each_sample);
            size_is_correct = FALSE;
        }

        if (size_is_correct) {
            /* the valid amplitude range for values based on the bits per sample */
            low_limit = 0l;
            high_limit = 0l;

            switch (header.bits_per_sample) {
            case 8:
                low_limit = -128;
                high_limit = 127;
                break;
            case 16:
                low_limit = -32768;
                high_limit = 32767;
                break;
            case 32:
                low_limit = -2147483648;
                high_limit = 2147483647;
                break;
            }

            for (i =1; i <= num_samples; i++) {
                read = fread(data_buffer, sizeof(data_buffer), 1, ptr);
                if (read == 1) {

                    /* dump the data read */
                    xchannels = 0;
                    data_in_channel = 0;
                    offset = 0; /* move the offset for every iteration in the loop below */
                    for (xchannels = 0; xchannels < header.channels; xchannels ++ ) {
                        /* convert data from little endian to big endian based on bytes in each channel sample */
                        if (bytes_in_each_channel == 4) {
                            data_in_channel = (data_buffer[offset] & 0x00ff) |
                                ((data_buffer[offset + 1] & 0x00ff) <<8) |
                                ((data_buffer[offset + 2] & 0x00ff) <<16) |
                                (data_buffer[offset + 3]<<24);
                        }
                        else if (bytes_in_each_channel == 2) {
                            data_in_channel = (data_buffer[offset] & 0x00ff) |
                                (data_buffer[offset + 1] << 8);
                        }
                        else if (bytes_in_each_channel == 1) {
                            data_in_channel = data_buffer[offset] & 0x00ff;
                            data_in_channel -= 128; /*in wave, 8-bit are unsigned, so shifting to signed */
                        }

                        offset += bytes_in_each_channel;
                        d[(int)i] = (double)data_in_channel;

                        /* check if value was in range */
                        if (data_in_channel < low_limit || data_in_channel > high_limit)
                            printf("**value out of rangen");
                    }

                }
                else {
                    printf("Error reading file. %d bytes\n", read);
                    break;
                }

            } /* for (i =1; i <= num_samples; i++) { */

        } /* if (size_is_correct) { */

    } /* if (header.format_type == 1) { */

    fclose(ptr);

    /* cleanup before quitting */
    free(filename);
    
    return d;
}