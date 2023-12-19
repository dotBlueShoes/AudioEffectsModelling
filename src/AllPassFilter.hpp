#pragma once
#include "Audio/Framework.hpp"

#include "Biquad.hpp";

class AllPassFilter {
private:
	double sampleRate = 44100.0;
public:

	Biquad biquad;
	array<double, COEFFICIENTS_COUNT> coefficients { 0 };

	
	//AudioFilterParameters
	double fc = 100.0;	// Filter cutoff or center frequency (Hz)
	double Q = 0.707;	// Filter Q

	
	void Reset();
	void SetSampleRate();
	void GetG();
	void GetS();
	void CalculateFilterCoefficients();
};