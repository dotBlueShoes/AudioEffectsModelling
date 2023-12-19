#include "ChorusAudioEffect.h"

void ChorusAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    cachedHalfDepthInSamples = Math::MilisecondsToSample(depth, 44100);
    cachedDelayInSamples = Math::MilisecondsToSample(delay, 44100);

    cachedDrySoundSize = drySoundSize;
    wetSoundSize = drySoundSize + cachedHalfDepthInSamples + (cachedDelayInSamples * feedbackIterations);
    cachedHalfDepthInSamples /= 2;
}

void ChorusAudioEffect::applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& wetSound) {

    const auto&& feedbackNormalized = Math::NormalizePercent(feedback);
    const auto&& dryNormalized = Math::NormalizePercent(dry);
    const auto&& wetNormalized = Math::NormalizePercent(wet);

    // 1. Reset
    lfo.Reset(lfoSampleRate);

    // 2-3. Set params
    lfo.Initialize((Waveform)waveform, lfoFrequency);

    // 4. Call processAudioSample pass input value in and receiving the output value as the return variable.

    // Create a copy of buffor with space before and after the original sound.
    int16_t* drySoundData = new int16_t[cachedDrySoundSize + (cachedHalfDepthInSamples * 2)];
    std::memset(drySoundData, 0, cachedHalfDepthInSamples * 2 /* int16 */);
    std::memcpy(drySoundData + cachedHalfDepthInSamples, wetSound.pcmData, cachedDrySoundSize * 2 /* int16 */);
    std::memset(drySoundData + cachedDrySoundSize + cachedHalfDepthInSamples, 0, cachedHalfDepthInSamples * 2 /* int16 */);

    // Spowolnienie/przyœpieszenie dzwiêku to zmiana pitchu
    //  ¿eby przyœpieszyæ dzwiêk omijamy co któryœ orginalny sample
    //  ¿eby spowolniæ dzwiêk dok³adamy ten sam sample

    // Prev
    //// Jeœli najpierw bêdê na i2015 wezmê i2023
    ////  a potem bêdê na i2016 i wezmê i2018
    ////  to znów otrzymujemy efekt czytania w z³ej kolejnoœci! A nie o to chodzi!

    { // Apply wet layer
        auto&& wetBeginIndex = cachedHalfDepthInSamples + cachedDelayInSamples;

        // 0'ed sound for delayInSamples value.
        std::memset(wetSound.pcmData, 0, wetBeginIndex * 2 /* int16 */);

        double currentIndex = wetBeginIndex;
        size j = 0;
        for (; currentIndex < cachedDrySoundSize && j < cachedDrySoundSize; ++j) {
            auto&& currentLFO = (lfo.RenderAudio().normal) + 1;
            auto&& drySampleI = j + wetBeginIndex;
            auto&& resultSample = wetSound.pcmData[drySampleI];

            currentIndex += currentLFO;

            resultSample = drySoundData[(size)currentIndex] * wetNormalized;
        }

        // Apply feedback-echo . Apply only on "flanger" and "none" settings.
        //if ((uint8_t)type <= ModulatedDelayType::flanger) {

            // Apply echo.
        for (size i = 0; i < feedbackIterations; ++i) {
            for (size j = 0; j < cachedDrySoundSize; ++j) {
                auto&& drySampleI = cachedHalfDepthInSamples + (cachedDelayInSamples * (i + 1)) + j;
                auto&& resultSample = wetSound.pcmData[drySampleI];
        
                resultSample = resultSample + ((float)(drySoundData[j]) * std::pow(feedbackNormalized, (i + 1)));
            }
        }

        //}
    }

    // Apply dry layer.
    for (size i = 0; i < cachedDrySoundSize; ++i) {
        auto&& sample = wetSound.pcmData[i];
        sample = sample + ((float)(drySoundData[i + cachedHalfDepthInSamples]) * dryNormalized);
    }

    delete[] drySoundData;

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

    { // BUTTONS
        ImGui::BeginDisabled(type == ModulatedDelayType::none);
        if (ImGui::Button("None")) {
            type = ModulatedDelayType::none;

            if (delay < 1) delay = 1;
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled(type == ModulatedDelayType::flanger);
        if ( ImGui::Button("Flanger")) {
            type = ModulatedDelayType::flanger;
            waveform = Waveform::triangle;

            if (depth < 2) delay = 2;
            else if (depth > 7) depth = 7;

            delay = 1;
            wet = 70;
            dry = 70;
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled(type == ModulatedDelayType::vibrato);
        if ( ImGui::Button("Vibrato")) {
            type = ModulatedDelayType::vibrato;
            waveform = Waveform::sine;

            if (depth < 3) delay = 3;
            else if (depth > 7) depth = 7;

            feedbackIterations = 0;
            delay = 0;
            wet = 100;
            dry = 0;
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled(type == ModulatedDelayType::chorus);
        if (ImGui::Button("Chorus")) {
            type = ModulatedDelayType::chorus;
            waveform = Waveform::triangle;

            if (delay < 1) delay = 1;

            feedbackIterations = 0;
            wet = 80;
            dry = 100;
            
        }
        ImGui::EndDisabled();
    }
    


    ImGui::SliderFloat("SampleRate [-]", &lfoSampleRate, 1, 10);

    switch (type) {
        case ModulatedDelayType::flanger: {
            ImGui::SliderFloat("Depth [ms]", &depth, 2, 7);

            ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);
            ImGui::SliderInt("Feedback Iterations [-]", &feedbackIterations, 0, 20);
        } break;
        case ModulatedDelayType::vibrato: {
            ImGui::SliderFloat("Depth [ms]", &depth, 3, 7);
        } break;
        case ModulatedDelayType::chorus: {
            ImGui::SliderFloat("Depth [ms]", &depth, 1, 30);
            ImGui::SliderFloat("Delay [ms]", &delay, 1, 30);
        } break;
        default:
            ImGui::SliderFloat("Depth [ms]", &depth, 1, 30);
            ImGui::SliderFloat("Delay [ms]", &delay, 0, 30);

            // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.
            ImGui::SliderInt("Waveform", &waveform, 0, waveformTypeCount - 1, elementName);

            ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);
            ImGui::SliderInt("Feedback Iterations [-]", &feedbackIterations, 0, 20);

            ImGui::SliderFloat("Wet [%]", &wet, 0, 100);
            ImGui::SliderFloat("Dry [%]", &dry, 0, 100);
    }
    

    //

    ImGui::End();
}