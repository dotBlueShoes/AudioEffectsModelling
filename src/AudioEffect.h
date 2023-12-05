#pragma once

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <string>
#include <stdio.h>

class AudioEffect
{
public:
    int windowNumber = 0;
    //virtual void applyEffect(size_t bufforSize, int16_t* dryBuffor, int16_t* wetBuffor) = 0;  // Pure virtual function for applying the effect
    virtual void applyEffect() = 0;  // Pure virtual function for applying the effect
    virtual ~AudioEffect() {}  // Ensure a virtual destructor for proper cleanup
    virtual void DisplayEffectWindow() = 0;
    void setWindowNumber(int x) {
        windowNumber = x;
    }
};

