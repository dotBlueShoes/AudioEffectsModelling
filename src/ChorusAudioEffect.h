#pragma once

#include "AudioEffect.h"

enum ModulatedDelayType : uint8_t {
	none = 0,
	flanger = 1,
	vibrato = 2,
	chorus = 3,
	whiteChorus = 4
};

class ChorusAudioEffect : public AudioEffect {

private:

	ModulatedDelayType type = ModulatedDelayType::chorus;
	LFO lfo;
	const float lfoFrequency = 0.98f;	// (under 20 Hz). Many chorus units have an LFO frequency range between 0.1 Hz and 6 Hz

	// Interface Parameters
	float lfoSampleRate = 2.0f; // 1.0001 -> 10.0 ?
	float depth = 14;
	float delay = 1;

	int waveform = Waveform::triangle; // fixed for chorus same as mixxer values!

	float feedback = 0;
	int feedbackIterations = 0;

	size cachedDrySoundSize = 0;
	uint16_t cachedHalfDepthInSamples = 0;
	uint16_t cachedDelayInSamples = 0;

public:

	void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) override;

	void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) override;

	void DisplayEffectWindow() override;

	ChorusAudioEffect() : AudioEffect(70, 100) {};
	
};

