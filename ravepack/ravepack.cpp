
#include "ravepack.h"

#include <Arduino.h>
#include <memory>

#include "Display.h"
#include "PatternSelect.h"

//#include "AudioHardware.h"
//#include "FftAnalyzer.h"


namespace {
  void handleSerial();
  bool handleSerialLine(const char *line);

//  void handleBrightControl();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Main

PatternSelect gPatternSelect(&gDisplay);

void setup() {
  Serial.begin(115200);
  Serial << "RavePack starting; Compiled " __DATE__ "  " __TIME__ << endl;

  gPatternSelect.loadFromEepromAndAdvance();
//  gPatternSelect.selectPattern(1);
  Serial << "Starting with pattern: "
         << gPatternSelect.currentPatternIndex() << endl;

  // NOTE: LED PIN IS USED FOR AUDIO SHIELD. DO NOT USE.
//  gStatusLed.begin();
  gDisplay.begin();

//  audio_hardware_begin();
//  gFftAnalyzer.begin();
//  gBeatDetector.begin();

	// sanity check delay - allows reprogramming if accidently blowing power w/leds
//  delay(2000);
}

void loop() {
  gFpsGovernor.startFrame();
  {
    handleSerial();

//    handleBrightControl();

    gPatternSelect.currentPattern().loop();

//    gDisplay.raw().fill_solid(CRGB::White);
    gDisplay.show();

    gPowerGovernor.measureFrame(gDisplay.raw());
  }
  gFpsGovernor.endFrame();
}

namespace {
//  void flash(CRGB color, int count) {
//    FastLED.setBrightness(255);
//    gDisplay.raw().fill_solid(CRGB::Black);
//    gDisplay.show();
//    delay(200);
//    for (int i = 0; i < count; i++) {
//      gDisplay.raw().fill_solid(color);
//      gDisplay.show();
//      delay(100);
//      gDisplay.raw().fill_solid(CRGB::Black);
//      gDisplay.show();
//      delay(50);
//    }
//    delay(200);
//  }
//  void handleBrightControl() {
//    float knob = 1 - analogRead(A1)/1023.f;
//    FastLED.setBrightness(knob * 255);
//
////    EVERY_N_MILLISECONDS(1000) {
////      Serial << "Bright " << knob << endl;
////    };
//
//    // "Hit" knob on top limit 2 times to enable sound control
//    // "Hit" knob on top limit 3 times to disable sound control
//    // Setting is persisted across restart.
//
//    const int hitTimeout = 1500;
//    {
//      static elapsedMillis hitCounterTimeout;
//      static int numHits = 0;
//      static int state = 0;
//      if (state == 0 && knob > 0.99) {
//        Serial << "********* Bright knob HI ON" << endl;
//        state = 1;
//        hitCounterTimeout = 0;
//        numHits++;
//      } else if (state == 1 && knob < 0.98) {
//        Serial << "********* Bright knob HI OFF" << endl;
//        state = 0;
//      }
//      if (hitCounterTimeout > hitTimeout && numHits > 0) {
//        Serial << "********* Bright knob HI GOT " << numHits << " HITS ***********"
//               << endl;
//        if (numHits == 2) {
//          set_sound_enable(true);
//          flash(CRGB::Blue, 2);
//        } else if (numHits == 3) {
//          set_sound_enable(false);
//          flash(CRGB::Red, 3);
//        }
//
//        hitCounterTimeout = 0;
//        numHits = 0;
//      }
//    }
//    {
//      static elapsedMillis hitCounterTimeout;
//      static int numHits = 0;
//      static int state = 0;
//      if (state == 0 && knob < 0.01) {
//        Serial << "********* Bright knob LOW ON" << endl;
//        state = 1;
//        hitCounterTimeout = 0;
//        numHits++;
//      } else if (state == 1 && knob > 0.02) {
//        Serial << "********* Bright knob LOW OFF" << endl;
//        state = 0;
//      }
//      if (hitCounterTimeout > hitTimeout && numHits > 0) {
//        Serial << "********* Bright knob LOW GOT " << numHits << " HITS ***********"
//               << endl;
//        if (numHits == 2) {
//          set_sound_enable(true);
//          set_mic_gain(52);
//          flash(CRGB::Green, 2);
//        } else if (numHits == 3) {
//          set_sound_enable(true);
//          set_mic_gain(32);
//          flash(CRGB::White, 3);
//        }
//
//        hitCounterTimeout = 0;
//        numHits = 0;
//      }
//    }
//  }

  void handleSerial() {
    const int kBufSize = 128;
    static char buf[kBufSize];
    static int next = 0;

    while (Serial.available()) {
      int c = Serial.read();
      if (c == '\r') {
        continue;
      }

      if (next < kBufSize) {
        buf[next] = c;
        next++;
      }

      if (c == '\n') {
        if (next >= kBufSize) {
          Serial << "Line too long; exceeds " << kBufSize << endl;
          next = 0;

        } else {
          buf[next - 1] = '\0';  // remove newline, add null terminator
          next = 0;

          if (!handleSerialLine(buf)) {
            Serial << "Parse error: '" << buf << "'" << endl;
          }
        }
      }
    }
  }

  bool handleSerialLine(const char *line) {
//    const char ab[] = "ab ";
//    if (strncmp(line, ab, strlen(ab)) == 0) {
//      return gAnalyzeBeat.processSerial(line + strlen(ab));
//    }
//
//    const char bd[] = "bd ";
//    if (strncmp(line, bd, strlen(bd)) == 0) {
//      return gBeatDetector.processSerial(line + strlen(bd));
//    }

//    const char fft[] = "fft ";
//    if (strncmp(line, fft, strlen(fft)) == 0) {
//      return gFftAnalyzer.processSerial(line + strlen(fft));
//    }

    const char p[] = "p ";
    if (strncmp(line, p, strlen(p)) == 0) {
      return gPatternSelect.currentPattern().processSerial(line + strlen(p));
    }

    if (strcmp(line, "n") == 0 || strcmp(line, "p") == 0) {
      gPatternSelect.cyclePattern(line[0] == 'n' ? 1 : -1);
      Serial << "Switch to effect " << gPatternSelect.currentPatternIndex() << endl;
      return true;
    }

//    if (strcmp(line, "p") == 0) {
//      gEffects.CyclePalette();
//      Serial << "Switch palette" << endl;
//      return true;
//    }

    if (strcmp(line, "fps") == 0) {
      gFpsGovernor.setShowFps(!gFpsGovernor.isShowFps());
      return true;
    }

    // fallback: send to current pattern
    return gPatternSelect.currentPattern().processSerial(line);
  }
}
