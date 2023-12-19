#include "AllPassFilter.hpp"


void AllPassFilter::Reset(const double& newSampleRate) {
	biquad.algorithm = Biquad::Algorithm::kTransposeCanonical;
	sampleRate = newSampleRate;
	biquad.Reset();
};


void AllPassFilter::SetSampleRate(const double& newSampleRate) {
	sampleRate = newSampleRate;
	CalculateFilterCoefficients();
};


double AllPassFilter::GetG() {
	return biquad.GetG();
};


double AllPassFilter::GetS() {
	return biquad.GetS();
};


void AllPassFilter::CalculateFilterCoefficients() {
	double w0 = 2 * 3.14159265358979323846 * fc / sampleRate;
	double cosw0 = cos(w0);
	double sinw0 = sin(w0);
	double alpha = sinw0 / (2.0 * Q);

	biquad.coefficients[(size_t)FilterCoefficients::a0] = 1.0 - alpha;
	biquad.coefficients[(size_t)FilterCoefficients::a1] = -2.0 * cosw0;
	biquad.coefficients[(size_t)FilterCoefficients::a2] = 1.0 + alpha;
	biquad.coefficients[(size_t)FilterCoefficients::b1] = -2.0 * cosw0;
	biquad.coefficients[(size_t)FilterCoefficients::b2] = 1.0 - alpha;
	spdlog::info("before norm sample : {}", biquad.coefficients[(size_t)FilterCoefficients::a0]);
	spdlog::info("before norm sample : {}", biquad.coefficients[(size_t)FilterCoefficients::a1]);
	spdlog::info("before norm sample : {}", biquad.coefficients[(size_t)FilterCoefficients::a2]);
	spdlog::info("before norm sample : {}", biquad.coefficients[(size_t)FilterCoefficients::b1]);
	spdlog::info("before norm sample : {}", biquad.coefficients[(size_t)FilterCoefficients::b2]);
};

double AllPassFilter::process(double input) {
	double output = (biquad.coefficients[(size_t)FilterCoefficients::a0] * input +
		biquad.coefficients[(size_t)FilterCoefficients::a1] * biquad.states[0] +
		biquad.coefficients[(size_t)FilterCoefficients::a2] * biquad.states[1] -
		biquad.coefficients[(size_t)FilterCoefficients::b1] * biquad.states[2] -
		biquad.coefficients[(size_t)FilterCoefficients::b2] * biquad.states[3]);

	// Update states
	biquad.states[1] = biquad.states[0];
	biquad.states[0] = input;
	biquad.states[3] = biquad.states[2];
	biquad.states[2] = output;

	return output;
}

