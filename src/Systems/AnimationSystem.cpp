#include "AnimationSystem.hpp"
#include "Core/Logger.hpp"
#include "Utils/Easing.hpp"

namespace Match3::Systems {

void AnimationSystem::Update(entt::registry& registry, float deltaTime)
{
    if (!m_enabled) return;
    
    UpdateTweenAnimations(registry, deltaTime);
    UpdateScaleAnimations(registry, deltaTime);
    UpdateFadeAnimations(registry, deltaTime);
    UpdateRotationAnimations(registry, deltaTime);
    UpdatePulseAnimations(registry, deltaTime);
}

void AnimationSystem::UpdateTweenAnimations(entt::registry& registry, float dt)
{
    auto view = registry.view<Components::Position, Components::TweenAnimation>();
    
    std::vector<entt::entity> finishedAnims;
    
    for (auto entity : view) {
        auto& pos = view.get<Components::Position>(entity);
        auto& anim = view.get<Components::TweenAnimation>(entity);
        
        if (anim.finished) continue;
        
        anim.elapsed += dt;
        
        if (anim.elapsed >= anim.duration) {
            // 动画完成
            pos.x = anim.endX;
            pos.y = anim.endY;
            anim.finished = true;
            finishedAnims.push_back(entity);
        } else {
            // 计算插值
            const float t = ApplyEasing(anim.GetProgress(), anim.easing);
            pos.x = Lerp(anim.startX, anim.endX, t);
            pos.y = Lerp(anim.startY, anim.endY, t);
        }
    }
    
    // 移除已完成的动画组件
    for (auto entity : finishedAnims) {
        if (registry.valid(entity)) {
            registry.remove<Components::TweenAnimation>(entity);
        }
    }
}

void AnimationSystem::UpdateScaleAnimations(entt::registry& registry, float dt)
{
    auto view = registry.view<Components::Renderable, Components::ScaleAnimation>();
    
    std::vector<entt::entity> finishedAnims;
    
    for (auto entity : view) {
        auto& render = view.get<Components::Renderable>(entity);
        auto& anim = view.get<Components::ScaleAnimation>(entity);
        
        if (anim.finished) continue;
        
        anim.elapsed += dt;
        
        if (anim.elapsed >= anim.duration) {
            // 动画完成
            render.scale = anim.endScale;
            anim.finished = true;
            finishedAnims.push_back(entity);
        } else {
            // 计算插值
            const float t = ApplyEasing(anim.GetProgress(), anim.easing);
            render.scale = Lerp(anim.startScale, anim.endScale, t);
        }
    }
    
    // 移除已完成的动画组件
    for (auto entity : finishedAnims) {
        if (registry.valid(entity)) {
            registry.remove<Components::ScaleAnimation>(entity);
        }
    }
}

void AnimationSystem::UpdateFadeAnimations(entt::registry& registry, float dt)
{
    auto view = registry.view<Components::Renderable, Components::FadeAnimation>();
    
    std::vector<entt::entity> finishedAnims;
    
    for (auto entity : view) {
        auto& render = view.get<Components::Renderable>(entity);
        auto& anim = view.get<Components::FadeAnimation>(entity);
        
        if (anim.finished) continue;
        
        anim.elapsed += dt;
        
        if (anim.elapsed >= anim.duration) {
            // 动画完成
            const float alpha = anim.endAlpha;
            render.a = static_cast<uint8_t>(alpha * 255.0f);
            anim.finished = true;
            finishedAnims.push_back(entity);
        } else {
            // 计算插值
            const float t = ApplyEasing(anim.GetProgress(), anim.easing);
            const float alpha = Lerp(anim.startAlpha, anim.endAlpha, t);
            render.a = static_cast<uint8_t>(alpha * 255.0f);
        }
    }
    
    // 移除已完成的动画组件
    for (auto entity : finishedAnims) {
        if (registry.valid(entity)) {
            registry.remove<Components::FadeAnimation>(entity);
        }
    }
}

void AnimationSystem::UpdateRotationAnimations(entt::registry& registry, float dt)
{
    auto view = registry.view<Components::Renderable, Components::RotationAnimation>();
    
    std::vector<entt::entity> finishedAnims;
    
    for (auto entity : view) {
        auto& render = view.get<Components::Renderable>(entity);
        auto& anim = view.get<Components::RotationAnimation>(entity);
        
        if (anim.finished) continue;
        
        anim.elapsed += dt;
        
        if (anim.elapsed >= anim.duration) {
            // 动画完成
            render.rotation = anim.endRotation;
            anim.finished = true;
            finishedAnims.push_back(entity);
        } else {
            // 计算插值
            const float t = ApplyEasing(anim.GetProgress(), anim.easing);
            render.rotation = Lerp(anim.startRotation, anim.endRotation, t);
        }
    }
    
    // 移除已完成的动画组件
    for (auto entity : finishedAnims) {
        if (registry.valid(entity)) {
            registry.remove<Components::RotationAnimation>(entity);
        }
    }
}

void AnimationSystem::UpdatePulseAnimations(entt::registry& registry, float dt)
{
    auto view = registry.view<Components::Renderable, Components::PulseAnimation>();
    
    for (auto entity : view) {
        auto& render = view.get<Components::Renderable>(entity);
        auto& anim = view.get<Components::PulseAnimation>(entity);
        
        anim.time += dt;
        
        // 使用sin函数生成脉冲效果
        const float phase = anim.time * anim.frequency * 2.0f * 3.14159f;
        const float pulse = std::sin(phase) * 0.5f + 0.5f; // [0, 1]
        
        // 应用到缩放
        render.scale = 1.0f + anim.intensity * pulse;
    }
}

float AnimationSystem::ApplyEasing(float t, Components::EasingType type) const
{
    using namespace Match3::Easing;
    
    switch (type) {
        case Components::EasingType::Linear:
            return Linear(t);
        case Components::EasingType::InQuad:
            return InQuad(t);
        case Components::EasingType::OutQuad:
            return OutQuad(t);
        case Components::EasingType::InOutQuad:
            return InOutQuad(t);
        case Components::EasingType::InCubic:
            return InCubic(t);
        case Components::EasingType::OutCubic:
            return OutCubic(t);
        case Components::EasingType::InOutCubic:
            return InOutCubic(t);
        case Components::EasingType::OutBounce:
            return OutBounce(t);
        case Components::EasingType::OutBack:
            return OutBack(t);
        default:
            return t;
    }
}

} // namespace Match3::Systems
