#include "Math.hpp"

namespace Math {

	uint64_t MilisecondsToSample(const uint16_t& miliseconds, uint16_t sampleRate) {
		if (miliseconds == 0) {
			return 0;
		}

		//float sampleRateInMiliseconds = sampleRate / 1000;

		uint16_t sampleRateInMiliseconds = sampleRate / 1000; // discarded .1 from 44.1
		uint16_t seconds = miliseconds / 1000;
		uint16_t rest = miliseconds % 1000;

		//return seconds * sampleRate;
		//return miliseconds * sampleRateInMiliseconds;
		//spdlog::info("Data: {}", ((float)rest * sampleRateInMiliseconds));
		//return (seconds * sampleRate) + (uint16_t)(rest * sampleRateInMiliseconds);

		return (seconds * sampleRate) + (rest * sampleRateInMiliseconds);
	}

	float NormalizePercent(const float& percent) {
		return percent / 100;
	}

	//template<class T>
	double Remap(const double& value, const double& oldRangeMin, const double& oldRangeMax, const double& newRangeMin, const double& newRangeMax) {
		return ((value - oldRangeMin) / (oldRangeMax - oldRangeMin)) * (newRangeMax - newRangeMin) + newRangeMin;
	}

}