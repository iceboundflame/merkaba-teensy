#include "Display.h"

StatusLed gStatusLed;
Display gDisplay;
FpsGovernor gFpsGovernor;
PowerGovernor gPowerGovernor;

void Display::begin() {
  setGamma(2.2, 255);

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

  raw().fill_solid(CRGB::Black);
  FastLED.show();
}
