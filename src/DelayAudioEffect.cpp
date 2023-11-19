#include "../include/DelayAudioEffect.h"

void DelayAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Basic Delay ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 


    ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);
    ImGui::SliderInt("Delay [ms]", &delay, 0, 1000);

    ImGui::End();
}