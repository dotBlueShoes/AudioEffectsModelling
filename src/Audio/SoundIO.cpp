#include "SoundIO.hpp"

namespace SoundIO {


    void ReadMono(const char* const soundFile, ReadWavData& monoData) {
    
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
    
        //monoData.pcm.resize(size_t(monoData.getTotalSamples()));
        //std::memcpy(monoData.pcm.data(), pSampleData, monoData.pcm.size() * /* two bytes in int16 */ 2);
    
        monoData.pcmSize = (size)monoData.getTotalSamples();
        monoData.pcmData = new int16_t[monoData.pcmSize];
        std::memcpy(monoData.pcmData, pSampleData, monoData.pcmSize * /* two bytes in int16 */ 2);
    
        drwav_free(pSampleData, nullptr);
    
    }
    
    
    void DestorySoundData(ReadWavData& sound) {
        delete[] sound.pcmData;
    }
    
    
    void ReadStereo(const char* const soundFile, ReadWavData& stereoData) {
    
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
    
        //stereoData.pcm.resize(size_t(stereoData.getTotalSamples()));
        //std::memcpy(stereoData.pcm.data(), pSampleData, stereoData.pcm.size() * /* two bytes in s15 */ 2);
    
        stereoData.pcmSize = (size)stereoData.getTotalSamples();
        stereoData.pcmData = new int16_t[stereoData.pcmSize];
        std::memcpy(stereoData.pcmData, pSampleData, stereoData.pcmSize * /* two bytes in int16 */ 2);
    
        drwav_free(pSampleData, nullptr);
    
    }

}