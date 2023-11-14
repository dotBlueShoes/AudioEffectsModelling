#include "Framework.hpp"
#include "Audio/Framework.hpp"


static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void applyDistortion() {
    std::cout << "Distortion applied" << std::endl;
    //something here
}

void applyReverb() {
    std::cout << "Reverb applied" << std::endl;
    //something here
}

void applyDelay(){
    std::cout << "Delay applied" << std::endl;
    //something here
}

void applyPhaser() {
    std::cout << "Phaser applied" << std::endl;
    //something here
}

void applyChorus() {
    std::cout << "Chorus applied" << std::endl;
    //something here
}



auto InitializeWindow() {

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


auto DestroyWindow(GLFWwindow* window) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}



namespace Controls {

    bool show_demo_window = false;
    bool isActive_distortion = false;
    int distortion_Priority = 1;
    bool isActive_reverb = false;
    int reverb_Priority = 2;
    bool isActive_delay = false;
    int delay_Priority = 3;
    bool isActive_phaser = false;
    int phaser_Priority = 4;
    bool isActive_chorus = false;
    int chorus_Priority = 5;

    auto DrawCall(const ImVec4& backgroundColor) {
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);




        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Audio Effects");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Distortion Effect Window", &isActive_distortion);
            ImGui::Checkbox("Reverb Effect Window", &isActive_reverb);
            ImGui::Checkbox("Delay Effect Window", &isActive_delay);
            ImGui::Checkbox("Phaser Effect Window", &isActive_phaser);
            ImGui::Checkbox("Chorus Effect Window", &isActive_chorus);
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&backgroundColor); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            if (ImGui::Button("Apply Effects")) {
                for (int i = 1; i <= 5; ++i) {
                    switch (i) {
                        case 1:
                            if (distortion_Priority == i && isActive_distortion) applyDistortion();
                            if (reverb_Priority == i && isActive_reverb) applyReverb();
                            if (delay_Priority == i && isActive_delay) applyDelay();
                            if (phaser_Priority == i && isActive_phaser) applyPhaser();
                            if (chorus_Priority == i && isActive_chorus) applyChorus();
                            break;
                        case 2:
                            if (distortion_Priority == i && isActive_distortion) applyDistortion();
                            if (reverb_Priority == i && isActive_reverb) applyReverb();
                            if (delay_Priority == i && isActive_delay) applyDelay();
                            if (phaser_Priority == i && isActive_phaser) applyPhaser();
                            if (chorus_Priority == i && isActive_chorus) applyChorus();
                            break;
                        case 3:
                            if (distortion_Priority == i && isActive_distortion) applyDistortion();
                            if (reverb_Priority == i && isActive_reverb) applyReverb();
                            if (delay_Priority == i && isActive_delay) applyDelay();
                            if (phaser_Priority == i && isActive_phaser) applyPhaser();
                            if (chorus_Priority == i && isActive_chorus) applyChorus();
                            break;
                        case 4:
                            if (distortion_Priority == i && isActive_distortion) applyDistortion();
                            if (reverb_Priority == i && isActive_reverb) applyReverb();
                            if (delay_Priority == i && isActive_delay) applyDelay();
                            if (phaser_Priority == i && isActive_phaser) applyPhaser();
                            if (chorus_Priority == i && isActive_chorus) applyChorus();
                            break;
                        case 5:
                            if (distortion_Priority == i && isActive_distortion) applyDistortion();
                            if (reverb_Priority == i && isActive_reverb) applyReverb();
                            if (delay_Priority == i && isActive_delay) applyDelay();
                            if (phaser_Priority == i && isActive_phaser) applyPhaser();
                            if (chorus_Priority == i && isActive_chorus) applyChorus();
                            break;
                    }
                }
            }
            if (ImGui::Button("Play Audio"))
                counter++;//playing audio function
            ImGui::End();
        }

        // Show distortion effect window.
        if (isActive_distortion) {
            ImGui::Begin("Distortion", &isActive_distortion);
            ImGui::Text("Adjust distortion parameters");
            ImGui::SliderInt("Effect Priority", &distortion_Priority, 1, 5);
            ImGui::End();
        }
        // Show reverb effect window.
        if (isActive_reverb) {
            ImGui::Begin("Reverb", &isActive_reverb);
            ImGui::Text("Adjust reverb parameters");
            ImGui::SliderInt("Effect Priority", &reverb_Priority, 1, 5);
            ImGui::End();
        }
        // Show delay effect window.
        if (isActive_delay) {
            ImGui::Begin("Delay", &isActive_delay);
            ImGui::Text("Adjust delay parameters");
            ImGui::SliderInt("Effect Priority", &delay_Priority, 1, 5);
            ImGui::End();
        }
        // Show phaser effect window.
        if (isActive_phaser) {
            ImGui::Begin("Phaser", &isActive_phaser);
            ImGui::Text("Adjust phaser parameters");
            ImGui::SliderInt("Effect Priority", &phaser_Priority, 1, 5);
            ImGui::End();
        }
        // Show chorus effect window.
        if (isActive_chorus) {
            ImGui::Begin("Chorus", &isActive_chorus);
            ImGui::Text("Adjust chorus parameters");
            ImGui::SliderInt("Effect Priority", &chorus_Priority, 1, 5);
            ImGui::End();
        }
    }

}


int main(int, char**) {

    GLFWwindow* window = InitializeWindow();
    const ImVec4 backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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

        Controls::DrawCall(backgroundColor);

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
    DestroyWindow(window);

    return 0;
}