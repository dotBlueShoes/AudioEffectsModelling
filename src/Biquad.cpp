#include "Biquad.hpp"

void Biquad::Reset() {
	memset(&states[0], 0, sizeof(double) * STATES_COUNT);
};


double Biquad::GetG() {
	return coefficients[(uint8_t)FilterCoefficients::a0];
};


double Biquad::GetS() {
	return 0; // ?
};