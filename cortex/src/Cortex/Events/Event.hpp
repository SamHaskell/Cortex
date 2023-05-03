#pragma once

#include "Cortex/Utils/Defines.h"

#include <string>
#include <functional>

namespace Cortex
{

    /*
        Event types are a class enum, add as desired.
        This is the event system to be used for the engine.
        Games will be able to register for these events but not affect the system.
    */

    enum class EventType
    {
        None = 0,
        WindowCreate = 1,
        WindowClose = 2,
        WindowMove = 3,
        WindowResize = 4,
        WindowFramebufferResize = 5,
        WindowFocus = 6,
        WindowDefocus = 7,
        KeyPress = 8,
        KeyRelease = 9,
        MouseButtonPress = 10,
        MouseButtonRelease = 11,
        MouseMove = 12,
        MouseScroll = 13,
    };

    typedef enum EventCategory
    {
        EVENT_SYSTEM_BIT = (1 << 0),
        EVENT_WINDOW_BIT = (1 << 1),
        EVENT_INPUT_BIT = (1 << 2),
        EVENT_KEYBOARD_BIT = (1 << 3),
        EVENT_MOUSE_BIT = (1 << 4),
        EVENT_MOUSE_BUTTON_BIT = (1 << 5),
    } EventCategory;

    class Event
    {
        friend class EventDispatcher;

    public:
        virtual EventType GetEventType() const = 0;
        virtual const char *GetName() const = 0;
        virtual i32 GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        inline b8 IsCategory(EventCategory category)
        {
            return GetCategoryFlags() & category;
        }

    protected:
        b8 m_Resolved = false;
    };

    class EventDispatcher
    {
        template <typename T>
        using EventFn = std::function<b8(T &)>;

    public:
        EventDispatcher(Event &event)
            : m_Event(event)
        {
        }
        template <typename T>
        b8 Dispatch(EventFn<T> func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.m_Resolved = func(*(T *)&m_Event); // Cast m_Event to T, then call func() on it.
                return true;
            }
            return false;
        }

    private:
        Event &m_Event;
    };
}