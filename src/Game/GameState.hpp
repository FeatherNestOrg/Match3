#pragma once

#include "Board.hpp"
#include <memory>

namespace Match3
{
    /**
     * @brief 游戏状态枚举
     */
    enum class PlayState
    {
        Idle, // 闲置（等待玩家输入）
        Swapping, // 交换动画中
        Matching, // 检测匹配中
        Eliminating, // 消除动画中
        Falling, // 下落中
        Filling, // 填充新宝石中
        GameOver, // 游戏结束
        Paused // 暂停
    };

    /**
     * @brief 游戏状态管理类
     */
    class GameState
    {
    public:
        GameState();
        ~GameState() = default;

        /**
         * @brief 初始化游戏状态
         */
        bool Initialize();

        /**
         * @brief 更新游戏逻辑
         * @param deltaTime 时间间隔（秒）
         */
        void Update(float deltaTime);

        /**
         * @brief 处理玩家点击
         * @param row 点击的行
         * @param col 点击的列
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
        [[nodiscard]] const Board& GetBoard() const { return *m_board; }
        [[nodiscard]] Board& GetBoard() { return *m_board; }
        [[nodiscard]] PlayState GetPlayState() const { return m_playState; }
        [[nodiscard]] int GetScore() const { return m_score; }
        [[nodiscard]] int GetMoves() const { return m_moves; }
        [[nodiscard]] int GetCombo() const { return m_combo; }
        [[nodiscard]] bool HasSelection() const { return m_selectedRow >= 0 && m_selectedCol >= 0; }
        [[nodiscard]] int GetSelectedRow() const { return m_selectedRow; }
        [[nodiscard]] int GetSelectedCol() const { return m_selectedCol; }

    private:
        /**
         * @brief 选择宝石
         */
        void SelectGem(int row, int col);

        /**
         * @brief 取消选择
         */
        void ClearSelection();

        /**
         * @brief 尝试交换宝石
         */
        bool TrySwap(int row, int col);

        /**
         * @brief 处理匹配和消除
         */
        void ProcessMatches();

        /**
         * @brief 更新分数
         */
        void UpdateScore(int matchedGems);

        /**
         * @brief 检查游戏是否结束
         */
        void CheckGameOver();

    private:
        std::unique_ptr<Board> m_board;
        PlayState m_playState;

        // 游戏数据
        int m_score;
        int m_moves;
        int m_combo;

        // 选中的宝石
        int m_selectedRow;
        int m_selectedCol;

        // 定时器
        float m_stateTimer;
    };
} // namespace Match3

