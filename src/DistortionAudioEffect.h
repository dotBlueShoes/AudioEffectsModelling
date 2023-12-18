#pragma once

#include "AudioEffect.h"

class DistortionAudioEffect : public AudioEffect
{
private:
	int type = 0;
	float gain = 1;
	float kParameter = 2; 

	size cachedDrySoundSize = 0;
	size cachedWetSoundSize = 0;
public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) override;

	DistortionAudioEffect() : AudioEffect(100, 0) {};
	void DisplayEffectWindow() override;
};

