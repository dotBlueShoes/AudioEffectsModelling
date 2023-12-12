#pragma once

#include "AudioEffect.h"

class ChorusAudioEffect : public AudioEffect
{
private:
	int delay = 0;
	int depth = 0;
	int sampleRate = 0;
	float feedback = 0;
	int wet = 0;
	int dry = 0;
public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect() override;

	ChorusAudioEffect() : AudioEffect() {};
	void DisplayEffectWindow() override;
};

