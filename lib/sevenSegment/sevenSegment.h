#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H

#include <Arduino.h>

// TODO Make this configurable
#define SEVEN_SEG_DATA 47
#define SEVEN_SEG_LATCH 33
#define SEVEN_SEG_CLOCK 34

// Set the 4 digits of the Seven Seg display, values should be between 0 and 9.
void setDigitsOnSevenSeg(uint8_t digitA, uint8_t digitB, uint8_t digitC,
                         uint8_t digitD);

// Required for multiplexing, add to setup().
void beginSevenSegThread();

#endif  // SEVEN_SEGMENT_H
