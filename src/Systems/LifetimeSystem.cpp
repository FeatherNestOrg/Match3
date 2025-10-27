#include "LifetimeSystem.hpp"
#include "Core/Logger.hpp"

namespace Match3::Systems {

void LifetimeSystem::Update(entt::registry& registry, float deltaTime)
{
    if (!m_enabled) return;
    
    m_cleanedThisFrame = 0;
    
    // 更新所有Lifetime组件
    auto view = registry.view<Components::Lifetime>();
    
    std::vector<entt::entity> toDestroy;
    
    for (auto entity : view) {
        auto& lifetime = view.get<Components::Lifetime>(entity);
        
        // 更新计时器
        lifetime.Update(deltaTime);
        
        // 如果过期，加入清理列表
        if (lifetime.IsExpired()) {
            toDestroy.push_back(entity);
        }
    }
    
    // 批量销毁过期实体
    for (auto entity : toDestroy) {
        registry.destroy(entity);
        ++m_cleanedThisFrame;
    }
    
    if (m_cleanedThisFrame > 0) {
        LOG_DEBUG("{}: Cleaned {} expired entities", GetName(), m_cleanedThisFrame);
    }
}

} // namespace Match3::Systems
