#pragma once

#include "System.hpp"
#include "BoardSystem.hpp"
#include "MatchDetectionSystem.hpp"
#include "../Components/Animation.hpp"
#include "Core/Config.hpp"
#include <vector>
#include <functional>

namespace Match3::Systems {

/**
 * @brief 交换请求结构
 */
struct SwapRequest {
    entt::entity gem1;
    entt::entity gem2;
    int row1, col1;
    int row2, col2;
    float duration;
    bool needsRevert = false;
    bool animationStarted = false;
    
    SwapRequest(entt::entity g1, entt::entity g2, 
                int r1, int c1, int r2, int c2, float dur = 0.3f)
        : gem1(g1), gem2(g2), row1(r1), col1(c1), row2(r2), col2(c2), duration(dur) {}
};

/**
 * @brief 宝石交换系统 - 处理玩家交换操作
 * 
 * 职责：
 * - 接收交换请求
 * - 验证交换有效性
 * - 触发交换动画
 * - 处理无效交换回退
 */
class SwapSystem : public System {
public:
    SwapSystem(BoardSystem& boardSystem, MatchDetectionSystem& matchSystem);
    
    void Update(entt::registry& registry, float deltaTime) override;
    
    [[nodiscard]] const char* GetName() const override { return "SwapSystem"; }
    
    /**
     * @brief 请求交换两个宝石
     * @param registry ECS注册表
     * @param row1, col1 第一个宝石位置
     * @param row2, col2 第二个宝石位置
     * @return 是否接受请求
     */
    bool RequestSwap(entt::registry& registry, int row1, int col1, int row2, int col2);
    
    /**
     * @brief 检查交换是否有效（会产生匹配）
     * @param registry ECS注册表
     * @param row1, col1 第一个宝石位置
     * @param row2, col2 第二个宝石位置
     * @return 是否有效
     */
    [[nodiscard]] bool IsValidSwap(entt::registry& registry, 
                                     int row1, int col1, int row2, int col2);
    
    /**
     * @brief 检查是否有正在进行的交换
     */
    [[nodiscard]] bool HasPendingSwaps() const { return !m_pendingSwaps.empty(); }
    
    /**
     * @brief 设置交换完成回调
     */
    using SwapCallback = std::function<void(bool valid)>;
    void SetSwapCallback(SwapCallback callback) { m_swapCallback = callback; }
    
private:
    BoardSystem& m_boardSystem;
    MatchDetectionSystem& m_matchSystem;
    std::vector<SwapRequest> m_pendingSwaps;
    SwapCallback m_swapCallback;
    
    // 开始交换动画
    void StartSwapAnimation(entt::registry& registry, SwapRequest& request);
    
    // 回退交换
    void RevertSwap(entt::registry& registry, const SwapRequest& request);
    
    // 检查动画是否完成
    bool IsSwapAnimationComplete(entt::registry& registry, const SwapRequest& request);
};

} // namespace Match3::Systems
