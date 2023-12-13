#include "DistortionAudioEffect.h"
#include <iostream>



void DistortionAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {

}

void DistortionAudioEffect::applyEffect(const int16_t* drySoundData, SoundIO::ReadWavData& sound)
{
    if (type == 0)
    {
        wetSound = gain * ((drySound * 3.0f) / 2.0f) * (1.0f - ((drySound * drySound) / 3.0f));
    }
    else if (type == 1)
    {
        wetSound = gain * (tanh(drySound * kParameter) / tanh(drySound));
    }

}

void DistortionAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Distortion ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    ImGui::SliderInt("Type [0, 1]", &type, 0, 1);
    ImGui::SliderFloat("Gain [-]", &gain, 0, 10);
    ImGui::SliderFloat("Parameter k [-]", &kParameter, 0, 10);


    ImGui::End();
}