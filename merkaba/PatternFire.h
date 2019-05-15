#pragma once

#include <FastLED.h>

#include "Display.h"
#include "DisplayBuffers.h"
#include "BasePattern.h"

#include "AudioHardware.h"

DEFINE_GRADIENT_PALETTE( fire_gp ) {
    0x00,     0,  0,  0,   //black
    0x55,   255,  0,  0,   //red
    0xAA,   255,255,  0,   //bright yellow
    0xFF,   255,255,255 }; //full white

// Gradient palette "bhw1_03_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_03.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.
DEFINE_GRADIENT_PALETTE( bhw1_03_gp ) {
    0,   0,  0,  0,
    0x55,  0x00,0x20,0x90,
    0xAA,  40,219,105,
    0xFF, 255,255,255};

// Gradient palette "es_emerald_dragon_11_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/emerald_dragon/tn/es_emerald_dragon_11.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.
DEFINE_GRADIENT_PALETTE( es_emerald_dragon_11_gp ) {
    0,   1,  8,  1,
    170,  78,156,  2,
    220, 197,250, 78,
    255, 229,244,192};

static CRGBPalette16 palettes[] = {
    fire_gp,
//    CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White),
    bhw1_03_gp,
    es_emerald_dragon_11_gp,
};

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

  int curPaletteId = 0;

//  static constexpr int HEIGHT = 15;
  int w_, h_;

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
//    w_ = 1; h_ = miniTetraBuf.data().size();
    w_ = 1;
    h_ = miniTetraBuf.data().size() + N_CENTER;
    heat.reset(new byte[w_ * h_]);
    mask.reset(new byte[display_->raw().size()]);
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
      // Change the target palette to a random one every 5 seconds.
      curPaletteId = (curPaletteId + 1) % (sizeof(palettes)/sizeof(palettes[0]));
      targetPalette = palettes[curPaletteId];

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
//        // Recommend that you use values 0-240 rather than
//        // the usual 0-255, as the last 15 colors will be
//        // 'wrapping around' from the hot end to the cold end,
//        // which looks wrong.
        byte colorIndex = scale8(heat[XY(x, y)], 240);
//        byte colorIndex = heat[XY(x,h_ )];

        int i = y;
//        int i = h_ - 1 - y;

        // override color 0 to ensure a black background?
        if (y <= edge.size()) {
          edge[i] = (colorIndex == 0)
                    ? CRGB::Black
                    : ColorFromPalette(currentPalette, colorIndex);
//        : ColorFromPalette(HeatColors_p, colorIndex);
        }
      }
    }

    for (int i = 0; i < 12; i++) {
      auto& seg = gOctaSegments[i];
      for (int i = 0; i < N_CENTER; ++i) {
        seg[i] = seg[N_PER_SEGMENT-1 - i] =
            ColorFromPalette(currentPalette,
                scale8(heat[XY(0, N_PER_SEGMENT + i)], 240));

        // copy to middle led
        if (i == N_CENTER - 1) {
          seg[N_CENTER] = seg[i];
        }
      }
//      seg.raw().fill_solid(ColorFromPalette(currentPalette,
//          map(intensity, 0, 1, 50, 254)));

//      seg.raw().fill_solid(ColorFromPalette(currentPalette,
//                                            heat[XY(0,h_-2)]));
    }
    miniTetraBuf.apply(display_, miniTetras);


    auto raw = display_->raw();

//    float intensity = 0.7;
//    float intensity = 1;
//    raw.nscale8(intensity * 255);
//    buf_.apply(display_);

    // sparkle mask
    for (int i = 0; i < raw.size(); ++i) {
      // fade to black
      mask[i] = scale8(mask[i], 255 - sparkleFade);

      if (random8() < sparkle) {
        mask[i] = 255;
      }

      raw[i].nscale8(mask[i]);
    }

    for (auto& led : raw) {
      led.nscale8(map(intensity, 0,1, 40,255));
    }
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

    if (strcmp(line, "r") == 0) {
      Serial << "randomize" << endl;
      randomize();
      return true;
    }
    return false;
  }
};
