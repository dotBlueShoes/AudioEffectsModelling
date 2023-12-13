#pragma once

#include "AudioEffect.h"

class DistortionAudioEffect : public AudioEffect
{
private:
	int type = 0;
	float gain = 1;
	float kParameter = 2; 
	//float drySound = 10;
	//float wetSound = 0;

	size cachedDrySoundSize = 0;
	size cachedWetSoundSize = 0;
public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const int16_t* drySoundData, SoundIO::ReadWavData& sound) override;

	DistortionAudioEffect() : AudioEffect() {};
	void DisplayEffectWindow() override;
};

