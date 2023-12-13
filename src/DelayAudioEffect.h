#pragma once

#include "AudioEffect.h"

class DelayAudioEffect : public AudioEffect
{
private:
	float feedback = 0;
    int delay = 0;

	size cachedDrySoundSize = 0;
	size cachedWetSoundSize = 0;
public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) override;

	DelayAudioEffect() : AudioEffect(), feedback(0.0f),  delay(0) {};
	void DisplayEffectWindow() override;
};

