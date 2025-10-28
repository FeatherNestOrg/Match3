#pragma once

#include "DisplayMode.hpp"
#include <utility>

namespace Match3::Display
{
    /**
     * @brief 平台特定的显示工具类
     */
    class PlatformDisplay
    {
    public:
        /**
         * @brief 获取设备的可用屏幕尺寸（Android 适配）
         * 在 Android 上，这会考虑系统栏、导航栏等
         * @return 宽度和高度（像素）
         */
        static std::pair<int, int> GetUsableScreenSize();

        /**
         * @brief 获取设备的完整屏幕尺寸
         * @return 宽度和高度（像素）
         */
        static std::pair<int, int> GetFullScreenSize();

        /**
         * @brief 检查是否为移动设备（Android/iOS）
         * @return 移动设备返回 true
         */
        static bool IsMobileDevice();

        /**
         * @brief 获取设备的 DPI
         * @return DPI 值
         */
        static float GetDeviceDPI();

        /**
         * @brief 获取设备的屏幕密度类别（Android）
         * @return 密度类别字符串 (ldpi, mdpi, hdpi, xhdpi, xxhdpi, xxxhdpi)
         */
        static const char* GetDensityCategory();

        /**
         * @brief 检查设备是否支持特定方向
         * @param landscape 是否检查横屏
         * @return 支持返回 true
         */
        static bool SupportsOrientation(bool landscape);

        /**
         * @brief 获取安全区域（考虑刘海屏等）
         * @return 安全区域的偏移和尺寸 (left, top, width, height)
         */
        static SDL_Rect GetSafeArea();

    private:
#ifdef __ANDROID__
        /**
         * @brief Android 特定的屏幕信息获取
         */
        static std::pair<int, int> GetAndroidUsableScreenSize();
        static float GetAndroidDPI();
#endif
    };

} // namespace Match3::Display
