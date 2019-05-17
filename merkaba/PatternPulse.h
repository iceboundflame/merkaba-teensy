#pragma once

#include <FastLED.h>

#include "Display.h"
#include "DisplayBuffers.h"
#include "BasePattern.h"
#include "PaletteManager.h"

#include "AudioHardware.h"

namespace {
  class Pulse {
    std::vector<Segment>* path_;
    float velocity_;  // in segments/sec
    float position_;  // in segments; continuous val from [ 0, path->size() )
    float size_ = 1;  // in leds

  public:
    Pulse( std::vector<Segment>* path, float velocity, float position )
    : path_(path), velocity_(velocity), position_(position) {
    }

    void step() {
      // XXX: is this accurate or should we measure time?
      position_ += velocity_ / FRAME_RATE;
      position_ = normalizePosition(position_);
    }

    void render(uint8_t* fullValBuf) {
//      int from = (velocity_ > 0)  ? 0      : -size_;
//      int to   = (velocity_ > 0)  ? size_  : 0;
      int from = (velocity_ < 0)  ? 0      : -size_;
      int to   = (velocity_ < 0)  ? size_  : 0;
      for (int i = from; i < to; i++) {
//        for (int i = -size_/2; i < size_/2; i++) {
        int fullIdx = posToRawFullBufIndex(position_ + (float)i / N_PER_SEGMENT);
        fullValBuf[fullIdx] = 255;
      };
    }

    void setSize(float size, bool advance=false) {
      size_ = size;
    }

    // put pos into [ 0, path->size() ) through modulo
    float normalizePosition(float pos) {
      pos = fmod(pos, path_->size());
      if (pos < 0) {
        pos += path_->size();
      }
      return pos;
    }

    // pos in [ 0, path->size() )
    int posToRawFullBufIndex(float pos) {
      pos = normalizePosition(pos);
      int segmentIdx = constrain((int) pos, 0, N_SEGMENTS_TOTAL-1);
      float fracPos = pos - segmentIdx;
      int fracPosLed = constrain(fracPos * N_PER_SEGMENT, 0, N_PER_SEGMENT-1);

      int fullIdx = (*path_)[segmentIdx].ledToRawFullBufIndex(fracPosLed);

      // being extra defensive.. hoping to fix an unknown crash
      fullIdx = constrain(fullIdx, 0, N_ALL-1);

      return fullIdx;
    }
  };
}

class PatternPulse : public BasePattern {
private:
  // "foreground" consists of just full-on (255) lines where the pulsers are.
  int fgFade = 0;  // amount to fade before this frame's fg is pasted onto bg
  int bgFade = 3;  // amount to fade bg on each frame.

  int bgSoundDimMin = 40;
  int bgSoundDimMax = 200;

  float newPulsesInterval = 0.5;
  float newPulsesTimer = newPulsesInterval;
  float intervalMin = 0.08;
  float intervalMax = 8;

  int nPulsesMin = 1;
  int nPulsesMax = 5;

  float velocityMin = 1;
  float velocityMax = 8;

  uint8_t fgbuf[N_ALL];
  uint8_t bgbuf[N_ALL];

  std::vector<Pulse> pulses;
  int nPulses = 5;

  int maxSize = N_PER_SEGMENT*2;

public:

  PatternPulse(Display* d): BasePattern(d) {
    for (int i = 0; i < N_ALL; ++i) {
      fgbuf[i] = bgbuf[i] = 0;
    }

    randomize();
  }

