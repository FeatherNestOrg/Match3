#pragma once

#include "UIComponent.hpp"
#include "Render/FontRenderer.hpp"
#include <string>

namespace Match3
{
    /**
     * @brief Text label UI component
     */
    class Label : public UIComponent
    {
    public:
        Label(int x, int y, const std::string& text, const std::string& fontId);
        ~Label() override = default;

        void Render(Renderer* renderer) override;

        // Text properties
        void SetText(const std::string& text);
        const std::string& GetText() const { return m_text; }

        void SetFontId(const std::string& fontId) { m_fontId = fontId; }
        const std::string& GetFontId() const { return m_fontId; }

        // Color
        void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        
        // Alignment
        void SetAlignment(TextAlign align) { m_alignment = align; }
        TextAlign GetAlignment() const { return m_alignment; }

        // Font renderer (must be set before rendering)
        void SetFontRenderer(FontRenderer* fontRenderer) { m_fontRenderer = fontRenderer; }

    private:
        std::string m_text;
        std::string m_fontId;
        uint8_t m_r, m_g, m_b, m_a;
        TextAlign m_alignment;
        FontRenderer* m_fontRenderer; // Not owned
    };
} // namespace Match3
