#include "InputManager.hpp"

namespace Match3
{
    InputManager::InputManager()
        : m_mouseX(0)
          , m_mouseY(0)
          , m_mouseDeltaX(0)
          , m_mouseDeltaY(0)
    {
    }

    void InputManager::HandleEvent(const SDL_Event& event)
    {
        switch (event.type)
        {
        case SDL_EVENT_MOUSE_MOTION:
            {
                const int oldX = m_mouseX;
                const int oldY = m_mouseY;
                m_mouseX = static_cast<int>(event.motion.x);
                m_mouseY = static_cast<int>(event.motion.y);
                m_mouseDeltaX = m_mouseX - oldX;
                m_mouseDeltaY = m_mouseY - oldY;

                if (m_mouseMoveCallback)
                {
                    m_mouseMoveCallback(m_mouseX, m_mouseY, m_mouseDeltaX, m_mouseDeltaY);
                }
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                const int button = event.button.button;
                m_mouseButtonStates[button] = true;
                m_mouseButtonPressed[button] = true;

                if (m_mouseClickCallback)
                {
                    m_mouseClickCallback(m_mouseX, m_mouseY, static_cast<MouseButton>(button));
                }
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                const int button = event.button.button;
                m_mouseButtonStates[button] = false;
                m_mouseButtonReleased[button] = true;
            }
            break;

        case SDL_EVENT_KEY_DOWN:
            if (!event.key.repeat) // 忽略重复按键
            {
                const SDL_Keycode key = event.key.key;
                m_keyStates[key] = true;
                m_keyPressed[key] = true;

                if (m_keyDownCallback)
                {
                    m_keyDownCallback(key);
                }
            }
            break;

        case SDL_EVENT_KEY_UP:
            {
                const SDL_Keycode key = event.key.key;
                m_keyStates[key] = false;
                m_keyReleased[key] = true;

                if (m_keyUpCallback)
                {
                    m_keyUpCallback(key);
                }
            }
            break;

        default:
            break;
        }
    }

    void InputManager::Update()
    {
        // 清除本帧的按下/释放状态
        m_mouseButtonPressed.clear();
        m_mouseButtonReleased.clear();
        m_keyPressed.clear();
        m_keyReleased.clear();
        m_mouseDeltaX = 0;
        m_mouseDeltaY = 0;
    }

    void InputManager::Reset()
    {
        m_mouseButtonStates.clear();
        m_mouseButtonPressed.clear();
        m_mouseButtonReleased.clear();
        m_keyStates.clear();
        m_keyPressed.clear();
        m_keyReleased.clear();
        m_mouseDeltaX = 0;
        m_mouseDeltaY = 0;
    }

    bool InputManager::IsMouseButtonDown(const MouseButton button) const
    {
        const int btn = static_cast<int>(button);
        const auto it = m_mouseButtonStates.find(btn);
        return it != m_mouseButtonStates.end() && it->second;
    }

    bool InputManager::IsMouseButtonPressed(const MouseButton button) const
    {
        const int btn = static_cast<int>(button);
        const auto it = m_mouseButtonPressed.find(btn);
        return it != m_mouseButtonPressed.end() && it->second;
    }

    bool InputManager::IsMouseButtonReleased(const MouseButton button) const
    {
        const int btn = static_cast<int>(button);
        const auto it = m_mouseButtonReleased.find(btn);
        return it != m_mouseButtonReleased.end() && it->second;
    }

    bool InputManager::IsKeyDown(const SDL_Keycode key) const
    {
        const auto it = m_keyStates.find(key);
        return it != m_keyStates.end() && it->second;
    }

    bool InputManager::IsKeyPressed(const SDL_Keycode key) const
    {
        const auto it = m_keyPressed.find(key);
        return it != m_keyPressed.end() && it->second;
    }

    bool InputManager::IsKeyReleased(const SDL_Keycode key) const
    {
        const auto it = m_keyReleased.find(key);
        return it != m_keyReleased.end() && it->second;
    }
} // namespace Match3

