#pragma once
#include "Framework.hpp"

#include "Audio/SoundIO.hpp"
#include "Math.hpp"
#include "LFO.hpp"

const ImVec2 guiControlOffset(0, 2);

class AudioEffect
{
protected:
    float wet;
    float dry;
    int windowNumber = 0;
public:
    virtual void getWetSoundSize(const size& drySoundSize, size& wetSoundSize) = 0;
    virtual void applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) = 0;  // Pure virtual function for applying the effect

    AudioEffect(float newWet, float newDry) : wet(newWet), dry(newDry) {};
    virtual ~AudioEffect() {}  // Ensure a virtual destructor for proper cleanup
    virtual void DisplayEffectWindow() = 0;
    void setWindowNumber(int x) {
        windowNumber = x;
    }
};

