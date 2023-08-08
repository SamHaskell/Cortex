#include "Cortex/Core/Entrypoint.hpp"

int main(int argc, char **argv)
{
    auto app = Cortex::CreateApp();
    if (!app->Run())
    {
        // Do whatever you want
        return EXIT_FAILURE;
    }
    delete app;
    return EXIT_SUCCESS;
}