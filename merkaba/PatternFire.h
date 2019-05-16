#pragma once

#include <FastLED.h>

#include "Display.h"
#include "DisplayBuffers.h"
#include "BasePattern.h"
#include "Palettes.h"

#include "AudioHardware.h"

class PatternFire : public BasePattern {
private:
  // Array of temperature readings at each simulation cell
  std::unique_ptr<byte[]> heat;

  // sparkle pattern mask
  std::unique_ptr<byte[]> mask;

  std::vector<int> miniTetras = {0,1,2,3,4,5,6,7};

  MiniTetraLegsByHeightBuffer miniTetraBuf;

  CRGBPalette16 currentPalette;
  CRGBPalette16 targetPalette = bhw1_03_gp;

//  int curPaletteId = 0;

  int w_, h_;

  // generate flames for one tetra leg + half a tetra base
  // these are mirrored to all tetras.
  static constexpr int N_CENTER = 7;

public:
  // There are two main parameters you can play with to control the look and
  // feel of your fire: COOLING (used in step 1 above), and SPARKING (used
  // in step 3 above).
  //
  // cooling: How much does the air cool as it rises?
  // Less cooling = taller flames.  More cooling = shorter flames.
  // Default 55, suggested range 20-100
  int cooling = 80;

  // sparking: What chance (out of 255) is there that a new spark will be lit?
  // Higher chance = more roaring fire.  Lower chance = more flickery fire.
  // Default 120, suggested range 50-200.
  int sparking = 150;

  int sparkle = 30;
  int sparkleFade = 20;

  PatternFire(Display* d): BasePattern(d) {
    w_ = 1;
    h_ = miniTetraBuf.data().size() + N_CENTER;
    heat.reset(new byte[w_ * h_]);
    mask.reset(new byte[display_->raw().size()]);

    nextPalette();
    randomize();
  }

  int XY(int x, int y) {
    if (y >= h_) { y = h_ - 1; }
    if (y < 0) { y = 0; }
    if (x >= w_) { x = w_ - 1; }
    if (x < 0) { x = 0; }
    return (y * w_) + x;
  }

  virtual void loop() {
    nblendPaletteTowardPalette(currentPalette, targetPalette, 48);

    EVERY_N_SECONDS(5) {
      nextPalette();
      randomize();
    };

    gFftAnalyzer.step();
    float intensity =
              gSoundEnable ? gFftAnalyzer.totalPeak()
                           : 1.f;
    if (gSoundEnable) {
      sparking = map(intensity, 0, 1, 50, 200);
      cooling = map(intensity, 0, 1, 100, 20);
    } else {
      sparking = 150;
      cooling = 80;
    }

    for (int x = 0; x < w_; x++) {
      // Step 1.  Cool down every cell a little
      for (int y = 0; y < h_; y++) {
        heat[XY(x,y)] = qsub8(heat[XY(x,y)],
                              random(0, ((cooling * 10) / h_) + 2));
      }

      // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      for (int y = 0; y < h_; y++) {
        heat[XY(x,y)] =
            (heat[XY(x, y + 1)]
             + heat[XY(x, y + 2)]
             + heat[XY(x, y + 2)]) / 3;
      }

      // Step 3.  Randomly ignite new 'sparks' of heat
      if (random(255) < (unsigned int) sparking) {
        heat[XY(x, h_-1)] = qadd8(heat[XY(x, h_-1)],
                                      random(160, 255));
      }

      // Step 4.  Map from heat cells to LED colors

      CRGBSet edge = miniTetraBuf.data();
      for (int y = 0; y < edge.size(); y++) {
        // Recommend that you use values 0-240 rather than
        // the usual 0-255, as the last 15 colors will be
        // 'wrapping around' from the hot end to the cold end,
        // which looks wrong.
        // See: https://github.com/FastLED/FastLED/issues/515
        byte colorIndex = scale8(heat[XY(x, y)], 240);

        edge[y] = ColorFromPalette(currentPalette, colorIndex);
//        // override color 0 to ensure a black background?
//        edge[y] = (colorIndex == 0)
//                  ? CRGB::Black
//                  : ColorFromPalette(currentPalette, colorIndex);
      }
    }

    for (int i = 0; i < 12; i++) {
      auto& seg = gOctaSegments[i];
      for (int i = 0; i < N_CENTER; ++i) {
        seg[i] = seg[N_PER_SEGMENT-1 - i] =
            ColorFromPalette(currentPalette,
//                             heat[XY(0, N_PER_SEGMENT + i)]);
                             scale8(heat[XY(0, N_PER_SEGMENT + i)], 240));

        // copy to middle led
        if (i == N_CENTER - 1) {
          seg[N_CENTER] = seg[i];
        }
      }
    }
    miniTetraBuf.apply(display_, miniTetras);

    auto raw = display_->raw();

    // sparkle mask
    for (int i = 0; i < raw.size(); ++i) {
      // fade to black
      mask[i] = scale8(mask[i], 255 - sparkleFade);

      if (random8() < sparkle) {
        mask[i] = 255;
      }

      raw[i].nscale8(mask[i]);
    }

    // intensity global brightness mask
    raw.nscale8(map(intensity, 0,1, 40,255));
  }

  void nextPalette() {
//    curPaletteId = (curPaletteId + 1) % gNumPalettes;
//    targetPalette = gPalettes[curPaletteId];
    targetPalette = gPalettes[random(0, gNumPalettes)];
  }

  void randomize() {
    sparkle = random(3, 120);
    sparkleFade = random(5, 50);
    Serial << "mask sparkle " << sparkle << endl;
    Serial << "mask sparkleFade " << sparkleFade << endl;
  }

  virtual bool processSerial(const char *line) {
    if (sscanf(line, "c %d", &cooling) == 1) {
      Serial << "Cooling " << cooling << endl;
      return true;
    }
    if (sscanf(line, "s %d", &sparking) == 1) {
      Serial << "Sparking " << sparking << endl;
      return true;
    }

    if (sscanf(line, "m1 %d", &sparkle) == 1) {
      Serial << "mask sparkle " << sparkle << endl;
      return true;
    }
    if (sscanf(line, "m2 %d", &sparkleFade) == 1) {
      Serial << "mask sparkleFade " << sparkleFade << endl;
      return true;
    }

    int targetPaletteId;
    if (sscanf(line, "p %d", &targetPaletteId) == 1) {
      Serial << "palette " << targetPaletteId << endl;
      targetPalette = gPalettes[targetPaletteId];
      return true;
    }

    if (strcmp(line, "r") == 0) {
      Serial << "randomize" << endl;
      randomize();
      return true;
    }


    // dump palette
    for (int i = 0; i < 256; ++i) {
      Serial << i << " ";
      printRGB(ColorFromPalette(currentPalette, i));
      Serial << endl;
    }

    return false;
  }
};
