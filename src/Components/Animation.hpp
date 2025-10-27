#pragma once

#include <cstdint>

namespace Match3::Components {

/**
 * @brief 缓动函数类型
 */
enum class EasingType : uint8_t {
    Linear,
    InQuad,
    OutQuad,
    InOutQuad,
    InCubic,
    OutCubic,
    InOutCubic,
    OutBounce,
    OutBack
};

/**
 * @brief 补间动画组件 - 位置插值动画
 */
struct TweenAnimation {
    float duration = 0.3f;          // 动画持续时间
    float elapsed = 0.0f;           // 已经过时间
    float startX = 0.0f;            // 起始X坐标
    float startY = 0.0f;            // 起始Y坐标
    float endX = 0.0f;              // 结束X坐标
    float endY = 0.0f;              // 结束Y坐标
    EasingType easing = EasingType::Linear;
    bool finished = false;          // 是否完成
    
    TweenAnimation() = default;
    TweenAnimation(float duration, float startX, float startY, 
                   float endX, float endY, 
                   EasingType easing = EasingType::Linear)
        : duration(duration), startX(startX), startY(startY),
          endX(endX), endY(endY), easing(easing) {}
    
    // 获取归一化进度 [0, 1]
    [[nodiscard]] float GetProgress() const {
        return elapsed / duration;
    }
    
    // 检查是否完成
    [[nodiscard]] bool IsFinished() const {
        return finished || elapsed >= duration;
    }
};

/**
 * @brief 缩放动画组件
 */
struct ScaleAnimation {
    float duration = 0.3f;
    float elapsed = 0.0f;
    float startScale = 1.0f;
    float endScale = 0.0f;
    EasingType easing = EasingType::Linear;
    bool finished = false;
    
    ScaleAnimation() = default;
    ScaleAnimation(float duration, float startScale, float endScale,
                   EasingType easing = EasingType::Linear)
        : duration(duration), startScale(startScale), 
          endScale(endScale), easing(easing) {}
    
    [[nodiscard]] float GetProgress() const {
        return elapsed / duration;
    }
    
    [[nodiscard]] bool IsFinished() const {
        return finished || elapsed >= duration;
    }
};

/**
 * @brief 淡出动画组件
 */
struct FadeAnimation {
    float duration = 0.3f;
    float elapsed = 0.0f;
    float startAlpha = 1.0f;        // 起始透明度 [0, 1]
    float endAlpha = 0.0f;          // 结束透明度 [0, 1]
    EasingType easing = EasingType::Linear;
    bool finished = false;
    
    FadeAnimation() = default;
    FadeAnimation(float duration, float startAlpha, float endAlpha,
                  EasingType easing = EasingType::Linear)
        : duration(duration), startAlpha(startAlpha), 
          endAlpha(endAlpha), easing(easing) {}
    
    [[nodiscard]] float GetProgress() const {
        return elapsed / duration;
    }
    
    [[nodiscard]] bool IsFinished() const {
        return finished || elapsed >= duration;
    }
};

/**
 * @brief 旋转动画组件
 */
struct RotationAnimation {
    float duration = 0.3f;
    float elapsed = 0.0f;
    float startRotation = 0.0f;     // 起始旋转角度（弧度）
    float endRotation = 0.0f;       // 结束旋转角度（弧度）
    EasingType easing = EasingType::Linear;
    bool finished = false;
    
    RotationAnimation() = default;
    RotationAnimation(float duration, float startRotation, float endRotation,
                      EasingType easing = EasingType::Linear)
        : duration(duration), startRotation(startRotation),
          endRotation(endRotation), easing(easing) {}
    
    [[nodiscard]] float GetProgress() const {
        return elapsed / duration;
    }
    
    [[nodiscard]] bool IsFinished() const {
        return finished || elapsed >= duration;
    }
};

/**
 * @brief 脉冲动画组件 - 循环动画
 */
struct PulseAnimation {
    float frequency = 2.0f;         // 频率（Hz）
    float intensity = 0.3f;         // 强度 [0, 1]
    float time = 0.0f;              // 累积时间
    
    PulseAnimation() = default;
    PulseAnimation(float frequency, float intensity)
        : frequency(frequency), intensity(intensity) {}
};

} // namespace Match3::Components
