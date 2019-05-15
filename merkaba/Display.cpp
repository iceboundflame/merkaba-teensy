#include "Display.h"
#include "DisplayBuffers.h"

StatusLed gStatusLed;
Display gDisplay;
FpsGovernor gFpsGovernor;
PowerGovernor gPowerGovernor;

Segment gOctaSegments[12] = {
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
};
