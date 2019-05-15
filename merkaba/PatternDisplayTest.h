#pragma once

#include "Display.h"
#include "BasePattern.h"
#include "DisplayBuffers.h"

class PatternDisplayTest : public BasePattern {
public:
  int seg_ = 0;
  int led_ = 0;
  int col_ = 0;

  PatternDisplayTest(Display* display) : BasePattern(display) { }

  void loop() {
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

    display_->raw().fill_solid(colors[col_]);

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
