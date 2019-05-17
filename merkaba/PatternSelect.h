#pragma once

#include <memory>

#include <Arduino.h>
#include <EEPROM.h>

#include "./BasePattern.h"
#include "util.h"

class Display;

const int EEPROM_PATTERN_ADDR = 0x7FF;

class PatternSelect {
public:
  enum {
    kPatternPulse,
    kPatternFire,
    kPatternLines,
//    kPatternAudio1,
//    kPatternAudio3,
//    kPatternAudio2,
//    kPatternAudio4,
    kPattern_N
  };

  enum Disabled {
    kPattern_X=kPattern_N,
    kPatternDisplayTest,
  };

  PatternSelect(Display* d): display_(d) {
    selectPattern(currentPatternIndex_);
  }

  void loadFromEepromAndAdvance() {
    selectPattern(EEPROM.read(EEPROM_PATTERN_ADDR) + 1);
    EEPROM.write(EEPROM_PATTERN_ADDR, currentPatternIndex_);
  }

  void randomPattern() {
    selectPattern(randomExcluding(0, kPattern_N, currentPatternIndex_));
  }

  void selectPattern(int i);

  void cyclePattern(int off) {
    selectPattern(currentPatternIndex_ + off);
  }

  inline int currentPatternIndex() { return currentPatternIndex_; }

  BasePattern& currentPattern() { return *currentPattern_.get(); }

private:
  std::unique_ptr<BasePattern> currentPattern_;
  int currentPatternIndex_ = 0;
  Display* display_;
};
