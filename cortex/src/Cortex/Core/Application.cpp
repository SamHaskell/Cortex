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
        CX_FATAL("This is a test message.");
        CX_ERROR("This is a test message.");
        CX_WARN("This is a test message.");
        CX_INFO("This is a test message.");
        CX_DEBUG("This is a test message.");
        CX_TRACE("This is a test message.");

        /* TODO: THIS IS WHERE WE START UP ENGINE SUBSYSTEMS. 
           With few exceptions, if we start something up here, we should
           call the corresponding Shutdown() method in the application Shutdown(). 
           The shutdown calls should occur in reverse order to the Init() calls. */

        if (!p_Window -> Shutdown()) {
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
        return CX_TRUE;
    }
}