#pragma once
#include "Framework.hpp"

#include "Audio/SoundIO.hpp"
#include "Math.hpp"
#include "LFO.hpp"

class AudioEffect
{
protected:
    float wet = 100;
    float dry = 100;
    int windowNumber = 0;
public:
    virtual void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) = 0;
    virtual void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) = 0;  // Pure virtual function for applying the effect

    virtual ~AudioEffect() {}  // Ensure a virtual destructor for proper cleanup
    virtual void DisplayEffectWindow() = 0;
    void setWindowNumber(int x) {
        windowNumber = x;
    }
};

