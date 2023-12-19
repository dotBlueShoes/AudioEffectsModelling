#pragma once
#include "Audio/Framework.hpp"

// The Biquad object implements a first or second order H(z) transfer 
//  function using one of four standard structures : Direct, Canonical, Transpose Direct, Transpose Canonical.


const size COEFFICIENTS_COUNT = 7;
const size STATES_COUNT = 4;


// Use this enum to easily access coefficents inside of arrays.
enum class FilterCoefficients : uint8_t { a0, a1, a2, b1, b2, c0, d0 };


class Biquad {
public:

	enum class Algorithm : uint8_t { 
		kDirect, 
		kCanonical, 
		kTransposeDirect, 
		kTransposeCanonical
	};

	/** Array of coefficients */
	array<double, COEFFICIENTS_COUNT> coefficients { 0.0f };

	/** Array of state (z^-1) registers */
	array<double, STATES_COUNT> states { 0.0f };

	/** Type of calculation (algorithm  structure) */
	Algorithm algorithm;

	/** For Harma loop resolution */
	double storageComponent = 0.0;

	void Reset();
	double GetG();
	double GetS();

};