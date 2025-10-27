#pragma once

#include "System.hpp"
#include "BoardSystem.hpp"
#include "../Components/Gem.hpp"
#include <vector>

namespace Match3::Systems {

/**
 * @brief 匹配信息结构
 */
struct MatchGroup {
    std::vector<entt::entity> gems;  // 匹配的宝石实体
    int matchId;                      // 匹配组ID
    Components::GemType type;         // 宝石类型
    
    MatchGroup() : matchId(0), type(Components::GemType::Empty) {}
    MatchGroup(int id, Components::GemType t) : matchId(id), type(t) {}
};

/**
 * @brief 匹配检测系统 - 检测棋盘上的匹配
 * 
 * 职责：
 * - 检测横向匹配（3个或更多相同类型）
 * - 检测纵向匹配（3个或更多相同类型）
 * - 标记匹配的宝石
 * - 提供匹配信息
 */
class MatchDetectionSystem : public System {
public:
    explicit MatchDetectionSystem(BoardSystem& boardSystem);
    
    void Update(entt::registry& registry, float deltaTime) override;
    
    [[nodiscard]] const char* GetName() const override { return "MatchDetectionSystem"; }
    
    /**
     * @brief 检测所有匹配
     * @param registry ECS注册表
     * @return 所有匹配组
     */
    [[nodiscard]] std::vector<MatchGroup> DetectMatches(entt::registry& registry);
    
    /**
     * @brief 标记匹配的宝石（添加Matched组件）
     * @param registry ECS注册表
     * @param matches 匹配组列表
     */
    void MarkMatches(entt::registry& registry, const std::vector<MatchGroup>& matches);
    
    /**
     * @brief 移除匹配标记（移除Matched组件）
     * @param registry ECS注册表
     */
    void ClearMatchMarks(entt::registry& registry);
    
private:
    BoardSystem& m_boardSystem;
    int m_nextMatchId = 1;
    
    // 检测单个方向的匹配
    void DetectHorizontalMatches(entt::registry& registry, 
                                  std::vector<MatchGroup>& matches);
    void DetectVerticalMatches(entt::registry& registry, 
                               std::vector<MatchGroup>& matches);
    
    // 检查实体是否可以匹配
    [[nodiscard]] bool CanMatch(entt::registry& registry, entt::entity entity) const;
    
    // 获取宝石类型
    [[nodiscard]] Components::GemType GetGemType(entt::registry& registry, 
                                                   entt::entity entity) const;
};

} // namespace Match3::Systems
