#pragma once

#include <vector>
#define __ASSERT_USE_STDERR

#include <assert.h>

#include <Arduino.h>

#include "Streaming.h"
#include "Display.h"

///
// Drawing utils

inline int revIndex(CRGBSet leds, int i, bool rev) {
  return rev ? leds.size() - 1 - i
      : i;
}

inline CRGB& rev(CRGBSet leds, int i, bool rev) {
  return rev ? leds[leds.size() - 1 - i]
             : leds[i];
}

//// Warning: 'rev' doesn't work if CRGBSet is reversed!
//void drawBar(CRGBSet leds, CRGB color, float len, bool rev=false) {
//  len = min(len, leds.size());
//  int lenFloor = floor(len);
//  int lenCeil = ceil(len);
//
////    if (rev)
////      leds = leds(leds.size() - 1, 0);
//
//  if (lenFloor > 0) {
//    leds(revIndex(leds, 0, rev),
//         revIndex(leds, lenFloor - 1, rev)).fill_solid(color);
//  }
//  if (lenCeil != lenFloor) {
//    int i = revIndex(leds, lenCeil - 1, rev);
//    leds[i] = color;
//    leds[i].nscale8((len - lenFloor) * 255);
//  }
//}
//
//void drawCenterBar(CRGBSet leds, CRGB color, float len) {
//  // FIXME: Odd #s?
////    drawBar(
////        CRGBSet(leds, leds.size()/2-1, 0),
////        color, len/2);
//  CRGBSet fwdHalf(leds, leds.size()/2, leds.size()-1);
//  drawBar(fwdHalf, color, len/2);
//  CRGBSet backHalf(leds, 0, leds.size()/2-1);
//  drawBar(backHalf, color, len/2, true);
////  backHalf = fwdHalf;
//}


/////////

class Segment {
public:
  int segmentIdx;
  bool reverse;

  Segment(int segmentNum_, bool reverse_)
      : segmentIdx(segmentNum_-1), reverse(reverse_) {
  }

  CRGBSet raw() {
    return gDisplay.getRawSegment(segmentIdx);
  }

  CRGB& operator[](int led) {
    return rev(raw(), led, reverse);
  }

  void copyFrom(CRGBSet& set) {
    assert(set.size() == N_PER_SEGMENT);
    for (int i = 0 ; i < N_PER_SEGMENT; ++i) {
      operator[](i) = set[i];
    }
  }
};
extern Segment gAllSegments[];
extern Segment* const  gOctaSegments;
extern Segment* const  gMiniTetraSegments;


class MiniTetraLegsByHeightBuffer {
  CRGB data_[N_PER_SEGMENT];

public:
  CRGBSet data() {  // top to bottom
    return CRGBSet(data_, N_PER_SEGMENT);
  }

  // mini tetras in [0..7]: 0-3 = down tetra, 4-7 = up tetra
  void apply(Display* display, std::vector<int>& miniTetras) {
    CRGBSet x = data();
    for (int miniTetra : miniTetras) {
      for (int segment = 0; segment < 3; segment++) {
        gMiniTetraSegments[miniTetra * 3 + segment].copyFrom(x);
      }
    }
  }
};
