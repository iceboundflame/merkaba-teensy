#pragma once

#include <algorithm>
#include <assert.h>

#include "Display.h"
#include "BasePattern.h"
#include "DisplayBuffers.h"

class PatternDisplayTest : public BasePattern {
public:
  int seg_ = 0;
  int led_ = 0;
  int col_ = 0;

  std::vector<int> tetras;

  MiniTetraLegsByHeightBuffer miniTetraBuf;

  PatternDisplayTest(Display* display) : BasePattern(display) { }

  void loop() {
    miniTetraBuf.data().fill_rainbow(0, 256/15);

    miniTetraBuf.apply(display_, tetras);
    return;

    CRGB colors[] = {
        CRGB::Red,
        CRGB::Orange,
        CRGB::Yellow,
        CRGB::Green,
        CRGB::Cyan,
        CRGB::Blue,
        CRGB::Magenta,
        CRGB::White,
        CRGB::Magenta,
        CRGB::Blue,
        CRGB::Cyan,
        CRGB::Green,
    };

    display_->raw().fill_solid(CRGB::Black); //colors[col_]);

//    if (seg_ == 36) {
    if (seg_ >= N_SEGMENTS_PER_SHAPE) {
      display_->raw().fill_solid(CRGB::White);
      seg_ = 0;
      led_ = 0;
    } else {
      gOctaSegments[seg_][led_] = colors[seg_];
//      gDisplay.getRawSegment(seg_)[led_] = CRGB::Red;
//      gDisplay.raw()[seg_ * 15 + led_] = CRGB::Red;

      led_ = (led_ + 1) % (N_PER_SEGMENT);
//      if (led_ == 0) seg_++;
    }

//    delay(96);
    delay(6);
  }

  virtual bool processSerial(const char* line) {
    {
      int idx;
      if (sscanf(line, "x%d", &idx) == 1) {
        if (std::find(tetras.begin(), tetras.end(), idx) != tetras.end()) {
          tetras.erase(std::remove(tetras.begin(), tetras.end(), idx),
              tetras.end());

          Serial << "Remove MiniTetra " << idx << endl;
        } else {
          tetras.push_back(idx);

          Serial << "Add MiniTetra " << idx << endl;
        }
        return true;
      }
    }

    if (sscanf(line, "c%d", &col_) == 1) {
      Serial << "color " << col_ << endl;
      return true;
    }

    if (sscanf(line, "%d", &seg_) == 1) {
      Serial << "Segment " << seg_ << endl;
      return true;
    }
    return false;

  }
};
