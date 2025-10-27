#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <vector>
#include "Systems/System.hpp"

namespace Match3 {

/**
 * @brief 系统管理器 - 统一管理和更新所有ECS系统
 * 
 * 职责：
 * - 注册和管理所有系统
 * - 按固定顺序更新系统
 * - 提供系统查询接口
 */
class SystemManager {
public:
    SystemManager() = default;
    
    /**
     * @brief 添加系统（用于Update循环）
     * @tparam T 系统类型
     * @param system 系统实例（智能指针）
     */
    template<typename T>
    void AddSystem(std::shared_ptr<T> system) {
        static_assert(std::is_base_of_v<Systems::System, T>, "T must inherit from System");
        m_systems.push_back(std::static_pointer_cast<Systems::System>(system));
    }
    
    /**
     * @brief 添加系统但不参与UpdateAll（仅保持生命周期）
     * @tparam T 系统类型
     * @param system 系统实例（智能指针）
     */
    template<typename T>
    void AddSystemNoUpdate(std::shared_ptr<T> system) {
        static_assert(std::is_base_of_v<Systems::System, T>, "T must inherit from System");
        m_managedSystems.push_back(std::static_pointer_cast<Systems::System>(system));
    }
    
    /**
     * @brief 更新所有已启用的系统
     * @param registry ECS注册表
     * @param deltaTime 帧时间差
     */
    void UpdateAll(entt::registry& registry, float deltaTime);
    
    /**
     * @brief 获取系统
     * @tparam T 系统类型
     * @return 系统指针，如果不存在返回nullptr
     */
    template<typename T>
    std::shared_ptr<T> GetSystem() {
        for (auto& system : m_systems) {
            if (auto casted = std::dynamic_pointer_cast<T>(system)) {
                return casted;
            }
        }
        return nullptr;
    }
    
    /**
     * @brief 启用/禁用所有系统
     */
    void SetAllEnabled(bool enabled);
    
    /**
     * @brief 获取系统数量
     */
    [[nodiscard]] size_t GetSystemCount() const { return m_systems.size(); }
    
    /**
     * @brief 清空所有系统
     */
    void Clear() { 
        m_systems.clear(); 
        m_managedSystems.clear();
    }
    
private:
    std::vector<std::shared_ptr<Systems::System>> m_systems;           // 参与UpdateAll的系统
    std::vector<std::shared_ptr<Systems::System>> m_managedSystems;   // 仅管理生命周期的系统
};

} // namespace Match3
