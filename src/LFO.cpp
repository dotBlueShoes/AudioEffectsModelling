#include "LFO.hpp"

double Math::ParabolicSine(const double& angle) {
	double y = B * angle + C * angle * fabs(angle);
	y = P * (y * fabs(y) - y) + y;
	return y;
}


double Math::UnipolarToBipolar(const double& value) {
	return 2.0 * value - 1.0;
}


void LFO::Initialize(const Waveform& newWaveform, const double& newFrequency) {
	if (newFrequency != frequency)
		// Update phase inc based on osc freq and fs.
		phaseIncremental = newFrequency / sampleRate;

	frequency = newFrequency;
	waveform = newWaveform;
}


void LFO::Reset(const double& newSampleRate) {
	// Set Values
	sampleRate = newSampleRate;
	phaseIncremental = frequency / sampleRate;

	// Reset Values
	moduloCounter = 0.0;
	moduloCounterQP = 0.25;
}


bool LFO::CheckAndWrapModulo(double& moduloCounter, const double& phaseIncremental) {

	// For positive frequencies
	if (phaseIncremental > 0.0f && moduloCounter >= 1.0f) {
		moduloCounter -= MODULO_COUNTER_MAX; // Reset plus the distance it exceeded.
		return true;
	}

	// For negative frequencies. This is unipolar. Remember!
	if (phaseIncremental < 0.0f && moduloCounter <= 0.0f) {
		moduloCounter += MODULO_COUNTER_MAX;
		return true;
	}

	return false;
}


bool LFO::AdvenceAndCheckWrapModulo(double& moduloCounter, const double& phaseIncremental) {
	moduloCounter += phaseIncremental; // Advence Counter.
	return CheckAndWrapModulo(moduloCounter, phaseIncremental);
}


void LFO::AdvanceModulo(double& moduloCounter, const double& phaseIncremental) { 
	moduloCounter += phaseIncremental;
};


LFOResult LFO::RenderAudio() {

	CheckAndWrapModulo(moduloCounter, phaseIncremental);

	moduloCounterQP = moduloCounter;

	AdvenceAndCheckWrapModulo(moduloCounterQP, 0.25f);

	LFOResult result { 0 };
	
	switch (waveform) {
		case Waveform::sine: {

			double angle = moduloCounter * 2.0f * Math::kPi - Math::kPi;
			result.normal = Math::ParabolicSine(-angle);

			angle = moduloCounterQP * 2.0 * Math::kPi - Math::kPi;
			result.quadPhase = Math::ParabolicSine(-angle);

		} break;

		case Waveform::triangle: {

			result.normal = Math::UnipolarToBipolar(moduloCounter);

			result.normal = 2.0 * fabs(result.normal) - 1.0;

			result.quadPhase = Math::UnipolarToBipolar(moduloCounterQP);

			result.quadPhase = 2.0 * fabs(result.quadPhase) - 1.0;

		} break;

		case Waveform::sawtooth: {

			result.normal = Math::UnipolarToBipolar(moduloCounter);
			result.quadPhase = Math::UnipolarToBipolar(moduloCounterQP);

		} break;
	}

	result.quadPhaseInverted = -result.quadPhase;
	result.inverted = -result.normal;

	AdvanceModulo(moduloCounter, phaseIncremental);

	return result;

}
