#include "DisplayManager.hpp"
#include "PlatformDisplay.hpp"
#include "Core/Logger.hpp"
#include <filesystem>

namespace Match3::Display
{
    DisplayManager::DisplayManager(SDL_Window* window, SDL_Renderer* renderer)
        : m_window(window)
        , m_renderer(renderer)
        , m_needsRedraw(false)
        , m_defaultConfigPath("display_settings.json")
    {
    }

    bool DisplayManager::Initialize()
    {
        LOG_INFO("Initializing DisplayManager...");

        if (!m_window || !m_renderer)
        {
            LOG_ERROR("Window or Renderer is null");
            return false;
        }

        // 创建子管理器
        m_resolutionManager = std::make_unique<ResolutionManager>(m_window);
        m_viewportManager = std::make_unique<ViewportManager>();
        m_displaySettings = std::make_unique<DisplaySettings>();

        // 检测平台信息
        if (PlatformDisplay::IsMobileDevice())
        {
            LOG_INFO("Running on mobile device");
            auto [width, height] = PlatformDisplay::GetUsableScreenSize();
            LOG_INFO("Usable screen size: {}x{}", width, height);
            LOG_INFO("Device DPI: {}", PlatformDisplay::GetDeviceDPI());
            LOG_INFO("Density category: {}", PlatformDisplay::GetDensityCategory());
            
            // 获取安全区域
            SDL_Rect safeArea = PlatformDisplay::GetSafeArea();
            LOG_INFO("Safe area: x={}, y={}, w={}, h={}", 
                     safeArea.x, safeArea.y, safeArea.w, safeArea.h);
        }
        else
        {
            LOG_INFO("Running on desktop device");
        }

        // 更新当前显示信息
        UpdateDisplayInfo();

        // 更新视口
        m_viewportManager->UpdateViewport(m_currentDisplayInfo);

        LOG_INFO("DisplayManager initialized successfully");
        return true;
    }

    bool DisplayManager::SetDisplayMode(DisplayMode mode)
    {
        LOG_INFO("Setting display mode to: {}", static_cast<int>(mode));

        if (!ApplyDisplayMode(mode))
        {
            LOG_ERROR("Failed to apply display mode");
            return false;
        }

        m_currentDisplayInfo.mode = mode;
        UpdateDisplayInfo();
        m_viewportManager->UpdateViewport(m_currentDisplayInfo);
        m_needsRedraw = true;

        return true;
    }

    DisplayMode DisplayManager::GetCurrentDisplayMode() const
    {
        return m_currentDisplayInfo.mode;
    }

    bool DisplayManager::SetResolution(int width, int height)
    {
        LOG_INFO("Setting resolution to: {}x{}", width, height);

        if (width <= 0 || height <= 0)
        {
            LOG_ERROR("Invalid resolution: {}x{}", width, height);
            return false;
        }

        // 设置窗口大小
        if (!SDL_SetWindowSize(m_window, width, height))
        {
            LOG_ERROR("Failed to set window size: {}", SDL_GetError());
            return false;
        }

        // 居中窗口
        SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

        UpdateDisplayInfo();
        m_viewportManager->UpdateViewport(m_currentDisplayInfo);
        m_needsRedraw = true;

        return true;
    }

    bool DisplayManager::SetResolutionPreset(const Resolution& resolution)
    {
        return SetResolution(resolution.width, resolution.height);
    }

    std::vector<Resolution> DisplayManager::GetAvailableResolutions() const
    {
        return m_resolutionManager->GetAvailableDisplayModes();
    }

    void DisplayManager::HandleWindowEvent(const SDL_WindowEvent& event)
    {
        switch (event.type)
        {
        case SDL_EVENT_WINDOW_RESIZED:
            LOG_DEBUG("Window resized to: {}x{}", event.data1, event.data2);
            UpdateDisplayInfo();
            m_viewportManager->UpdateViewport(m_currentDisplayInfo);
            m_needsRedraw = true;
            break;

        case SDL_EVENT_WINDOW_MAXIMIZED:
            LOG_DEBUG("Window maximized");
            UpdateDisplayInfo();
            m_viewportManager->UpdateViewport(m_currentDisplayInfo);
            m_needsRedraw = true;
            break;

        case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
            LOG_DEBUG("Window display scale changed");
            UpdateDisplayInfo();
            m_viewportManager->UpdateViewport(m_currentDisplayInfo);
            m_needsRedraw = true;
            break;

        default:
            break;
        }
    }

    void DisplayManager::HandleDisplayChangeEvent(const SDL_DisplayEvent& event)
    {
        LOG_DEBUG("Display event received");
        UpdateDisplayInfo();
        m_viewportManager->UpdateViewport(m_currentDisplayInfo);
        m_needsRedraw = true;
    }

