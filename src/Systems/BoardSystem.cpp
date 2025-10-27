#include "BoardSystem.hpp"
#include "Core/Logger.hpp"
#include <random>
#include <algorithm>

namespace Match3::Systems {

BoardSystem::BoardSystem(int rows, int cols, EntityFactory& factory)
    : m_rows(rows), m_cols(cols), m_factory(factory)
{
    InitializeGrid();
    LOG_INFO("{}: Created {}x{} board", GetName(), rows, cols);
}

void BoardSystem::Update(entt::registry& registry, float deltaTime)
{
    if (!m_enabled) return;
    
    // BoardSystem主要提供查询和操作接口
    // 实际的游戏逻辑由GameStateManager驱动
}

void BoardSystem::InitializeGrid()
{
    m_grid.resize(m_rows);
    for (int row = 0; row < m_rows; ++row) {
        m_grid[row].resize(m_cols, entt::null);
    }
}

void BoardSystem::InitializeBoard(entt::registry& registry, int gemTypes)
{
    LOG_INFO("{}: Initializing board with {} gem types", GetName(), gemTypes);
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> typeDist(0, gemTypes - 1);
    
    // 创建所有宝石实体
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            auto type = static_cast<Components::GemType>(typeDist(gen));
            auto entity = m_factory.CreateGem(row, col, type);
            m_grid[row][col] = entity;
        }
    }
    
    LOG_INFO("{}: Board initialized with {} gems", GetName(), m_rows * m_cols);
}

void BoardSystem::RebuildGridIndex(entt::registry& registry)
{
    // 清空网格
    for (auto& row : m_grid) {
        std::fill(row.begin(), row.end(), entt::null);
    }
    
    // 重新索引所有宝石
    auto view = registry.view<Components::GridPosition, Components::Gem>();
    int count = 0;
    
    for (auto entity : view) {
        const auto& gridPos = view.get<Components::GridPosition>(entity);
        const auto& gem = view.get<Components::Gem>(entity);
        
        // 跳过空宝石
        if (gem.IsEmpty()) {
            continue;
        }
        
        if (IsValidPosition(gridPos.row, gridPos.col)) {
            m_grid[gridPos.row][gridPos.col] = entity;
            ++count;
        }
    }
    
    LOG_DEBUG("{}: Rebuilt grid index with {} gems", GetName(), count);
}

entt::entity BoardSystem::GetGemAt(int row, int col) const
{
    if (!IsValidPosition(row, col)) {
        return entt::null;
    }
    return m_grid[row][col];
}

bool BoardSystem::IsValidPosition(int row, int col) const
{
    return row >= 0 && row < m_rows && col >= 0 && col < m_cols;
}

bool BoardSystem::IsEmpty(int row, int col) const
{
    return GetGemAt(row, col) == entt::null;
}

void BoardSystem::SwapGems(entt::registry& registry, int row1, int col1, int row2, int col2)
{
    if (!IsValidPosition(row1, col1) || !IsValidPosition(row2, col2)) {
        LOG_WARN("{}: Invalid swap positions ({},{}) <-> ({},{})", 
                 GetName(), row1, col1, row2, col2);
        return;
    }
    
    auto entity1 = m_grid[row1][col1];
    auto entity2 = m_grid[row2][col2];
    
    // 交换网格索引
    std::swap(m_grid[row1][col1], m_grid[row2][col2]);
    
    // 更新实体的网格位置组件
    if (entity1 != entt::null) {
        auto& gridPos1 = registry.get<Components::GridPosition>(entity1);
        gridPos1.row = row2;
        gridPos1.col = col2;
    }
    
    if (entity2 != entt::null) {
        auto& gridPos2 = registry.get<Components::GridPosition>(entity2);
        gridPos2.row = row1;
        gridPos2.col = col1;
    }
    
    LOG_DEBUG("{}: Swapped ({},{}) <-> ({},{})", GetName(), row1, col1, row2, col2);
}

int BoardSystem::ApplyGravity(entt::registry& registry)
{
    int movedCount = 0;
    
    // 从下往上，从左往右遍历
    for (int col = 0; col < m_cols; ++col) {
        // 对每一列应用重力
        for (int row = m_rows - 1; row >= 0; --row) {
            if (m_grid[row][col] != entt::null) {
                continue; // 已经有宝石
            }
            
            // 找到上方最近的非空宝石
            for (int aboveRow = row - 1; aboveRow >= 0; --aboveRow) {
                if (m_grid[aboveRow][col] != entt::null) {
                    // 移动宝石下落
                    auto entity = m_grid[aboveRow][col];
                    m_grid[row][col] = entity;
                    m_grid[aboveRow][col] = entt::null;
                    
                    // 更新网格位置组件
                    auto& gridPos = registry.get<Components::GridPosition>(entity);
                    gridPos.row = row;
                    gridPos.col = col;
                    
                    ++movedCount;
                    break;
                }
            }
        }
    }
    
    if (movedCount > 0) {
        LOG_DEBUG("{}: Applied gravity, moved {} gems", GetName(), movedCount);
    }
    
    return movedCount;
}

int BoardSystem::FillEmptySlots(entt::registry& registry, int gemTypes)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> typeDist(0, gemTypes - 1);
    
    int filledCount = 0;
    
    // 遍历棋盘，填充空位
    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            if (m_grid[row][col] == entt::null) {
                // 创建新宝石
                auto type = static_cast<Components::GemType>(typeDist(gen));
                auto entity = m_factory.CreateGem(row, col, type);
                
                // 新宝石从上方开始（用于动画）
                auto& pos = registry.get<Components::Position>(entity);
                pos.y = Config::BOARD_OFFSET_Y - Config::GEM_SIZE; // 在棋盘上方
                
                // 设置初始透明度和缩放（用于动画）
                auto& render = registry.get<Components::Renderable>(entity);
                render.a = 0;       // 完全透明
                render.scale = 0.0f; // 缩放为0
                
                m_grid[row][col] = entity;
                ++filledCount;
            }
        }
    }
    
    if (filledCount > 0) {
        LOG_DEBUG("{}: Filled {} empty slots", GetName(), filledCount);
    }
    
    return filledCount;
}

} // namespace Match3::Systems
