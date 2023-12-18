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

	// Interface Parameters
	float minDelay = 10;
	float modDepth = 1;
	//int sampleRate = 0;
	int waveform = Waveform::sine;
	float feedback = 0;

	size cachedDrySoundSize = 0;
	uint16_t cachedHalfDepthInSamples = 0;

public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) override;

	void DisplayEffectWindow() override;

	ChorusAudioEffect() : AudioEffect(50, 50) {};
	
};

