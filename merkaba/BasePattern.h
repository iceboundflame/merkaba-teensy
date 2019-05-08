#pragma once

class Display;

class BasePattern {
public:
  BasePattern(Display* display): display_(display) { }

  virtual ~BasePattern() { }
  virtual void loop() { }
  virtual bool processSerial(const char* line) { return false; }

protected:
  Display* display_;
//  void delayForFps(int fps_);
};

