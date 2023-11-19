#pragma once

#include "AudioEffect.h"

class DelayAudioEffect : public AudioEffect
{
private:
	float feedback = 0;
    int delay = 0;
public:
	void applyEffect() override {
		// Implementation specific to DelayAudioEffect
	}

	DelayAudioEffect() : AudioEffect(), feedback(0.0f),  delay(0) {};
	void DisplayEffectWindow() override;
};

