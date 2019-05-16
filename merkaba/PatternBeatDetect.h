#pragma once

#include <FastLED.h>

#include "Display.h"
#include "DisplayBuffers.h"
#include "BasePattern.h"
#include "PaletteManager.h"

#include "AudioHardware.h"

class PatternPulse : public BasePattern {
private:
  int sparkle = 30;
  int sparkleFade = 20;
  int numSubsegs = 2;

  int vals[N_SEGMENTS_TOTAL][N_PER_SEGMENT];

public:

  PatternPulse(Display* d): BasePattern(d) {
    randomize();
  }

  virtual void loop() {
    EVERY_N_SECONDS(5) {
      gPaletteManager.nextPalette();
      randomize();
    };

    // NB can't call fftanalyzer step() or else beat detector will not see fft.available().
//    gBeatDetector.BeatDetectorLoop();

//    float intensity =
//              gSoundEnable ? gFftAnalyzer.totalPeak()
//                           : 1.f;

//    auto raw = display_->raw();
//    if (gBeatDetector.lowBeat) {
//      for (int i = 0; i < N_SEGMENTS_PER_SHAPE; ++i) {
//        gOctaSegments[i].raw().fill_solid(CRGB::Red);
//      }
//    }
//
//    if (gBeatDetector.midBeat) {
//      for (int i = 0; i < N_SEGMENTS_PER_SHAPE; ++i) {
//        gMiniTetraSegments[i].raw().fill_solid(CRGB::Yellow);
//      }
//    }
//
//    if (gBeatDetector.highBeat) {
//      for (int i = 0; i < N_SEGMENTS_PER_SHAPE; ++i) {
//        gMiniTetraSegments[N_SEGMENTS_PER_SHAPE + i].raw().fill_solid(CRGB::White);
//      }
//    }
//
//    raw.nscale8(245);
  }

  void randomize() {
  }

  virtual bool processSerial(const char *line) {
//    if (sscanf(line, "s %d", &sparkle) == 1) {
//      Serial << "sparkle " << sparkle << endl;
//      return true;
//    }

    if (strcmp(line, "r") == 0) {
      Serial << "randomize" << endl;
      randomize();
      return true;
    }
    return false;
  }
};
