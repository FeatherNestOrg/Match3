#pragma once

#include "Gem.hpp"
#include "MatchDetector.hpp"
#include <vector>
#include <utility>
#include <optional>

namespace Match3
{
    /**
     * @brief 游戏棋盘类 - 管理宝石阵列和游戏逻辑
     */
    class Board
    {
    public:
        /**
         * @brief 构造函数
         * @param rows 行数
         * @param cols 列数
         * @param gemTypes 宝石类型数量
         */
        Board(int rows, int cols, int gemTypes);

        /**
         * @brief 初始化棋盘
         * @return 成功返回 true
         */
        bool Initialize();

        /**
         * @brief 重置棋盘
         */
        void Reset();

        /**
         * @brief 获取指定位置的宝石
         */
        [[nodiscard]] const Gem& GetGem(int row, int col) const;
        [[nodiscard]] Gem& GetGem(int row, int col);

        /**
         * @brief 交换两个宝石
         * @param row1 第一个宝石的行
         * @param col1 第一个宝石的列
         * @param row2 第二个宝石的行
         * @param col2 第二个宝石的列
         * @return 是否可以交换
         */
        bool SwapGems(int row1, int col1, int row2, int col2);

        /**
         * @brief 检查两个位置是否相邻
         */
        [[nodiscard]] bool AreAdjacent(int row1, int col1, int row2, int col2) const;

        /**
         * @brief 检测所有匹配
         * @return 匹配列表
         */
        [[nodiscard]] std::vector<Match> DetectMatches() const;

        /**
         * @brief 移除匹配的宝石
         * @param matches 匹配列表
         * @return 移除的宝石数量
         */
        int RemoveMatches(const std::vector<Match>& matches);

        /**
         * @brief 应用重力（宝石下落）
         * @return 是否有宝石移动
         */
        bool ApplyGravity();

        /**
         * @brief 填充空位
         * @return 填充的宝石数量
         */
        int FillEmptySlots();

        /**
         * @brief 检查是否有可能的移动
         * @return 是否有可移动的宝石
         */
        [[nodiscard]] bool HasPossibleMoves() const;

        /**
         * @brief 获取提示（可以产生匹配的移动）
         * @return 可移动的两个位置 {row1, col1, row2, col2}，如果没有返回空
         */
        [[nodiscard]] std::optional<std::tuple<int, int, int, int>> GetHint() const;

        // Getters
        [[nodiscard]] int GetRows() const { return m_rows; }
        [[nodiscard]] int GetCols() const { return m_cols; }
        [[nodiscard]] int GetGemTypes() const { return m_gemTypes; }
        [[nodiscard]] const std::vector<std::vector<Gem>>& GetBoard() const { return m_board; }

    private:
        /**
         * @brief 生成随机宝石（确保初始没有匹配）
         */
        void GenerateGemAt(int row, int col);

        /**
         * @brief 检查交换后是否会产生匹配
         */
        [[nodiscard]] bool WouldSwapCreateMatch(int row1, int col1, int row2, int col2) const;

    private:
        int m_rows;
        int m_cols;
        int m_gemTypes;
        std::vector<std::vector<Gem>> m_board;
    };
} // namespace Match3

