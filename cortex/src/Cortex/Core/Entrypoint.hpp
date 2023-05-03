#pragma once

#include "Cortex/Utils/Asserts.h"
#include "Cortex/Utils/Logging.hpp"

#include "Cortex/Core/Application.hpp"

extern Cortex::Application *Cortex::CreateApplication();

int main(int argc, char **argv)
{
    auto app = Cortex::CreateApplication();
    if (!app->Run())
    {
        CX_FATAL("Application failed to Run.");
    }
    delete app;
}