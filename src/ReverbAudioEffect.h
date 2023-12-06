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
	void applyEffect() override {
		// Implementation specific to DelayAudioEffect
	}

	void getProcessedSize(const size_t& drySoundSize, const uint32_t& sampleRate, size_t& wetSoundSize) override {

	}

	ReverbAudioEffect() : AudioEffect() {};
	void DisplayEffectWindow() override;
};

