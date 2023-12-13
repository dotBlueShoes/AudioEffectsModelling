#include "DelayAudioEffect.h"


void DelayAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    auto&& delayInSamples = Math::MilisecondsToSample(delay, 44100);

    // Save for use during applyEffect.
    cachedDrySoundSize = drySoundSize;
    wetSoundSize = drySoundSize + delayInSamples;
    cachedWetSoundSize = wetSoundSize;

    //spdlog::info("dis: {}", delayInSamples);
    //spdlog::info("dss: {}", cachedDrySoundSize);
    //spdlog::info("wss: {}", cachedWetSoundSize);
}


void DelayAudioEffect::applyEffect(const int16_t* aaa, SoundIO::ReadWavData& wetSound) {

    auto&& delayInSamples = Math::MilisecondsToSample(delay, 44100);
    auto&& feedbackNormalized = Math::NormalizePercent(feedback);

    //spdlog::info("c: {}", cachedDrySoundSize);

    // Create a copy of unmodified data buffor.
    int16_t* drySoundData = new int16_t[cachedDrySoundSize];
    std::memcpy(drySoundData, wetSound.pcmData, cachedDrySoundSize * 2 /* int16 */);

    //spdlog::info("copied");

    //spdlog::info("f: {}", feedbackNormalized);

    for (size i = 0; i < cachedDrySoundSize; ++i) {
        wetSound.pcmData[i] *= feedbackNormalized;
    }

    //spdlog::info("ws: {}", wetSound.pcmSize);
    
    //uint64_t temp = 0;

    // Add delayed sound
    for (size i = 0; i < cachedDrySoundSize; ++i) {
        //temp += wetSound.pcmData[delayInSamples + i] < 0;
        wetSound.pcmData[delayInSamples + i] += drySoundData[i];
    }

    //spdlog::info("t: {}, {}", temp, delayInSamples);

    delete[] drySoundData;


    //spdlog::info("huh");
}


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