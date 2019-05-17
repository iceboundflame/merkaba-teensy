#pragma once

#include <Arduino.h>

#include <WS2812Serial.h>
#define USE_WS2812SERIAL
#include <FastLED.h>

#include "Streaming.h"
#include "util.h"

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
constexpr int MAX_MILLIAMPS = 8000 / 2;
constexpr int BATTERY_MILLIWATT_HOURS = (216000 * 0.75);

constexpr float FRAME_RATE = 60;


class Display {
public:
  void begin();

  // maxValue0 is the largest input value that is allowed to be rendered as 0
  // (full off).
  void setGamma(float gamma, int maxValue0) {
    for (int i = 0; i < 256; ++i) {
      gammaLut[i] = round(pow((float)i / 255, gamma) * 255);

      if (i > maxValue0 && gammaLut[i] == 0) {
        gammaLut[i] = 1;
      }

      Serial << i << " " << gammaLut[i] << "    vs " << scale8(i, i) << endl;
    }
  }

  CRGBSet getStrand(int i) {
    return CRGBSet(leds_ + i * N_PER_STRAND, N_PER_STRAND);
  }

  CRGBSet getRawSegment(int i) {
//    Serial << "Segment " << i << endl;
    return CRGBSet(leds_ + i * N_PER_SEGMENT, N_PER_SEGMENT);
  }

  CRGBSet raw() { return CRGBSet(leds_, N_ALL); }

  void show() {
    // gamma correction
    for (auto& rgb : raw()) {
//      rgb.nscale8(rgb);
//      rgb.nscale8_video(rgb);

      rgb.r = gammaLut[rgb.r];
      rgb.g = gammaLut[rgb.g];
      rgb.b = gammaLut[rgb.b];
    }
//    raw().napplyGamma_video(2.2);

    FastLED.show();
  }

private:

  CRGB leds_[N_ALL];

  uint8_t gammaLut[256];
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
    constexpr long kDesiredLoopMicros = 1000000 / FRAME_RATE;
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

  float mwSumPowerOn = 0;
  int samplesSincePowerOn = 0;

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

      mwSumPowerOn += mwAvg;
      samplesSincePowerOn++;
      float mwAvgPowerOn = mwSumPowerOn / samplesSincePowerOn;
      Serial << "     Since power on: "
             << mwAvgPowerOn / VOLTS /1000 << " A; "
             << mwAvgPowerOn /1000 << " W; "
             << (BATTERY_MILLIWATT_HOURS / mwAvgPowerOn) << " h runtime"
             << "\n";

      mwSum = 0;
      mwMax = 0;
    }

//    Serial << frameMw << endl;
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