    std::pair<int, int> DisplayManager::WindowToGameCoords(int windowX, int windowY) const
    {
        return m_viewportManager->WindowToGameCoords(windowX, windowY);
    }

    std::pair<int, int> DisplayManager::GameToRenderCoords(float gameX, float gameY) const
    {
        return m_viewportManager->GameToRenderCoords(gameX, gameY);
    }

    void DisplayManager::SetScalingStrategy(ScalingStrategy strategy)
    {
        LOG_INFO("Setting scaling strategy to: {}", static_cast<int>(strategy));
        m_viewportManager->SetScalingStrategy(strategy);
        m_currentDisplayInfo.strategy = strategy;
        m_viewportManager->UpdateViewport(m_currentDisplayInfo);
        m_needsRedraw = true;
    }

    ScalingStrategy DisplayManager::GetScalingStrategy() const
    {
        return m_viewportManager->GetScalingStrategy();
    }

    bool DisplayManager::SaveDisplaySettings(const std::string& filePath)
    {
        std::string path = filePath.empty() ? m_defaultConfigPath : filePath;

        DisplaySettings::Settings settings;
        settings.displayMode = m_currentDisplayInfo.mode;
        settings.windowWidth = m_currentDisplayInfo.windowWidth;
        settings.windowHeight = m_currentDisplayInfo.windowHeight;
        settings.renderWidth = m_currentDisplayInfo.renderWidth;
        settings.renderHeight = m_currentDisplayInfo.renderHeight;
        settings.scalingStrategy = m_currentDisplayInfo.strategy;

        m_displaySettings->SetSettings(settings);
        return m_displaySettings->SaveToFile(path);
    }

    bool DisplayManager::LoadDisplaySettings(const std::string& filePath)
    {
        std::string path = filePath.empty() ? m_defaultConfigPath : filePath;

        // 检查文件是否存在
        if (!std::filesystem::exists(path))
        {
            LOG_INFO("Display settings file not found: {}, using defaults", path);
            return false;
        }

        if (!m_displaySettings->LoadFromFile(path))
        {
            LOG_WARN("Failed to load display settings from: {}", path);
            return false;
        }

        // 应用加载的设置
        return ApplySettings(m_displaySettings->GetSettings());
    }

    SDL_FRect DisplayManager::GetViewportRect() const
    {
        return m_viewportManager->GetViewportRect();
    }

    std::vector<SDL_FRect> DisplayManager::GetLetterboxRects() const
    {
        return m_viewportManager->GetLetterboxRects();
    }

    bool DisplayManager::ApplyDisplayMode(DisplayMode mode)
    {
        switch (mode)
        {
        case DisplayMode::WINDOWED:
            if (!SDL_SetWindowFullscreen(m_window, false))
            {
                LOG_ERROR("Failed to set windowed mode: {}", SDL_GetError());
                return false;
            }
            SDL_SetWindowBordered(m_window, true);
            break;

        case DisplayMode::FULLSCREEN_EXCLUSIVE:
            if (!SDL_SetWindowFullscreen(m_window, true))
            {
                LOG_ERROR("Failed to set fullscreen exclusive mode: {}", SDL_GetError());
                return false;
            }
            break;

        case DisplayMode::FULLSCREEN_BORDERLESS:
            // 无边框窗口模式
            SDL_SetWindowBordered(m_window, false);
            
            // 获取显示器边界并最大化窗口
            SDL_DisplayID displayID = SDL_GetDisplayForWindow(m_window);
            SDL_Rect bounds;
            if (SDL_GetDisplayBounds(displayID, &bounds))
            {
                SDL_SetWindowPosition(m_window, bounds.x, bounds.y);
                SDL_SetWindowSize(m_window, bounds.w, bounds.h);
            }
            else
            {
                LOG_ERROR("Failed to get display bounds: {}", SDL_GetError());
                return false;
            }
            break;
        }

        return true;
    }

    void DisplayManager::UpdateDisplayInfo()
    {
        m_currentDisplayInfo = m_resolutionManager->GetCurrentDisplayInfo();
        m_currentDisplayInfo.strategy = m_viewportManager->GetScalingStrategy();
    }

    bool DisplayManager::ApplySettings(const DisplaySettings::Settings& settings)
    {
        LOG_INFO("Applying display settings...");

        // 先设置分辨率
        if (!SetResolution(settings.windowWidth, settings.windowHeight))
        {
            LOG_ERROR("Failed to apply resolution from settings");
            return false;
        }

        // 设置显示模式
        if (!SetDisplayMode(settings.displayMode))
        {
            LOG_ERROR("Failed to apply display mode from settings");
            return false;
        }

        // 设置缩放策略
        SetScalingStrategy(settings.scalingStrategy);

        LOG_INFO("Display settings applied successfully");
        return true;
    }

} // namespace Match3::Display
