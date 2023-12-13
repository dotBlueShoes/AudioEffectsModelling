#pragma once

#include "AudioEffect.h"

class DistortionAudioEffect : public AudioEffect
{
private:
	int type = 0;
	float gain = 0;
public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const int16_t* drySoundData, SoundIO::ReadWavData& sound) override;

	DistortionAudioEffect() : AudioEffect() {};
	void DisplayEffectWindow() override;
};

