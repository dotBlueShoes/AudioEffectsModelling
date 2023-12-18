#pragma once

#include "AudioEffect.h"
#include "CombFilter.h"

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

	std::vector<CombFilter> combFilters;
	float RT60 = 60; // Reverberation time
	//int sampleRate;


	size cachedDrySoundSize = 0;
	size cachedWetSoundSize = 0;
public:
	
	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) override;

	ReverbAudioEffect() : AudioEffect(50, 50), sampleRate(44100), wet(50), decay(0.3) {
		// Initialize delay buffers and indices
		delay1 = 100; // Example delay times in milliseconds
		delay2 = 200;
		delay3 = 300;
		delay4 = 400;
	}
	//ReverbAudioEffect(int sr, float rt60);
	void DisplayEffectWindow() override;
};

