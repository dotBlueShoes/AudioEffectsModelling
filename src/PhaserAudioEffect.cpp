#include "../include/PhaserAudioEffect.h"

void PhaserAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Phaser ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);
    ImGui::SliderInt("Rate [ms]", &rate, 0, 1000);
    ImGui::SliderFloat("Depth [%]", &depth, 0, 100);
    ImGui::SliderFloat("Offset [-]", &offset, -1.0, 1.0);
    ImGui::SliderFloat("Intensity [%]", &intensity, 0, 100);
    ImGui::SliderInt("Stages [ms]", &stages, 1, 3);

    ImGui::End();
}