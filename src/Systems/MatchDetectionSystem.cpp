#include "MatchDetectionSystem.hpp"
#include "Core/Logger.hpp"
#include <numeric>

namespace Match3::Systems {

MatchDetectionSystem::MatchDetectionSystem(BoardSystem& boardSystem)
    : m_boardSystem(boardSystem)
{
}

void MatchDetectionSystem::Update(entt::registry& registry, float deltaTime)
{
    if (!m_enabled) return;
    
    // MatchDetectionSystem主要提供检测接口
    // 实际的检测由GameStateManager在需要时调用
}

std::vector<MatchGroup> MatchDetectionSystem::DetectMatches(entt::registry& registry)
{
    std::vector<MatchGroup> matches;
    
    // 检测横向匹配
    DetectHorizontalMatches(registry, matches);
    
    // 检测纵向匹配
    DetectVerticalMatches(registry, matches);
    
    if (!matches.empty()) {
        LOG_DEBUG("{}: Detected {} match groups with {} total gems",
                 GetName(), matches.size(),
                 std::accumulate(matches.begin(), matches.end(), 0,
                     [](int sum, const MatchGroup& m) { return sum + m.gems.size(); }));
    }
    
    return matches;
}

void MatchDetectionSystem::DetectHorizontalMatches(entt::registry& registry, 
                                                     std::vector<MatchGroup>& matches)
{
    const int rows = m_boardSystem.GetRows();
    const int cols = m_boardSystem.GetCols();
    
    for (int row = 0; row < rows; ++row) {
        int matchStart = 0;
        Components::GemType matchType = Components::GemType::Empty;
        
        for (int col = 0; col <= cols; ++col) {
            entt::entity currentEntity = entt::null;
            Components::GemType currentType = Components::GemType::Empty;
            
            if (col < cols) {
                currentEntity = m_boardSystem.GetGemAt(row, col);
                if (currentEntity != entt::null && CanMatch(registry, currentEntity)) {
                    currentType = GetGemType(registry, currentEntity);
                }
            }
            
            // 检查是否延续匹配
            if (currentType == matchType && matchType != Components::GemType::Empty) {
                // 继续匹配
                continue;
            } else {
                // 匹配中断，检查是否形成匹配
                const int matchLength = col - matchStart;
                if (matchLength >= 3 && matchType != Components::GemType::Empty) {
                    // 形成匹配
                    MatchGroup group(m_nextMatchId++, matchType);
                    for (int i = matchStart; i < col; ++i) {
                        auto entity = m_boardSystem.GetGemAt(row, i);
                        if (entity != entt::null) {
                            group.gems.push_back(entity);
                        }
                    }
                    matches.push_back(group);
                }
                
                // 开始新的匹配序列
                matchStart = col;
                matchType = currentType;
            }
        }
    }
}

void MatchDetectionSystem::DetectVerticalMatches(entt::registry& registry, 
                                                   std::vector<MatchGroup>& matches)
{
    const int rows = m_boardSystem.GetRows();
    const int cols = m_boardSystem.GetCols();
    
    for (int col = 0; col < cols; ++col) {
        int matchStart = 0;
        Components::GemType matchType = Components::GemType::Empty;
        
        for (int row = 0; row <= rows; ++row) {
            entt::entity currentEntity = entt::null;
            Components::GemType currentType = Components::GemType::Empty;
            
            if (row < rows) {
                currentEntity = m_boardSystem.GetGemAt(row, col);
                if (currentEntity != entt::null && CanMatch(registry, currentEntity)) {
                    currentType = GetGemType(registry, currentEntity);
                }
            }
            
            // 检查是否延续匹配
            if (currentType == matchType && matchType != Components::GemType::Empty) {
                // 继续匹配
                continue;
            } else {
                // 匹配中断，检查是否形成匹配
                const int matchLength = row - matchStart;
                if (matchLength >= 3 && matchType != Components::GemType::Empty) {
                    // 形成匹配
                    MatchGroup group(m_nextMatchId++, matchType);
                    for (int i = matchStart; i < row; ++i) {
                        auto entity = m_boardSystem.GetGemAt(i, col);
                        if (entity != entt::null) {
                            group.gems.push_back(entity);
                        }
                    }
                    matches.push_back(group);
                }
                
                // 开始新的匹配序列
                matchStart = row;
                matchType = currentType;
            }
        }
    }
}

void MatchDetectionSystem::MarkMatches(entt::registry& registry, 
                                        const std::vector<MatchGroup>& matches)
{
    int totalGems = 0;
    
    for (const auto& match : matches) {
        for (auto entity : match.gems) {
            // 添加Matched组件（使用emplace_or_replace避免崩溃）
            registry.emplace_or_replace<Components::Matched>(entity, match.matchId, match.gems.size());
            
            // 更新Gem状态
            if (registry.all_of<Components::Gem>(entity)) {
                auto& gem = registry.get<Components::Gem>(entity);
                gem.state = Components::GemState::Matched;
            }
            
            ++totalGems;
        }
    }
    
    if (totalGems > 0) {
        LOG_DEBUG("{}: Marked {} gems as matched", GetName(), totalGems);
    }
}

void MatchDetectionSystem::ClearMatchMarks(entt::registry& registry)
{
    // 移除所有Matched组件
    auto view = registry.view<Components::Matched>();
    registry.destroy(view.begin(), view.end());
}

bool MatchDetectionSystem::CanMatch(entt::registry& registry, entt::entity entity) const
{
    if (!registry.valid(entity)) {
        return false;
    }
    
    if (!registry.all_of<Components::Gem>(entity)) {
        return false;
    }
    
    const auto& gem = registry.get<Components::Gem>(entity);
    return gem.canMatch && !gem.IsEmpty();
}

Components::GemType MatchDetectionSystem::GetGemType(entt::registry& registry, 
                                                       entt::entity entity) const
{
    if (!registry.all_of<Components::Gem>(entity)) {
        return Components::GemType::Empty;
    }
    
    return registry.get<Components::Gem>(entity).type;
}

} // namespace Match3::Systems
