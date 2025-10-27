#pragma once

#include <entt/entt.hpp>
#include <vector>
#include "Components/Common.hpp"
#include "Components/Gem.hpp"
#include "Components/Animation.hpp"
#include "Components/Particle.hpp"

namespace Match3 {

/**
 * @brief 实体工厂 - 集中管理实体创建
 * 
 * 使用工厂模式创建各种实体，确保组件组合的一致性。
 */
class EntityFactory {
public:
    explicit EntityFactory(entt::registry& registry);
    
    /**
     * @brief 创建宝石实体
     * @param row 行位置
     * @param col 列位置
     * @param type 宝石类型
     * @return 创建的实体
     */
    entt::entity CreateGem(int row, int col, Components::GemType type);
    
    /**
     * @brief 创建粒子实体
     * @param x 世界坐标X
     * @param y 世界坐标Y
     * @param vx 速度X
     * @param vy 速度Y
     * @param r, g, b, a 颜色
     * @param size 粒子大小
     * @param lifetime 生命周期
     * @return 创建的实体
     */
    entt::entity CreateParticle(float x, float y, 
                                 float vx, float vy,
                                 uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255,
                                 float size = 5.0f,
                                 float lifetime = 1.0f);
    
    /**
     * @brief 创建爆炸效果（多个粒子）
     * @param x 中心X坐标
     * @param y 中心Y坐标
     * @param r, g, b 颜色
     * @param particleCount 粒子数量
     * @param spreadSpeed 扩散速度
     */
    void CreateExplosion(float x, float y,
                         uint8_t r, uint8_t g, uint8_t b,
                         int particleCount = 20,
                         float spreadSpeed = 150.0f);
    
    /**
     * @brief 批量创建棋盘宝石
     * @param rows 行数
     * @param cols 列数
     * @param gemTypes 宝石类型数量
     * @return 创建的所有实体
     */
    std::vector<entt::entity> CreateBoard(int rows, int cols, int gemTypes);
    
private:
    entt::registry& m_registry;
};

} // namespace Match3
