#pragma once
#include "Framework.hpp"

#include <dr_wav.h>


// Sygna³ analogowy - Ci¹g³y w czasie i amplitudzie, podatny na zniekszta³cenia i trudny do przenoszenia na du¿e odleg³oœci.
//  Ciep³e i naturalne brzmienie.

// Sygna³ cyfrowy - Reprezentowany przez ci¹g binarny, jest odporny na zak³ócenia i ³atwy do strumieniowania, skalowania i 
//  przesy³ania na du¿e odleg³oœci.

// PCM - Pulse-code modulation
// Jest to metoda s³u¿¹ca do cyfrowej reprezentacji rzeczywistego sygna³u. W struminiowaniu PCM amplituda sygna³u analogowego
//  jest próbkowana w jednakowych odstêpach, a ka¿da próbka jest kwantowana do najbli¿szej wartoœci w zakresie stopni cyfrowych.
//  Strumieñ PCM ma dwie podstawowe w³aœciwoœci, które okeœlaj¹ wiernoœæ strumienia wzglêdem oryginalnego sygna³u analogowego:
//   - Czêstotliwoœæ próbkowania ( Sampling Rate ), czyli zrobiona liczba próbek na sekundê.
//   - G³êbia bitowa ( Bit Depth ), liczba bitów opisuj¹cych jedn¹ próbkê dŸwiêku (tzn. liczbê odzwierciedlaj¹c¹ chwilow¹ wartoœæ sygna³u).

// FIR filters
// DSP filter
// IIR filters

// PCM Data format
// b³êdne !
// dzwiêk nie jest grany jeœli pomiêdzy samplami nie ma ró¿nicy...
// (*= 2) == (<<= 1) dzwiêk staje siê g³oœniejszy. 0 - pojawia siê na m³odszym bitcie
// (/= 2) == (>>= 1) dzwiêk jest bardzo g³oœny i z przesterami. 0 = pojawia siê na starszym bitcie
// Czyli:
// 0b0111'1111'1111'1111 - dzwiêk najg³oœniejszy
// 0b1111'1111'1111'1111 - dzwiêk najcichszy

// Volume w formacie PCM zmieniamy za pomoc¹ mno¿enia!
// * 0 -> wyciszenie
// * 2 -> x2

namespace SoundIO {


    struct ReadWavData {

        uint32_t channels = 0;
        uint32_t sampleRate = 0;
        drwav_uint64 totalPCMFrameCount = 0;

        //std::vector<int16_t> pcm;

        size pcmSize = 0;
        int16_t* pcmData = nullptr;

        drwav_uint64 getTotalSamples() {
            return totalPCMFrameCount * channels;
        }

    };


    // Read a mono file using dr_wav.h.
    void ReadMono(const char* const soundFile, ReadWavData& monoData);

    // Frees memory.
    void DestorySoundData(ReadWavData& sound);

    // Read a stereo file using dr_wav.h
    void ReadStereo(const char* const soundFile, ReadWavData& stereoData);

}