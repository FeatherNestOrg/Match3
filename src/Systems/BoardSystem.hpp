#pragma once

#include "System.hpp"
#include "../Components/Common.hpp"
#include "../Components/Gem.hpp"
#include "Factories/EntityFactory.hpp"
#include "Core/Config.hpp"
#include <vector>
#include <optional>

namespace Match3::Systems {

/**
 * @brief 棋盘系统 - 管理棋盘网格和宝石实体
 * 
 * 职责：
 * - 维护网格索引（快速查找）
 * - 提供网格位置查询
 * - 宝石交换逻辑
 * - 应用重力和下落
 * - 填充空位
 */
class BoardSystem : public System {
public:
    BoardSystem(int rows, int cols, EntityFactory& factory);
    
    void Update(entt::registry& registry, float deltaTime) override;
    
    [[nodiscard]] const char* GetName() const override { return "BoardSystem"; }
    
    /**
     * @brief 初始化棋盘（创建所有宝石实体）
     * @param registry ECS注册表
     * @param gemTypes 宝石类型数量
     */
    void InitializeBoard(entt::registry& registry, int gemTypes);
    
    /**
     * @brief 重建网格索引
     * @param registry ECS注册表
     */
    void RebuildGridIndex(entt::registry& registry);
    
    /**
     * @brief 获取指定网格位置的宝石实体
     * @param row 行
     * @param col 列
     * @return 实体，如果为空返回entt::null
     */
    [[nodiscard]] entt::entity GetGemAt(int row, int col) const;
    
    /**
     * @brief 检查位置是否在棋盘范围内
     */
    [[nodiscard]] bool IsValidPosition(int row, int col) const;
    
    /**
     * @brief 检查位置是否为空
     */
    [[nodiscard]] bool IsEmpty(int row, int col) const;
    
    /**
     * @brief 交换两个宝石
     * @param registry ECS注册表
     * @param row1, col1 第一个宝石位置
     * @param row2, col2 第二个宝石位置
     */
    void SwapGems(entt::registry& registry, int row1, int col1, int row2, int col2);
    
    /**
     * @brief 应用重力（宝石下落填补空位）
     * @param registry ECS注册表
     * @return 移动的宝石数量
     */
    int ApplyGravity(entt::registry& registry);
    
    /**
     * @brief 填充空位（生成新宝石）
     * @param registry ECS注册表
     * @param gemTypes 宝石类型数量
     * @return 生成的新宝石数量
     */
    int FillEmptySlots(entt::registry& registry, int gemTypes);
    
    /**
     * @brief 获取棋盘尺寸
     */
    [[nodiscard]] int GetRows() const { return m_rows; }
    [[nodiscard]] int GetCols() const { return m_cols; }
    
private:
    int m_rows, m_cols;
    EntityFactory& m_factory;
    
    // 网格索引：grid[row][col] = entity
    std::vector<std::vector<entt::entity>> m_grid;
    
    // 初始化网格
    void InitializeGrid();
};

} // namespace Match3::Systems
