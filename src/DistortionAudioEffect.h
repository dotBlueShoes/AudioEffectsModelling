#pragma once

#include "AudioEffect.h"

class DistortionAudioEffect : public AudioEffect
{
private:
	int type = 0;
	float gain = 0;
public:
	void applyEffect() override {
		// Implementation specific to DelayAudioEffect
	}

	DistortionAudioEffect() : AudioEffect() {};
	void DisplayEffectWindow() override;
};

