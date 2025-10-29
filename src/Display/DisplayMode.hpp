#pragma once

#include <cstdint>

namespace Match3::Display
{
    /**
     * @brief 显示模式类型
     */
    enum class DisplayMode
    {
        WINDOWED,             // 窗口化模式
        FULLSCREEN_EXCLUSIVE, // 独占全屏
        FULLSCREEN_BORDERLESS // 无边框全屏
    };

    /**
     * @brief 缩放策略
     */
    enum class ScalingStrategy
    {
        ASPECT_RATIO_STRICT, // 保持宽高比，letterbox/pillarbox
        FILL_SCREEN,         // 填满屏幕，可能拉伸
        INTEGER_SCALE,       // 整数倍缩放（像素艺术游戏适用）
        DYNAMIC_SCALE        // 动态缩放，适配不同屏幕
    };

    /**
     * @brief 分辨率结构体
     */
    struct Resolution
    {
        int width;
        int height;
        const char* label; // "1920x1080", "1280x720", 等

        bool operator==(const Resolution& other) const
        {
            return width == other.width && height == other.height;
        }
    };

    /**
     * @brief 显示信息结构体
     */
    struct DisplayInfo
    {
        int windowWidth;      // 窗口宽度（像素）
        int windowHeight;     // 窗口高度（像素）
        int renderWidth;      // 渲染分辨率
        int renderHeight;     // 渲染分辨率高度
        int physicalWidth;    // 物理显示器宽度
        int physicalHeight;   // 物理显示器高度
        float dpiScale;       // DPI 缩放因子
        DisplayMode mode;     // 显示模式
        ScalingStrategy strategy; // 缩放策略
    };

} // namespace Match3::Display
