#include "DistortionAudioEffect.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdint>


void DistortionAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    cachedDrySoundSize = drySoundSize;
    cachedWetSoundSize = wetSoundSize;
}

void DistortionAudioEffect::applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound)
{
    // Variables for normalization
    float maxVal = 0.0f;
    float minVal = 0.0f;

    // The maximum value for scaling from int16_t to float range [-1.0, 1.0]
    const float maxInt16 = static_cast<float>(std::numeric_limits<int16_t>::max());

    // First pass: Apply distortion and find max/min values for normalization
    for (size i = 0; i < cachedDrySoundSize; ++i) {
        // Convert to float and scale to range [-1.0, 1.0]
        float sample = sound.pcmData[i] / maxInt16;

        if (type == 0) {
            // Simple clipping distortion
            sample = gain * sample;
            sample = std::max(-1.0f, std::min(sample, 1.0f)); // Clipping to [-1, 1]
        }
        else if (type == 1) {
            // Weird distortion
            sample = gain * ((sample * 3.0f) / 2.0f) * (1.0f - ((sample * sample) / 3.0f));
        }
        else if (type == 2) {
            // Weirder distortion
            sample = gain * (tanh(sample * kParameter));
        }

        // Store the processed sample back, scaling it to the original int16_t range
        sound.pcmData[i] = static_cast<int16_t>(std::max(-maxInt16, std::min(sample * maxInt16, maxInt16)));

        // Update max/min values for normalization
        maxVal = std::max(maxVal, sample);
        minVal = std::min(minVal, sample);
    }
    spdlog::info("maxVal : {}", maxVal);
    spdlog::info("minVal : {}", minVal);

    // Compute normalization factor
    float normalizationFactor = std::max(std::abs(maxVal), std::abs(minVal));
    spdlog::info("normalizationFactor : {}", normalizationFactor);

    // Second pass: Normalize and scale back to int16_t
    if (normalizationFactor > 0.0f) {
        for (size i = 0; i < cachedDrySoundSize; ++i) {
            float sample = sound.pcmData[i] / maxInt16; // Scale to [-1.0, 1.0] range
            sample = sample / normalizationFactor; // Normalize
            sound.pcmData[i] = static_cast<int16_t>(sample * maxInt16); // Scale back to int16_t range
        }
    }

    spdlog::info("maxVal : {}", maxVal);
    spdlog::info("minVal : {}", minVal);
    spdlog::info("normalizationFactor : {}", normalizationFactor);


   /* if (type == 0)
    {
        wetSound = gain * ((drySound * 3.0f) / 2.0f) * (1.0f - ((drySound * drySound) / 3.0f));
    }
    else if (type == 1)
    {
        wetSound = gain * (tanh(drySound * kParameter) / tanh(drySound));
    }*/

    /*
    for (size i = 0; i < cachedDrySoundSize; ++i) {
        if (type == 0)
        {
           // spdlog::info("type 0 data index : {}", i);
           // spdlog::info("type 0 raw data : {}", sound.pcmData[i]);
            sound.pcmData[i] = gain * ((sound.pcmData[i] * 3.0f) / 2.0f); //* (1.0f - ((sound.pcmData[i] * sound.pcmData[i]) / 3.0f)); 
            //sound.pcmData[i] = sound.pcmData[i];

           // spdlog::info("type 0 wet data : {}", sound.pcmData[i]);
        }
        else if (type == 1)
        {
            //spdlog::info("type 1 data : {}", sound.pcmData[i]);
            sound.pcmData[i] = gain * (tanh(sound.pcmData[i] * kParameter));
        }
    }
    */

}

void DistortionAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Distortion ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    ImGui::SliderInt("Type [0, 1]", &type, 0, 2);
    ImGui::SliderFloat("Gain [-]", &gain, 0, 10);
    ImGui::SliderFloat("Parameter k [-]", &kParameter, 0, 2);


    ImGui::End();
}