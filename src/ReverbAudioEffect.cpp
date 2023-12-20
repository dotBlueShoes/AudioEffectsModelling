#include "ReverbAudioEffect.h"




void ReverbAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    // Determine the wet sound size based on the maximum delay
    int maxDelay = std::max({ delay1, delay2, delay3, delay4 });
    int delaySamples = Math::MilisecondsToSample(maxDelay, sampleRate);
    cachedDrySoundSize = drySoundSize;
    cachedWetSoundSize = drySoundSize + delaySamples;
    wetSoundSize = cachedWetSoundSize;
}

void ReverbAudioEffect::applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) {

    const auto&& dryNormalized = Math::NormalizePercent(dry);
    const auto&& wetNormalized = Math::NormalizePercent(wet);

    int16_t* drySoundData = new int16_t[cachedDrySoundSize];
    std::memcpy(drySoundData, sound.pcmData, cachedDrySoundSize * 2 /* int16 */);

    combFilters.clear();
    combFilters.push_back(CombFilter(Math::MilisecondsToSample(delay1, sampleRate), RT60, sampleRate));
    combFilters.push_back(CombFilter(Math::MilisecondsToSample(delay2, sampleRate), RT60, sampleRate));
    combFilters.push_back(CombFilter(Math::MilisecondsToSample(delay3, sampleRate), RT60, sampleRate));
    combFilters.push_back(CombFilter(Math::MilisecondsToSample(delay4, sampleRate), RT60, sampleRate));
    constexpr float maxInt16 = static_cast<float>(std::numeric_limits<int16_t>::max());

    for (size i = 0; i < cachedWetSoundSize; ++i) {
        float inputSample = sound.pcmData[i] / maxInt16;
        float wetSample = 0.0f;

        // Process the sample through each comb filter
        for (auto& filter : combFilters) {
            wetSample += filter.process(inputSample);
            //spdlog::info("out sample : {}", wetSample);
        }   
        wetSample /= combFilters.size(); // Normalize the combined output
        wetSample *= wetNormalized;
        wetSample = std::max(-1.0f, std::min(wetSample, 1.0f)); // Clipping to [-1, 1]
        sound.pcmData[i] = static_cast<int16_t>(wetSample * maxInt16);
    }


    // Apply dry layer.
    for (size i = 0; i < cachedDrySoundSize; ++i) {
        auto&& sample = sound.pcmData[i];
        sample = sample + ((float)(drySoundData[i]) * dryNormalized);
    }

    delete[] drySoundData;

   /*// The maximum value for scaling from int16_t to float range [-1.0, 1.0]
    constexpr float maxInt16 = static_cast<float>(std::numeric_limits<int16_t>::max());

    const auto&& dryNormalized = Math::NormalizePercent(dry);
    const auto&& wetNormalized = Math::NormalizePercent(wet);
    // Apply reverb effect directly within the buffer
    int delaySamples1 = Math::MilisecondsToSample(delay1, sampleRate);
    int delaySamples2 = Math::MilisecondsToSample(delay2, sampleRate);
    int delaySamples3 = Math::MilisecondsToSample(delay3, sampleRate);
    int delaySamples4 = Math::MilisecondsToSample(delay4, sampleRate);

    for (size i = 0; i < cachedWetSoundSize; ++i) {
        float wetSample = sound.pcmData[i] / maxInt16;
        if (i >= delaySamples1) wetSample += sound.pcmData[i - delaySamples1] / maxInt16 * decay * wetNormalized;
        if (i >= delaySamples2) wetSample += sound.pcmData[i - delaySamples2] / maxInt16 * decay * wetNormalized;
        if (i >= delaySamples3) wetSample += sound.pcmData[i - delaySamples3] / maxInt16 * decay * wetNormalized;
        if (i >= delaySamples4) wetSample += sound.pcmData[i - delaySamples4] / maxInt16 * decay * wetNormalized;


        wetSample = std::max(-1.0f, std::min(wetSample, 1.0f)); // Clipping to [-1, 1]
        sound.pcmData[i] = static_cast<int16_t>(wetSample * maxInt16);

    }*/
}


void ReverbAudioEffect::DisplayEffectWindow()
{
    const char STRING_EFFECT_BASIC_DELAY[] = "Effect Reverb ";
    char windowTitle[100]; // Buffer to store the final string with enough space

    snprintf(windowTitle, sizeof(windowTitle), "%s%d", STRING_EFFECT_BASIC_DELAY, windowNumber+1);

    ImGui::Begin(windowTitle); 

    // No brightness setting ??

    ImGui::Text("Different delay values simulate different rooms");
    ImGui::SliderInt("Delay 1 [sam]", &delay1, 0, 500);
    ImGui::SliderInt("Delay 2 [sam]", &delay2, 0, 500);
    ImGui::SliderInt("Delay 3 [sam]", &delay3, 0, 500);
    ImGui::SliderInt("Delay 4 [sam]", &delay4, 0, 500);

    ImGui::Dummy(guiControlOffset);
    ImGui::Text("How long will it take to decay - simulates longer and smaller enviroments");
    ImGui::SliderFloat("Decay [-]", &RT60, 0, 5);

    ImGui::Dummy(guiControlOffset);
    ImGui::Text("Echo");
    ImGui::SliderFloat("Wet [%]", &wet, 0, 100);
    ImGui::SliderFloat("Dry [%]", &dry, 0, 100);

    ImGui::End();
}