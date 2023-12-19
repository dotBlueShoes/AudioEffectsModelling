#include "PhaserAudioEffect.h"

void PhaserAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    cachedDrySoundSize = drySoundSize;
    wetSoundSize = drySoundSize;
}

void PhaserAudioEffect::applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) {

    // 1. Reset
    lfo.Reset(lfoSampleRate);

    // 2-3. Set params
    lfo.Initialize(Waveform::sine, lfoFrequency);

    for (size i = 0; i < cachedDrySoundSize; ++i) {
        auto&& lfoCurrent = lfo.RenderAudio().normal;

        double depthNormalized = Math::NormalizePercent(depth);
        double modulatorValue = lfoCurrent * depthNormalized;

        spdlog::info("v: {}", modulatorValue);

        // --- calculate modulated values for each APF
        //AudioFilterParameters params = apf[0].getParameters();
        //params.fc = doBipolarModulation(modulatorValue, apf0_minF, apf0_maxF);
        //apf[0].setParameters(params);

        //params = apf[1].getParameters();
        //params.fc = doBipolarModulation(modulatorValue, apf1_minF, apf1_maxF);
        //apf[1].setParameters(params);
    }

}

void PhaserAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Phaser ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    
    //ImGui::SliderInt("Rate [ms]", &lfoSampleRate, 0, 1000);

    ImGui::SliderFloat("SampleRate [-]", &lfoSampleRate, 1, 10);
    ImGui::SliderFloat("Depth [%]", &depth, 0, 100);

    ImGui::SliderFloat("Offset [-]", &offset, -1.0, 1.0);
    ImGui::SliderFloat("Intensity [%]", &intensity, 0, 100);
    ImGui::SliderInt("Stages [ms]", &stages, 1, 3);

    ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);
    ImGui::SliderInt("Feedback Iterations [-]", &feedbackIterations, 0, 20);

    ImGui::SliderFloat("Wet [%]", &wet, 0, 100);
    ImGui::SliderFloat("Dry [%]", &dry, 0, 100);

    ImGui::End();
}