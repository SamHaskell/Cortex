#include "Cortex/Core/Events.hpp"

#include <sstream>

namespace Cortex {
    std::string KeyEvent::ToString() {
        std::stringstream ss;
        ss << "KeyEvent: " << KeyActionStrings[Action] << " on " << KeyCode;
        return ss.str();
    }

    std::string MouseButtonEvent::ToString() {
        std::stringstream ss;
        ss << "MouseButtonEvent: " << MouseButtonActionStrings[Action] << " on " << ButtonCode;
        return ss.str();
    }

    std::string MouseMoveEvent::ToString() {
        std::stringstream ss;
        ss << "MouseMoveEvent: Cursor move to (" << X << ", " << Y << ")";
        return ss.str();
    }

    std::string WindowCloseEvent::ToString() {
        std::stringstream ss;
        ss << "WindowCloseEvent: Request to close window received";
        return ss.str();
    }

    std::string WindowFramebufferSizeEvent::ToString() {
        std::stringstream ss;
        ss << "WindowSizeEvent: Window framebuffer resized to (" << Width << "," << Height << ")";
        return ss.str();
    }

    std::string WindowSizeEvent::ToString() {
        std::stringstream ss;
        ss << "WindowSizeEvent: Window resized to (" << Width << "," << Height << ")";
        return ss.str();
    }
}