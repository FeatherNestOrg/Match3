#pragma once

#include <entt/entt.hpp>
#include <string>

namespace Match3::Systems {

/**
 * @brief 系统基类 - 所有ECS系统的基类
 * 
 * 系统负责处理拥有特定组件的实体，实现游戏逻辑。
 * 系统应该是无状态的，所有数据都存储在组件中。
 */
class System {
public:
    virtual ~System() = default;
    
    /**
     * @brief 更新系统
     * @param registry ECS注册表
     * @param deltaTime 帧时间差（秒）
     */
    virtual void Update(entt::registry& registry, float deltaTime) = 0;
    
    /**
     * @brief 系统启用时调用
     */
    virtual void OnEnable() {}
    
    /**
     * @brief 系统禁用时调用
     */
    virtual void OnDisable() {}
    
    /**
     * @brief 检查系统是否启用
     */
    [[nodiscard]] bool IsEnabled() const { return m_enabled; }
    
    /**
     * @brief 设置系统启用状态
     */
    void SetEnabled(bool enabled) {
        if (m_enabled != enabled) {
            m_enabled = enabled;
            if (m_enabled) {
                OnEnable();
            } else {
                OnDisable();
            }
        }
    }
    
    /**
     * @brief 获取系统名称（用于调试）
     */
    [[nodiscard]] virtual const char* GetName() const = 0;
    
protected:
    bool m_enabled = true;
};

} // namespace Match3::Systems
