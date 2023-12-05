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


// Things that might not work:
// 1. Delay between change from AL_INITIAL to AL_PLAYING - Is it ensured ImGui::Button will be clicked once ?
// 2. Buffor - 1024 is too small



int main(int argumentsCount, char** arguments) {

    GLFWwindow* window = Render::InitializeWindowRender();
    const ImVec4 backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    ALCdevice* device = OpenAL::CreateAudioDevice();
    spdlog::info("OpenAL Device: {}", alcGetString(device, ALC_DEVICE_SPECIFIER));

    ALCcontext* context = OpenAL::CreateAudioContext(device);


    // Apparently, the alGetError() will return this error before a context is created.
    //  Starting from alcMakeContextCurrent() I can use this function to check for errors.
    // https://stackoverflow.com/questions/71066647/openal-soft-40964-in-alcopendevice-al-invalid-operation
    OpenAL::CheckError("context");


    OpenAL::CreateListener3D();
    

    const float pitch = 1.0f, gain = 1.0f;


    // Prepere variables for audio source buffers.
    array<ALuint, 2> monoSourceBuffers { NULL };
    auto&& mainSourceBuffer = monoSourceBuffers[0];
    auto&& changeSourceBuffer = monoSourceBuffers[1];


    // Prepare variables for sounds data and their buffors.
    array<SoundIO::ReadWavData, SOUNDS::SOUND_FILES.size()> monoDatas { NULL };
    array<OpenAL::Buffered::BufferQueue, SOUNDS::SOUND_FILES.size()> soundsBuffors { NULL };


    //  Load wav data. And prep intitial buffors data.
    for (size_t i = 0; i < SOUNDS::SOUND_FILES.size(); ++i) {
        SoundIO::ReadMono(SOUNDS::SOUND_FILES[i], monoDatas[i]);

        spdlog::info("Sound PCM size: {}", monoDatas[i].pcm.size());

        // Set Total Buffers Count.
        soundsBuffors[i].buffersTotal = monoDatas[i].pcm.size() / OpenAL::Buffered::BUFFER_SIZE;
        soundsBuffors[i].buffersTotal += (monoDatas[i].pcm.size() % OpenAL::Buffered::BUFFER_SIZE) > 0;

        spdlog::info("Sound buffersTotal: {}", soundsBuffors[i].buffersTotal);

        OpenAL::Buffered::CreateMonoSoundBuffers(soundsBuffors[i]);
    }


    const bool isLoopedOnInitialBuffors = false;
    const size_t initialSoundIndex = 0;
    auto&& initialSound = soundsBuffors[initialSoundIndex];


    mainSourceBuffer = OpenAL::Buffered::CreateMonoSource(isLoopedOnInitialBuffors, pitch, gain);
    //changeSourceBuffer = OpenAL::Buffered::CreateMonoSource(initialSound, false, pitch, gain); // Error. buffers were reserved for other source and we couldnt change them.

    ALint sourceState = NULL;


    Controls::DrawCallParams drawCallParams {
        backgroundColor,
        SOUNDS::SOUND_FILES.size(),
        //monoSoundBuffers.data(),
        soundsBuffors.data(),
        monoDatas.data(),
        monoSourceBuffers.data(),
        sourceState,
        pitch,
        gain
    };


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

    //for (auto&& soundSource : monoSourceBuffers) {
    //    OpenAL::DestroySource(soundSource);
    //}

    OpenAL::DestroySource(monoSourceBuffers[0]);

    
    for (auto&& soundBuffor : soundsBuffors) {
        OpenAL::Buffered::DestorySound(soundBuffor);
    }

    OpenAL::DestoryContext(context);
    OpenAL::DestoryDevice(device);

    Render::DestroyWindowRender(window);

    return 0;
}