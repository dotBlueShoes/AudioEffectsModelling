#pragma once
#include "Framework.hpp"

namespace Math {

	uint64_t MilisecondsToSample(const uint16_t& miliseconds, uint16_t sampleRate);

	// 0.0 -> 100.0 to 0.0 -> 1.0
	float NormalizePercent(const float& percent);

	//template<class T>
	double Remap(const double& value, const double& oldRangeMin, const double& oldRangeMax, const double& newRangeMin, const double& newRangeMax);

}