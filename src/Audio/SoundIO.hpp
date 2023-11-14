#pragma once
#include "Framework.hpp"

#include <dr_wav.h>


// Sygna� analogowy - Ci�g�y w czasie i amplitudzie, podatny na zniekszta�cenia i trudny do przenoszenia na du�e odleg�o�ci.
//  Ciep�e i naturalne brzmienie.

// Sygna� cyfrowy - Reprezentowany przez ci�g binarny, jest odporny na zak��cenia i �atwy do strumieniowania, skalowania i 
//  przesy�ania na du�e odleg�o�ci.

// PCM - Pulse-code modulation
// Jest to metoda s�u��ca do cyfrowej reprezentacji rzeczywistego sygna�u. W struminiowaniu PCM amplituda sygna�u analogowego
//  jest pr�bkowana w jednakowych odst�pach, a ka�da pr�bka jest kwantowana do najbli�szej warto�ci w zakresie stopni cyfrowych.
//  Strumie� PCM ma dwie podstawowe w�a�ciwo�ci, kt�re oke�laj� wierno�� strumienia wzgl�dem oryginalnego sygna�u analogowego:
//   - Cz�stotliwo�� pr�bkowania ( Sampling Rate ), czyli zrobiona liczba pr�bek na sekund�.
//   - G��bia bitowa ( Bit Depth ), liczba bit�w opisuj�cych jedn� pr�bk� d�wi�ku (tzn. liczb� odzwierciedlaj�c� chwilow� warto�� sygna�u).

// FIR filters
// DSP filter
// IIR filters

// PCM Data format
// dzwi�k nie jest grany je�li pomi�dzy samplami nie ma r�nicy...
// (*= 2) == (<<= 1) dzwi�k staje si� g�o�niejszy. 0 - pojawia si� na m�odszym bitcie
// (/= 2) == (>>= 1) dzwi�k jest bardzo g�o�ny i z przesterami. 0 = pojawia si� na starszym bitcie
// Czyli:
// 0b0111'1111'1111'1111 - dzwi�k najg�o�niejszy
// 0b1111'1111'1111'1111 - dzwi�k najcichszy

namespace SoundIO {


    struct ReadWavData {

        uint32_t channels = 0;
        uint32_t sampleRate = 0;
        drwav_uint64 totalPCMFrameCount = 0;
        std::vector<int16_t> pcm;

        drwav_uint64 getTotalSamples() {
            return totalPCMFrameCount * channels;
        }

    };


    // Read a mono file using dr_wav.h
	auto ReadMono (const char* const soundFile, ReadWavData& monoData) {

        drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16(
            soundFile,
            &monoData.channels, &monoData.sampleRate, &monoData.totalPCMFrameCount,
            nullptr
        );

        if (pSampleData == nullptr) {
            std::cerr << "fail!" << std::endl;
            drwav_free(pSampleData, nullptr);
            exit(FAILURE);
        }

        if (monoData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max())) {
            std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
            drwav_free(pSampleData, nullptr);
            exit(FAILURE);
        }

        monoData.pcm.resize(size_t(monoData.getTotalSamples()));
        std::memcpy(monoData.pcm.data(), pSampleData, monoData.pcm.size() * /* two bytes in int16 */ 2);
        drwav_free(pSampleData, nullptr);

	}

    // Read a stereo file using dr_wav.h
    auto ReadStereo(const char* const soundFile, ReadWavData& stereoData) {
        
        drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16(
            soundFile, 
            &stereoData.channels, &stereoData.sampleRate, &stereoData.totalPCMFrameCount, 
            nullptr
        );
        
        if (pSampleData == nullptr) {
            std::cerr << "failed to load audio file" << std::endl;
            exit(FAILURE);
        }

        if (stereoData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max())) {
            std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
            exit(FAILURE);
        }

        stereoData.pcm.resize(size_t(stereoData.getTotalSamples()));
        std::memcpy(stereoData.pcm.data(), pSampleData, stereoData.pcm.size() * /* two bytes in s15 */ 2);
        drwav_free(pSampleData, nullptr);
        
    }

}