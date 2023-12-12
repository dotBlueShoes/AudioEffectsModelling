#pragma once
#include "Framework.hpp"

#include "Math.hpp"

class AudioEffect
{
protected:
    int windowNumber = 0;
public:
    virtual void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) = 0;
    virtual void applyEffect() = 0;  // Pure virtual function for applying the effect
    virtual ~AudioEffect() {}  // Ensure a virtual destructor for proper cleanup
    virtual void DisplayEffectWindow() = 0;
    void setWindowNumber(int x) {
        windowNumber = x;
    }
};

