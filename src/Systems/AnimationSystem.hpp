#pragma once

#include "System.hpp"
#include "../Components/Common.hpp"
#include "../Components/Animation.hpp"
#include <cmath>

namespace Match3::Systems {

/**
 * @brief 动画系统 - 处理所有动画组件
 * 
 * 支持的动画类型：
 * - TweenAnimation: 位置补间动画
 * - ScaleAnimation: 缩放动画
 * - FadeAnimation: 淡出动画
 * - RotationAnimation: 旋转动画
 * - PulseAnimation: 脉冲动画（循环）
 */
class AnimationSystem : public System {
public:
    AnimationSystem() = default;
    
    void Update(entt::registry& registry, float deltaTime) override;
    
    [[nodiscard]] const char* GetName() const override { return "AnimationSystem"; }
    
private:
    // 各种动画更新函数
    void UpdateTweenAnimations(entt::registry& registry, float dt);
    void UpdateScaleAnimations(entt::registry& registry, float dt);
    void UpdateFadeAnimations(entt::registry& registry, float dt);
    void UpdateRotationAnimations(entt::registry& registry, float dt);
    void UpdatePulseAnimations(entt::registry& registry, float dt);
    
    // 应用缓动函数
    [[nodiscard]] float ApplyEasing(float t, Components::EasingType type) const;
    
    // 线性插值
    [[nodiscard]] float Lerp(float a, float b, float t) const {
        return a + (b - a) * t;
    }
};

} // namespace Match3::Systems
