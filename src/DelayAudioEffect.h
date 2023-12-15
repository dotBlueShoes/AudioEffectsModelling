#pragma once

#include "AudioEffect.h"

class DelayAudioEffect : public AudioEffect
{
private:
	float feedback;
    int delay;
	int iterations;

	size cachedDrySoundSize = 0;
	uint16_t cachedDelayInSamples = 0;
public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) override;

	DelayAudioEffect() : AudioEffect(), feedback(50.0f),  delay(400), iterations(1) {};
	void DisplayEffectWindow() override;
};

