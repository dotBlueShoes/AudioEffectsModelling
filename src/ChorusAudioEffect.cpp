#include "ChorusAudioEffect.h"

void ChorusAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Chorus ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    ImGui::SliderInt("Delay [ms]", &delay, 0, 1000);
    ImGui::SliderInt("Depth [ms]", &depth, 0, 1000);
    ImGui::SliderInt("SampleRate [Hz]", &sampleRate, 0, 1000);
    ImGui::SliderFloat("Feedback [%]", &feedback, 0, 1000);
    ImGui::SliderInt("Wet [dB]", &wet, -32, 32);
    ImGui::SliderInt("Dry [dB]", &dry, -32, 32);

    ImGui::End();
}