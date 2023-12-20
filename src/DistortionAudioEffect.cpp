#include "DistortionAudioEffect.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdint>


void DistortionAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    cachedDrySoundSize = drySoundSize;
    cachedWetSoundSize = wetSoundSize;
}

void DistortionAudioEffect::applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& wetSound)
{
    const auto&& dryNormalized = Math::NormalizePercent(dry);
    const auto&& wetNormalized = Math::NormalizePercent(wet);

    // The maximum value for scaling from int16_t to float range [-1.0, 1.0]
    constexpr float maxInt16 = static_cast<float>(std::numeric_limits<int16_t>::max());

    // Variables for normalization
    float maxVal = 0.0f;
    float minVal = 0.0f;

    

    int16_t* drySoundData = new int16_t[cachedDrySoundSize];
    std::memcpy(drySoundData, wetSound.pcmData, cachedDrySoundSize * 2 /* int16 */);

    // Apply wet layer
    // First pass: Apply distortion and find max/min values for normalization
    for (size i = 0; i < cachedDrySoundSize; ++i) {
        // Convert to float and scale to range [-1.0, 1.0]
        float sample = wetSound.pcmData[i] / maxInt16;
 

        if (type == 0) {
            // Simple clipping distortion
            sample = gain * sample;
            sample = std::max(-1.0f, std::min(sample, 1.0f)) * wetNormalized; // Clipping to [-1, 1]
        }
        else if (type == 1) {
            // Weird distortion
            sample = gain * ((sample * 3.0f) / 2.0f) * (1.0f - ((sample * sample) / 3.0f));
            sample = std::max(-1.0f, std::min(sample, 1.0f)) * wetNormalized; // Clipping to [-1, 1]
        }
        else if (type == 2) {
            // Weirder distortion
            sample = gain * (tanh(sample * kParameter));
            sample = std::max(-1.0f, std::min(sample, 1.0f)) * wetNormalized; // Clipping to [-1, 1]
        }
        // Store the processed sample back, scaling it to the original int16_t range
        wetSound.pcmData[i] = static_cast<int16_t>(std::max(-maxInt16, std::min(sample * maxInt16, maxInt16)));

        // Update max/min values for normalization
        maxVal = std::max(maxVal, sample);
        minVal = std::min(minVal, sample);
    }

    // Apply dry layer.
    for (size i = 0; i < cachedDrySoundSize; ++i) {
        auto&& sample = wetSound.pcmData[i];
        sample = sample + ((float)(drySoundData[i]) * dryNormalized);
    }

    delete[] drySoundData;


    // Compute normalization factor
    //float normalizationFactor = std::max(std::abs(maxVal), std::abs(minVal));
    //spdlog::info("normalizationFactor : {}", normalizationFactor);

    // Second pass: Normalize and scale back to int16_t
    //if (normalizationFactor > 0.0f) {
    //    for (size i = 0; i < cachedDrySoundSize; ++i) {
    //        float sample = sound.pcmData[i] / maxInt16; // Scale to [-1.0, 1.0] range
    //        sample = (sample * dryNormalized) / normalizationFactor; // Normalize
    //        sound.pcmData[i] = static_cast<int16_t>(sample * maxInt16); // Scale back to int16_t range
    //        sound.pcmData[i] *= wetNormalized;
    //        //spdlog::info("sample : {}", sound.pcmData[i]);
    //    }
    //}

}

void DistortionAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Distortion ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    ImGui::Text("Distortion algorithm");
    ImGui::SliderInt("Type [0, 2]", &type, 0, 2);

    ImGui::Dummy(guiControlOffset);
    ImGui::Text("How much overdrive is being produced");
    ImGui::SliderFloat("Gain [-]", &gain, 0, 10);

    if (type == 2) {
        ImGui::Dummy(guiControlOffset);
        ImGui::SliderFloat("Parameter k [-]", &kParameter, 0, 2);
    }

    ImGui::Dummy(guiControlOffset);
    ImGui::Text("Mixer");
    ImGui::SliderFloat("Wet [%]", &wet, 0, 100);
    ImGui::SliderFloat("Dry [%]", &dry, 0, 100);


    ImGui::End();
}