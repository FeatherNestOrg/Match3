#pragma once

#include <SDL3/SDL.h>
#include <functional>
#include <unordered_map>

namespace Match3
{
    /**
     * @brief 输入管理器 - 统一处理键盘、鼠标等输入
     */
    class InputManager
    {
    public:
        /**
         * @brief 鼠标按钮枚举
         */
        enum class MouseButton
        {
            Left = SDL_BUTTON_LEFT,
            Middle = SDL_BUTTON_MIDDLE,
            Right = SDL_BUTTON_RIGHT
        };

        /**
         * @brief 鼠标事件回调类型
         */
        using MouseClickCallback = std::function<void(int x, int y, MouseButton button)>;
        using MouseMoveCallback = std::function<void(int x, int y, int dx, int dy)>;
        using KeyCallback = std::function<void(SDL_Keycode key)>;

        InputManager();
        ~InputManager() = default;

        /**
         * @brief 处理 SDL 事件
         * @param event SDL 事件
         */
        void HandleEvent(const SDL_Event& event);

        /**
         * @brief 更新输入状态（每帧调用）
         */
        void Update();

        /**
         * @brief 重置输入状态
         */
        void Reset();

        // === 鼠标状态查询 ===
        [[nodiscard]] int GetMouseX() const { return m_mouseX; }
        [[nodiscard]] int GetMouseY() const { return m_mouseY; }
        [[nodiscard]] bool IsMouseButtonDown(MouseButton button) const;
        [[nodiscard]] bool IsMouseButtonPressed(MouseButton button) const; // 本帧按下
        [[nodiscard]] bool IsMouseButtonReleased(MouseButton button) const; // 本帧释放

        // === 键盘状态查询 ===
        [[nodiscard]] bool IsKeyDown(SDL_Keycode key) const;
        [[nodiscard]] bool IsKeyPressed(SDL_Keycode key) const; // 本帧按下
        [[nodiscard]] bool IsKeyReleased(SDL_Keycode key) const; // 本帧释放

        // === 事件回调注册 ===
        void SetMouseClickCallback(MouseClickCallback callback) { m_mouseClickCallback = std::move(callback); }
        void SetMouseMoveCallback(MouseMoveCallback callback) { m_mouseMoveCallback = std::move(callback); }
        void SetKeyDownCallback(KeyCallback callback) { m_keyDownCallback = std::move(callback); }
        void SetKeyUpCallback(KeyCallback callback) { m_keyUpCallback = std::move(callback); }

    private:
        // 鼠标状态
        int m_mouseX;
        int m_mouseY;
        int m_mouseDeltaX;
        int m_mouseDeltaY;

        std::unordered_map<int, bool> m_mouseButtonStates; // 当前状态
        std::unordered_map<int, bool> m_mouseButtonPressed; // 本帧按下
        std::unordered_map<int, bool> m_mouseButtonReleased; // 本帧释放

        // 键盘状态
        std::unordered_map<SDL_Keycode, bool> m_keyStates; // 当前状态
        std::unordered_map<SDL_Keycode, bool> m_keyPressed; // 本帧按下
        std::unordered_map<SDL_Keycode, bool> m_keyReleased; // 本帧释放

        // 事件回调
        MouseClickCallback m_mouseClickCallback;
        MouseMoveCallback m_mouseMoveCallback;
        KeyCallback m_keyDownCallback;
        KeyCallback m_keyUpCallback;
    };
} // namespace Match3

