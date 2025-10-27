#include "UIManager.hpp"
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
                [&id](const std::shared_ptr<UIComponent>& comp) {
                    return comp->GetId() == id;
                }),
            m_components.end()
        );
    }

    std::shared_ptr<UIComponent> UIManager::GetComponent(const std::string& id)
    {
        auto it = std::find_if(m_components.begin(), m_components.end(),
            [&id](const std::shared_ptr<UIComponent>& comp) {
                return comp->GetId() == id;
            });
        
        return (it != m_components.end()) ? *it : nullptr;
    }

    void UIManager::Clear()
    {
        m_components.clear();
    }

    void UIManager::Update(float deltaTime)
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

    void UIManager::HandleMouseMove(int mouseX, int mouseY)
    {
        // Process in reverse order (top to bottom)
        for (auto it = m_components.rbegin(); it != m_components.rend(); ++it)
        {
            auto& component = *it;
            if (component->IsVisible() && component->IsEnabled())
            {
                if (component->HandleMouseMove(mouseX, mouseY))
                {
                    // Event consumed
                    break;
                }
            }
        }
    }

    void UIManager::HandleMouseDown(int mouseX, int mouseY)
    {
        // Process in reverse order (top to bottom)
        for (auto it = m_components.rbegin(); it != m_components.rend(); ++it)
        {
            auto& component = *it;
            if (component->IsVisible() && component->IsEnabled())
            {
                if (component->HandleMouseDown(mouseX, mouseY))
                {
                    // Event consumed
                    break;
                }
            }
        }
    }

    void UIManager::HandleMouseUp(int mouseX, int mouseY)
    {
        // Process in reverse order (top to bottom)
        for (auto it = m_components.rbegin(); it != m_components.rend(); ++it)
        {
            auto& component = *it;
            if (component->IsVisible() && component->IsEnabled())
            {
                if (component->HandleMouseUp(mouseX, mouseY))
                {
                    // Event consumed
                    break;
                }
            }
        }
    }

    void UIManager::SortComponents()
    {
        std::sort(m_components.begin(), m_components.end(),
            [](const std::shared_ptr<UIComponent>& a, const std::shared_ptr<UIComponent>& b) {
                return a->GetZOrder() < b->GetZOrder();
            });
    }
} // namespace Match3
