#include "Display.h"
#include "DisplayBuffers.h"

StatusLed gStatusLed;
Display gDisplay;
FpsGovernor gFpsGovernor;
PowerGovernor gPowerGovernor;


Segment gAllSegments[36] = {
    // ===================
    // OCTA SEGMENTS
    // all segments direction = coming forward or clockwise

    // back legs, clockwise from lower left
    Segment(36, true),
    Segment(28, false),
    Segment(25, false),
    Segment(27, true),

    // square, clockwise from lower left
    Segment(32, true),
    Segment(29, false),
    Segment(26, false),
    Segment(35, false),

    // front legs, clockwise from lower left
    Segment(33, false),
    Segment(31, true),
    Segment(30, false),
    Segment(34, true),

    // ===================
    // MINI TETRA SEGMENTS
    // all segments direction = outside coming in.

    // down tetra
    // top front left (j)
    Segment(4, false), Segment(6, false), Segment(3, true),
    // top back (g)
    Segment(5, true), Segment(12, false), Segment(11, true),
    // top front right (h)
    Segment(10, false), Segment(7, true), Segment(8, false),
    // bottom (i)
    Segment(2, false), Segment(1, true), Segment(9, true),

    // up tetra
    // bottom back left (l)
    Segment(23, true), Segment(24, false), Segment(19, true),
    // bottom back right (n)
    Segment(14, false), Segment(13, true), Segment(16, false),
    // bottom front (k)
    Segment(17, true), Segment(20, false), Segment(18, false),
    // top (m)
    Segment(22, false), Segment(21, true), Segment(15, true),
};

Segment* const gOctaSegments = gAllSegments;

Segment* const gMiniTetraSegments = gAllSegments + 12;

void Display::begin() {
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
