#pragma once

#include "Framework.hpp"
#include "Math.hpp"

class AudioEffect
{
public:
    int windowNumber = 0;
    //virtual void applyEffect(size_t bufforSize, int16_t* dryBuffor, int16_t* wetBuffor) = 0;  // Pure virtual function for applying the effect
    virtual void applyEffect() = 0;  // Pure virtual function for applying the effect
    virtual void getProcessedSize(const size_t& drySoundSize, const uint32_t& sampleRate, size_t& wetSoundSize) = 0;
    virtual ~AudioEffect() {}  // Ensure a virtual destructor for proper cleanup
    virtual void DisplayEffectWindow() = 0;
    void setWindowNumber(int x) {
        windowNumber = x;
    }
};

