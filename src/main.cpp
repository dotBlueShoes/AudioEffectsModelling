// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <stdio.h>
#include <iostream>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually. 
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void applyDistortion()
{
    std::cout << "Distortion applied" << std::endl;
    //something here
}
void applyReverb()
{
    std::cout << "Reverb applied" << std::endl;
    //something here
}
void applyDelay()
{
    std::cout << "Delay applied" << std::endl;
    //something here
}
void applyPhaser()
{
    std::cout << "Phaser applied" << std::endl;
    //something here
}
void applyChorus()
{
    std::cout << "Chorus applied" << std::endl;
    //something here
}




int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

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
    if (window == NULL)
        return 1;
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
    if (err)
    {
        spdlog::error("Failed to initialize OpenGL loader!");
        return 1;
    }
    spdlog::info("Successfully initialized OpenGL loader!");

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    bool show_demo_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
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
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            if (ImGui::Button("Apply Effects"))
            {
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
        if (isActive_distortion)
        {
            ImGui::Begin("Distortion", &isActive_distortion);  
            ImGui::Text("Adjust distortion parameters");
            ImGui::SliderInt("Effect Priority", &distortion_Priority, 1, 5);
            ImGui::End();
        }        
        // Show reverb effect window.
        if (isActive_reverb)
        {
            ImGui::Begin("Reverb", &isActive_reverb);  
            ImGui::Text("Adjust reverb parameters"); 
            ImGui::SliderInt("Effect Priority", &reverb_Priority, 1, 5);
            ImGui::End();
        }
        // Show delay effect window.
        if (isActive_delay)
        {
            ImGui::Begin("Delay", &isActive_delay);   
            ImGui::Text("Adjust delay parameters");
            ImGui::SliderInt("Effect Priority", &delay_Priority, 1, 5);
            ImGui::End();
        }
        // Show phaser effect window.
        if (isActive_phaser)
        {
            ImGui::Begin("Phaser", &isActive_phaser);  
            ImGui::Text("Adjust phaser parameters");
            ImGui::SliderInt("Effect Priority", &phaser_Priority, 1, 5);
            ImGui::End();
        }
        // Show chorus effect window.
        if (isActive_chorus)
        {
            ImGui::Begin("Chorus", &isActive_chorus);  
            ImGui::Text("Adjust chorus parameters");
            ImGui::SliderInt("Effect Priority", &chorus_Priority, 1, 5);
            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}