  virtual void loop() {
    newPulsesTimer -= 1/FRAME_RATE;
    if (newPulsesTimer < 0) {
      newPulses();
      newPulsesTimer = newPulsesInterval;
    }

    EVERY_N_SECONDS(5) {
      gPaletteManager.nextPalette();
      randomize();
    };

    gFftAnalyzer.step();
    float intensity =
              gSoundEnable ? gFftAnalyzer.totalPeak()
                           : 1.f;

//    Serial.println("1");
    for (int i = 0; i < N_ALL; ++i) {
      fgbuf[i] = 0;
    }

    for (auto& pulse : pulses) {
      pulse.setSize(map(intensity, 0,1, 1, maxSize));
      pulse.step();
      pulse.render(fgbuf);
    }

//    Serial.println("2");

    CRGBSet raw = gDisplay.raw();
    for (int i = 0; i < N_ALL; ++i) {
      raw[i] = gPaletteManager.colorNoWrap(
          max(bgbuf[i], fgbuf[i]));

      // bg pulse with music
      if (fgbuf[i] == 0) {
        raw[i].nscale8_video(map(intensity, 0,1, bgSoundDimMin, bgSoundDimMax));
      }
//      raw[i] = scale8(raw[i], 128);

      bgbuf[i] = max(bgbuf[i], scale8(fgbuf[i], 255 - fgFade));

      // uniformly fade bg
      bgbuf[i] = scale8(bgbuf[i], 255 - bgFade);
    }

//    Serial.println("3");
  }

  void randomize() {
    newPulses();

    newPulsesInterval = newPulsesTimer =
        random(intervalMin*100, intervalMax*100)/100.f;
    Serial << "new interval " << newPulsesInterval << endl;
  }

  void newPulses() {
    pulses.clear();
    nPulses = random(nPulsesMin, nPulsesMax+1);
    for (int i = 0; i < nPulses; ++i) {
      auto& orbit = gOrbits[random(0, gOrbits.size())];
      float v = (random(0, 2) ? 1 : -1)
          * random(velocityMin*100, velocityMax*100)/100.f;
      float p = (float)random(0, orbit.size() * N_PER_SEGMENT) / N_PER_SEGMENT;
      pulses.emplace_back(&orbit, v, p);
    }
  }

  virtual bool processSerial(const char *line) {
    if (strcmp(line, "r") == 0) {
      Serial << "randomize" << endl;
      randomize();
      return true;
    }

    {
      if (sscanf(line, "bgf %d", &bgFade) == 1) {
        Serial << "bgFade " << bgFade << endl;
        return true;
      }
    }

    {
      if (sscanf(line, "fgf %d", &fgFade) == 1) {
        Serial << "fgFade " << fgFade << endl;
        return true;
      }
    }

    {
      if (sscanf(line, "nmin %d", &nPulsesMin) == 1) { Serial.println("ok"); return true; }
      if (sscanf(line, "nmax %d", &nPulsesMax) == 1) { Serial.println("ok"); return true; }
    }

    {
      if (sscanf(line, "vmin %f", &velocityMin) == 1) { Serial.println("ok"); return true; }
      if (sscanf(line, "vmax %f", &velocityMax) == 1) { Serial.println("ok"); return true; }
    }

    {
      if (sscanf(line, "max %d", &maxSize) == 1) {
        Serial << "maxSize " << maxSize << endl;
        return true;
      }
    }

    {
      if (sscanf(line, "bgmin %d", &bgSoundDimMin) == 1) {
        Serial << bgSoundDimMin << endl;
        return true;
      }
      if (sscanf(line, "bgmax %d", &bgSoundDimMax) == 1) {
        Serial << bgSoundDimMax << endl;
        return true;
      }
    }

    {
      if (sscanf(line, "i %f", &newPulsesInterval) == 1) {
        newPulsesTimer = newPulsesInterval;
        Serial.println("ok"); return true; }
      if (sscanf(line, "imin %f", &intervalMin) == 1) { Serial.println("ok"); return true; }
      if (sscanf(line, "imax %f", &intervalMax) == 1) { Serial.println("ok"); return true; }
    }

    {
      int orbit;
      if (sscanf(line, "o %d", &orbit) == 1) {
        Serial << "orbit " << orbit << endl;
        pulses.clear();
        pulses.emplace_back(&gOrbits[orbit], 5, 0);
        return true;
      }
    }

    return false;
  }
};

// todo: kaleidoscope effects?
// todo: auto-advance and randomize effects.
// todo: see if gain control is needed.

// FIXME: Some kind of crash somewhere sometimes in PatternPulse??

