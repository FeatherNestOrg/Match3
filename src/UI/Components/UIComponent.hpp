#pragma once

#include <string>
#include <memory>

namespace Match3
{
    class Renderer;

    /**
     * @brief Base class for all UI components
     */
    class UIComponent
    {
    public:
        UIComponent(int x, int y, int width, int height);
        virtual ~UIComponent() = default;

        // Rendering
        virtual void Render(Renderer* renderer) = 0;

        // Update (called each frame)
        virtual void Update(float deltaTime) {}

        // Event handling
        virtual bool HandleMouseMove(int mouseX, int mouseY);
        virtual bool HandleMouseDown(int mouseX, int mouseY);
        virtual bool HandleMouseUp(int mouseX, int mouseY);

        // Position and size
        void SetPosition(int x, int y);
        void SetSize(int width, int height);
        void GetPosition(int& x, int& y) const;
        void GetSize(int& width, int& height) const;

        // Visibility and enabled state
        void SetVisible(bool visible) { m_visible = visible; }
        bool IsVisible() const { return m_visible; }
        
        void SetEnabled(bool enabled) { m_enabled = enabled; }
        bool IsEnabled() const { return m_enabled; }

        // Z-order for rendering depth
        void SetZOrder(int zOrder) { m_zOrder = zOrder; }
        int GetZOrder() const { return m_zOrder; }

        // ID for component identification
        void SetId(const std::string& id) { m_id = id; }
        const std::string& GetId() const { return m_id; }

    protected:
        // Check if point is inside component bounds
        bool ContainsPoint(int x, int y) const;

        int m_x, m_y;           // Position
        int m_width, m_height;  // Size
        bool m_visible;         // Is component visible
        bool m_enabled;         // Is component enabled
        int m_zOrder;           // Rendering order (higher = on top)
        std::string m_id;       // Component identifier
    };
} // namespace Match3
