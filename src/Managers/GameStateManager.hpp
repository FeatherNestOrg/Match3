#pragma once

#include <entt/entt.hpp>
#include <memory>
#include "Managers/SystemManager.hpp"
#include "Factories/EntityFactory.hpp"
#include "Systems/BoardSystem.hpp"
#include "Systems/MatchDetectionSystem.hpp"
#include "Systems/SwapSystem.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Render/Renderer.hpp"

namespace Match3
{
    /**
     * @brief 游戏状态枚举
     */
    enum class ECSPlayState
    {
        Idle, // 等待玩家输入
        Swapping, // 交换动画中
        Matching, // 检测匹配
        Eliminating, // 消除动画
        Falling, // 下落中
        Filling, // 填充新宝石
        GameOver, // 游戏结束
        Paused // 暂停
    };

    /**
     * @brief ECS游戏状态管理器
     *
     * 职责：
     * - 管理游戏状态机
     * - 协调所有ECS系统
     * - 处理游戏逻辑流程
     * - 管理计分和连击
     */
    class GameStateManager
    {
    public:
        explicit GameStateManager(Renderer* renderer);
        ~GameStateManager();

        /**
         * @brief 初始化游戏
         * @param rows 棋盘行数
         * @param cols 棋盘列数
         * @param gemTypes 宝石类型数
         */
        void Initialize(int rows, int cols, int gemTypes);

        /**
         * @brief 更新游戏逻辑
         * @param deltaTime 帧时间差
         */
        void Update(float deltaTime);

        /**
         * @brief 渲染游戏
         */
        void Render();

        /**
         * @brief 处理玩家点击
         * @param row 行
         * @param col 列
         */
        void HandleClick(int row, int col);

        /**
         * @brief 开始新游戏
         */
        void StartNewGame();

        /**
         * @brief 重置游戏
         */
        void Reset();

        // Getters
        [[nodiscard]] ECSPlayState GetPlayState() const { return m_currentState; }
        [[nodiscard]] int GetScore() const { return m_score; }
        [[nodiscard]] int GetMoves() const { return m_moves; }
        [[nodiscard]] int GetCombo() const { return m_combo; }
        [[nodiscard]] entt::registry& GetRegistry() { return m_registry; }

    private:
        // ECS核心
        entt::registry m_registry;
        std::unique_ptr<EntityFactory> m_factory;
        std::unique_ptr<SystemManager> m_systemManager;

        // 系统引用（快速访问）
        Systems::BoardSystem* m_boardSystem = nullptr;
        Systems::MatchDetectionSystem* m_matchSystem = nullptr;
        Systems::SwapSystem* m_swapSystem = nullptr;
        Systems::AnimationSystem* m_animSystem = nullptr;
        Systems::RenderSystem* m_renderSystem = nullptr;

        // 游戏状态
        ECSPlayState m_currentState = ECSPlayState::Idle;
        float m_stateTimer = 0.0f;

        // 游戏数据
        int m_rows = 8;
        int m_cols = 8;
        int m_gemTypes = 6;
        int m_score = 0;
        int m_moves = 0;
        int m_combo = 0;

        // 选择状态
        int m_selectedRow = -1;
        int m_selectedCol = -1;
        bool m_hasSelection = false;

        // 渲染器
        Renderer* m_renderer = nullptr;

        // 状态更新函数
        void UpdateIdleState(float dt);
        void UpdateSwappingState(float dt);
        void UpdateMatchingState(float dt);
        void UpdateEliminatingState(float dt);
        void UpdateFallingState(float dt);
        void UpdateFillingState(float dt);

        // 辅助函数
        void SetState(ECSPlayState newState);
        void SelectGem(int row, int col);
        void ClearSelection();
        void ClearAllSelectionAnimations(); // 清除所有选中动画
        void TrySwap(int row, int col);
        void ProcessMatches();
        void AddScore(int matchCount);

        // 回调
        void OnSwapComplete(bool valid);
    };
} // namespace Match3
