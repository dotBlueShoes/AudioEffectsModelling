#pragma once

#include "AudioEffect.h"

class ChorusAudioEffect : public AudioEffect
{
private:
	int delay = 0;
	int depth = 0;
	int sampleRate = 0;
	float feedback = 0;
	int wet = 0;
	int dry = 0;
public:
	void applyEffect() override {
		// Implementation specific to DelayAudioEffect
	}

	void getProcessedSize(const size_t& drySoundSize, const uint32_t& sampleRate, size_t& wetSoundSize) override {

	}

	ChorusAudioEffect() : AudioEffect() {};
	void DisplayEffectWindow() override;
};

