#pragma once

#include <FastLED.h>

#include "Display.h"
#include "DisplayBuffers.h"
#include "BasePattern.h"
#include "PaletteManager.h"

#include "AudioHardware.h"

class PatternLines : public BasePattern {
private:
  int sparkle = 30;
  int sparkleFade = 20;
  int numSubsegs = 2;

  int vals[N_SEGMENTS_TOTAL][N_PER_SEGMENT];

public:
  PatternLines(Display* d): BasePattern(d) {
    randomize();
  }

  virtual void loop() {
    EVERY_N_SECONDS(5) {
      gPaletteManager.nextPalette();
      randomize();
    };

    gFftAnalyzer.step();
    float intensity =
              gSoundEnable ? gFftAnalyzer.totalPeak()
                           : 1.f;

    auto raw = display_->raw();

    int subsparkle = map(intensity, 0,1, 1,sparkle * 1.5);

    for (int i = 0; i < N_SEGMENTS_TOTAL; ++i) {
      for (int subseg = 0; subseg < numSubsegs; ++subseg) {
        if (random8() < subsparkle / numSubsegs) {
          int from = N_PER_SEGMENT / numSubsegs * subseg;
          int to = N_PER_SEGMENT / numSubsegs * (subseg + 1);

          // assign leftover leds to last segment
          if (subseg == numSubsegs-1) {
            to = N_PER_SEGMENT;
          }

          //        int from = random(0, N_PER_SEGMENT/2);
          //        int to = N_PER_SEGMENT - random(1, N_PER_SEGMENT/2);

          for (int j = from; j < to; ++j) {
            vals[i][j] = 255;
          }
        }
      }
    }

    for (int i = 0; i < N_SEGMENTS_TOTAL; ++i) {
      Segment seg = gAllSegments[i];
      for (int j = 0; j < N_PER_SEGMENT; ++j) {
        seg[j] = gPaletteManager.colorNoWrap(vals[i][j]);

        // fade to black
        vals[i][j] = scale8(vals[i][j], 255 - sparkleFade);
      }
    }

    for (auto& led : raw) {
      led.nscale8(map(intensity, 0,1, 40,255));
    }
  }

  void randomize() {
    int subsegChoices[] = {1,1,2,2,3,5};
    numSubsegs = subsegChoices[random(0, 6)];

    sparkle = random(3, 12);

    float rnd = random8()/255.;
    rnd = pow(rnd, 3); // rescale to make low values more likely
    sparkleFade = map(rnd, 0, 255, 5, 50);

    Serial << "subsegments " << numSubsegs << endl;
    Serial << "sparkle " << sparkle << endl;
    Serial << "sparkleFade " << sparkleFade << endl;
  }

  virtual bool processSerial(const char *line) {
    if (sscanf(line, "s %d", &sparkle) == 1) {
      Serial << "sparkle " << sparkle << endl;
      return true;
    }
    if (sscanf(line, "f %d", &sparkleFade) == 1) {
      Serial << "sparkleFade " << sparkleFade << endl;
      return true;
    }
    if (sscanf(line, "sub %d", &numSubsegs) == 1) {
      Serial << "subsegments " << numSubsegs << endl;
      return true;
    }

    if (strcmp(line, "r") == 0) {
      Serial << "randomize" << endl;
      randomize();
      return true;
    }
    return false;
  }
};
