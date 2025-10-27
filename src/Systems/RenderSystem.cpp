#include "RenderSystem.hpp"
#include "Core/Logger.hpp"

namespace Match3::Systems {

RenderSystem::RenderSystem(Renderer* renderer)
    : m_renderer(renderer)
{
    if (!m_renderer) {
        LOG_ERROR("RenderSystem: renderer is null!");
    }
}

void RenderSystem::Update(entt::registry& registry, float deltaTime)
{
    if (!m_enabled || !m_renderer) return;
    
    // 先渲染宝石（背景层）
    RenderGems(registry);
    
    // 再渲染粒子（前景层）
    RenderParticles(registry);
}

void RenderSystem::RenderGems(entt::registry& registry)
{
    // 渲染所有宝石
    auto view = registry.view<Components::Position, Components::Renderable, Components::Gem>();
    
    int renderCount = 0;
    for (auto entity : view) {
        const auto& pos = view.get<Components::Position>(entity);
        const auto& render = view.get<Components::Renderable>(entity);
        const auto& gem = view.get<Components::Gem>(entity);
        
        // 跳过空宝石或完全透明的宝石
        if (gem.IsEmpty() || render.a == 0) {
            continue;
        }
        
        RenderGem(pos, render, gem);
        renderCount++;
    }
    
    // 只在第一帧打印一次
    static bool firstFrame = true;
    if (firstFrame) {
        LOG_INFO("RenderSystem: Rendered {} gems", renderCount);
        firstFrame = false;
    }
}

void RenderSystem::RenderParticles(entt::registry& registry)
{
    // 渲染所有粒子
    auto view = registry.view<Components::Position, Components::Renderable, Components::Particle>();
    
    for (auto entity : view) {
        const auto& pos = view.get<Components::Position>(entity);
        const auto& render = view.get<Components::Renderable>(entity);
        
        // 跳过完全透明的粒子
        if (render.a == 0) {
            continue;
        }
        
        RenderParticle(pos, render);
    }
}

void RenderSystem::RenderGem(const Components::Position& pos,
                              const Components::Renderable& render,
                              const Components::Gem& gem)
{
    const int centerX = static_cast<int>(pos.x);
    const int centerY = static_cast<int>(pos.y);
    const int radius = static_cast<int>(render.radius * render.scale);
    
    // 绘制宝石主体（实心圆）
    const float alpha = render.a / 255.0f;  // 转换为0.0-1.0范围
    m_renderer->FillCircleWithAlpha(centerX, centerY, radius,
                                    render.r, render.g, render.b, alpha);
    
    // 绘制边框
    const auto& borderColor = Config::GEM_BORDER_COLOR;
    const auto borderAlpha = static_cast<uint8_t>(borderColor.a * (render.a / 255.0f));
    m_renderer->SetDrawColor(borderColor.r, borderColor.g, borderColor.b, borderAlpha);
    m_renderer->DrawCircle(centerX, centerY, radius);
    
    // 添加高光效果（只在宝石足够大时）
    if (render.scale > 0.5f && radius > 10) {
        const int highlightOffset = radius / Config::GEM_HIGHLIGHT_OFFSET_DIVISOR;
        const int highlightRadius = highlightOffset;
        const int highlightX = centerX - highlightOffset;
        const int highlightY = centerY - highlightOffset;
        
        const auto& highlightColor = Config::GEM_HIGHLIGHT_COLOR;
        const auto highlightAlpha = static_cast<uint8_t>(highlightColor.a * (render.a / 255.0f));
        m_renderer->SetDrawColor(highlightColor.r, highlightColor.g, highlightColor.b, highlightAlpha);
        m_renderer->FillCircle(highlightX, highlightY, highlightRadius);
    }
    
    // 如果宝石被选中，绘制高亮环
    // 这里可以通过检查Selected组件来实现，但为了简化先不实现
}

void RenderSystem::RenderParticle(const Components::Position& pos,
                                   const Components::Renderable& render)
{
    const int centerX = static_cast<int>(pos.x);
    const int centerY = static_cast<int>(pos.y);
    const int radius = static_cast<int>(render.radius * render.scale);
    
    // 绘制粒子（实心圆）
    const float alpha = render.a / 255.0f;  // 转换为0.0-1.0范围
    m_renderer->FillCircleWithAlpha(centerX, centerY, radius,
                                    render.r, render.g, render.b, alpha);
}

} // namespace Match3::Systems
