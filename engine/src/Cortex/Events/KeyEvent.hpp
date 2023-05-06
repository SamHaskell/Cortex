#pragma once

#include "Cortex/Events/Event.hpp"

namespace Cortex
{
    class KeyPressEvent : public Event
    {
    public:
        KeyPressEvent(u32 keycode, b8 repeated)
        {
            m_KeyCode = keycode;
            m_Repeated = repeated;
        }

        inline u32 GetKeyCode() { return m_KeyCode; }
        inline b8 IsRepeated() { return m_Repeated; }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "KeyPressEvent: KeyCode: %i , Repeated: %i", m_KeyCode, m_Repeated);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::KeyPress; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "KeyPressEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_KEYBOARD_BIT | EVENT_INPUT_BIT; }

    private:
        u32 m_KeyCode;
        b8 m_Repeated;
    };

    class KeyReleaseEvent : public Event
    {
    public:
        KeyReleaseEvent(u32 keycode)
        {
            m_KeyCode = keycode;
        }

        inline u32 GetKeyCode() { return m_KeyCode; }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "KeyReleaseEvent: KeyCode: %i", m_KeyCode);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::KeyRelease; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "KeyReleaseEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_KEYBOARD_BIT | EVENT_INPUT_BIT; }

    private:
        u32 m_KeyCode;
    };
}