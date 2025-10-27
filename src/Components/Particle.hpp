#pragma once

#include <cstdint>

namespace Match3::Components {

/**
 * @brief 粒子组件 - 标记实体为粒子
 */
struct Particle {
    float size = 5.0f;              // 粒子大小
    float gravity = 500.0f;         // 重力加速度（向下）
    
    Particle() = default;
    explicit Particle(float size, float gravity = 500.0f)
        : size(size), gravity(gravity) {}
};

/**
 * @brief 爆炸效果标记组件 - 用于触发粒子爆炸效果
 */
struct ExplosionEffect {
    int particleCount = 20;         // 粒子数量
    float spreadSpeed = 150.0f;     // 扩散速度
    bool triggered = false;         // 是否已触发
    
    ExplosionEffect() = default;
    ExplosionEffect(int count, float speed)
        : particleCount(count), spreadSpeed(speed) {}
};

/**
 * @brief 闪烁效果组件 - 小星星闪烁效果
 */
struct SparkleEffect {
    int sparkleCount = 8;           // 闪烁数量
    float lifetime = 0.5f;          // 生命周期
    bool triggered = false;
    
    SparkleEffect() = default;
    SparkleEffect(int count, float lifetime)
        : sparkleCount(count), lifetime(lifetime) {}
};

} // namespace Match3::Components
