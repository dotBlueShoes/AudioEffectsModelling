#include "Framework.hpp"

#include "Audio/Framework.hpp"
#include "Audio/OpenAl.hpp"
#include "Audio/SoundIO.hpp"
#include "Audio/Display.hpp"
#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include "../include/AudioEffect.h"
#include "../include/DelayAudioEffect.h"
#include "../include/DistortionAudioEffect.h"
#include "../include/ChorusAudioEffect.h"
#include "../include/PhaserAudioEffect.h"
#include "../include/ReverbAudioEffect.h"
#include "Audio/Effects/Effects.hpp"


static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void applyDistortion() {
    spdlog::info("Distortion applied");
    //something here
}

void applyReverb() {
    spdlog::info("Reverb applied");
    //something here
}

void applyDelay(){
    spdlog::info("Delay applied");
    //something here
}

void applyPhaser() {
    spdlog::info("Phaser applied");
    //something here
}

void applyChorus() {
    spdlog::info("Chorus applied");
    //something here
}



auto InitializeWindowRender() {

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) exit(1);

    // Decide GL+GLSL versions
    #if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    #else
    // GL 4.3 + GLSL 430
    const char* glsl_version = "#version 430";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    #endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == nullptr) exit(1);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    #if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
    #elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    #endif

    if (err) {
        spdlog::error("Failed to initialize OpenGL loader!");
        exit(1);
    }

    spdlog::info("Successfully initialized OpenGL loader!");

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();

    return window;
}


auto DestroyWindowRender(GLFWwindow* window) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}



namespace Controls {


    struct DrawCallParams {
        // Window draw
        ImVec4 backgroundColor;
        // Sounds
        size_t soundsCount;
        ALuint* sounds;
        ALuint* sources;
        ALint sourceState;
        float pitch, gain;
    };

    bool isPlaying = false;
    void DrawSampleSelection(DrawCallParams& drawCallParams) {
        const char STRING_SAMPLE_SELECTION[] = "Sample Selection";
        ImGui::Begin(STRING_SAMPLE_SELECTION);
        {
            const array<char, 11> controlBase{ "Sample XYZ" };
            const int32_t reserved_chars = 7;
            ImGui::Text("Nylon Strings");
            for (size_t i = 0; i < drawCallParams.soundsCount; ++i) {
                // Format Control Name
                array<char, 11> controlName = controlBase;
                auto result = std::to_chars(controlName.data() + reserved_chars, controlName.data() + controlName.size(), i + 1, 10);
                *(result.ptr) = '\0';
                if (ImGui::Button(controlName.data())) {
                    OpenAL::PlaySound(drawCallParams.sources[i], drawCallParams.sourceState);
                }
            }
            ImGui::Text("Dry Settings");
            if (ImGui::SliderFloat("Gain", &drawCallParams.gain, 0, OpenAL::MAX_GAIN)) {
                for (size_t i = 0; i < drawCallParams.soundsCount; ++i) {
                    alSourcef(drawCallParams.sounds[i], AL_GAIN, drawCallParams.gain);
                    OpenAL::CheckError("Gain");
                }
            }
            if (ImGui::SliderFloat("Pitch", &drawCallParams.pitch, 0, 10)) {
                for (size_t i = 0; i < drawCallParams.soundsCount; ++i) {
                    alSourcef(drawCallParams.sounds[i], AL_PITCH, drawCallParams.pitch);
                    OpenAL::CheckError("Pitch");
                }
            }
        }
        ImGui::End();
    }



    vector<int> effects_queue;
    std::vector<std::unique_ptr<AudioEffect>> effects_queue_temp;

    auto DrawEffectQueue() {

        const char STRING_EFFECT_QUEUE[] = "Effect Queue";

        const array<char, 11> controlBase { "Effect XYZ" };
        const int32_t reserved_chars = 7;

        int isGoingToBeRemoved = -1;

        ImGui::Begin(STRING_EFFECT_QUEUE);

        if (ImGui::Button("Clear Effects")) {
            effects_queue.clear();
        }

        for (size_t i = 0; i < effects_queue.size(); ++i) {

            const char* items[] = { "Remove", Effects::STRING_DISTORTION, Effects::STRING_DELAY, Effects::STRING_PHASER, Effects::STRING_CHORUS, Effects::STRING_REVERB};
            //static int item_current_idx = 0; // Here we store our selection data as an index.

            const ImVec2 listboxSize(200, 3.5 * ImGui::GetTextLineHeightWithSpacing());


            // Format Control Name
            array<char, 11> controlName = controlBase;
            auto result = std::to_chars(controlName.data() + reserved_chars, controlName.data() + controlName.size(), i + 1, 10);
            *(result.ptr) = '\0';


            if (ImGui::BeginListBox(controlName.data(), listboxSize)) {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                    const bool is_selected = (effects_queue[i] == n);

                    if (ImGui::Selectable(items[n], is_selected)) {
                        switch (n)
                        {
                        case 0:
                            isGoingToBeRemoved = i;
                            break;
                        case 1:
                            effects_queue_temp[i] = std::make_unique<DistortionAudioEffect>();
                            break;
                        case 2:
                            effects_queue_temp[i] = std::make_unique<DelayAudioEffect>();
                            break;
                        case 3:
                            effects_queue_temp[i] = std::make_unique<PhaserAudioEffect>();
                            break;
                        case 4:
                            effects_queue_temp[i] = std::make_unique<ChorusAudioEffect>();
                            break;
                        case 5:
                            effects_queue_temp[i] = std::make_unique<ReverbAudioEffect>();
                            break;
                        }
                        effects_queue_temp[i]->setWindowNumber(i);
                        // replace with switch 
                        //  as not only queue is being formed here.
                        // but also additional windows for each effect should be shown/changed with selection.

                        if (n != 0) {
                            effects_queue[i] = n;
                        }

                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    //if (is_selected) ImGui::SetItemDefaultFocus();
                }

                ImGui::EndListBox();
            }
        }

        if (isGoingToBeRemoved >= 0) {
            effects_queue.erase(effects_queue.begin() + isGoingToBeRemoved);
            effects_queue_temp.erase(effects_queue_temp.begin() + isGoingToBeRemoved);
            isGoingToBeRemoved = -1;
        }

        if (ImGui::Button("Add Effect")) {
            effects_queue.push_back(1);
            effects_queue_temp.push_back(std::make_unique<DistortionAudioEffect>());
            effects_queue_temp.back()->setWindowNumber(effects_queue_temp.size()-1);
        }

        ImGui::End();
    }

