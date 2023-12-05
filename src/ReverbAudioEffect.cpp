#include "ReverbAudioEffect.h"

void ReverbAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Reverb ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    ImGui::SliderInt("Delay 1 [ms]", &delay1, 0, 1000);
    ImGui::SliderInt("Delay 2 [ms]", &delay2, 0, 1000);
    ImGui::SliderInt("Delay 3 [ms]", &delay3, 0, 1000);
    ImGui::SliderInt("Delay 4 [ms]", &delay4, 0, 1000);

    ImGui::End();
}