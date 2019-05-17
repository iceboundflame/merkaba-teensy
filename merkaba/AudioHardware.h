#pragma once

#include <Audio.h>
#include <EEPROM.h>
#include "Streaming.h"

#include "FftAnalyzer.h"
#include "BeatDetector.h"

extern AudioInputAnalog gAudioInput;

extern FftAnalyzer gFftAnalyzer;

extern bool gSoundEnable;

//extern BeatDetector gBeatDetector;

const static int EEPROM_ADDR_SOUND_ENABLE = 0x7fe;
const static int EEPROM_ADDR_MIC_GAIN = 0x7fd;
const static int EEPROM_ADDR_MIC_AR = 0x7fc;

inline void set_sound_enable(bool onoff, bool save=true) {
  gSoundEnable = onoff;
  Serial << "Sound enable: " << gSoundEnable << endl;
  if (save) EEPROM.write(EEPROM_ADDR_SOUND_ENABLE, gSoundEnable);
}

enum Gain {
  kGain20_40,  // may not work
  kGain30_50,
  kGain40_60,  // default
};

enum AttackRelease {
  kAR1_500,
  kAR1_2000,  // may not work
  kAR1_4000,  // default
};

static constexpr int PIN_GAIN = 15;
static constexpr int PIN_ATTACK_RELEASE = 14;

inline void set_mic_gain(Gain gain, bool save=true) {
  switch (gain) {
    case kGain40_60:
    default:
      pinMode(PIN_GAIN, INPUT);
      break;
    case kGain30_50:
      pinMode(PIN_GAIN, OUTPUT);
      digitalWrite(PIN_GAIN, LOW);
      break;
    case kGain20_40:
      pinMode(PIN_GAIN, OUTPUT);

      // NOTE this may not work without level shifter;
      // output is 3.3V but Vdd is 5V on module.
      digitalWrite(PIN_GAIN, HIGH);
      break;
  }
  Serial << "Mic gain: " << gain << endl;
  if (save) EEPROM.write(EEPROM_ADDR_MIC_GAIN, gain);
}

inline void set_mic_ar(AttackRelease ar, bool save=true) {
  switch (ar) {
    case kAR1_4000:
    default:
      pinMode(PIN_GAIN, INPUT);
      break;
    case kAR1_500:
      pinMode(PIN_GAIN, OUTPUT);
      digitalWrite(PIN_GAIN, LOW);
      break;
    case kAR1_2000:
      pinMode(PIN_GAIN, OUTPUT);

      // NOTE this may not work without level shifter;
      // output is 3.3V but Vdd is 5V on module.
      digitalWrite(PIN_GAIN, HIGH);
      break;
  }
  Serial << "Attack/release ratio: " << ar << endl;
  if (save) EEPROM.write(EEPROM_ADDR_MIC_AR, ar);
}

inline void audio_hardware_begin() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // load EEPROM settings
//  set_sound_enable(EEPROM.read(EEPROM_ADDR_SOUND_ENABLE), false);
//  set_mic_gain(EEPROM.read(EEPROM_ADDR_MIC_GAIN), false);
//  set_mic_ar(EEPROM.read(EEPROM_ADDR_MIC_AR), false);

  set_sound_enable(true, false);
  set_mic_gain(kGain30_50, false);
  // found best AR is 1_500 for music!
  set_mic_ar(kAR1_500, false);
}
