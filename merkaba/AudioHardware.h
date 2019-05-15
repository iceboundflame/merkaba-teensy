#pragma once

#include <Audio.h>
#include <EEPROM.h>
#include "Streaming.h"

#include "FftAnalyzer.h"

extern AudioInputAnalog gAudioInput;

extern FftAnalyzer gFftAnalyzer;

extern bool gSoundEnable;

//extern BeatDetector gBeatDetector;

const static int EEPROM_ADDR_SOUND_ENABLE = 0x7fe;
const static int EEPROM_ADDR_MIC_GAIN = 0x7fd;

inline void set_sound_enable(bool onoff, bool save=true) {
  gSoundEnable = onoff;
  Serial << "Sound enable: " << gSoundEnable << endl;
  if (save) EEPROM.write(EEPROM_ADDR_SOUND_ENABLE, gSoundEnable);
}

//inline void set_mic_gain(uint8_t dB, bool save=true) {
//  gAudioShield.micGain(dB);
//  Serial << "Mic gain: " << dB << endl;
//  if (save) EEPROM.write(EEPROM_ADDR_MIC_GAIN, dB);
//}

inline void audio_hardware_begin() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // load EEPROM settings
  set_sound_enable(EEPROM.read(EEPROM_ADDR_SOUND_ENABLE), false);
//  set_mic_gain(EEPROM.read(EEPROM_ADDR_MIC_GAIN), false);
}
