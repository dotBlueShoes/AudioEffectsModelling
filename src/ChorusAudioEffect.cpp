#include "ChorusAudioEffect.h"

void ChorusAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    cachedHalfDepthInSamples = Math::MilisecondsToSample(depth, 44100);
    cachedHalfDelayInSamples = Math::MilisecondsToSample(delay, 44100);

    cachedDrySoundSize = drySoundSize;
    wetSoundSize = drySoundSize + cachedHalfDepthInSamples + cachedHalfDelayInSamples;
    cachedHalfDepthInSamples /= 2;
}

void ChorusAudioEffect::applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& wetSound) {

    //const auto&& lfoSampleRate = 100.0f;
    //const auto&& lfoFrequency = 10.0f;       // We want very high frequency. 
    const auto&& dryNormalized = Math::NormalizePercent(dry);
    const auto&& wetNormalized = Math::NormalizePercent(wet);

    // 1. Reset
    lfo.Reset(lfoSampleRate);

    // 2-3. Set params
    lfo.Initialize((Waveform)waveform, lfoFrequency);

    // 4. Call processAudioSample pass input value in and receiving the output value as the return variable.

    //for (size i = 0; i < 20; ++i) {
    //    LFOResult result = lfo.RenderAudio();
    //    spdlog::info("n: {:1.5}, i: {:1.5}", result.normal, result.inverted);
    //    spdlog::info("q: {:1.5}, j: {:1.5}", result.quadPhase, result.quadPhaseInverted);
    //}

    //spdlog::info("modDepth: {}", cachedHalfDelayInSamples);

    // Create a copy of buffor with space before and after the original sound.
    int16_t* drySoundData = new int16_t[cachedDrySoundSize + (cachedHalfDepthInSamples * 2)];
    std::memset(drySoundData, 0, cachedHalfDepthInSamples * 2 /* int16 */);
    std::memcpy(drySoundData + cachedHalfDepthInSamples, wetSound.pcmData, cachedDrySoundSize * 2 /* int16 */);
    std::memset(drySoundData + cachedDrySoundSize + cachedHalfDepthInSamples, 0, cachedHalfDepthInSamples * 2 /* int16 */);

    // spowolnienie/przyœpieszenie dzwiêku to zmiana pitchu
    // ¿eby przyœpieszyæ dzwiêk omijamy co któryœ orginalny sample
    // ¿aby spowolniæ dzwiêk dok³adamy ten sam sample

    // jeœli najpierw bêdê na 2015 wezmê 2023
    // a potem bêdê na 2016 i wezmê 2018
    // to znów mamy ten sam efekt! A nie o to chodzi!

    { // Apply wet layer
        auto&& wetBeginIndex = cachedHalfDepthInSamples + cachedHalfDelayInSamples;

        // 0'ed sound for delayInSamples value.
        std::memset(wetSound.pcmData, 0, wetBeginIndex * 2 /* int16 */);

        double currentIndex = wetBeginIndex;
        size j = 0;
        for (; currentIndex < cachedDrySoundSize && j < cachedDrySoundSize; ++j) {
            //auto&& currentLFO = lfo.RenderAudio().normal * depth;
            auto&& currentLFO = (lfo.RenderAudio().normal) + 1;
            auto&& drySampleI = j + wetBeginIndex;
            auto&& resultSample = wetSound.pcmData[drySampleI];

            //if (currentLFO < 1) {
            //    //currentLFO /= depth;
            //    currentLFO = 0;
            //} else {
            //    // [1.0-2.0] to [1.0-depth]
            //    currentLFO = Math::Remap(currentLFO, 1.0f, 2.0f, 1.0f, depth);
            //}

            //spdlog::info("lfo: {}", currentLFO);

            //currentLFO = !(currentLFO < 1) * currentLFO;

            currentIndex += currentLFO;
            //spdlog::info("d: {}, w: {}", i + cachedHalfDelayInSamples, currentIndex);

            resultSample = drySoundData[(size)currentIndex] * wetNormalized;
        }

        spdlog::info("i: {}", currentIndex);
        spdlog::info("j: {}", j);

        //for (size i = 0; i < cachedDrySoundSize; ++i) {
        //    auto&& currentLFO = lfo.RenderAudio().normal;
        //    auto&& drySampleI = i + cachedHalfDepthInSamples;
        //    auto&& previousSample = wetSound.pcmData[drySampleI - 1];
        //    auto&& resultSample = wetSound.pcmData[drySampleI];
        //
        //    if (currentLFO > 0) {
        //        // It's faster so take sample from the future.
        //        size&& wetSampleI = drySampleI + (int16_t)(cachedHalfDepthInSamples * currentLFO);
        //        auto&& wetSample = drySoundData[wetSampleI];
        //        resultSample = wetSample * wetNormalized;
        //    } else {
        //        // It's slower so take sample from the past.
        //        auto&& wetSample = previousSample;
        //        resultSample = wetSample * wetNormalized;
        //    }
        //}

        // It's wrong! It's not about sample position but about sample pitch!
        //for (size i = 0; i < cachedDrySoundSize; ++i) {
        //    auto&& sample = wetSound.pcmData[i + cachedHalfDepthInSamples];
        //    size selectedSample = i + cachedHalfDepthInSamples + (int16_t)(cachedHalfDepthInSamples * lfo.RenderAudio().normal);
        //    spdlog::info("o: {}, s: {}", i + cachedHalfDepthInSamples, selectedSample);
        //    auto&& wetSample = wetSound.pcmData[selectedSample];
        //    sample = wetSample * wetNormalized;
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

            wet = 80;
            dry = 100;
        }
        ImGui::EndDisabled();
    }
    


    ImGui::SliderFloat("SampleRate [-]", &lfoSampleRate, 1, 10);

    switch (type) {
        case ModulatedDelayType::flanger: {
            ImGui::SliderFloat("Depth [ms]", &depth, 2, 7);
        } break;
        case ModulatedDelayType::vibrato: {
            ImGui::SliderFloat("Depth [ms]", &depth, 3, 7);
        } break;
        default:
            ImGui::SliderFloat("Depth [ms]", &depth, 1, 30);
            ImGui::SliderFloat("Delay [ms]", &delay, 1, 30);
    }
    

    //ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);
    //ImGui::SliderInt("Iterations [-]", &iterations, 0, 100);

    // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.
    if (type == ModulatedDelayType::none) {
        ImGui::SliderInt("Waveform", &waveform, 0, waveformTypeCount - 1, elementName);
    }

    

    if (type == ModulatedDelayType::none) {
        ImGui::SliderFloat("Wet [%]", &wet, 0, 100);
        ImGui::SliderFloat("Dry [%]", &dry, 0, 100);
    }

    ImGui::End();
}