    auto DrawEffectBasicDelay() {
        const char STRING_EFFECT_BASIC_DELAY[] = "Effect Basic Delay";
        ImGui::Begin(STRING_EFFECT_BASIC_DELAY);

        static float feedback = 0;
        static int delay = 0;

        ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);
        ImGui::SliderInt("Delay [ms]", &delay, 0, 1000);

        ImGui::End();
    }

    auto DrawEffectReverb() {
        const char STRING_EFFECT_BASIC_DELAY[] = "Effect Reverb";
        ImGui::Begin(STRING_EFFECT_BASIC_DELAY);

        static int delay1 = 0;
        static int delay2 = 0;
        static int delay3 = 0;
        static int delay4 = 0;

        ImGui::SliderInt("Delay 1 [-]", &delay1, 0, 4000);
        ImGui::SliderInt("Delay 2 [-]", &delay2, 0, 4000);
        ImGui::SliderInt("Delay 3 [-]", &delay3, 0, 4000);
        ImGui::SliderInt("Delay 4 [-]", &delay4, 0, 4000);

        ImGui::End();
    }

    auto DrawEffectChorus() {
        const char STRING_EFFECT_BASIC_DELAY[] = "Effect Chorus";
        ImGui::Begin(STRING_EFFECT_BASIC_DELAY);

        static int delay = 0;
        static int depth = 0;
        static int sampleRate = 0;
        static float feedback = 0;
        static int wet = 0;
        static int dry = 0;

        ImGui::SliderInt("Delay [ms]", &delay, 0, 1000);
        ImGui::SliderInt("Depth [ms]", &depth, 0, 1000);
        ImGui::SliderInt("SampleRate [Hz]", &sampleRate, 0, 1000);
        ImGui::SliderFloat("Feedback [%]", &feedback, 0, 1000);
        ImGui::SliderInt("Wet [dB]", &wet, -32, 32);
        ImGui::SliderInt("Dry [dB]", &dry, -32, 32);

        ImGui::End();
    }

    auto DrawEffectDistortion() {
        const char STRING_EFFECT_BASIC_DELAY[] = "Effect Distortion";
        ImGui::Begin(STRING_EFFECT_BASIC_DELAY);

        static int type = 0;
        static float gain = 0;
        static int dry = 0;
        static int wet = 0;

        ImGui::SliderInt("Type [0, 1]", &type, 0, 1);
        ImGui::SliderFloat("Gain [-]", &gain, 0, 10);
        ImGui::SliderInt("Dry [dB]", &dry, -32, 32);
        ImGui::SliderInt("Wet [dB]", &wet, -32, 32);

        ImGui::End();
    }

    auto DrawEffectPhaser() {
        const char STRING_EFFECT_BASIC_DELAY[] = "Effect Phaser";
        ImGui::Begin(STRING_EFFECT_BASIC_DELAY);

        static float feedback = 0;
        static int rate = 0;
        static float depth = 0;
        static float offset = 0;
        static float intensity = 0;
        static int stages = 0;

        ImGui::SliderFloat("Feedback [%]", &feedback, 0, 100);
        ImGui::SliderInt("Rate [ms]", &rate, 0, 1000);
        ImGui::SliderFloat("Depth [%]", &depth, 0, 100);
        ImGui::SliderFloat("Offset [-]", &offset, -1.0, 1.0);
        ImGui::SliderFloat("Intensity [%]", &intensity, 0, 100);
        ImGui::SliderInt("Stages [ms]", &stages, 1, 3);

        ImGui::End();
    }

    auto DrawCall(DrawCallParams& drawCallParams) {

        DrawSampleSelection(drawCallParams);
        DrawEffectQueue();
        for (auto& effect : effects_queue_temp) {
            effect->DisplayEffectWindow();
        }
       // DrawEffectBasicDelay();
       // DrawEffectReverb();
       // DrawEffectChorus();
       // DrawEffectDistortion();
       // DrawEffectPhaser();

    }

}


int main(int, char**) {

    GLFWwindow* window = InitializeWindowRender();
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

    DestroyWindowRender(window);

    return 0;
}