
#include "AudioHardware.h"

AudioInputAnalog gAudioInput(A9);

FftAnalyzer gFftAnalyzer(gAudioInput);
//BeatDetector gBeatDetector(gFftAnalyzer.fft());

//AudioAnalyzeBeat gAnalyzeBeat;
//static AudioConnection analyzerBeatPatchCord(gAudioInput, 0, gAnalyzeBeat, 0);

bool gSoundEnable = true;
