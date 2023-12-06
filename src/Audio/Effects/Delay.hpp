#pragma once
#include "../Framework.hpp"

// To change data buffor We change the following:
// alSourcei(currentSource, AL_BUFFER, drawCallParams.sounds[i]);

// Because thats only a pointer. Instead of changing the whole buffor every param change we could apply
//  the effect only partially.

// We could use more then one buffer. Apply the effect at the current buffer and at the next one.
// https://developer.tizen.org/dev-guide/2.4/org.tizen.tutorials/html/native/multimedia/openal_tutorial_n.htm

// PCM data fills buffer A, B, C, D each 1024
// Sounds starts playing. We're at A.
// User sets Delay effect. We're replacing our buffor A with the one using this effect and feedback data from buffor D.
// As we reach buffor B we do the same for B and so on. Be aware that buffor A and A* are different! - We don't apply that effect inf. that way.

//

namespace Delay {

	

	// I don't need the position in the buffer we're curr at! I apply that effect for whole buffor.
	//  How do i tell that buffor A previous buffor is buffor D? Can i make it at first null == no feedback ?
	
	// Can I apply the effect at buffor change ? or somewhere before ?

	// instead of previousBuffer i should use a vector to get buffers beyond 1024 samples ...

	auto ApplyDelay(const ALuint& dryBuffer, const ALuint& previousBuffer, const size_t& bufferSize, const float& feedback, const uint32_t& delay) {
		ALuint wetBuffer = dryBuffer;

		// proccess

		return wetBuffer;
	}

	// Not exacly. ALuint type is for declared buffor. I need to create/change a buffor.
	auto ApplyDelay(const size_t& dryBufferIndex, const ALuint* buffers, const size_t& bufferSize, const float& feedback, const uint32_t& delay) {
		ALuint wetBuffer = 0;


		// Get feedback buffor.
		// !!! delay != 0 
		size_t feedbackBufforIndex = delay / bufferSize;

		// proccess
		auto&& feedbackBuffer = buffers[feedbackBufforIndex];
		auto&& currentBuffer = buffers[dryBufferIndex];

		// for (uint32_t i = 0; i < bufferSize; ++i) {
		//	wetBuffer[i] = currentBuffer[i] + (feedback * feedbackBuffer[i])
		// }


		return wetBuffer;
	}

}