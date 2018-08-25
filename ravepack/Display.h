#pragma once

#include <Arduino.h>

//#include <WS2812Serial.h>
//#define USE_WS2812SERIAL
#include <FastLED.h>

#include "Streaming.h"


// utils

inline void printRGB(const CRGB& rgb) {
  Serial << _HEX(rgb.r) << ":"
         << _HEX(rgb.g) << ":"
         << _HEX(rgb.b);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Display

#define DATA_STRAND0 5   // strap
#define DATA_STRAND1 6   // strap
#define DATA_STRAND2 21  // back
#define DATA_STRAND3 8   // top/sides

#define N_STRAND0 60
#define N_STRAND1 60
#define N_STRAND2 41
#define N_STRAND3 102
#define N_ALL N_STRAND0 + N_STRAND1 + N_STRAND2 + N_STRAND3

#define VOLTS 5
#define MAX_MILLIAMPS 3500

class Display {
public:
  void begin() {
    // WS2812SERIAL?
    {
      auto &ctl = FastLED.addLeds<WS2812B, DATA_STRAND0, GRB>(
          strands_[0], strands_[0].size());
      ctl.setCorrection(TypicalLEDStrip);
    }
    {
      auto &ctl = FastLED.addLeds<WS2812B, DATA_STRAND1, GRB>(
          strands_[1], strands_[1].size());
      ctl.setCorrection(TypicalLEDStrip);
    }
    {
      auto &ctl = FastLED.addLeds<WS2812B, DATA_STRAND2, GRB>(
          strands_[2], strands_[2].size());
      ctl.setCorrection(TypicalLEDStrip);
    }
    {
      auto &ctl = FastLED.addLeds<WS2812B, DATA_STRAND3, GRB>(
          strands_[3], strands_[3].size());
      ctl.setCorrection(TypicalLEDStrip);
    }

    FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_MILLIAMPS);
  }

  /**
   * Each face has two vertical legs, then the base leg.
   * Pixel order starts from bottom of right leg, CCW.
   */
  CRGBSet& getStrand(int i) {
    return strands_[i];
  }

  CRGBSet raw() { return CRGBSet(leds_, N_ALL); }

  void show() {
//    // hacky gamma correction
//    for (auto& rgb : raw()) {
//      rgb.nscale8_video(rgb);
//    }

    // black out bad pixel
    strands_[1][0] = CRGB::Black;
    FastLED.show();
  }

private:

  CRGB leds_[N_ALL];
  CRGBSet strands_[4] = {
      CRGBSet(leds_, N_STRAND0),
      CRGBSet(leds_ + N_STRAND0, N_STRAND1),
      CRGBSet(leds_ + N_STRAND0 + N_STRAND1, N_STRAND2),
      CRGBSet(leds_ + N_STRAND0 + N_STRAND1 + N_STRAND2, N_STRAND3)
  };
};


constexpr int LED_PIN = 13;

class StatusLed {
public:
  void begin() {
    pinMode(LED_PIN, OUTPUT);
  }

  void blink() {
    set(!ledState_);
  }
  void set(bool ledState) {
    digitalWrite(LED_PIN, ledState ? LOW : HIGH);
    ledState_ = ledState;
  }

private:
  bool ledState_ = false;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class FpsGovernor {
  long start_;
  bool showFps_ = false;

  int loopN_ = 0;
  long lastPrint_ = 0;

public:
  void startFrame() {
    start_ = micros();
  }
  void endFrame() {
    constexpr long kDesiredLoopMicros = 1000000 / 60;
    const int kPrintEvery = 50;

    long elapsed = micros() - start_;
    long delay = kDesiredLoopMicros - elapsed;
    if (delay > 0) {
      delayMicroseconds(delay);
    }

    loopN_++;
    if ((loopN_ % kPrintEvery == 0) && showFps_) {
      long elapsedSinceLastPrint = micros() - lastPrint_;
      Serial << "fps: "
             << (1000000 * kPrintEvery / (float)elapsedSinceLastPrint)
             << " avg / "
             << (1000000 / (float)elapsed)
             << endl;
      lastPrint_ = micros();
    }
  }

  void setShowFps(bool showFps) {
    showFps_ = showFps;
  }
  bool isShowFps() {
    return showFps_;
  }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class PowerGovernor {
  bool showPower_ = true;
  int loopN_ = 0;

  float mwSum = 0;
  float mwMax = 0;

public:
  void measureFrame(CRGBSet leds) {
    uint32_t frameMw = min(
        calculate_unscaled_power_mW(leds, leds.size()),
        (uint32_t) (VOLTS * MAX_MILLIAMPS));
    mwSum += frameMw;
    mwMax = max(mwMax, frameMw);

    const int kPrintEvery = 1500;
    loopN_++;
    if ((loopN_ % kPrintEvery == 0) && showPower_) {
      float mwAvg = mwSum / kPrintEvery;

      Serial << "Average power usage: "
             << mwAvg / VOLTS /1000 << " A; "
             << mwAvg /1000 << " W; "
             << (83600 * 0.75 / mwAvg) << " h runtime"
             << "\n";
      Serial << "   Peak power usage: "
             << mwMax / VOLTS /1000 << " A; "
             << mwMax /1000 << " W"
             << "\n";

      mwSum = 0;
      mwMax = 0;
    }
  }

  void setShowPower(bool showPower) {
    showPower_ = showPower;
  }
  bool isShowPower() {
    return showPower_;
  }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern StatusLed gStatusLed;
extern Display gDisplay;
extern FpsGovernor gFpsGovernor;
extern PowerGovernor gPowerGovernor;
