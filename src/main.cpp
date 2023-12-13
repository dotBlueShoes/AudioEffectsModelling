#include "Framework.hpp"

#include "Audio/Framework.hpp"
#include "Audio/OpenAl.hpp"
#include "Audio/SoundIO.hpp"
#include "Audio/Display.hpp"
#include "Audio/Effects/Effects.hpp"

#include "DelayAudioEffect.h"
#include "DistortionAudioEffect.h"
#include "ChorusAudioEffect.h"
#include "PhaserAudioEffect.h"
#include "ReverbAudioEffect.h"

#include "Render/Render.hpp"


int main(int argumentsCount, char** arguments) {

    GLFWwindow* window = Render::InitializeWindowRender();
    const ImVec4 backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    ALCdevice* device = OpenAL::CreateAudioDevice();
    //std::cout << "OpenAL Device: " << alcGetString(device, ALC_DEVICE_SPECIFIER) << std::endl;
    spdlog::info("OpenAL Device: {}", alcGetString(device, ALC_DEVICE_SPECIFIER));


    ALCcontext* context = OpenAL::CreateAudioContext(device);


    // Apparently, the alGetError() will return this error before a context is created.
    //  Starting from alcMakeContextCurrent() I can use this function to check for errors.
    // https://stackoverflow.com/questions/71066647/openal-soft-40964-in-alcopendevice-al-invalid-operation
    OpenAL::CheckError("context");


    OpenAL::CreateListener3D();
    

    const float pitch = 1.0f, gain = 1.0f;


    //{ // Display Sound Buffor data.
    //    std::cout << "SR: " << monoData.sampleRate << std::endl;
    //    std::cout << "CH: " << monoData.channels << std::endl;
    //    std::cout << "BS: " << monoData.pcm.size() << std::endl;
    //    std::cout << "FC: " << monoData.totalPCMFrameCount << std::endl;
    //    //spdlog::info("SR: %ui", monoData.sampleRate);
    //    //spdlog::info("CH: %ui", monoData.channels);
    //    //spdlog::info("BS: %s", monoData.pcm.size());
    //    //spdlog::info("FC: %ulli", monoData.totalPCMFrameCount);
    //    //Display::PrintFromTo(monoData.pcm.data(), 20);
    //}


    // Prepere space for sounds data.
    array<SoundIO::ReadWavData, SOUNDS::SOUND_FILES.size()> soundsData { NULL };


    // Prepere space for sound buffers.
    array<ALuint, SOUNDS::SOUND_FILES.size()> monoSounds { NULL };


    // Prepere space for source buffers.
    array<ALuint, 2> monoSources { NULL };
    auto&& mainSourceBuffer = monoSources[0];
    auto&& changeSourceBuffer = monoSources[1];
    

    // Read .wav file.
    for (size i = 0; i < SOUNDS::SOUND_FILES.size(); ++i) {
        SoundIO::ReadMono(SOUNDS::SOUND_FILES[i], soundsData[i]);


        // Load data into sound buffers.
        monoSounds[i] = OpenAL::CreateMonoSound(soundsData[i]);
    }

    const size initialSoundIndex = 0;
    auto&& initialSound = monoSounds[initialSoundIndex];


    // Load a sound source for mono.
    mainSourceBuffer = OpenAL::CreateMonoSource(initialSound, false, pitch, gain);
    changeSourceBuffer = OpenAL::CreateMonoSource(initialSound, false, pitch, gain);

    ALint sourceState = NULL;


    Controls::DrawCallParams drawCallParams {
        backgroundColor,
        SOUNDS::SOUND_FILES.size(),
        soundsData.data(),
        monoSounds.data(),
        monoSources.data(),
        sourceState,
        pitch,
        gain
    };


    // TODO
    // 1. hit play
    // 2. time from each effect is being counted to create final time; (effectfunction)
    // 3. we create space for pcmData
    // 4. Initialize it with the original sound
    // 5. We send that to applayEffect for each effect (effectfunction)
    // 6. We set the source to play that sound
    // 7. We play that sound
    // 8. we destory space for pcm data

    

    //{ // Delay Effect
    //
    //    // Get delay time in samples from effect.
    //    const uint16_t delayInSamples = 18210;
    //    const float feedbackNormalized = 0.1;
    //
    //    auto&& selectedSound = soundsData[0];
    //    auto&& drySoundSize = selectedSound.pcmSize;
    //    auto&& drySoundData = selectedSound.pcmData;
    //
    //    // Count required space for whole sound.
    //
    //    // in 
    //    size wetSoundSize = drySoundSize + delayInSamples;
    //
    //    // Reserve space and Fill with initial PCM data.
    //    int16_t* pcmDelayed = new int16_t[wetSoundSize];
    //    {
    //        size i = 0;
    //        for (; i < drySoundSize; ++i) {
    //            pcmDelayed[i] = drySoundData[i];
    //        }
    //    
    //        for (; i < wetSoundSize; ++i) {
    //            pcmDelayed[i] = 0;
    //        }
    //    }
    //    
    //
    //    // Create a copy of the original sound. with new pcm data.
    //    OpenAL::Effects::soundDataFinal = SoundIO::ReadWavData { selectedSound.channels, selectedSound.sampleRate, selectedSound.totalPCMFrameCount, wetSoundSize, pcmDelayed };
    //    
    //    {   // Calculate new sound.
    //        
    //        // Calc. feedback
    //        for (size i = 0; i < drySoundSize; ++i) {
    //            OpenAL::Effects::soundDataFinal.pcmData[i] *= feedbackNormalized;
    //        }
    //
    //        // Add delayed sound
    //        for (size i = 0; i < drySoundSize; ++i) {
    //            OpenAL::Effects::soundDataFinal.pcmData[delayInSamples + i] += drySoundData[i];
    //        }
    //    }
    //    
    //
    //
    //    // Create buffor and load data into it for newly created sound.
    //    OpenAL::Effects::soundFinal = OpenAL::CreateMonoSound(OpenAL::Effects::soundDataFinal);
    //
    //
    //    alSourcei(mainSourceBuffer, AL_BUFFER, OpenAL::Effects::soundFinal);
    //    OpenAL::CheckError("source-sound-assignment");
    //
    //
    //    OpenAL::PlaySound(mainSourceBuffer, sourceState);
    //
    //    // Free sound data memory.
    //    SoundIO::DestorySoundData(OpenAL::Effects::soundDataFinal);
    //}


    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        Controls::DrawCall(drawCallParams);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    
    { // Cleanup

        for (auto&& soundSource : monoSources) {
            OpenAL::DestroySource(soundSource);
        }

        for (auto&& soundBuffor : monoSounds) {
            OpenAL::DestorySound(soundBuffor);
        }

        OpenAL::DestorySound(OpenAL::Effects::soundFinal);

        for (auto&& sound : soundsData) {
            SoundIO::DestorySoundData(sound);
        }

        OpenAL::DestoryContext(context);
        OpenAL::DestoryDevice(device);

        Render::DestroyWindowRender(window);

    }
    

    return 0;
}