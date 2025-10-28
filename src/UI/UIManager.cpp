#include "UIManager.hpp"

#include <ranges>
#include "Render/Renderer.hpp"

namespace Match3
{
    UIManager::UIManager()
        : m_fontRenderer(nullptr)
    {
    }

    void UIManager::AddComponent(std::shared_ptr<UIComponent> component)
    {
        if (component)
        {
            m_components.push_back(component);
            SortComponents();
        }
    }

    void UIManager::RemoveComponent(const std::string& id)
    {
        m_components.erase(
            std::remove_if(m_components.begin(), m_components.end(),
                           [&id](const std::shared_ptr<UIComponent>& comp)
                           {
                               return comp->GetId() == id;
                           }),
            m_components.end()
        );
    }

    std::shared_ptr<UIComponent> UIManager::GetComponent(const std::string& id)
    {
        const auto it = std::ranges::find_if(m_components,
                                             [&id](const std::shared_ptr<UIComponent>& comp)
                                             {
                                                 return comp->GetId() == id;
                                             });

        return (it != m_components.end()) ? *it : nullptr;
    }

    void UIManager::Clear()
    {
        m_components.clear();
    }

    void UIManager::Update(const float deltaTime)
    {
        for (auto& component : m_components)
        {
            if (component->IsVisible() && component->IsEnabled())
            {
                component->Update(deltaTime);
            }
        }
    }

    void UIManager::Render(Renderer* renderer)
    {
        // Components are already sorted by Z-order
        for (auto& component : m_components)
        {
            if (component->IsVisible())
            {
                component->Render(renderer);
            }
        }
    }

    bool UIManager::HandleMouseMove(int mouseX, int mouseY)
    {
        // Process in reverse order (top to bottom)
        for (auto& component : std::ranges::reverse_view(m_components))
        {
            if (component->IsVisible() && component->IsEnabled())
            {
                if (component->HandleMouseMove(mouseX, mouseY))
                {
                    // Event consumed
                    return true;
                }
            }
        }
        return false;
    }

    bool UIManager::HandleMouseDown(int mouseX, int mouseY)
    {
        // Process in reverse order (top to bottom)
        for (auto& component : std::ranges::reverse_view(m_components))
        {
            if (component->IsVisible() && component->IsEnabled())
            {
                if (component->HandleMouseDown(mouseX, mouseY))
                {
                    // Event consumed
                    return true;
                }
            }
        }
        return false;
    }

    bool UIManager::HandleMouseUp(int mouseX, int mouseY)
    {
        // Process in reverse order (top to bottom)
        for (auto& component : std::ranges::reverse_view(m_components))
        {
            if (component->IsVisible() && component->IsEnabled())
            {
                if (component->HandleMouseUp(mouseX, mouseY))
                {
                    // Event consumed
                    return true;
                }
            }
        }
        return false;
    }

    void UIManager::SortComponents()
    {
        std::ranges::sort(m_components,
                          [](const std::shared_ptr<UIComponent>& a, const std::shared_ptr<UIComponent>& b)
                          {
                              return a->GetZOrder() < b->GetZOrder();
                          });
    }
} // namespace Match3
