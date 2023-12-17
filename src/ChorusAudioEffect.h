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

	// Interface Parameters
	int minDelay = 10;
	int modDepth = 10;
	int sampleRate = 0;
	float feedback = 0;

public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) override;

	void DisplayEffectWindow() override;

	ChorusAudioEffect() : AudioEffect() {};
	
};

