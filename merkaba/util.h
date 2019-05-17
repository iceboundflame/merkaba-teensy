#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "Streaming.h"

// utils

inline void printRGB(const CRGB& rgb) {
  Serial << _HEX(rgb.r) << ":"
         << _HEX(rgb.g) << ":"
         << _HEX(rgb.b);
}

inline int randomExcluding(int min, int max, int exclude) {
  int val = exclude;
  while (val == exclude) {
    val = random(min, max);
  }
  return val;
}

extern bool gAutoAdvance;
