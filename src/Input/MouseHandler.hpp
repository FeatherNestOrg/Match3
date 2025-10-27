#pragma once

#include "Core/Config.hpp"
#include <optional>
#include <map>

namespace Match3
{
    /**
     * @brief 鼠标处理器 - 处理鼠标与游戏棋盘的交互
     */
    class MouseHandler
    {
    public:
        /**
         * @brief 屏幕坐标转换为棋盘坐标
         * @param screenX 屏幕 X 坐标
         * @param screenY 屏幕 Y 坐标
         * @return 棋盘坐标 {row, col}，如果不在棋盘范围内返回空
         */
        [[nodiscard]] static std::optional<std::pair<int, int>> ScreenToBoard(int screenX, int screenY);

        /**
         * @brief 棋盘坐标转换为屏幕坐标（中心点）
         * @param row 行
         * @param col 列
         * @return 屏幕坐标 {x, y}
         */
        [[nodiscard]] static std::pair<int, int> BoardToScreen(int row, int col);

        /**
         * @brief 获取宝石的渲染矩形
         * @param row 行
         * @param col 列
         * @return 矩形 {x, y, width, height}
         */
        [[nodiscard]] static std::tuple<int, int, int, int> GetGemRect(int row, int col);

        /**
         * @brief 检查屏幕坐标是否在棋盘范围内
         * @param screenX 屏幕 X 坐标
         * @param screenY 屏幕 Y 坐标
         * @return 是否在棋盘范围内
         */
        [[nodiscard]] static bool IsInBoardBounds(int screenX, int screenY);

        /**
         * @brief 检查棋盘坐标是否有效
         * @param row 行
         * @param col 列
         * @return 是否有效
         */
        [[nodiscard]] static bool IsValidBoardPosition(int row, int col);
    };
} // namespace Match3
