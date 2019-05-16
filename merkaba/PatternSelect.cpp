#include "./PatternSelect.h"

#include "BasePattern.h"
#include "Palettes.h"
#include "./PatternDisplayTest.h"
#include "./PatternFire.h"
#include "./PatternLines.h"
#include "./PatternPulse.h"
//#include "./PatternRadialSymTest.h"
//#include "./PatternEdgesTest.h"
//#include "./PatternAudio1.h"
//#include "./PatternAudio2.h"
//#include "./PatternAudio3.h"
//#include "./PatternAudio4.h"

void PatternSelect::selectPattern(int i) {
  currentPattern_.release();

  if (i >= kPattern_N) i %= kPattern_N;
  while (i < 0) i += kPattern_N;
  currentPatternIndex_ = i;

  switch (i) {
    default:
    case kPatternDisplayTest:
      currentPattern_.reset(new PatternDisplayTest(display_));
      break;

    case kPatternFire:
      currentPattern_.reset(new PatternFire(display_));
      break;

    case kPatternLines:
      currentPattern_.reset(new PatternLines(display_));
      break;

    case kPatternPulse:
      currentPattern_.reset(new PatternPulse(display_));
      break;

//    case kPatternEdgesTest: currentPattern_.reset(new PatternEdgesTest(display_)); break;
//    case kPatternAudio1: currentPattern_.reset(new PatternAudio1(display_)); break;
//    case kPatternAudio2: currentPattern_.reset(new PatternAudio2(display_)); break;
//    case kPatternAudio3: currentPattern_.reset(new PatternAudio3(display_)); break;
//    case kPatternAudio4: currentPattern_.reset(new PatternAudio4(display_)); break;
  }
}
