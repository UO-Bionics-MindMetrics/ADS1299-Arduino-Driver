/* main.cpp
 * This file contains configuration settings for the operation of the
 *  ADS1299 in main.cpp
 * 
 * Authors: Mingye Chen
 */
#ifndef configs_h
#define configs_h

#define SQUARE_SINE_TEST // define to perform square sine test
#define DATA_LOGGER_MODE // define to set microcontroller to data logger mode
#undef DATA_LOGGER_MODE

#define LOGSIZE 100 // number of samples to save in the data log
#define NUMBEROFCHANNELS 1 // number of channels in use

#endif