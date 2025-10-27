#pragma once

#include "System.hpp"
#include "../Components/Common.hpp"
#include "../Components/Particle.hpp"

namespace Match3::Systems {

/**
 * @brief 粒子系统 - 处理粒子物理和更新
 * 
 * 职责：
 * - 更新粒子位置（根据速度）
 * - 应用重力加速度
 * - 更新粒子生命周期（配合LifetimeSystem）
 */
class ParticleSystem : public System {
public:
    ParticleSystem() = default;
    
    void Update(entt::registry& registry, float deltaTime) override;
    
    [[nodiscard]] const char* GetName() const override { return "ParticleSystem"; }
    
private:
    void UpdatePhysics(entt::registry& registry, float dt);
    void UpdateVisuals(entt::registry& registry, float dt);
};

} // namespace Match3::Systems
