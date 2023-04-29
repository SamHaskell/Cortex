#pragma once

#include "Cortex/Events/Event.hpp"

namespace Cortex
{
    class WindowCreateEvent : public Event
    {
    public:
        WindowCreateEvent(const char *title, u32 width, u32 height)
        {
            m_Title = title;
            m_Width = width;
            m_Height = height;
        }

        inline std::string GetTitle() { return std::string(m_Title); }
        inline u32 GetWidth() { return m_Width; }
        inline u32 GetHeight() { return m_Height; }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "WindowCreateEvent: Window: \"%s\" Dimensions: (%i, %i)", m_Title, m_Width, m_Height);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::WindowCreate; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "WindowCreateEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_WINDOW_BIT | EVENT_SYSTEM_BIT; }

    private:
        const char *m_Title;
        u32 m_Width;
        u32 m_Height;
    };

    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent(const char* title)
        {
            m_Title = title;
        }

        inline std::string GetTitle() { return std::string(m_Title); }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "WindowCloseEvent: Window: \"%s\"", m_Title);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::WindowClose; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "WindowCloseEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_WINDOW_BIT | EVENT_SYSTEM_BIT; }

    private:
        const char *m_Title;
    };

    class WindowMoveEvent : public Event
    {
    public:
        WindowMoveEvent(const char* title, i32 xpos, i32 ypos)
        {
            m_Title = title;
            m_PositionX = xpos;
            m_PositionY = ypos;
        }

        inline std::string GetTitle() { return std::string(m_Title); }
        inline i32 GetX() { return m_PositionX; }
        inline i32 GetY() { return m_PositionY; }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "WindowMoveEvent: Window: \"%s\" Position: (%i, %i)", m_Title, m_PositionX, m_PositionY);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::WindowMove; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "WindowMoveEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_WINDOW_BIT | EVENT_SYSTEM_BIT; }

    private:
        const char* m_Title;
        i32 m_PositionX;
        i32 m_PositionY;
    };

    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(const char* title, u32 width, u32 height)
        {
            m_Title = title;
            m_Width = width;
            m_Height = height;
        }

        inline std::string GetTitle() { return std::string(m_Title); }
        inline u32 GetWidth() { return m_Width; }
        inline u32 GetHeight() { return m_Height; }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "WindowResizeEvent: Window: \"%s\" Dimensions: (%i, %i)", m_Title, m_Width, m_Height);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::WindowResize; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "WindowResizeEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_WINDOW_BIT | EVENT_SYSTEM_BIT; }

    private:
        const char* m_Title;
        u32 m_Width;
        u32 m_Height;
    };

    class WindowFocusEvent : public Event
    {
    public:
        WindowFocusEvent(const char* title)
        {
            m_Title = title;
        }

        inline std::string GetTitle() { return std::string(m_Title); }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "WindowFocusEvent: Window: \"%s\"", m_Title);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::WindowFocus; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "WindowFocusEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_WINDOW_BIT | EVENT_SYSTEM_BIT; }

    private:
        const char *m_Title;
    };

    class WindowDefocusEvent : public Event
    {
    public:
        WindowDefocusEvent(const char* title)
        {
            m_Title = title;
        }

        inline std::string GetTitle() { return std::string(m_Title); }

        std::string ToString() const override
        {
            char output[128];
            memset(output, 0, sizeof(output));
            snprintf(output, sizeof(output), "WindowDefocusEvent: Window: \"%s\"", m_Title);
            return std::string(output);
        }

        static EventType GetStaticType() { return EventType::WindowDefocus; }
        virtual EventType GetEventType() const override { return GetStaticType(); }
        virtual const char *GetName() const override { return "WindowDefocusEvent"; }
        virtual i32 GetCategoryFlags() const override { return EVENT_WINDOW_BIT | EVENT_SYSTEM_BIT; }

    private:
        const char *m_Title;
    };

}