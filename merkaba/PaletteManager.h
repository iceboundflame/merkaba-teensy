#pragma once

#include <FastLED.h>
#include "Streaming.h"
#include "Display.h"
#include "util.h"

extern CRGBPalette16 gPalettes[];
extern int gNumPalettes;

class PaletteManager {
  CRGBPalette16 currentPalette_;
  CRGBPalette16 targetPalette_;

  int timeConstant_ = 48;
  int curPaletteId_ = 0;

public:
  PaletteManager() {
    currentPalette_ = targetPalette_ = gPalettes[random(0, gNumPalettes)];
  }

  void nextPalette() {
//    curPaletteId_ = (curPaletteId + 1) % gNumPalettes;
    curPaletteId_ = randomExcluding(0, gNumPalettes, curPaletteId_);
    targetPalette_ = gPalettes[curPaletteId_];

    Serial << "next palette: " << curPaletteId_ << endl;
  }

  void loop() {
    nblendPaletteTowardPalette(currentPalette_, targetPalette_, timeConstant_);

    // we'll let the pattern call this.
//    EVERY_N_SECONDS(5) {
//      nextPalette();
//    };
  }

  bool processSerial(const char *line) {
    if (sscanf(line, "%d", &curPaletteId_) == 1) {
      targetPalette_ = gPalettes[curPaletteId_];
      Serial << "choose palette: " << curPaletteId_ << endl;
      return true;
    }

    if (strcmp(line, "n") == 0) {
      nextPalette();
      return true;
    }

    if (strcmp(line, "dump") == 0) {
      // dump palette
      for (int i = 0; i < 256; ++i) {
        Serial << i << " ";
        printRGB(ColorFromPalette(currentPalette_, i));
        Serial << endl;
      }
      return true;
    }

    return false;
  }

  CRGBPalette16& current() {
    return currentPalette_;
  }
//  CRGB color(uint8_t val) {
//    return ColorFromPalette(currentPalette_, val);
//  }
  CRGB colorNoWrap(uint8_t val) {
    return ColorFromPalette(currentPalette_, scale8(val, 240));
  }

};

extern PaletteManager gPaletteManager;