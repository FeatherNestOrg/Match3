#pragma once

#include "DisplayMode.hpp"
#include <SDL3/SDL.h>
#include <array>
#include <vector>
#include <utility>

namespace Match3::Display
{
    /**
     * @brief 分辨率管理器 - 负责检测和管理可用的屏幕分辨率
     */
    class ResolutionManager
    {
    public:
        // 预定义分辨率列表
        static constexpr std::array<Resolution, 8> PRESET_RESOLUTIONS = {{
            {800, 600, "800x600"},
            {1024, 768, "1024x768"},
            {1280, 720, "1280x720 (HD)"},
            {1366, 768, "1366x768"},
            {1920, 1080, "1920x1080 (FHD)"},
            {2560, 1440, "2560x1440 (2K)"},
            {3840, 2160, "3840x2160 (4K)"},
            {1024, 1024, "Square 1024"}
        }};

        // 支持的宽高比
        static constexpr std::array<float, 4> ASPECT_RATIOS = {{
            4.0f / 3.0f,   // 1.333
            16.0f / 10.0f, // 1.6
            16.0f / 9.0f,  // 1.778
            1.0f           // 1:1 方形
        }};

        explicit ResolutionManager(SDL_Window* window);

        /**
         * @brief 获取显示器支持的所有显示模式
         * @return 可用的分辨率列表
         */
        std::vector<Resolution> GetAvailableDisplayModes();

        /**
         * @brief 获取当前显示信息
         * @return 当前显示信息结构体
         */
        DisplayInfo GetCurrentDisplayInfo();

        /**
         * @brief 检查指定分辨率是否被支持
         * @param width 宽度
         * @param height 高度
         * @return 支持返回 true
         */
        bool IsResolutionSupported(int width, int height);

        /**
         * @brief 查找最接近目标宽高比的分辨率
         * @param targetAspect 目标宽高比
         * @return 最接近的分辨率指针，找不到返回 nullptr
         */
        const Resolution* FindClosestResolution(float targetAspect);

        /**
         * @brief 获取 DPI 缩放因子
         * @return DPI 缩放因子
         */
        float GetDPIScale();

        /**
         * @brief 应用 DPI 缩放到指定尺寸
         * @param width 宽度
         * @param height 高度
         * @return 缩放后的宽度和高度
         */
        std::pair<int, int> ApplyDPIScaling(int width, int height);

        /**
         * @brief 获取主显示器的原生分辨率
         * @return 原生分辨率
         */
        Resolution GetNativeResolution();

        /**
         * @brief 获取显示器的物理尺寸（像素）
         * @return 宽度和高度
         */
        std::pair<int, int> GetDisplayBounds();

    private:
        SDL_Window* m_window;
        std::vector<const SDL_DisplayMode*> m_displayModes;
        float m_dpiScale;
        SDL_DisplayID m_currentDisplay;

        /**
         * @brief 刷新显示模式列表
         */
        void RefreshDisplayModes();

        /**
         * @brief 计算 DPI 缩放因子
         */
        void CalculateDPIScale();
    };

} // namespace Match3::Display
