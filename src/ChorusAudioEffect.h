#pragma once

#include "AudioEffect.h"

enum ModulatedDelayType : uint8_t {
	flanger = 0,
	vibrato = 1,
	chorus = 2,
	whiteChorus = 3
};

class ChorusAudioEffect : public AudioEffect {

private:

	LFO lfo;

	// lfo
	//double lfoRate = 10.0f;//0.0f;
	//double lfoDepth = 2.0f;
	float lfoSampleRate = 2.0f; // 1.0001 -> 10.0 ?
	float lfoFrequency = 0.98f;	// (under 20 Hz). Many chorus units have an LFO frequency range between 0.1 Hz and 6 Hz

	// Interface Parameters
	float delay = 7;
	//float depth = 2;
	//int sampleRate = 0;
	int waveform = Waveform::triangle; // fixed for chorus same as mixxer values!
	float feedback = 0;

	size cachedDrySoundSize = 0;
	uint16_t cachedHalfDelayInSamples = 0;

public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) override;

	void DisplayEffectWindow() override;

	ChorusAudioEffect() : AudioEffect(80, 100) {};
	
};

