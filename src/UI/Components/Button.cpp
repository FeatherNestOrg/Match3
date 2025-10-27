#include "Button.hpp"
#include "Render/Renderer.hpp"
#include "Core/Logger.hpp"

namespace Match3
{
    Button::Button(int x, int y, int width, int height, const std::string& text, const std::string& fontId)
        : UIComponent(x, y, width, height)
        , m_text(text)
        , m_fontId(fontId)
        , m_state(ButtonState::Normal)
        , m_normalR(70), m_normalG(70), m_normalB(70), m_normalA(255)
        , m_hoverR(100), m_hoverG(100), m_hoverB(100), m_hoverA(255)
        , m_pressedR(50), m_pressedG(50), m_pressedB(50), m_pressedA(255)
        , m_textR(255), m_textG(255), m_textB(255), m_textA(255)
        , m_borderEnabled(true)
        , m_borderR(150), m_borderG(150), m_borderB(150), m_borderA(255)
        , m_fontRenderer(nullptr)
    {
    }

    void Button::Render(Renderer* renderer)
    {
        if (!m_visible)
            return;

        // Select color based on state
        uint8_t r, g, b, a;
        switch (m_state)
        {
            case ButtonState::Hovered:
                r = m_hoverR; g = m_hoverG; b = m_hoverB; a = m_hoverA;
                break;
            case ButtonState::Pressed:
                r = m_pressedR; g = m_pressedG; b = m_pressedB; a = m_pressedA;
                break;
            case ButtonState::Normal:
            default:
                r = m_normalR; g = m_normalG; b = m_normalB; a = m_normalA;
                break;
        }

        // Render button background
        renderer->SetDrawColor(r, g, b, a);
        renderer->FillRect(m_x, m_y, m_width, m_height);

        // Render border
        if (m_borderEnabled)
        {
            renderer->SetDrawColor(m_borderR, m_borderG, m_borderB, m_borderA);
            renderer->DrawRect(m_x, m_y, m_width, m_height);
            renderer->DrawRect(m_x + 1, m_y + 1, m_width - 2, m_height - 2);
        }

        // Render text
        if (m_fontRenderer && !m_text.empty())
        {
            int textWidth = m_fontRenderer->MeasureText(m_text, m_fontId);
            int textHeight = m_fontRenderer->GetTextHeight(m_fontId);
            
            if (textWidth > 0 && textHeight > 0)
            {
                int textX = m_x + (m_width - textWidth) / 2;
                int textY = m_y + (m_height - textHeight) / 2;
                
                m_fontRenderer->RenderText(m_text, textX, textY, m_fontId,
                                          m_textR, m_textG, m_textB, m_textA);
            }
        }
    }

    bool Button::HandleMouseMove(int mouseX, int mouseY)
    {
        if (!m_enabled)
            return false;

        if (ContainsPoint(mouseX, mouseY))
        {
            if (m_state == ButtonState::Normal)
            {
                m_state = ButtonState::Hovered;
            }
            return true;
        }
        else
        {
            if (m_state == ButtonState::Hovered)
            {
                m_state = ButtonState::Normal;
            }
            return false;
        }
    }

    bool Button::HandleMouseDown(int mouseX, int mouseY)
    {
        if (!m_enabled)
            return false;

        if (ContainsPoint(mouseX, mouseY))
        {
            m_state = ButtonState::Pressed;
            return true;
        }
        return false;
    }

    bool Button::HandleMouseUp(int mouseX, int mouseY)
    {
        if (!m_enabled)
            return false;

        if (m_state == ButtonState::Pressed && ContainsPoint(mouseX, mouseY))
        {
            m_state = ButtonState::Hovered;
            
            // Trigger callback
            if (m_onClick)
            {
                std::string id = m_id.empty() ? m_text : m_id;
                LOG_DEBUG("Button '{}' clicked", id);
                m_onClick();
            }
            return true;
        }
        
        if (ContainsPoint(mouseX, mouseY))
        {
            m_state = ButtonState::Hovered;
        }
        else
        {
            m_state = ButtonState::Normal;
        }
        
        return false;
    }

    void Button::SetNormalColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_normalR = r; m_normalG = g; m_normalB = b; m_normalA = a;
    }

    void Button::SetHoverColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_hoverR = r; m_hoverG = g; m_hoverB = b; m_hoverA = a;
    }

    void Button::SetPressedColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_pressedR = r; m_pressedG = g; m_pressedB = b; m_pressedA = a;
    }

    void Button::SetTextColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_textR = r; m_textG = g; m_textB = b; m_textA = a;
    }

    void Button::SetBorderColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_borderR = r; m_borderG = g; m_borderB = b; m_borderA = a;
    }
} // namespace Match3
