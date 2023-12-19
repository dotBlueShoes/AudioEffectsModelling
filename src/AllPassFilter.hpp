#pragma once
#include "Audio/Framework.hpp"

#include "Biquad.hpp"
#include <cmath>
#include <spdlog/spdlog.h>

class AllPassFilter {
private:
	double sampleRate = 44100.0;
public:

	Biquad biquad;
	array<double, COEFFICIENTS_COUNT> coefficients { 0 };

	
	//AudioFilterParameters
	double fc = 100.0;	// Filter cutoff or center frequency (Hz)
	double Q = 0.707;	// Filter Q

	
	void Reset(const double& newSampleRate);
	void SetSampleRate(const double& newSampleRate);
	double GetG();
	double GetS();
	void CalculateFilterCoefficients();
	double process(double input);
};