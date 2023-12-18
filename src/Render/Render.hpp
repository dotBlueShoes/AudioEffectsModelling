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
        size soundsCount;
        SoundIO::ReadWavData* soundsData;
        ALuint* sounds;
        ALuint* sources;
        

        ALint sourceState;

        //size_t sound_index = 0;
        float pitch = 1.0f, gain = 1.0f;
    };

    size selectedOriginalSoundIndex = 0;

    void DrawSampleSelection(DrawCallParams& drawCallParams) {
        const char STRING_SAMPLE_SELECTION[] = "Sample Selection";

        auto&& currentSource = drawCallParams.sources[0];

        ImGui::Begin(STRING_SAMPLE_SELECTION);

        {
            const array<char, 11> controlBase { "Sample XYZ" };
            const int32_t reserved_chars = 7;

            ImGui::Text("Nylon Strings");

            for (size i = 0; i < drawCallParams.soundsCount; ++i) {

                // Format Control Name
                array<char, 11> controlName = controlBase;
                auto result = std::to_chars(controlName.data() + reserved_chars, controlName.data() + controlName.size(), i + 1, 10);
                *(result.ptr) = '\0';

                ImGui::PushID(i);

                if (ImGui::Button("Select")) {
                    spdlog::info("OpenGL: PLAYING - SelectSound");
                    selectedOriginalSoundIndex = i;
                }

                ImGui::PopID();

                ImGui::SameLine();

                ImGui::Text(controlName.data());

            }

            
            alGetSourcei(currentSource, AL_SOURCE_STATE, &drawCallParams.sourceState);
            switch (drawCallParams.sourceState) {
                case AL_INITIAL: {
                    if (ImGui::Button("Play")) {
                        spdlog::info("OpenGL: INITIAL - PlaySound");
                        OpenAL::PlaySound(currentSource, drawCallParams.soundsData[selectedOriginalSoundIndex], drawCallParams.sourceState);
                    }
                } break;

                case AL_STOPPED: {
                    if (ImGui::Button("Play")) {
                        spdlog::info("OpenGL: PLAYING - PlaySound");
                        OpenAL::StopSound(currentSource);
                        OpenAL::PlaySound(currentSource, drawCallParams.soundsData[selectedOriginalSoundIndex], drawCallParams.sourceState);
                    }
                } break;

                case AL_PLAYING: {
                    if (ImGui::Button("Stop")) {
                        spdlog::info("OpenGL: STOPPED - StopSound");
                        OpenAL::StopSound(currentSource);
                    }
                } break;

                case AL_PAUSED: {
                    if (ImGui::Button("Resume")) {
                        spdlog::info("OpenGL: PAUSED - PlaySound");
                        OpenAL::PlaySound(currentSource, drawCallParams.soundsData[selectedOriginalSoundIndex], drawCallParams.sourceState);
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

                OpenAL::CheckError("6");
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

    auto DrawEffectQueue() {

        const char STRING_EFFECT_QUEUE[] = "Effect Queue";

        const array<char, 11> controlBase { "Effect XYZ" };
        const int32_t reserved_chars = 7;

        int isGoingToBeRemoved = -1;

        ImGui::Begin(STRING_EFFECT_QUEUE);

       if (ImGui::Button("Clear Effects")) {
            effects_queue.clear();
            OpenAL::Effects::effectsQueue.clear();
        }

        for (size_t i = 0; i < OpenAL::Effects::effectsQueue.size(); ++i) {

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
                                OpenAL::Effects::effectsQueue[i] = std::make_unique<DistortionAudioEffect>();
                                break;
                            case 2:
                                OpenAL::Effects::effectsQueue[i] = std::make_unique<DelayAudioEffect>();
                                break;
                            case 3:
                                OpenAL::Effects::effectsQueue[i] = std::make_unique<PhaserAudioEffect>();
                                break;
                            case 4:
                                OpenAL::Effects::effectsQueue[i] = std::make_unique<ChorusAudioEffect>();
                                break;
                            case 5:
                                OpenAL::Effects::effectsQueue[i] = std::make_unique<ReverbAudioEffect>();
                                break;
                        }
                        OpenAL::Effects::effectsQueue[i]->setWindowNumber(i);
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
            OpenAL::Effects::effectsQueue.erase(OpenAL::Effects::effectsQueue.begin() + isGoingToBeRemoved);
            isGoingToBeRemoved = -1;
        }

        if (ImGui::Button("Add Effect")) {
            effects_queue.push_back(1);
            OpenAL::Effects::effectsQueue.push_back(std::make_unique<DistortionAudioEffect>());
            OpenAL::Effects::effectsQueue.back()->setWindowNumber(OpenAL::Effects::effectsQueue.size() - 1);
        }

        ImGui::End();
    }


    auto DrawCall(DrawCallParams& drawCallParams) {

        DrawSampleSelection(drawCallParams);
        DrawEffectQueue();
        for (auto& effect : OpenAL::Effects::effectsQueue) {
            effect->DisplayEffectWindow();
        }

    }

}