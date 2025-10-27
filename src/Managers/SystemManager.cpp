#include "SystemManager.hpp"
#include "Core/Logger.hpp"

namespace Match3 {

void SystemManager::UpdateAll(entt::registry& registry, float deltaTime)
{
    // 按顺序更新所有已启用的系统
    for (auto& system : m_systems) {
        if (system && system->IsEnabled()) {
            system->Update(registry, deltaTime);
        }
    }
}

void SystemManager::SetAllEnabled(bool enabled)
{
    for (auto& system : m_systems) {
        if (system) {
            system->SetEnabled(enabled);
        }
    }
    
    LOG_INFO("SystemManager: {} {} systems", 
             enabled ? "Enabled" : "Disabled", m_systems.size());
}

} // namespace Match3
