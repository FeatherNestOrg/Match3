#pragma once

#include <cstdint>

namespace Match3
{
    /**
     * @brief 游戏配置常量
     */
    namespace Config
    {
        // 窗口设置
        inline constexpr int WINDOW_WIDTH = 800;
        inline constexpr int WINDOW_HEIGHT = 600;
        inline constexpr auto WINDOW_TITLE = "Match-3 Game";

        // 游戏板设置
        inline constexpr int BOARD_ROWS = 8;
        inline constexpr int BOARD_COLS = 8;
        inline constexpr int GEM_SIZE = 64; // 宝石大小（像素）
        inline constexpr int GEM_TYPES = 6; // 宝石类型数量
        inline constexpr int BOARD_OFFSET_X = 150; // 棋盘 X 偏移
        inline constexpr int BOARD_OFFSET_Y = 50; // 棋盘 Y 偏移

        // 游戏逻辑
        inline constexpr int MIN_MATCH_COUNT = 3; // 最小匹配数量
        inline constexpr float SWAP_DURATION = 0.2f; // 交换动画时长（秒）
        inline constexpr float FALL_DURATION = 0.3f; // 下落动画时长（秒）
        inline constexpr float MATCH_DELAY = 0.15f; // 匹配延迟（秒）
        inline constexpr float ELIMINATION_DURATION = 0.25f; // 消除动画时长（秒）

        // 计分
        inline constexpr int BASE_SCORE = 50; // 基础分数（每个宝石）
        inline constexpr float COMBO_MULTIPLIER = 1.5f; // 连击倍数
        inline constexpr int SPECIAL_GEM_BONUS = 100; // 特殊宝石奖励

        // 颜色定义（用于简单渲染）
        struct GemColor
        {
            uint8_t r, g, b, a;
        };

        inline constexpr GemColor GEM_COLORS[GEM_TYPES] = {
            {255, 0, 0, 255}, // 红色
            {0, 255, 0, 255}, // 绿色
            {0, 0, 255, 255}, // 蓝色
            {255, 255, 0, 255}, // 黄色
            {255, 0, 255, 255}, // 紫色
            {0, 255, 255, 255} // 青色
        };

        // 背景颜色
        inline constexpr GemColor BG_COLOR = {20, 20, 30, 255};
        inline constexpr GemColor GRID_COLOR = {60, 60, 80, 255};
        inline constexpr GemColor SELECTED_COLOR = {255, 255, 255, 200};

        // 宝石渲染效果
        inline constexpr GemColor GEM_BORDER_COLOR = {255, 255, 255, 180}; // 宝石边框颜色
        inline constexpr GemColor GEM_HIGHLIGHT_COLOR = {255, 255, 255, 150}; // 宝石高光颜色
        inline constexpr int GEM_MARGIN = 4; // 宝石与格子边缘的边距
        inline constexpr int GEM_HIGHLIGHT_OFFSET_DIVISOR = 3; // 高光偏移除数（相对于半径）
    } // namespace Config
} // namespace Match3

