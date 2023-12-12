#pragma once

#include "AudioEffect.h"

class DelayAudioEffect : public AudioEffect
{
private:
	float feedback = 0;
    int delay = 0;
public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect() override;

	DelayAudioEffect() : AudioEffect(), feedback(0.0f),  delay(0) {};
	void DisplayEffectWindow() override;
};

