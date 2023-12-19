#include "PhaserAudioEffect.h"


double PhaserAudioEffect::ModulateFrequency(double lfoValue, double minFrequency, double maxFrequency) {
    // Ensure lfoValue is in the range [-1, 1]
    lfoValue = std::max(-1.0, std::min(lfoValue, 1.0));

    // Scale and shift the LFO value from [-1, 1] to [0, 1]
    double normalizedLfoValue = (lfoValue + 1.0) * 0.5;

    // Interpolate between the min and max frequencies based on the LFO value
    return minFrequency + normalizedLfoValue * (maxFrequency - minFrequency);
}


void PhaserAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    cachedDrySoundSize = drySoundSize;
    wetSoundSize = drySoundSize;
}

void PhaserAudioEffect::applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& sound) {
    // Reset and initialize the LFO
    lfo.Reset(lfoSampleRate);
    lfo.Initialize(Waveform::sine, lfoFrequency);

    const auto&& dryNormalized = Math::NormalizePercent(dry);
    const auto&& wetNormalized = Math::NormalizePercent(wet);


    // The maximum value for scaling from int16_t to float range [-1.0, 1.0]
    constexpr float maxInt16 = static_cast<float>(std::numeric_limits<int16_t>::max());

    // Define min and max frequencies for each APF stage
    const double minFrequencies[PHASER_STAGES] = { APF_0_MIN_FREQUENCY, APF_1_MIN_FREQUENCY, /*...*/ };
    const double maxFrequencies[PHASER_STAGES] = { APF_0_MAX_FREQUENCY, APF_1_MAX_FREQUENCY, /*...*/ };

    // Process each sample through the phaser stages
    for (size i = 0; i < originalSoundSize; ++i) {
        // Render the current LFO value
        auto&& lfoCurrent = lfo.RenderAudio().normal;

        // Modulate the frequency of each all-pass filter based on the LFO value
        for (size stage = 0; stage < PHASER_STAGES; ++stage) {
            double modulatedFrequency = ModulateFrequency(lfoCurrent, minFrequencies[stage], maxFrequencies[stage]);
            allPassFilter[stage].fc = modulatedFrequency;
            allPassFilter[stage].CalculateFilterCoefficients();
        }


        // Calculate feedback components
        // ...
        // Omitted for brevity - implement feedback calculations as per the algorithm

        // Process the sample through the cascade of APFs
        float inputSample = static_cast<float>(sound.pcmData[i]) / maxInt16;
        float wetSample = inputSample; // Start with the input sample

        for (auto& filter : allPassFilter) {
            //spdlog::info("before norm sample : {}", wetSample);
            wetSample = filter.process(wetSample);
        }

        // Combine the wet and dry signals
        float drySample = inputSample * dryNormalized;
        wetSample = (wetSample * wetNormalized) + drySample;

        // Clip the output to avoid overflow
        wetSample = std::max(-1.0f, std::min(wetSample, 1.0f));

        //spdlog::info("dry sample : {}", sound.pcmData[i]);
        // Convert back to int16_t and write to the output buffer
        sound.pcmData[i] = static_cast<int16_t>(wetSample * maxInt16);
       // spdlog::info("sample : {}", sound.pcmData[i]);
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