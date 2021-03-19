#ifndef READER_H
#define READER_H

/**
 * @brief Get the sample data from wav file
 * 
 * @param filename the filename
 * @param fs [in/out] ptr to the sample rate variable. set in funct
 * @return double* point to sample data.
 */
double* get_file_data(char *filename, int *fs);

#endif