#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H

#include <Arduino.h>

// TODO Make this configurable
#define SEVEN_SEG_DATA 47
#define SEVEN_SEG_LATCH 33
#define SEVEN_SEG_CLOCK 34


void setDualDigitsOnSevenSeg(uint8_t digitAB, uint8_t digitCD);

// Required for multiplexing, add to setup().
void beginSevenSegThread();

#endif  // SEVEN_SEGMENT_H
