#pragma once

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <string>
#include <stdio.h>

class AudioEffect
{
protected:
    int windowNumber = 0;
public:
    virtual void applyEffect() = 0;  // Pure virtual function for applying the effect
    virtual ~AudioEffect() {}  // Ensure a virtual destructor for proper cleanup
    virtual void DisplayEffectWindow();
    void setWindowNumber(int x) {
        windowNumber = x;
    }
};

