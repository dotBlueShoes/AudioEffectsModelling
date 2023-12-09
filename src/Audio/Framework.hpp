#pragma once


// OpenAL Tutorial with dr_wav.h - https://www.youtube.com/watch?v=WvND0djMcfE


#include <iostream>
#include <vector>
#include <bitset>
#include <array>


using std::vector;
using std::bitset;
using std::array;

using size = size_t;

// CONSTANTS

const char OPENAL_FAIL_GET_DEFAULT_DEVICE[] = "Failed to get the default device.";
const char OPENAL_ERROR[] = "OpenAL Error: ";

const int32_t FAILURE = -1;
const int32_t SUCCESS = 0;

#define RES_SOUNDS "res/sounds/"
#define RES_SAMPLES "samples/"
#define RES_ELECTRIC "electric_string/"
#define RES_NYLON "nylon_string/"

//const char soundFile[] = "sounds/electric_string/guitar_electric_string_1_e4.wav";

namespace SOUNDS {

	const char SAMPLE_STEREO[] = RES_SOUNDS RES_SAMPLES "stereo.wav";
	const char SAMPLE_MONO[]   = RES_SOUNDS RES_SAMPLES "mono.wav";

	const char ELECTRIC_1E4[] = RES_SOUNDS RES_ELECTRIC "1_e4.wav";
	const char ELECTRIC_2B3[] = RES_SOUNDS RES_ELECTRIC "2_b3.wav";
	const char ELECTRIC_3G3[] = RES_SOUNDS RES_ELECTRIC "3_g3.wav";
	const char ELECTRIC_4D3[] = RES_SOUNDS RES_ELECTRIC "4_d3.wav";
	const char ELECTRIC_5A2[] = RES_SOUNDS RES_ELECTRIC "5_a2.wav";
	const char ELECTRIC_6E2[] = RES_SOUNDS RES_ELECTRIC "6_e2.wav";


	const array<const char*, 6> SOUND_FILES {
		ELECTRIC_6E2,
		ELECTRIC_5A2,
		ELECTRIC_4D3,
		ELECTRIC_3G3,
		ELECTRIC_2B3,
		ELECTRIC_1E4,
	};

}

