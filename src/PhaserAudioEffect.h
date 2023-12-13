#pragma once

#include "AudioEffect.h"

class PhaserAudioEffect : public AudioEffect
{
private:
	float feedback = 0;
	int rate = 0;
	float depth = 0;
	float offset = 0;
	float intensity = 0;
	int stages = 0;
public:
	
	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) override;

	PhaserAudioEffect() : AudioEffect(){};
	void DisplayEffectWindow() override;
};

