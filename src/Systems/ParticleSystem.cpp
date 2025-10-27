#include "ParticleSystem.hpp"
#include "Core/Logger.hpp"

namespace Match3::Systems {

void ParticleSystem::Update(entt::registry& registry, float deltaTime)
{
    if (!m_enabled) return;
    
    UpdatePhysics(registry, deltaTime);
    UpdateVisuals(registry, deltaTime);
}

void ParticleSystem::UpdatePhysics(entt::registry& registry, float dt)
{
    // 更新所有有Particle和Velocity组件的实体
    auto view = registry.view<Components::Position, Components::Velocity, Components::Particle>();
    
    for (auto entity : view) {
        auto& pos = view.get<Components::Position>(entity);
        auto& vel = view.get<Components::Velocity>(entity);
        const auto& particle = view.get<Components::Particle>(entity);
        
        // 应用重力
        vel.vy += particle.gravity * dt;
        
        // 更新位置
        pos.x += vel.vx * dt;
        pos.y += vel.vy * dt;
    }
}

void ParticleSystem::UpdateVisuals(entt::registry& registry, float dt)
{
    // 粒子随着生命周期接近结束而淡出
    auto view = registry.view<Components::Renderable, Components::Lifetime, Components::Particle>();
    
    for (auto entity : view) {
        auto& render = view.get<Components::Renderable>(entity);
        const auto& lifetime = view.get<Components::Lifetime>(entity);
        
        // 计算淡出效果（最后20%的生命周期开始淡出）
        const float progress = lifetime.GetProgress();
        if (progress > 0.8f) {
            const float fadeProgress = (progress - 0.8f) / 0.2f;
            const float alpha = 1.0f - fadeProgress;
            render.a = static_cast<uint8_t>(alpha * 255.0f);
        }
    }
}

} // namespace Match3::Systems
