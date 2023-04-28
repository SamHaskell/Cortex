#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Core/Application.hpp"

extern Cortex::Application* Cortex::CreateApplication();

int main (int argc, char** argv) {
    auto app = Cortex::CreateApplication();
    if (!app->Init()) {
        // TODO: ERROR HANDLING
    }
    if (!app->Run()) {
        // TODO: ERROR HANDLING
    }
    if (!app->Shutdown()) {
        // TODO: ERROR HANDLING
    }
    delete app;
}