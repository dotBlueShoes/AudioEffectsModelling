#pragma once

#include "AudioEffect.h"

class ReverbAudioEffect : public AudioEffect
{
private:
	int delay1 = 0;
	int delay2 = 0;
	int delay3 = 0;
	int delay4 = 0;

	float wet;
	float decay;
	int sampleRate;

public:
	
	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) override;

	ReverbAudioEffect() : AudioEffect(50, 50), sampleRate(44100), wet(0.5), decay(0.5) {
		// Initialize delay buffers and indices
		delay1 = 100; // Example delay times in milliseconds
		delay2 = 200;
		delay3 = 300;
		delay4 = 400;
	}
	void DisplayEffectWindow() override;
};

