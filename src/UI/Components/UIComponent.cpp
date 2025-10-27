#include "UIComponent.hpp"

namespace Match3
{
    UIComponent::UIComponent(int x, int y, int width, int height)
        : m_x(x)
        , m_y(y)
        , m_width(width)
        , m_height(height)
        , m_visible(true)
        , m_enabled(true)
        , m_zOrder(0)
    {
    }

    bool UIComponent::HandleMouseMove(int mouseX, int mouseY)
    {
        // Base implementation - can be overridden by derived classes
        return false;
    }

    bool UIComponent::HandleMouseDown(int mouseX, int mouseY)
    {
        // Base implementation - can be overridden by derived classes
        return false;
    }

    bool UIComponent::HandleMouseUp(int mouseX, int mouseY)
    {
        // Base implementation - can be overridden by derived classes
        return false;
    }

    void UIComponent::SetPosition(int x, int y)
    {
        m_x = x;
        m_y = y;
    }

    void UIComponent::SetSize(int width, int height)
    {
        m_width = width;
        m_height = height;
    }

    void UIComponent::GetPosition(int& x, int& y) const
    {
        x = m_x;
        y = m_y;
    }

    void UIComponent::GetSize(int& width, int& height) const
    {
        width = m_width;
        height = m_height;
    }

    bool UIComponent::ContainsPoint(int x, int y) const
    {
        return x >= m_x && x < m_x + m_width &&
               y >= m_y && y < m_y + m_height;
    }
} // namespace Match3
