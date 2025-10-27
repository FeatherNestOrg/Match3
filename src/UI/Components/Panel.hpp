#pragma once

#include "UIComponent.hpp"

namespace Match3
{
    /**
     * @brief Panel/background UI component - renders a filled rectangle
     */
    class Panel : public UIComponent
    {
    public:
        Panel(int x, int y, int width, int height);
        ~Panel() override = default;

        void Render(Renderer* renderer) override;

        // Color
        void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        
        // Border
        void SetBorderEnabled(bool enabled) { m_borderEnabled = enabled; }
        void SetBorderColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        void SetBorderWidth(int width) { m_borderWidth = width; }

    private:
        uint8_t m_r, m_g, m_b, m_a;
        bool m_borderEnabled;
        uint8_t m_borderR, m_borderG, m_borderB, m_borderA;
        int m_borderWidth;
    };
} // namespace Match3
