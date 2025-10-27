#include "Label.hpp"
#include "Render/Renderer.hpp"

namespace Match3
{
    Label::Label(int x, int y, const std::string& text, const std::string& fontId)
        : UIComponent(x, y, 0, 0)
        , m_text(text)
        , m_fontId(fontId)
        , m_r(255), m_g(255), m_b(255), m_a(255)
        , m_alignment(TextAlign::Left)
        , m_fontRenderer(nullptr)
    {
    }

    void Label::Render(Renderer* renderer)
    {
        if (!m_visible || !m_fontRenderer || m_text.empty())
            return;

        m_fontRenderer->RenderText(m_text, m_x, m_y, m_fontId, m_r, m_g, m_b, m_a, m_alignment);
    }

    void Label::SetText(const std::string& text)
    {
        m_text = text;
        
        // Update width based on new text if font renderer is available
        if (m_fontRenderer)
        {
            int width = m_fontRenderer->MeasureText(m_text, m_fontId);
            if (width > 0)
            {
                m_width = width;
            }
            
            int height = m_fontRenderer->GetTextHeight(m_fontId);
            if (height > 0)
            {
                m_height = height;
            }
        }
    }

    void Label::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_r = r;
        m_g = g;
        m_b = b;
        m_a = a;
    }
} // namespace Match3
