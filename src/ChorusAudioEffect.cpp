#include "ChorusAudioEffect.h"

void ChorusAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    wetSoundSize = drySoundSize + Math::MilisecondsToSample(modDepth, 44100);
}

void ChorusAudioEffect::applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) {

    // 1. Reset
    lfo.Reset(44100);

    // 2-3. Set params
    lfo.Initialize((Waveform)waveform, lfoRate);

    // 4. Call processAudioSample pass input value in and receiving the output value as the return variable.

    for (size i = 0; i < 20; ++i) {
        LFOResult result = lfo.RenderAudio();

        spdlog::info("a: {:1.10f}", result.normal);
        spdlog::info("b: {:1.10f}", result.inverted);
        spdlog::info("c: {:1.10f}", result.quadPhase);
        spdlog::info("d: {:1.10f}", result.quadPhaseInverted);
    }
}

void ChorusAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Chorus ";
    const size waveformTypeCount(3);
    const char* waveformNames[waveformTypeCount] { "Sine", "Triangle", "Sawtooth" };
    const char* elementName = (waveform >= 0 && waveform < waveformTypeCount) ? waveformNames[waveform] : "Unknown";

    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    ImGui::SliderFloat("Delay [ms]", &minDelay, 0, 30);
    ImGui::SliderFloat("Depth [ms]", &modDepth, 1, 30);
    //ImGui::SliderInt("SampleRate [Hz]", &sampleRate, 0, 1000);

    // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.
    ImGui::SliderInt("Waveform", &waveform, 0, waveformTypeCount - 1, elementName); 

    ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);

    ImGui::SliderFloat("Wet [%]", &wet, 0, 100);
    ImGui::SliderFloat("Dry [%]", &dry, 0, 100);

    ImGui::End();
}