
// NOTES:
// To use Serial3 (pin 8 data for WS2812SERIAL), must compile with 120MHz
// overclock and latest version of Arduino/Teensyduino.


#include "merkaba.h"


#define __ASSERT_USE_STDERR

#include <assert.h>


#include <Arduino.h>
#include <Adafruit_SleepyDog.h>
#include <memory>

#include "Display.h"
#include "PatternSelect.h"
#include "PaletteManager.h"

#include "AudioHardware.h"
#include "FftAnalyzer.h"


namespace {
  void handleSerial();
  bool handleSerialLine(const char *line);
  void seedRngWithAudio();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Main

PatternSelect gPatternSelect(&gDisplay);
bool gAutoAdvance = true;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial << "Merkaba starting; Compiled " __DATE__ "  " __TIME__ << endl;

  audio_hardware_begin();
  seedRngWithAudio();

  int countdownMS = Watchdog.enable(1000);
  Serial << "Watchdog: " << countdownMS << "ms" << endl;

//  delay(1000);
//  int x = analogRead(A1);
//  Serial << "Seed " << x << endl;
//  randomSeed(x);
//  delay(1000);

  gPaletteManager.begin();
//  gPatternSelect.loadFromEepromAndAdvance();
  gPatternSelect.selectPattern(0);
  Serial << "Starting with pattern: "
         << gPatternSelect.currentPatternIndex() << endl;

  gStatusLed.begin();
  gDisplay.begin();

  gFftAnalyzer.begin();
//  gBeatDetector.begin();

	// sanity check delay - allows reprogramming if accidently blowing power w/leds
//  delay(2000);
}

void loop() {
  Watchdog.reset();

  gFpsGovernor.startFrame();
  {
    handleSerial();

    EVERY_N_SECONDS(30) {
      if (gAutoAdvance) {
        gPatternSelect.randomPattern();
      }
    }

    gPaletteManager.loop();
    gPatternSelect.currentPattern().loop();

    EVERY_N_MILLISECONDS(250) {
      gStatusLed.blink();
    }

    gDisplay.show();

    gPowerGovernor.measureFrame(gDisplay.raw());
  }
  gFpsGovernor.endFrame();
}

namespace {
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
    {
      const char sound[] = "sound ";
      if (strncmp(line, sound, strlen(sound)) == 0) {
        int enable;
        if (sscanf(line + strlen(sound), "%d", &enable)) {
          set_sound_enable(enable);
          Serial << "Sound enable: " << (enable != 0) << endl;
        }
        return true;
      }
    }

    {
      const char fft[] = "fft ";
      if (strncmp(line, fft, strlen(fft)) == 0) {
        return gFftAnalyzer.processSerial(line + strlen(fft));
      }
    }

    {
      const char c[] = "c ";
      if (strncmp(line, c, strlen(c)) == 0) {
        return gPaletteManager.processSerial(line + strlen(c));
      }
    }

    {
      if (strcmp(line, "n") == 0 || strcmp(line, "p") == 0) {
        gPatternSelect.cyclePattern(line[0] == 'n' ? 1 : -1);
        Serial << "Switch to effect " << gPatternSelect.currentPatternIndex()
               << endl;
        return true;
      }
    }

    {
      const char p[] = "pn ";
      if (strncmp(line, p, strlen(p)) == 0) {
        return gPatternSelect.currentPattern().processSerial(line + strlen(p));
      }
    }

    {
      float gamma;
      int maxValue0;
      if (sscanf(line, "gamma %f %d", &gamma, &maxValue0) == 2) {
        gDisplay.setGamma(gamma, maxValue0);
        return true;
      }
    }

    {
      int gain;
      if (sscanf(line, "gain %d", &gain) == 1) {
        set_mic_gain((Gain) gain);
        return true;
      }

      int ar;
      if (sscanf(line, "ar %d", &ar) == 1) {
        set_mic_ar((AttackRelease) ar);
        return true;
      }
    }

    {
      int v;
      if (sscanf(line, "auto %d", &v) == 1) {
        gAutoAdvance = (v != 0);
        Serial.println("ok"); return true;
      }
    }

    {
      int v;
      if (sscanf(line, "sleep %d", &v) == 1) {
        Serial.println("ok...");
        delay(v);
        Serial.println("ok");
        return true;
      }
    }

    if (strcmp(line, "fps") == 0) {
      gFpsGovernor.setShowFps(!gFpsGovernor.isShowFps());
      return true;
    }

    // fallback: send to current pattern
    return gPatternSelect.currentPattern().processSerial(line);
  }


  AudioRecordQueue recordQueue;
  AudioConnection patch(gAudioInput, recordQueue);
  void seedRngWithAudio() {
    // read a few samples from audio input for random seed
    recordQueue.begin();
    while (!recordQueue.available()) {
      delay(1);
    }
    int16_t* buf = recordQueue.readBuffer();

    int seed = (buf[0] << 16) | buf[1];

    Serial << "Seed: " << _HEX(seed) << endl;
    recordQueue.clear();
    recordQueue.end();
  }
}


// misc utils

// handle diagnostic informations given by assertion and abort program execution:
void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp) {
  // transmit diagnostic informations through serial link.
  Serial.println(__func);
  Serial.println(__file);
  Serial.println(__lineno, DEC);
  Serial.println(__sexp);
  Serial.flush();

  // abort program execution.
  abort();
}

// not sure why needed: https://forum.pjrc.com/threads/49802-STL-and-undefined-reference-to-std-__throw_bad_alloc()
namespace std {
  void __throw_bad_alloc() {
    Serial.println("Unable to allocate memory");
    abort();
  }
  void __throw_length_error( char const*e ) {
    Serial.print("Length Error :"); Serial.println(e);
    abort();
  }
}
