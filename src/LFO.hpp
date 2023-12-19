#pragma once
#include "Framework.hpp"

enum Waveform : uint8_t {
	sine = 0,
	triangle = 1,
	sawtooth = 2
};


struct LFOResult {
	double normal = 0.0;
	double inverted = 0.0;
	double quadPhase = 0.0;
	double quadPhaseInverted = 0.0;
};


namespace Math {

	const double kPi = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899;
	const double B = 4.0 / kPi;
	const double C = -4.0 / (kPi * kPi);
	const double P = 0.225;

	// Parabolic sinusoidal calcualtion; NOTE: input is -pi to +pi. http://devmaster.net/posts/9648/fast-and-accurate-sine-cosine
	double ParabolicSine(const double& angle);

	// Calculates the bipolar [-1.0, +1.0] value FROM a unipolar [0.0, +1.0] value
	double UnipolarToBipolar(const double& value);
}


class LFO {

private:
	const double MODULO_COUNTER_MAX = 1.0f;

	double moduloCounter = 0.0f;		// Module counter for normal output.
	double moduloCounterQP = 0.25f;		// Module counter for quad-phase outputs.
	double phaseIncremental = 0.0f;		// Current phase increment value.
	double sampleRate = 0.0f;			// Sample Rate.

public:
	Waveform waveform;
	double frequency;

	void Initialize(const Waveform& newWaveform, const double& newFrequency);

	// Resets counters and indexes.
	void Reset(const double& newSampleRate);

	// Renders the LFO synthesized waveform.
	LFOResult RenderAudio();

	// Check the modulo counter and wrap if needed. Unipolar!
	bool CheckAndWrapModulo(double& moduloCounter, const double& phaseIncremental);

	// Advence the modulo counter, then check the modulo counter and wrap it if needed.
	bool AdvenceAndCheckWrapModulo(double& moduloCounter, const double& phaseIncremental);

	// Advence modulo counter...
	void AdvanceModulo(double& moduloCounter, const double& phaseIncremental);

};