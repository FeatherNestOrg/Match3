#include "Panel.hpp"
#include "Render/Renderer.hpp"

namespace Match3
{
    Panel::Panel(int x, int y, int width, int height)
        : UIComponent(x, y, width, height)
        , m_r(100), m_g(100), m_b(100), m_a(255)
        , m_borderEnabled(false)
        , m_borderR(200), m_borderG(200), m_borderB(200), m_borderA(255)
        , m_borderWidth(2)
    {
    }

    void Panel::Render(Renderer* renderer)
    {
        if (!m_visible)
            return;

        // Render filled rectangle
        renderer->SetDrawColor(m_r, m_g, m_b, m_a);
        renderer->FillRect(m_x, m_y, m_width, m_height);

        // Render border if enabled
        if (m_borderEnabled)
        {
            renderer->SetDrawColor(m_borderR, m_borderG, m_borderB, m_borderA);
            
            // Draw border as multiple rectangles for thickness
            for (int i = 0; i < m_borderWidth; ++i)
            {
                renderer->DrawRect(m_x + i, m_y + i, 
                                 m_width - 2 * i, m_height - 2 * i);
            }
        }
    }

    void Panel::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_r = r;
        m_g = g;
        m_b = b;
        m_a = a;
    }

    void Panel::SetBorderColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_borderR = r;
        m_borderG = g;
        m_borderB = b;
        m_borderA = a;
    }
} // namespace Match3
