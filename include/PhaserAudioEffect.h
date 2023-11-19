#pragma once

#include "../include/AudioEffect.h"=

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
	void applyEffect() override {
		// Implementation specific to DelayAudioEffect
	}

	PhaserAudioEffect() : AudioEffect(){};
	void DisplayEffectWindow() override;
};

