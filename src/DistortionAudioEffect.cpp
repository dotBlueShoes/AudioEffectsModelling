#include "DistortionAudioEffect.h"

void DistortionAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {

}

void DistortionAudioEffect::applyEffect() {

}

void DistortionAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Distortion ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    ImGui::SliderInt("Type [0, 1]", &type, 0, 1);
    ImGui::SliderFloat("Gain [-]", &gain, 0, 10);



    ImGui::End();
}