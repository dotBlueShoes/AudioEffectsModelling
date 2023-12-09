#pragma once

#include "AudioEffect.h"

class DelayAudioEffect : public AudioEffect
{
private:
	float feedback = 0;
    int delay = 0;
public:
	void applyEffect() override;
	void getProcessedSize(const size_t& drySoundSize, const uint32_t& sampleRate, size_t& wetSoundSize) override;
	DelayAudioEffect() : AudioEffect(), feedback(0.0f),  delay(0) {};
	void DisplayEffectWindow() override;
};

