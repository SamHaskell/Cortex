#include "Cortex/Core/Application.hpp"
#include "Cortex/Utils/Logging.hpp"

#include <iostream>

namespace Cortex {
    Application::Application() {
        m_Finished = CX_FALSE;
        m_Suspended = CX_FALSE;
    }
    Application::~Application() {

    }
    b8 Application::Init() {

        /* TODO: THIS IS WHERE WE START UP ENGINE SUBSYSTEMS. 
           With few exceptions, if we start something up here, we should
           call the corresponding Shutdown() method in the application Shutdown(). 
           The shutdown calls should occur in reverse order to the Init() calls. */

        CX_INFO("Engine Initialising ...");

        if (!p_Window -> Init()) {
            return CX_FALSE;
        }
        
        return CX_TRUE;
    }
    b8 Application::Run() {
        while (!m_Finished) {
            // Persistant stuff goes here ...
            while (!m_Suspended) {
                p_Window -> Update();
            }
        }

        return CX_TRUE;
    }
    b8 Application::Shutdown() {

        if (!p_Window -> Init()) {
            return CX_FALSE;
        }
        
        return CX_TRUE;
    }
}