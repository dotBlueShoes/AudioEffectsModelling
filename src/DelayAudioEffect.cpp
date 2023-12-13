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


void DelayAudioEffect::applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& wetSound) {

    auto&& delayInSamples = Math::MilisecondsToSample(delay, 44100);
    auto&& feedbackNormalized = Math::NormalizePercent(feedback);

    spdlog::info("a: {}, b: {}, c: {}", cachedDrySoundSize, cachedWetSoundSize, delayInSamples);
    spdlog::info("f: {}", feedbackNormalized);

    // Create a copy of unmodified data buffor.
    int16_t* drySoundData = new int16_t[cachedDrySoundSize];
    std::memcpy(drySoundData, wetSound.pcmData, cachedDrySoundSize * 2 /* int16 */);

    //for (size i = 0; i < cachedDrySoundSize; ++i) {
    //    wetSound.pcmData[i] *= feedbackNormalized;
    //}

    //spdlog::info("ws: {}", wetSound.pcmSize);
    
    //uint64_t temp = 0;

    // Add delayed sound
    //for (size i = 0; i < cachedDrySoundSize; ++i) {
    //    //temp += wetSound.pcmData[delayInSamples + i] < 0;
    //    wetSound.pcmData[delayInSamples + i] = wetSound.pcmData[delayInSamples + i] + drySoundData[i];
    //}


    

    // Brak initial delay!!!!

    // 4400, 8800, 17600
    const auto&& effectStart = cachedWetSoundSize - originalSoundSize;

    spdlog::info("s: {}, a: {}, b: {}", effectStart, cachedDrySoundSize, wetSound.pcmSize);

    for (size i = 0; i < cachedDrySoundSize; ++i) {
        wetSound.pcmData[effectStart + i] = wetSound.pcmData[effectStart + i] + ((float)(drySoundData[i]) * feedbackNormalized);
    }

    //for (size i = 0; i < cachedDrySoundSize; ++i) {
    //    auto&& currentSample = wetSound.pcmData[effectStart + i];
    //    currentSample = delayInSamples + ((float)(drySoundData[i]) * feedbackNormalized);
    //}


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