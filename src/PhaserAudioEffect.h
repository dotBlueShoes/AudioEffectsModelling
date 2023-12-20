#pragma once

#include "AudioEffect.h"
#include "AllPassFilter.hpp"


// Minimum and maximum phase rotation frequencies for the NSC phaser APFs.
// 1 16 Hz 1.6 kHz
// 2 33 Hz 3.3 kHz
// 3 48 Hz 4.8 kHz
// 4 98 Hz 9.8 kHz
// 5 160 Hz 16 kHz
// 6 260 Hz 26 kHz
const unsigned int PHASER_STAGES = 6;
const double APF_0_MIN_FREQUENCY = 16.0;
const double APF_0_MAX_FREQUENCY = 1600.0;
const double APF_1_MIN_FREQUENCY = 33.0;
const double APF_1_MAX_FREQUENCY = 3300.0;
const double APF_2_MIN_FREQUENCY = 48.0;
const double APF_2_MAX_FREQUENCY = 4800.0;
const double APF_3_MIN_FREQUENCY = 98.0;
const double APF_3_MAX_FREQUENCY = 9800.0;
const double APF_4_MIN_FREQUENCY = 160.0;
const double APF_4_MAX_FREQUENCY = 16000.0;
const double APF_5_MIN_FREQUENCY = 260.0;
const double APF_5_MAX_FREQUENCY = 20480.0;


class PhaserAudioEffect : public AudioEffect
{
private:

	// It includes six AudioFilter objects and one LFO object.
	AllPassFilter allPassFilter[PHASER_STAGES];
	LFO lfo;

	const float lfoFrequency = 0.98f;	// (under 20 Hz). Many chorus units have an LFO frequency range between 0.1 Hz and 6 Hz

	// Interface Parameters
	float lfoSampleRate = 0.03f; //1.5f; // 1.0001 -> 10.0 ?
	float depth = 50;

	//float offset = 0;
	float intensity = 50;
	int stages = 2;

	float feedback = 100;  // This phaser design sounds best with feedback(intensity) values between 75 % and 95 % .
	//int feedbackIterations = 0;


	size cachedDrySoundSize = 0;

public:
	
	double ModulateFrequency(double lfoValue, double minFrequency, double maxFrequency);

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) override;

	PhaserAudioEffect() : AudioEffect(50, 50) {};
	void DisplayEffectWindow() override;
};

