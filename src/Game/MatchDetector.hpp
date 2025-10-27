#pragma once

#include "Gem.hpp"
#include <vector>
#include <utility>

namespace Match3
{
    /**
     * @brief 匹配信息结构
     */
    struct Match
    {
        std::vector<std::pair<int, int>> positions; // 匹配的宝石位置 (row, col)
        GemType type; // 匹配的宝石类型
        bool isHorizontal; // 是否为横向匹配

        Match(GemType gemType, bool horizontal)
            : type(gemType), isHorizontal(horizontal)
        {
        }
    };

    /**
     * @brief 匹配检测器 - 检测棋盘上的匹配
     */
    class MatchDetector
    {
    public:
        /**
         * @brief 检测所有匹配
         * @param board 棋盘数据（二维数组）
         * @param rows 行数
         * @param cols 列数
         * @return 所有匹配的列表
         */
        static std::vector<Match> DetectMatches(
            const std::vector<std::vector<Gem>>& board,
            int rows,
            int cols
        );

        /**
         * @brief 检测指定位置是否有匹配
         * @param board 棋盘数据
         * @param row 行位置
         * @param col 列位置
         * @param rows 总行数
         * @param cols 总列数
         * @return 是否有匹配
         */
        static bool HasMatchAt(
            const std::vector<std::vector<Gem>>& board,
            int row,
            int col,
            int rows,
            int cols
        );

    private:
        /**
         * @brief 检测横向匹配
         */
        static std::vector<Match> DetectHorizontalMatches(
            const std::vector<std::vector<Gem>>& board,
            int rows,
            int cols
        );

        /**
         * @brief 检测纵向匹配
         */
        static std::vector<Match> DetectVerticalMatches(
            const std::vector<std::vector<Gem>>& board,
            int rows,
            int cols
        );
    };
} // namespace Match3

