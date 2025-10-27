#pragma once

#include <cstdint>

namespace Match3::Components {

/**
 * @brief 位置组件 - 世界坐标（像素单位）
 */
struct Position {
    float x = 0.0f;
    float y = 0.0f;
    
    Position() = default;
    Position(float x, float y) : x(x), y(y) {}
};

/**
 * @brief 网格位置组件 - 逻辑坐标（棋盘行列）
 */
struct GridPosition {
    int row = 0;
    int col = 0;
    
    GridPosition() = default;
    GridPosition(int row, int col) : row(row), col(col) {}
};

/**
 * @brief 速度组件
 */
struct Velocity {
    float vx = 0.0f;
    float vy = 0.0f;
    
    Velocity() = default;
    Velocity(float vx, float vy) : vx(vx), vy(vy) {}
};

/**
 * @brief 渲染组件 - 视觉属性
 */
struct Renderable {
    int radius = 30;            // 半径（像素）
    uint8_t r = 255;            // 颜色 Red
    uint8_t g = 255;            // 颜色 Green
    uint8_t b = 255;            // 颜色 Blue
    uint8_t a = 255;            // 透明度 Alpha
    float scale = 1.0f;         // 缩放比例
    float rotation = 0.0f;      // 旋转角度（弧度）
    
    Renderable() = default;
    Renderable(int radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : radius(radius), r(r), g(g), b(b), a(a) {}
};

/**
 * @brief 生命周期组件 - 用于临时实体（如粒子）
 */
struct Lifetime {
    float duration = 1.0f;      // 总持续时间
    float elapsed = 0.0f;       // 已经过时间
    bool alive = true;          // 是否存活
    
    Lifetime() = default;
    explicit Lifetime(float duration) : duration(duration) {}
    
    // 更新生命周期
    void Update(float dt) {
        elapsed += dt;
        if (elapsed >= duration) {
            alive = false;
        }
    }
    
    // 获取归一化的生命周期进度 [0, 1]
    [[nodiscard]] float GetProgress() const {
        return elapsed / duration;
    }
    
    // 检查是否过期
    [[nodiscard]] bool IsExpired() const {
        return !alive || elapsed >= duration;
    }
};

} // namespace Match3::Components
