#pragma once

#include "../include/AudioEffect.h"=

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

	ReverbAudioEffect() : AudioEffect() {};
	void DisplayEffectWindow() override;
};

