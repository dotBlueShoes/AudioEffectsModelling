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
    std::cout << "OpenAL Device: " << alcGetString(device, ALC_DEVICE_SPECIFIER) << std::endl;


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


    // Prepere space for sound buffers.
    array<ALuint, SOUNDS::SOUND_FILES.size()> monoSoundBuffers { NULL };


    // Prepere space for source buffers.
    array<ALuint, SOUNDS::SOUND_FILES.size()> monoSourceBuffers { NULL };
    

    // Read .wav file.
    for (size_t i = 0; i < SOUNDS::SOUND_FILES.size(); ++i) {
        SoundIO::ReadWavData monoData;
        SoundIO::ReadMono(SOUNDS::SOUND_FILES[i], monoData);

        // Load data into sound buffers.
        monoSoundBuffers[i] = OpenAL::CreateMonoSound(monoData);

        // Load a sound source for mono.
        monoSourceBuffers[i] = OpenAL::CreateMonoSource(monoSoundBuffers[i], false, pitch, gain);
    }


    for (auto&& sound : monoSoundBuffers) {
        alSourcef(sound, AL_MAX_GAIN, OpenAL::MAX_GAIN);
        OpenAL::CheckError("MaxGain");
    }
    


    ALint sourceState = NULL;


    Controls::DrawCallParams drawCallParams {
        backgroundColor,
        SOUNDS::SOUND_FILES.size(),
        monoSoundBuffers.data(),
        monoSourceBuffers.data(),
        sourceState,
        pitch,
        gain
    };


    //// Play mono sound.
    //OpenAL::PlaySound(monoSourceBuffers[0], sourceState);
    //
    //
    //// Change values mid
    //alSourcef(monoSoundBuffers[0], AL_MAX_GAIN, 2.0f);
    //OpenAL::CheckError("1");
    //alSourcef(monoSoundBuffers[0], AL_GAIN, 2.0f);
    //OpenAL::CheckError("2");
    //alSourcef(monoSoundBuffers[0], AL_PITCH, 1.2f);
    //OpenAL::CheckError("3");
    //
    //
    //// Play it again with changed values.
    //OpenAL::PlaySound(monoSourceBuffers[0], sourceState);


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

    // Cleanup

    for (auto&& soundSource : monoSourceBuffers) {
        OpenAL::DestroySource(soundSource);
    }

    for (auto&& soundBuffor : monoSoundBuffers) {
        OpenAL::DestorySound(soundBuffor);
    }

    OpenAL::DestoryContext(context);
    OpenAL::DestoryDevice(device);

    Render::DestroyWindowRender(window);

    return 0;
}