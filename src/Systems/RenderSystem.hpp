#pragma once

#include "System.hpp"
#include "../Components/Common.hpp"
#include "../Components/Gem.hpp"
#include "../Components/Particle.hpp"
#include "Render/Renderer.hpp"
#include "Core/Config.hpp"

namespace Match3::Systems {

/**
 * @brief 渲染系统 - 渲染所有可见实体
 * 
 * 职责：
 * - 渲染所有有Renderable组件的实体
 * - 区分渲染宝石、粒子等不同类型
 * - 应用缩放、旋转、透明度等视觉效果
 */
class RenderSystem : public System {
public:
    explicit RenderSystem(Renderer* renderer);
    
    void Update(entt::registry& registry, float deltaTime) override;
    
    [[nodiscard]] const char* GetName() const override { return "RenderSystem"; }
    
private:
    // 渲染不同类型的实体
    void RenderGems(entt::registry& registry);
    void RenderParticles(entt::registry& registry);
    
    // 渲染单个实体
    void RenderGem(const Components::Position& pos,
                   const Components::Renderable& render,
                   const Components::Gem& gem);
    
    void RenderParticle(const Components::Position& pos,
                        const Components::Renderable& render);
    
    Renderer* m_renderer;
};

} // namespace Match3::Systems
