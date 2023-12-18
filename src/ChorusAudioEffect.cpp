#include "ChorusAudioEffect.h"

void ChorusAudioEffect::getWetSoundSize(const size& drySoundSize, size& wetSoundSize) {
    cachedHalfDepthInSamples = Math::MilisecondsToSample(modDepth, 44100);

    cachedDrySoundSize = drySoundSize;
    wetSoundSize = drySoundSize + cachedHalfDepthInSamples;
    cachedHalfDepthInSamples /= 2;
}

void ChorusAudioEffect::applyEffect(const size& originalSoundSize, SoundIO::ReadWavData& wetSound) {

    const auto&& lfoSampleRate = 10.0f;
    const auto&& lfoFrequency = 1.0f;       // We want very high frequency. 
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

    spdlog::info("modDepth: {}", cachedHalfDepthInSamples);

    // Create a copy of buffor with space before and after the original sound.
    int16_t* drySoundData = new int16_t[cachedDrySoundSize + (cachedHalfDepthInSamples * 2)];
    std::memset(drySoundData, 0, cachedHalfDepthInSamples * 2 /* int16 */);
    std::memcpy(drySoundData + cachedHalfDepthInSamples, wetSound.pcmData, cachedDrySoundSize * 2 /* int16 */);
    std::memset(drySoundData + cachedDrySoundSize + cachedHalfDepthInSamples, 0, cachedHalfDepthInSamples * 2 /* int16 */);

    // spowolnienie/przyœpieszenie dzwiêku to zmiana pitchu
    // ¿eby przyœpieszyæ dzwiêk omijamy co któryœ orginalny sample
    // ¿aby spowolniæ dzwiêk dok³adamy ten sam sample

    { // Apply wet layer

        // 0'ed sound for delayInSamples value.
        std::memset(wetSound.pcmData, 0, cachedHalfDepthInSamples * 2 /* int16 */);

        for (size i = 0; i < cachedDrySoundSize; ++i) {
            auto&& currentLFO = lfo.RenderAudio().normal;
            auto&& drySampleI = i + cachedHalfDepthInSamples;
            auto&& previousSample = wetSound.pcmData[drySampleI - 1];
            auto&& resultSample = wetSound.pcmData[drySampleI];

            if (currentLFO > 0) {
                // It's faster so take sample from the future.
                size&& wetSampleI = i + cachedHalfDepthInSamples + (int16_t)(cachedHalfDepthInSamples * currentLFO);
                auto&& wetSample = drySoundData[wetSampleI];
                resultSample = wetSample * wetNormalized;

                //spdlog::info("o: {}, s: {}", drySampleI, wetSampleI);
            } else {
                // It's slower so take sample from the past.
                auto&& wetSample = previousSample;
                resultSample = wetSample * wetNormalized;
            }
            

            
            //int16_t wetSample;

            //if (wetSampleI > drySampleI) { 
            //    spdlog::info("{}, {}", wetSampleI, drySampleI);
            //    wetSample = wetSound.pcmData[wetSampleI];
            //} else {
            //    wetSample = 0;
            //}

            

            //resultSample = wetSample * wetNormalized;
        }

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

    ImGui::SliderFloat("Delay [ms]", &minDelay, 0, 30);
    ImGui::SliderFloat("Depth [ms]", &modDepth, 1, 30);
    //ImGui::SliderInt("SampleRate [Hz]", &sampleRate, 0, 1000);

    // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.
    ImGui::SliderInt("Waveform", &waveform, 0, waveformTypeCount - 1, elementName); 

    ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);

    ImGui::SliderFloat("Wet [%]", &wet, 0, 100);
    ImGui::SliderFloat("Dry [%]", &dry, 0, 100);

    ImGui::End();
}