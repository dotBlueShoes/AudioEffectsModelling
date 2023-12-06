#pragma once

#include "AudioEffect.h"

class DistortionAudioEffect : public AudioEffect
{
private:
	int type = 0;
	float gain = 0;
	int dry = 0;
	int wet = 0;
public:
	void applyEffect() override {
		// Implementation specific to DelayAudioEffect
	}

	void getProcessedSize(const size_t& drySoundSize, const uint32_t& sampleRate, size_t& wetSoundSize) override {

	}

	DistortionAudioEffect() : AudioEffect() {};
	void DisplayEffectWindow() override;
};

