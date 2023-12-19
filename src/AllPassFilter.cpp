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

};

