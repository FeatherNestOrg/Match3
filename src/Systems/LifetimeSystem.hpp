#pragma once

#include "System.hpp"
#include "../Components/Common.hpp"
#include <vector>

namespace Match3::Systems {

/**
 * @brief 生命周期系统 - 管理有Lifetime组件的实体
 * 
 * 职责：
 * - 更新所有实体的生命周期计时器
 * - 标记过期的实体
 * - 清理已死亡的实体
 */
class LifetimeSystem : public System {
public:
    LifetimeSystem() = default;
    
    void Update(entt::registry& registry, float deltaTime) override;
    
    [[nodiscard]] const char* GetName() const override { return "LifetimeSystem"; }
    
    // 获取统计信息
    [[nodiscard]] int GetCleanedCount() const { return m_cleanedThisFrame; }
    
private:
    int m_cleanedThisFrame = 0;
};

} // namespace Match3::Systems
