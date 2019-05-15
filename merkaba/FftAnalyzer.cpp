#include "FftAnalyzer.h"

#include <Arduino.h>

#include "Streaming.h"

namespace {
//  // bucket width ~ 1.168^(bucket #)
//  const int kLogScaledBucketWidths[FftAnalyzer::kBuckets + 1]
//      {1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 5, 6, 6, 8, 9, 10, 12, 14, 16, 18};
  // first bucket: number of low freq bins to ignore (too noisy)

  const int kLogScaledBucketWidths[FftAnalyzer::kBuckets + 1]
      {1, 2, 3, 4, 6, 7, 11, 14, 19, 26, 34};

  inline float movingAvg(float val, float lastAvg, float decay) {
    return lastAvg * decay + val * (1 - decay);
  }

  inline float peakVal(float val, float lastPeak, float decay) {
    return max(val, movingAvg(val, lastPeak, decay));
  }
}

bool FftAnalyzer::processSerial(const char *line) {
  const int kCmdLen = 16;
  char command[kCmdLen];
  if (sscanf(line, "%16s", command) != 1)
    return false;

  float *param = nullptr;
  if (strcasecmp(command, "avgAlpha") == 0) {
    param = &config_.averageDbAlpha_;
  } else if (strcasecmp(command, "scale") == 0) {
    param = &config_.scaledFullScaleDb_;
  } else if (strcasecmp(command, "bias") == 0) {
    param = &config_.scaledBias_;
  } else if (strcasecmp(command, "suppress") == 0) {
    param = &config_.scaledSuppressQuieterThanDb_;
  } else if (strcasecmp(command, "bassAlpha") == 0) {
    param = &config_.bassPeakAlpha_;
  } else if (strcasecmp(command, "peakAlpha") == 0) {
    param = &config_.scaledPeakAlpha_;
  } else if (strcasecmp(command, "avg") == 0) {
    printAvg = !printAvg;
  } else if (strcasecmp(command, "cal") == 0) {
    for (int i = 0; i < kBuckets; ++i) {
      config_.silentCalibratedDb_[i] = avgDb_[i];
    }
  } else if (strcasecmp(command, "p") == 0) {
    Serial << endl;
    Serial << "avgAlpha " << _FLOAT(config_.averageDbAlpha_, 5) << endl;
    Serial << "scale " << _FLOAT(config_.scaledFullScaleDb_, 5) << endl;
    Serial << "bias " << _FLOAT(config_.scaledBias_, 5) << endl;
    Serial << "suppress " << _FLOAT(config_.scaledSuppressQuieterThanDb_, 5)
           << endl;
    Serial << "bassAlpha " << _FLOAT(config_.bassPeakAlpha_, 5) << endl;
    Serial << "peakAlpha " << _FLOAT(config_.scaledPeakAlpha_, 5) << endl;
    Serial << endl;

    Serial << "Cal: ";
    for (int i = 0; i < kBuckets; ++i) {
      Serial << _FLOAT(config_.silentCalibratedDb_[i], 5) << ", ";
    }
    Serial << endl;
    Serial << endl;

    printAvg = false; // don't flood
  } else {
    return false;
  }

  if (param) {
    if (sscanf(line, "%*s %f", param) == 1) {
      Serial << "Set: " << command << " " << _FLOAT(*param, 5) << endl;
    }
  }

  return true;
}

void FftAnalyzer::begin() {
  // Configure the window algorithm to use
  fft_.windowFunction(AudioWindowHanning256);

  // average together every 5 samples, for approx 60Hz updates.
  fft_.averageTogether(5);

  for (int i = 0; i < kBuckets; ++i) {
    db_[i] = avgDb_[i] = scaled_[i] = 0;
    avgDb_[i] = -45;
  }

  avgRms_ = -40;
}

void FftAnalyzer::step() {
  // RMS analysis
  float rawRms = rms_.read();
  float rms = 10 * log10(rawRms * rawRms);
  avgRms_ = movingAvg(rms, avgRms_, config_.rmsAlpha_);
  scaledRms_ = map(rms, avgRms_, avgRms_ + config_.scaledFullScaleDb_,
                   config_.scaledBias_, config_.scaledBias_ + 1);
  scaledRms_ = constrain(scaledRms_, 0, 1);
  scaledRmsPeak_ = peakVal(scaledRms_, scaledRmsPeak_,
                           config_.bassPeakAlpha_);

//  Serial<<rms<<" " << avgRms_ << " " << scaledRms_ << endl;

//  Serial<<bass_<<" " << vocal_ << " " << percuss_ << endl;




  // get FFT energy in log-scaled buckets
  int cumsum = kLogScaledBucketWidths[0]; // skip
  for (int i = 0; i < kBuckets; ++i) {
    float val = fft_.read(cumsum, cumsum + kLogScaledBucketWidths[i + 1] - 1);
    cumsum += kLogScaledBucketWidths[i];

    val = 10 * log10(val * val);
    db_[i] = max(-60, val);
  }

  // compute moving per-band average
  for (int i = 0; i < kBuckets; ++i) {
    avgDb_[i] = movingAvg(db_[i], avgDb_[i], config_.averageDbAlpha_);
  }

//  // smooth across adjacent kSmoothBucketsWidth frequency bands
//  const int kSmoothBucketsWidth = 0;
//  for (int i = 0; i < kBuckets; ++i) {
//    int n = 0;
//    avgDbSmoothed_[i] = 0;
//    for (int j = max(0, i - kSmoothBucketsWidth);
//         j < min(kBuckets, i + 1 + kSmoothBucketsWidth); j++) {
//      avgDbSmoothed_[i] += avgDb_[j];
//      n++;
//    }
//    avgDbSmoothed_[i] /= n;
//  }

  // scale each band to a value 0-1
  for (int i = 0; i < kBuckets; ++i) {
    scaled_[i] = (db_[i] - max(config_.silentCalibratedDb_[i] +
                               config_.scaledSuppressQuieterThanDb_,
                               avgDb_[i])) / config_.scaledFullScaleDb_ +
                 config_.scaledBias_;
    scaled_[i] = constrain(scaled_[i], 0, 1);

    // also compute peaks: raise immediately, fall with exponential decay
    scaledPeak_[i] = peakVal(scaled_[i], scaledPeak_[i],
                             config_.scaledPeakAlpha_);
  }

  bass_ = scaledBuckets(0, 1);
  bassPeak_ = peakVal(bass_, bassPeak_, config_.bassPeakAlpha_);

  vocal_ = scaledBuckets(1, 3);
  vocalPeak_ = peakVal(vocal_, vocalPeak_, config_.bassPeakAlpha_);

  percuss_ = scaledBuckets(5, 10);
  percussPeak_ = peakVal(percuss_, percussPeak_, config_.bassPeakAlpha_);

  total_ = scaledBuckets(0, 10);
  totalPeak_ = peakVal(total_, totalPeak_, config_.bassPeakAlpha_);

  static int lc = 0;
  if (++lc > 10 && printAvg) {
//    Serial << bass_ << " " << bassPeak_ << endl;
    for (int i = 0; i < kBuckets; ++i) {
//      Serial << db_[i] << ", ";
      Serial << avgDb_[i] << ", ";
    }
    Serial << endl;
//    for (int i = 0; i < kBuckets; ++i){
//      Serial << avgDbSmoothed_[i] << " ";
//    }
//    Serial << endl;
//    Serial << endl;
    lc = 0;
  }
}
