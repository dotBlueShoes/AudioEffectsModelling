#pragma once

#include "AudioEffect.h"

class ReverbAudioEffect : public AudioEffect
{
private:
	int delay1 = 0;
	int delay2 = 0;
	int delay3 = 0;
	int delay4 = 0;
public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect() override;

	ReverbAudioEffect() : AudioEffect() {};
	void DisplayEffectWindow() override;
};

