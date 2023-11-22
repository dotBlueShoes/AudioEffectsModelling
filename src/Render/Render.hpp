#include "Framework.hpp"


namespace Render {

    static void glfw_error_callback(int error, const char* description) {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
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

}

namespace Controls {


    struct DrawCallParams {
        // Window draw
        ImVec4 backgroundColor;
        // Sounds
        size_t soundsCount;
        //ALuint* sounds;
        OpenAL::Buffered::BufferQueue* queuesBuffers;
        SoundIO::ReadWavData* wavDatas;
        ALuint* sources;
        ALint sourceState;
        float pitch = 1.0f, gain = 1.0f;
    };

    size_t soundIndex = 0;

    void DrawSampleSelection(DrawCallParams& drawCallParams) {
        const char STRING_SAMPLE_SELECTION[] = "Sample Selection";

        auto&& currentSource = drawCallParams.sources[0];

        ImGui::Begin(STRING_SAMPLE_SELECTION);

        {
            const array<char, 11> controlBase { "Sample XYZ" };
            const int32_t reserved_chars = 7;

            ImGui::Text("Nylon Strings");

            for (size_t i = 0; i < drawCallParams.soundsCount; ++i) {

                // Format Control Name
                array<char, 11> controlName = controlBase;
                auto result = std::to_chars(controlName.data() + reserved_chars, controlName.data() + controlName.size(), i + 1, 10);
                *(result.ptr) = '\0';

                ImGui::Text(controlName.data());

                ImGui::SameLine();

                ImGui::PushID(i);

                if (ImGui::Button("Select")) {

                    alGetSourcei(currentSource, AL_SOURCE_STATE, &drawCallParams.sourceState);
                    OpenAL::CheckError("select-get-source_state");

                    // Stop currently playing sound on source.
                    spdlog::info("OpenGL: PLAYING - StopSound");
                    OpenAL::Buffered::StopSound(currentSource, drawCallParams.sourceState);

                    // 1. DEQUEUE all buffers.
                    alSourcei(currentSource, AL_BUFFER, 0);

                    // 2. Queue new buffers.
                    auto&& soundQueueBuffers = drawCallParams.queuesBuffers[i];
                    for (size_t j = 0; j < soundQueueBuffers.buffers.size(); ++j) {
                        alSourceQueueBuffers(currentSource, 1, &soundQueueBuffers.buffers[j]);
                        OpenAL::CheckError("set-queue-mono");
                    }

                    // 3. Change the soundIndex to point at newly selected sound.
                    soundIndex = i;

                }

                ImGui::PopID();

            }


            alGetSourcei(currentSource, AL_SOURCE_STATE, &drawCallParams.sourceState);
            OpenAL::CheckError("select-get-source_state");


            // Play / Stop button.
            switch (drawCallParams.sourceState) {
                case AL_INITIAL:
                case AL_STOPPED:
                case AL_PAUSED: {
                    if (ImGui::Button("Play")) {
                        OpenAL::Buffered::PlaySound(currentSource, drawCallParams.wavDatas[soundIndex], OpenAL::Buffered::BUFFER_SIZE, drawCallParams.queuesBuffers[soundIndex].buffersTotal);
                    }
                } break;

                case AL_PLAYING: {
                    if (ImGui::Button("Stop")) {
                        spdlog::info("OpenGL: PLAYING - StopSound");
                        OpenAL::Buffered::StopSound(currentSource, drawCallParams.sourceState);
                    }
                } break;

                default: {
                    spdlog::error("OPENGL: NOT A VALID ENUM VALUE");
                }
            }

            ImGui::SameLine();

            {
                ALint isLooped = false;
                alGetSourcei(currentSource, AL_LOOPING, &isLooped);
                OpenAL::CheckError("select-set-looping");

                if (ImGui::Checkbox("Looped", (bool*)&isLooped)) {
                    alSourcei(currentSource, AL_LOOPING, isLooped);
                }
            }


            static ALfloat temp = 1.0;

            ImGui::Text("Dry");

            {
                alGetSourcef(currentSource, AL_GAIN, &temp);
                if (ImGui::SliderFloat("Gain##IN", &temp, 0, OpenAL::MAX_GAIN)) {
                    alSourcef(currentSource, AL_GAIN, temp);
                    OpenAL::CheckError("Gain");
                }

                alGetSourcef(currentSource, AL_PITCH, &temp);
                if (ImGui::SliderFloat("Pitch##IN", &temp, 0, 10)) {
                    alSourcef(currentSource, AL_PITCH, temp);
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

            const char* items[] = { "Remove", Effects::STRING_DISTORTION, Effects::STRING_DELAY, Effects::STRING_PHASER, Effects::STRING_CHORUS, Effects::STRING_REVERB };
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
                        switch (n) {
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
            effects_queue_temp.back()->setWindowNumber(effects_queue_temp.size() - 1);
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

    }

}