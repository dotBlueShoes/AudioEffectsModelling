#include "ChorusAudioEffect.h"

void ChorusAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    cachedHalfDelayInSamples = Math::MilisecondsToSample(delay, 44100);

    cachedDrySoundSize = drySoundSize;
    wetSoundSize = drySoundSize + cachedHalfDelayInSamples;
    cachedHalfDelayInSamples /= 2;
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
    int16_t* drySoundData = new int16_t[cachedDrySoundSize + (cachedHalfDelayInSamples * 2)];
    std::memset(drySoundData, 0, cachedHalfDelayInSamples * 2 /* int16 */);
    std::memcpy(drySoundData + cachedHalfDelayInSamples, wetSound.pcmData, cachedDrySoundSize * 2 /* int16 */);
    std::memset(drySoundData + cachedDrySoundSize + cachedHalfDelayInSamples, 0, cachedHalfDelayInSamples * 2 /* int16 */);

    // spowolnienie/przy�pieszenie dzwi�ku to zmiana pitchu
    // �eby przy�pieszy� dzwi�k omijamy co kt�ry� orginalny sample
    // �aby spowolni� dzwi�k dok�adamy ten sam sample

    // je�li najpierw b�d� na 2015 wezm� 2023
    // a potem b�d� na 2016 i wezm� 2018
    // to zn�w mamy ten sam efekt! A nie o to chodzi!

    { // Apply wet layer

        // 0'ed sound for delayInSamples value.
        std::memset(wetSound.pcmData, 0, cachedHalfDelayInSamples * 2 /* int16 */);

        double currentIndex = cachedHalfDelayInSamples;
        size j = 0;
        for (; currentIndex < cachedDrySoundSize && j < cachedDrySoundSize; ++j) {
            //auto&& currentLFO = lfo.RenderAudio().normal * depth;
            auto&& currentLFO = (lfo.RenderAudio().normal) + 1;
            auto&& drySampleI = j + cachedHalfDelayInSamples;
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
        sample = sample + ((float)(drySoundData[i + cachedHalfDelayInSamples]) * dryNormalized);
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

    ImGui::SliderFloat("SampleRate [%]", &lfoSampleRate, 1, 10);
    ImGui::SliderFloat("Depth [ms]", &delay, 1, 30);
    //ImGui::SliderFloat("Depth [ms]", &depth, 1, 4);

    // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.
    //ImGui::SliderInt("Waveform", &waveform, 0, waveformTypeCount - 1, elementName); 

    //ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);

    
    //ImGui::SliderFloat("Frequency [%]", &lfoFrequency, 1, 1000);

    //ImGui::SliderFloat("Wet [%]", &wet, 0, 100);
    //ImGui::SliderFloat("Dry [%]", &dry, 0, 100);

    ImGui::End();
}