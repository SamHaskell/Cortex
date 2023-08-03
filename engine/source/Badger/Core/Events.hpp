#pragma once

#include "Badger/Base/Base.hpp"

#include <string>

namespace Badger
{
    enum class EventTag
    {
        KeyEvent,
        MouseButtonEvent,
        MouseMoveEvent,
        WindowCloseEvent,
        WindowFramebufferSizeEvent,
        WindowSizeEvent,
    };

    enum KeyAction
    {
        KEY_PRESS,
        KEY_RELEASE,
        KEY_REPEAT
    };

    static const char* KeyActionStrings[] = {
        "Press",
        "Release",
        "Repeat"
    };

    enum MouseButtonAction
    {
        MOUSEBUTTON_PRESS,
        MOUSEBUTTON_RELEASE
    };

    static const char* MouseButtonActionStrings[] = {
        "Press",
        "Release"
    };

    struct KeyEvent
    {
        i32 KeyCode;
        KeyAction Action;
        std::string ToString();
    };

    struct MouseButtonEvent
    {
        i32 ButtonCode;
        MouseButtonAction Action;
        std::string ToString();
    };

    struct MouseMoveEvent
    {
        f64 X;
        f64 Y;
        std::string ToString();
    };

    struct WindowCloseEvent
    {
        std::string ToString();
    };

    struct WindowFramebufferSizeEvent
    {
        i32 Width;
        i32 Height;
        std::string ToString();
    };

    struct WindowSizeEvent
    {
        i32 Width;
        i32 Height;
        std::string ToString();
    };

    struct Event
    {
        EventTag Tag;
        union
        {
            KeyEvent KeyEvent;
            MouseButtonEvent MouseButtonEvent;
            MouseMoveEvent MouseMoveEvent;
            WindowCloseEvent WindowCloseEvent;
            WindowFramebufferSizeEvent WindowFramebufferSizeEvent;
            WindowSizeEvent WindowSizeEvent;
        };
    };
}