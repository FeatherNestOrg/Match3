#pragma once

#include "Components/UIComponent.hpp"
#include "Render/FontRenderer.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace Match3
{
    class Renderer;

    /**
     * @brief Manages UI components and event routing
     */
    class UIManager
    {
    public:
        UIManager();
        ~UIManager() = default;

        /**
         * @brief Add a UI component
         */
        void AddComponent(std::shared_ptr<UIComponent> component);

        /**
         * @brief Remove a UI component by ID
         */
        void RemoveComponent(const std::string& id);

        /**
         * @brief Get a component by ID
         */
        std::shared_ptr<UIComponent> GetComponent(const std::string& id);

        /**
         * @brief Clear all components
         */
        void Clear();

        /**
         * @brief Update all components
         */
        void Update(float deltaTime);

        /**
         * @brief Render all components
         */
        void Render(Renderer* renderer);

        /**
         * @brief Handle mouse move event
         * @return true if event was consumed by a UI component
         */
        bool HandleMouseMove(int mouseX, int mouseY);

        /**
         * @brief Handle mouse button down event
         * @return true if event was consumed by a UI component
         */
        bool HandleMouseDown(int mouseX, int mouseY);

        /**
         * @brief Handle mouse button up event
         * @return true if event was consumed by a UI component
         */
        bool HandleMouseUp(int mouseX, int mouseY);

        /**
         * @brief Set font renderer for UI components
         */
        void SetFontRenderer(FontRenderer* fontRenderer) { m_fontRenderer = fontRenderer; }

        /**
         * @brief Get font renderer
         */
        FontRenderer* GetFontRenderer() const { return m_fontRenderer; }

    private:
        std::vector<std::shared_ptr<UIComponent>> m_components;
        FontRenderer* m_fontRenderer; // Not owned

        // Sort components by Z-order
        void SortComponents();
    };
} // namespace Match3
