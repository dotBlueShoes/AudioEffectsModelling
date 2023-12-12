#include "ReverbAudioEffect.h"

void ReverbAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    //wetSoundSize = drySoundSize + Math::MilisecondsToSample(delay, 44100);
}

void ReverbAudioEffect::applyEffect() {
        // Implementation specific to DelayAudioEffect
}


void ReverbAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Reverb ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    ImGui::SliderInt("Delay 1 [-]", &delay1, 0, 4000);
    ImGui::SliderInt("Delay 2 [-]", &delay2, 0, 4000);
    ImGui::SliderInt("Delay 3 [-]", &delay3, 0, 4000);
    ImGui::SliderInt("Delay 4 [-]", &delay4, 0, 4000);

    ImGui::End();
}