#pragma once

#include <Audio.h>

typedef AudioAnalyzeFFT256 FftType;

class FftAnalyzer {
  FftType fft_;
  AudioAnalyzeRMS rms_;
  AudioConnection patchCord_, patchCord1_;

public:
  FftAnalyzer(AudioInputAnalog& input)
      : patchCord_(input, 0, fft_, 0)
      , patchCord1_(input, 0, rms_, 0)
  { }

  void begin();
  FftType &fft() { return fft_; }

  void step();

  bool processSerial(const char* line);

  inline float scaledRms() { return scaledRms_; }
  inline float scaledRmsPeak() { return scaledRmsPeak_; }

//  const static int kBuckets = 20;
  const static int kBuckets = 10;

  inline float db(int i) { return db_[i]; }
  inline float avgDb(int i) { return avgDb_[i]; }
  inline float scaled(int i) { return scaled_[i]; }
  inline float scaledPeak(int i) { return scaledPeak_[i]; }
  inline float bassPeak() { return bassPeak_; }
  inline float vocalPeak() { return vocalPeak_; }
  inline float percussPeak() { return percussPeak_; }
  inline float totalPeak() { return totalPeak_; }

  float scaledBuckets(int minBucket, int maxBucket) {
    minBucket = constrain(minBucket, 0, kBuckets);
    maxBucket = constrain(maxBucket, 0, kBuckets);

    float v = 0;
    for (int i = minBucket; i < maxBucket; ++i)
      v += scaled_[i];
    return constrain(v / (maxBucket - minBucket), 0, 1);
  }

  float scaledBucketsMax(int minBucket, int maxBucket) {
    minBucket = constrain(minBucket, 0, kBuckets);
    maxBucket = constrain(maxBucket, 0, kBuckets);

    float v = 0;
    for (int i = minBucket; i < maxBucket; ++i)
      v = max(v, scaled_[i]);
    return v;
  }

private:
  float scaledRms_;
  float scaledRmsPeak_;
  float avgRms_;

  // raw decibels for each bucket, this time sample
  float db_[kBuckets];

  // moving average of recent decibels in each freq bucket
  float avgDb_[kBuckets];

  // average, smoothed across buckets
//  float avgDbSmoothed_[kBuckets];

  // decibels scaled to relative values in range 0-1 for visualization
  float scaled_[kBuckets];

  // decibels scaled to relative values in range 0-1 for visualization;
  // peak value, with exp decay
  float scaledPeak_[kBuckets];

  // scaled 0-1 summaries for particular bands
  float bass_;
  float bassPeak_;
  float vocal_;
  float vocalPeak_;
  float percuss_;
  float percussPeak_;
  float total_;
  float totalPeak_;

  struct Config {
    float rmsAlpha_ = 0.998;

    float averageDbAlpha_ = 0.998;
    float scaledFullScaleDb_ = 6;

    float scaledBias_ = 0.5;
    float bassPeakAlpha_ = 0.7;
    float scaledPeakAlpha_ = 0.9;

    float silentCalibratedDb_[FftAnalyzer::kBuckets];
    // suppress noises less than X dB louder than calibrated silence
    float scaledSuppressQuieterThanDb_ = 6;

    Config()
    : silentCalibratedDb_
      // 2 low bin discarded:
      // best with 'suppress 3'
//      {-45.34, -52.82, -56.96, -58.73, -59.56, -59.97, -59.29, -59.71, -59.99, -59.94, -59.85, -59.72, -59.98, -59.60, -58.71, -58.40, -59.20, -59.36, -58.95, -59.78,}
      // 1 low bin discarded:
//      {-35.94324, -45.73346, -51.49456, -54.62032, -56.32635, -59.00157, -56.94453, -58.37403, -59.47331, -59.45071, -59.16853, -59.07907, -59.64961, -58.92981, -58.00125, -58.01011, -58.19865, -57.98878, -57.91255, -59.19236,}
        {-40, -52, -56.32635, -56.94453, -59.47331, -59.16853, -59.64961, -58.00125, -58.19865, -57.91255,}
    {}
  };
  Config config_;

  bool printAvg = false;
};
