#pragma once

#include "UIComponent.hpp"
#include "Render/FontRenderer.hpp"
#include <string>
#include <functional>

namespace Match3
{
    /**
     * @brief Button UI component with hover and press states
     */
    class Button : public UIComponent
    {
    public:
        using ClickCallback = std::function<void()>;

        Button(int x, int y, int width, int height, const std::string& text, const std::string& fontId);
        ~Button() override = default;

        void Render(Renderer* renderer) override;

        // Event handling
        bool HandleMouseMove(int mouseX, int mouseY) override;
        bool HandleMouseDown(int mouseX, int mouseY) override;
        bool HandleMouseUp(int mouseX, int mouseY) override;

        // Text
        void SetText(const std::string& text) { m_text = text; }
        const std::string& GetText() const { return m_text; }

        void SetFontId(const std::string& fontId) { m_fontId = fontId; }
        const std::string& GetFontId() const { return m_fontId; }

        // Colors for different states
        void SetNormalColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        void SetHoverColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        void SetPressedColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        void SetTextColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

        // Border
        void SetBorderEnabled(bool enabled) { m_borderEnabled = enabled; }
        void SetBorderColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

        // Callback
        void SetOnClick(ClickCallback callback) { m_onClick = callback; }

        // Font renderer
        void SetFontRenderer(FontRenderer* fontRenderer) { m_fontRenderer = fontRenderer; }

    private:
        enum class ButtonState
        {
            Normal,
            Hovered,
            Pressed
        };

        std::string m_text;
        std::string m_fontId;
        ButtonState m_state;
        
        // Colors
        uint8_t m_normalR, m_normalG, m_normalB, m_normalA;
        uint8_t m_hoverR, m_hoverG, m_hoverB, m_hoverA;
        uint8_t m_pressedR, m_pressedG, m_pressedB, m_pressedA;
        uint8_t m_textR, m_textG, m_textB, m_textA;
        
        // Border
        bool m_borderEnabled;
        uint8_t m_borderR, m_borderG, m_borderB, m_borderA;
        
        ClickCallback m_onClick;
        FontRenderer* m_fontRenderer; // Not owned
    };
} // namespace Match3
