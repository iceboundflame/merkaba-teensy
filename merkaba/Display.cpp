#include "Display.h"

#include <EEPROM.h>

StatusLed gStatusLed;
Display gDisplay;
FpsGovernor gFpsGovernor;
PowerGovernor gPowerGovernor;

namespace {
  constexpr int EEPROM_ADDR_BRIGHT_MODE = 0x600;

  void loadAndToggleBrightModeFromEeprom() {
    gDisplay.setMaxMilliamps(10000);

//    bool brightMode = EEPROM.read(EEPROM_ADDR_BRIGHT_MODE);
//    if (brightMode) {
//      gDisplay.setMaxMilliamps(8000);
//      gDisplay.raw().fill_solid(CRGB::White);
//    } else {
//      gDisplay.setMaxMilliamps(4000);
//      gDisplay.raw().fill_solid(CRGB::Orange);
//    }
//    EEPROM.write(EEPROM_ADDR_BRIGHT_MODE, !brightMode);
//
//    gDisplay.show();
//    delay(100);
//    gDisplay.raw().fill_solid(CRGB::Black);
//    gDisplay.show();
  }
}

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

  raw().fill_solid(CRGB::Black);
  show();

  loadAndToggleBrightModeFromEeprom();
}
