#include "SwapSystem.hpp"
#include "Core/Logger.hpp"
#include <cmath>

namespace Match3::Systems {

SwapSystem::SwapSystem(BoardSystem& boardSystem, MatchDetectionSystem& matchSystem)
    : m_boardSystem(boardSystem), m_matchSystem(matchSystem)
{
}

void SwapSystem::Update(entt::registry& registry, float deltaTime)
{
    if (!m_enabled) return;
    
    // 检查待处理的交换
    auto it = m_pendingSwaps.begin();
    while (it != m_pendingSwaps.end()) {
        auto& request = *it;
        
        // 如果动画还未开始，开始动画
        if (!request.animationStarted) {
            StartSwapAnimation(registry, request);
            request.animationStarted = true;
        }
        
        // 检查动画是否完成
        if (IsSwapAnimationComplete(registry, request)) {
            if (request.needsRevert) {
                // 回退无效交换
                LOG_DEBUG("{}: Reverting invalid swap", GetName());
                RevertSwap(registry, request);
            }
            
            // 通知完成
            if (m_swapCallback) {
                m_swapCallback(!request.needsRevert);
            }
            
            // 移除请求
            it = m_pendingSwaps.erase(it);
        } else {
            ++it;
        }
    }
}

bool SwapSystem::RequestSwap(entt::registry& registry, 
                              int row1, int col1, int row2, int col2)
{
    // 检查位置有效性
    if (!m_boardSystem.IsValidPosition(row1, col1) || 
        !m_boardSystem.IsValidPosition(row2, col2)) {
        LOG_WARN("{}: Invalid swap positions ({},{}) <-> ({},{})", 
                 GetName(), row1, col1, row2, col2);
        return false;
    }
    
    // 检查是否相邻
    const int rowDiff = std::abs(row1 - row2);
    const int colDiff = std::abs(col1 - col2);
    if (!((rowDiff == 1 && colDiff == 0) || (rowDiff == 0 && colDiff == 1))) {
        LOG_DEBUG("{}: Gems are not adjacent", GetName());
        return false;
    }
    
    // 检查是否有正在进行的交换
    if (HasPendingSwaps()) {
        LOG_DEBUG("{}: Swap already in progress", GetName());
        return false;
    }
    
    // 获取宝石实体
    auto gem1 = m_boardSystem.GetGemAt(row1, col1);
    auto gem2 = m_boardSystem.GetGemAt(row2, col2);
    
    if (gem1 == entt::null || gem2 == entt::null) {
        LOG_WARN("{}: One or both gems are null", GetName());
        return false;
    }
    
    // 执行交换（临时）
    m_boardSystem.SwapGems(registry, row1, col1, row2, col2);
    
    // 检查交换是否有效
    const bool valid = IsValidSwap(registry, row1, col1, row2, col2);
    
    // 如果无效，标记需要回退
    SwapRequest request(gem1, gem2, row1, col1, row2, col2, Config::SWAP_DURATION);
    request.needsRevert = !valid;
    
    m_pendingSwaps.push_back(request);
    
    LOG_INFO("{}: Swap requested: ({},{}) <-> ({},{}), valid={}", 
             GetName(), row1, col1, row2, col2, valid);
    
    return true;
}

bool SwapSystem::IsValidSwap(entt::registry& registry, 
                              int row1, int col1, int row2, int col2)
{
    // 检测交换后是否有匹配
    auto matches = m_matchSystem.DetectMatches(registry);
    
    // 只要有任何匹配，交换就有效
    return !matches.empty();
}

void SwapSystem::StartSwapAnimation(entt::registry& registry, SwapRequest& request)
{
    // 获取两个宝石的位置组件
    if (!registry.valid(request.gem1) || !registry.valid(request.gem2)) {
        LOG_WARN("{}: Invalid gems in swap request", GetName());
        return;
    }
    
    auto& pos1 = registry.get<Components::Position>(request.gem1);
    auto& pos2 = registry.get<Components::Position>(request.gem2);
    
    // 为两个宝石添加补间动画（使用emplace_or_replace避免崩溃）
    registry.emplace_or_replace<Components::TweenAnimation>(request.gem1,
        request.duration,
        pos1.x, pos1.y,
        pos2.x, pos2.y,
        Components::EasingType::InOutQuad);
    
    registry.emplace_or_replace<Components::TweenAnimation>(request.gem2,
        request.duration,
        pos2.x, pos2.y,
        pos1.x, pos1.y,
        Components::EasingType::InOutQuad);
    
    // 更新Gem状态
    if (registry.all_of<Components::Gem>(request.gem1)) {
        auto& gem1 = registry.get<Components::Gem>(request.gem1);
        gem1.state = Components::GemState::Swapping;
    }
    
    if (registry.all_of<Components::Gem>(request.gem2)) {
        auto& gem2 = registry.get<Components::Gem>(request.gem2);
        gem2.state = Components::GemState::Swapping;
    }
    
    LOG_DEBUG("{}: Started swap animation", GetName());
}

void SwapSystem::RevertSwap(entt::registry& registry, const SwapRequest& request)
{
    // 注意：宝石已经在棋盘上交换过了
    // 需要再次交换回来，并播放回退动画
    
    // 交换回棋盘
    m_boardSystem.SwapGems(registry, request.row2, request.col2, 
                           request.row1, request.col1);
    
    // 获取当前位置
    if (!registry.valid(request.gem1) || !registry.valid(request.gem2)) {
        return;
    }
    
    auto& pos1 = registry.get<Components::Position>(request.gem1);
    auto& pos2 = registry.get<Components::Position>(request.gem2);
    
    // 添加回退动画（位置交换回去）（使用emplace_or_replace避免崩溃）
    registry.emplace_or_replace<Components::TweenAnimation>(request.gem1,
        request.duration * 0.5f, // 回退动画更快
        pos1.x, pos1.y,
        pos2.x, pos2.y,
        Components::EasingType::InOutQuad);
    
    registry.emplace_or_replace<Components::TweenAnimation>(request.gem2,
        request.duration * 0.5f,
        pos2.x, pos2.y,
        pos1.x, pos1.y,
        Components::EasingType::InOutQuad);
    
    // 恢复状态
    if (registry.all_of<Components::Gem>(request.gem1)) {
        auto& gem1 = registry.get<Components::Gem>(request.gem1);
        gem1.state = Components::GemState::Idle;
    }
    
    if (registry.all_of<Components::Gem>(request.gem2)) {
        auto& gem2 = registry.get<Components::Gem>(request.gem2);
        gem2.state = Components::GemState::Idle;
    }
    
    LOG_DEBUG("{}: Reverted swap", GetName());
}

bool SwapSystem::IsSwapAnimationComplete(entt::registry& registry, 
                                          const SwapRequest& request)
{
    // 检查两个宝石是否都没有TweenAnimation组件
    const bool gem1Complete = !registry.all_of<Components::TweenAnimation>(request.gem1);
    const bool gem2Complete = !registry.all_of<Components::TweenAnimation>(request.gem2);
    
    return gem1Complete && gem2Complete;
}

} // namespace Match3::Systems
