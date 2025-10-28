#include "PlatformDisplay.hpp"
#include "Core/Logger.hpp"
#include <SDL3/SDL.h>

#ifdef __ANDROID__
#include <jni.h>
#include <android/native_window_jni.h>
#endif

namespace Match3::Display
{
    std::pair<int, int> PlatformDisplay::GetUsableScreenSize()
    {
#ifdef __ANDROID__
        return GetAndroidUsableScreenSize();
#else
        // 桌面平台：使用 SDL 的显示边界
        SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
        SDL_Rect bounds;
        
        if (SDL_GetDisplayUsableBounds(primaryDisplay, &bounds))
        {
            return {bounds.w, bounds.h};
        }
        else
        {
            LOG_WARN("Failed to get usable display bounds: {}", SDL_GetError());
            // 回退到全屏尺寸
            return GetFullScreenSize();
        }
#endif
    }

    std::pair<int, int> PlatformDisplay::GetFullScreenSize()
    {
        SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
        SDL_Rect bounds;
        
        if (SDL_GetDisplayBounds(primaryDisplay, &bounds))
        {
            return {bounds.w, bounds.h};
        }
        else
        {
            LOG_ERROR("Failed to get display bounds: {}", SDL_GetError());
            // 回退到默认分辨率
            return {1920, 1080};
        }
    }

    bool PlatformDisplay::IsMobileDevice()
    {
#if defined(__ANDROID__) || defined(__IPHONEOS__) || defined(__IOS__)
        return true;
#else
        return false;
#endif
    }

    float PlatformDisplay::GetDeviceDPI()
    {
#ifdef __ANDROID__
        return GetAndroidDPI();
#else
        SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
        float ddpi = 0.0f;
        
        if (SDL_GetDisplayContentScale(primaryDisplay) > 0.0f)
        {
            // SDL3 使用内容缩放而不是 DPI
            float scale = SDL_GetDisplayContentScale(primaryDisplay);
            ddpi = 96.0f * scale; // 假设基准 DPI 为 96
        }
        else
        {
            LOG_WARN("Failed to get display content scale");
            ddpi = 96.0f; // 默认 DPI
        }
        
        return ddpi;
#endif
    }

    const char* PlatformDisplay::GetDensityCategory()
    {
        float dpi = GetDeviceDPI();
        
        // Android 密度类别
        if (dpi < 120)
            return "ldpi";
        else if (dpi < 160)
            return "mdpi";
        else if (dpi < 240)
            return "hdpi";
        else if (dpi < 320)
            return "xhdpi";
        else if (dpi < 480)
            return "xxhdpi";
        else
            return "xxxhdpi";
    }

    bool PlatformDisplay::SupportsOrientation(bool landscape)
    {
#ifdef __ANDROID__
        // Android 上，我们可以检查当前显示方向
        SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
        SDL_Rect bounds;
        
        if (SDL_GetDisplayBounds(primaryDisplay, &bounds))
        {
            bool isLandscape = bounds.w > bounds.h;
            return landscape ? isLandscape : !isLandscape;
        }
        
        // 如果无法确定，假设都支持
        return true;
#else
        // 桌面平台支持所有方向
        return true;
#endif
    }

    SDL_Rect PlatformDisplay::GetSafeArea()
    {
        // 获取可用区域作为安全区域
        auto [width, height] = GetUsableScreenSize();
        
        SDL_Rect safeArea{0, 0, width, height};
        
#ifdef __ANDROID__
        // Android 上可能需要额外处理刘海屏、导航栏等
        // 这里返回可用屏幕区域
        // 实际应用中可能需要通过 JNI 调用 Android API 获取更精确的安全区域
        SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
        SDL_Rect usableBounds;
        
        if (SDL_GetDisplayUsableBounds(primaryDisplay, &usableBounds))
        {
            SDL_Rect fullBounds;
            if (SDL_GetDisplayBounds(primaryDisplay, &fullBounds))
            {
                // 计算偏移（导航栏、状态栏等）
                safeArea.x = usableBounds.x - fullBounds.x;
                safeArea.y = usableBounds.y - fullBounds.y;
                safeArea.w = usableBounds.w;
                safeArea.h = usableBounds.h;
            }
        }
#endif
        
        return safeArea;
    }

#ifdef __ANDROID__
    std::pair<int, int> PlatformDisplay::GetAndroidUsableScreenSize()
    {
        // 在 Android 上，SDL3 会自动处理大部分情况
        // 使用 SDL_GetDisplayUsableBounds 获取可用区域
        SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
        SDL_Rect bounds;
        
        if (SDL_GetDisplayUsableBounds(primaryDisplay, &bounds))
        {
            LOG_INFO("Android usable screen size: {}x{}", bounds.w, bounds.h);
            return {bounds.w, bounds.h};
        }
        
        // 回退方案：使用完整屏幕尺寸
        LOG_WARN("Failed to get usable bounds on Android, using full screen");
        return GetFullScreenSize();
    }

    float PlatformDisplay::GetAndroidDPI()
    {
        SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
        float scale = SDL_GetDisplayContentScale(primaryDisplay);
        
        if (scale > 0.0f)
        {
            // Android 通常基于 160 DPI
            float dpi = 160.0f * scale;
            LOG_INFO("Android DPI: {} (scale: {})", dpi, scale);
            return dpi;
        }
        
        LOG_WARN("Failed to get Android DPI, using default 160");
        return 160.0f;
    }
#endif

} // namespace Match3::Display
