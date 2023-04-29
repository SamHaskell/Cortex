#pragma once

#include "Cortex/Events/Event.hpp"

namespace Cortex
{
    class MouseButtonPressEvent : public Event
    {
    public:
        MouseButtonPressEvent(u32 buttoncode)
        {
            m_ButtonCode = buttoncode;
        }

        inline u32 GetButtonCode() { return m_ButtonCode; }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "MouseButtonPressEvent: ButtonCode: %i", m_ButtonCode);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::MouseButtonPress; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "MouseButtonPressEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_MOUSE_BIT | EVENT_INPUT_BIT; }

    private:
        u32 m_ButtonCode;
    };

    class MouseButtonReleaseEvent : public Event
    {
    public:
        MouseButtonReleaseEvent(u32 buttoncode)
        {
            m_ButtonCode = buttoncode;
        }

        inline u32 GetButtonCode() { return m_ButtonCode; }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "MouseButtonReleaseEvent: ButtonCode: %i", m_ButtonCode);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::MouseButtonRelease; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "MouseButtonReleaseEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_MOUSE_BIT | EVENT_INPUT_BIT; }

    private:
        u32 m_ButtonCode;
    };

    class MouseMoveEvent : public Event
    {
    public:
        MouseMoveEvent(f64 xpos, f64 ypos)
        {
            m_PositionX = xpos;
            m_PositionY = ypos;
        }

        inline f64 GetX() { return m_PositionX; }
        inline f64 GetY() { return m_PositionY; }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "MouseMoveEvent: New Position: (%f, %f)", m_PositionX, m_PositionY);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::MouseMove; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "MouseMoveEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_MOUSE_BIT | EVENT_INPUT_BIT; }

    private:
        f64 m_PositionX;
        f64 m_PositionY;
    };

    class MouseScrollEvent : public Event
    {
    public:
        MouseScrollEvent(f64 xdelta, f64 ydelta)
        {
            m_DeltaX = xdelta;
            m_DeltaY = ydelta;
        }

        inline f64 GetDeltaX() { return m_DeltaX; }
        inline f64 GetDeltaY() { return m_DeltaY; }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "MouseScrollEvent: Scroll Delta: (%f, %f)", m_DeltaX, m_DeltaY);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::MouseScroll; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "MouseScrollEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_MOUSE_BIT | EVENT_INPUT_BIT; }

    private:
        f64 m_DeltaX;
        f64 m_DeltaY;
    };

}