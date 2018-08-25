#pragma once

#include <Arduino.h>

#include "Streaming.h"
#include "Display.h"

///
// Drawing utils

inline int revIndex(CRGBSet leds, int i, bool rev) {
  return rev ? leds.size() - 1 - i
      : i;
}

// Warning: 'rev' doesn't work if CRGBSet is reversed!
void drawBar(CRGBSet leds, CRGB color, float len, bool rev=false) {
  len = min(len, leds.size());
  int lenFloor = floor(len);
  int lenCeil = ceil(len);

//    if (rev)
//      leds = leds(leds.size() - 1, 0);

  if (lenFloor > 0) {
    leds(revIndex(leds, 0, rev),
         revIndex(leds, lenFloor - 1, rev)).fill_solid(color);
  }
  if (lenCeil != lenFloor) {
    int i = revIndex(leds, lenCeil - 1, rev);
    leds[i] = color;
    leds[i].nscale8((len - lenFloor) * 255);
  }
}

void drawCenterBar(CRGBSet leds, CRGB color, float len) {
  // FIXME: Odd #s?
//    drawBar(
//        CRGBSet(leds, leds.size()/2-1, 0),
//        color, len/2);
  CRGBSet fwdHalf(leds, leds.size()/2, leds.size()-1);
  drawBar(fwdHalf, color, len/2);
  CRGBSet backHalf(leds, 0, leds.size()/2-1);
  drawBar(backHalf, color, len/2, true);
//  backHalf = fwdHalf;
}
