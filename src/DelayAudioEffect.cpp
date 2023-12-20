#include "DelayAudioEffect.h"


void DelayAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    auto&& delayInSamples = Math::MilisecondsToSample(delay, 44100);

    // Save for use during applyEffect.
    cachedDrySoundSize = drySoundSize;
    cachedDelayInSamples = delayInSamples;

    wetSoundSize = drySoundSize + (delayInSamples * iterations);
}


void DelayAudioEffect::applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& wetSound) {

    const auto&& feedbackNormalized = Math::NormalizePercent(feedback);
    const auto&& dryNormalized = Math::NormalizePercent(dry);
    const auto&& wetNormalized = Math::NormalizePercent(wet);

    auto&& delayInSamples = cachedDelayInSamples;

    // Create a copy of yet unmodified data buffor.
    int16_t* drySoundData = new int16_t[cachedDrySoundSize];
    std::memcpy(drySoundData, wetSound.pcmData, cachedDrySoundSize * 2 /* int16 */);

    { // Apply wet layer. 

        { // Apply 1st delay.
            // 0'ed sound for delayInSamples value.
            std::memset(wetSound.pcmData, 0, delayInSamples * 2 /* int16 */);

            // Set the 1'st echo sound with said wet% level and feedback.
            for (size i = 0; i < cachedDrySoundSize; ++i) {
                auto&& sample = wetSound.pcmData[delayInSamples + i];
                sample = (float)(drySoundData[i]) * wetNormalized * feedbackNormalized;
            }
        }

        // Apply echo.
        for (size i = 1; i < iterations; ++i) {
            for (size j = 0; j < cachedDrySoundSize; ++j) {
                auto&& sample = wetSound.pcmData[(delayInSamples * (i + 1)) + j];

                sample = sample + ((float)(drySoundData[j]) * std::pow(feedbackNormalized, (i + 1)));
            }
        }
    }

    // Apply dry layer.
    for (size i = 0; i < cachedDrySoundSize; ++i) {
        auto&& sample = wetSound.pcmData[i];
        sample = sample + ((float)(drySoundData[i]) * dryNormalized);
    }

    delete[] drySoundData;

}


void DelayAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Basic Delay ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    ImGui::Text("Delays the initial sound by time set");
    ImGui::SliderInt("Delay [ms]", &delay, 0, 1000);

    ImGui::Dummy(guiControlOffset);
    ImGui::Text("Echo");
    ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);
    ImGui::SliderInt("Iterations [-]", &iterations, 1, 20);

    ImGui::Dummy(guiControlOffset);
    ImGui::Text("Mixer");
    ImGui::SliderFloat("Wet [%]", &wet, 0, 100);
    ImGui::SliderFloat("Dry [%]", &dry, 0, 100);

    ImGui::End();
}