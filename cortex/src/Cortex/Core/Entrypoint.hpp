#pragma once

#include "Cortex/Utils/Asserts.h"

#include "Cortex/Core/Application.hpp"

extern Cortex::Application* Cortex::CreateApplication();

int main (int argc, char** argv) {
    auto app = Cortex::CreateApplication();
    if (!app->Init()) {
        CX_FATAL("Application failed to Initialise.");
    }
    if (!app->Run()) {
        CX_FATAL("Application failed to Run.");
    }
    if (!app->Shutdown()) {
        CX_FATAL("Application did not shut down successfully.");
    }
    delete app;
}