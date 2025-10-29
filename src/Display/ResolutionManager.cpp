#include "ResolutionManager.hpp"
#include "PlatformDisplay.hpp"
#include "Core/Logger.hpp"
#include <cmath>
#include <limits>
#include <algorithm>

namespace Match3::Display
{
    ResolutionManager::ResolutionManager(SDL_Window* window)
        : m_window(window)
          , m_dpiScale(1.0f)
          , m_currentDisplay()
    {
        if (m_window)
        {
            m_currentDisplay = SDL_GetDisplayForWindow(m_window);
            RefreshDisplayModes();
            CalculateDPIScale();
        }
    }

    std::vector<Resolution> ResolutionManager::GetAvailableDisplayModes()
    {
        std::vector<Resolution> resolutions;

        if (!m_window)
        {
            LOG_WARN("Window is null, returning preset resolutions only");
            resolutions.assign(PRESET_RESOLUTIONS.begin(), PRESET_RESOLUTIONS.end());
            return resolutions;
        }

        // 获取显示器支持的所有模式
        int count = 0;
        SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(m_currentDisplay, &count);

        if (modes && count > 0)
        {
            // 去重并添加到列表
            for (int i = 0; i < count; ++i)
            {
                if (modes[i])
                {
                    Resolution res{
                        modes[i]->w,
                        modes[i]->h,
                        nullptr // 稍后可以格式化
                    };

                    // 检查是否已存在
                    auto it = std::find(resolutions.begin(), resolutions.end(), res);
                    if (it == resolutions.end())
                    {
                        resolutions.push_back(res);
                    }
                }
            }
            SDL_free(modes);
        }

        // 如果没有检测到任何模式，使用预设列表
        if (resolutions.empty())
        {
            LOG_WARN("No display modes detected, using preset resolutions");
            resolutions.assign(PRESET_RESOLUTIONS.begin(), PRESET_RESOLUTIONS.end());
        }

        // 按分辨率从小到大排序
        std::sort(resolutions.begin(), resolutions.end(),
                  [](const Resolution& a, const Resolution& b)
                  {
                      return (a.width * a.height) < (b.width * b.height);
                  });

        return resolutions;
    }

    DisplayInfo ResolutionManager::GetCurrentDisplayInfo()
    {
        DisplayInfo info{};

        if (!m_window)
        {
            LOG_ERROR("Window is null in GetCurrentDisplayInfo");
            return info;
        }

        // 获取窗口尺寸
        SDL_GetWindowSize(m_window, &info.windowWidth, &info.windowHeight);

        // 获取渲染器输出尺寸（可能与窗口不同，特别是在高DPI屏幕上）
        SDL_Renderer* renderer = SDL_GetRenderer(m_window);
        if (renderer)
        {
            SDL_GetCurrentRenderOutputSize(renderer, &info.renderWidth, &info.renderHeight);
        }
        else
        {
            info.renderWidth = info.windowWidth;
            info.renderHeight = info.windowHeight;
        }

        // 获取物理显示器尺寸
        SDL_Rect bounds;
        if (SDL_GetDisplayBounds(m_currentDisplay, &bounds))
        {
            info.physicalWidth = bounds.w;
            info.physicalHeight = bounds.h;
        }
        else
        {
            LOG_WARN("Failed to get display bounds: {}", SDL_GetError());
            info.physicalWidth = info.windowWidth;
            info.physicalHeight = info.windowHeight;
        }

        info.dpiScale = m_dpiScale;

        // 检测当前显示模式
        if (SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN)
        {
            info.mode = DisplayMode::FULLSCREEN_EXCLUSIVE;
        }
        else
        {
            info.mode = DisplayMode::WINDOWED;
        }

        info.strategy = ScalingStrategy::ASPECT_RATIO_STRICT;

        return info;
    }

    bool ResolutionManager::IsResolutionSupported(int width, int height)
    {
        if (!m_window)
        {
            return false;
        }

        int count = 0;
        SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(m_currentDisplay, &count);

        if (!modes || count == 0)
        {
            SDL_free(modes);
            return false;
        }

        bool found = false;
        for (int i = 0; i < count; ++i)
        {
            if (modes[i] && modes[i]->w == width && modes[i]->h == height)
            {
                found = true;
                break;
            }
        }

        SDL_free(modes);
        return found;
    }

    const Resolution* ResolutionManager::FindClosestResolution(float targetAspect)
    {
        const Resolution* closest = nullptr;
        float minDiff = std::numeric_limits<float>::max();

        for (const auto& res : PRESET_RESOLUTIONS)
        {
            float aspect = static_cast<float>(res.width) / static_cast<float>(res.height);
            float diff = std::abs(aspect - targetAspect);

            if (diff < minDiff)
            {
                minDiff = diff;
                closest = &res;
            }
        }

        return closest;
    }

    float ResolutionManager::GetDPIScale()
    {
        return m_dpiScale;
    }

    std::pair<int, int> ResolutionManager::ApplyDPIScaling(int width, int height)
    {
        return {
            static_cast<int>(width * m_dpiScale),
            static_cast<int>(height * m_dpiScale)
        };
    }

    Resolution ResolutionManager::GetNativeResolution()
    {
        if (!m_window)
        {
            return {800, 600, "800x600 (Default)"};
        }

        const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(m_currentDisplay);
        if (mode)
        {
            return {mode->w, mode->h, "Native"};
        }

        LOG_WARN("Failed to get desktop display mode: {}", SDL_GetError());
        return {1920, 1080, "1920x1080 (Default)"};
    }

    std::pair<int, int> ResolutionManager::GetDisplayBounds()
    {
        if (!m_window)
        {
            return {800, 600};
        }

        SDL_Rect bounds;
        if (SDL_GetDisplayBounds(m_currentDisplay, &bounds))
        {
            return {bounds.w, bounds.h};
        }

        LOG_WARN("Failed to get display bounds: {}", SDL_GetError());
        return {1920, 1080};
    }

    std::pair<int, int> ResolutionManager::GetUsableScreenSize()
    {
        return PlatformDisplay::GetUsableScreenSize();
    }

    bool ResolutionManager::IsMobileDevice()
    {
        return PlatformDisplay::IsMobileDevice();
    }

    void ResolutionManager::RefreshDisplayModes()
    {
        if (!m_window)
        {
            return;
        }

        m_currentDisplay = SDL_GetDisplayForWindow(m_window);

        int count = 0;
        SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(m_currentDisplay, &count);

        m_displayModes.clear();
        if (modes && count > 0)
        {
            m_displayModes.reserve(count);
            for (int i = 0; i < count; ++i)
            {
                if (modes[i])
                {
                    m_displayModes.push_back(modes[i]);
                }
            }
            SDL_free(modes);
        }

        LOG_INFO("Found {} display modes", m_displayModes.size());
    }

    void ResolutionManager::CalculateDPIScale()
    {
        if (!m_window)
        {
            m_dpiScale = 1.0f;
            return;
        }

        // SDL3 提供了获取窗口显示缩放的函数
        float scale = SDL_GetWindowDisplayScale(m_window);

        if (scale > 0.0f)
        {
            m_dpiScale = scale;
            LOG_INFO("DPI scale detected: {}", m_dpiScale);
        }
        else
        {
            m_dpiScale = 1.0f;
            LOG_WARN("Failed to get DPI scale, using default 1.0");
        }
    }
} // namespace Match3::Display
