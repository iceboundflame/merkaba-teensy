#pragma once

#include <Arduino.h>

#include <WS2812Serial.h>
#define USE_WS2812SERIAL
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

constexpr int DATA_STRAND0 = 1;   // down tetrahedron
constexpr int DATA_STRAND1 = 10;  // up tetrahedron
constexpr int DATA_STRAND2 = 8;   // octahedron
constexpr int N_PER_STRAND = 180;
constexpr int N_ALL = (N_PER_STRAND * 3);
constexpr int N_PER_SEGMENT = 15;
constexpr int N_SEGMENTS_PER_SHAPE = 12;
constexpr int N_SEGMENTS_TOTAL = 36;

constexpr int VOLTS = 5;
// divide by 2 since LEDs are double sided
constexpr int MAX_MILLIAMPS = 10000 / 2;
constexpr int BATTERY_MILLIWATT_HOURS = (216000 * 0.75);



class Display {
public:
  void begin() {
    // WS2812SERIAL?
    {
      auto &ctl = FastLED.addLeds<WS2812SERIAL, DATA_STRAND0, BRG>(
          getStrand(0), N_PER_STRAND);
      ctl.setCorrection(TypicalLEDStrip);
    }
    {
      auto &ctl = FastLED.addLeds<WS2812SERIAL, DATA_STRAND1, BRG>(
          getStrand(1), N_PER_STRAND);
      ctl.setCorrection(TypicalLEDStrip);
    }
    {
      auto &ctl = FastLED.addLeds<WS2812SERIAL, DATA_STRAND2, BRG>(
          getStrand(2), N_PER_STRAND);
      ctl.setCorrection(TypicalLEDStrip);
    }

    FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, MAX_MILLIAMPS);
  }

  /**
   * Each face has two vertical legs, then the base leg.
   * Pixel order starts from bottom of right leg, CCW.
   */
  CRGBSet getStrand(int i) {
    return CRGBSet(leds_ + i * N_PER_STRAND, N_PER_STRAND);
  }

  CRGBSet getRawSegment(int i) {
//    Serial << "Segment " << i << endl;
    return CRGBSet(leds_ + i * N_PER_SEGMENT, N_PER_SEGMENT);
  }

//  CRGBSet getTetrahedral(int i) {
//
//    return getSegment()
//  }

//  CRGBSet getOctahedral(int i) {
//    return getSegment(gOctaSegments[i].segmentIdx);
//  }

  CRGBSet raw() { return CRGBSet(leds_, N_ALL); }

  void show() {
    // hacky gamma correction
    for (auto& rgb : raw()) {
      rgb.nscale8(rgb);
    }
//    raw().napplyGamma_video(2.2);

    FastLED.show();
  }

private:

  CRGB leds_[N_ALL];
};


//////////


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

    frameMw *= 2;  // we have double-sided LEDs.

    mwSum += frameMw;
    mwMax = max(mwMax, frameMw);

    const int kPrintEvery = 1500;
    loopN_++;
    if ((loopN_ % kPrintEvery == 0) && showPower_) {
      float mwAvg = mwSum / kPrintEvery;

      Serial << "Average power usage: "
             << mwAvg / VOLTS /1000 << " A; "
             << mwAvg /1000 << " W; "
             << (BATTERY_MILLIWATT_HOURS / mwAvg) << " h runtime"
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
