#pragma once

#include "DisplayMode.hpp"
#include "ResolutionManager.hpp"
#include "ViewportManager.hpp"
#include "DisplaySettings.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include <utility>

namespace Match3::Display
{
    /**
     * @brief 显示管理器 - 主控制中心，协调各个子系统
     */
    class DisplayManager
    {
    public:
        explicit DisplayManager(SDL_Window* window, SDL_Renderer* renderer);
        ~DisplayManager() = default;

        // 禁止拷贝
        DisplayManager(const DisplayManager&) = delete;
        DisplayManager& operator=(const DisplayManager&) = delete;

        /**
         * @brief 初始化显示系统
         * @return 成功返回 true
         */
        bool Initialize();

        /**
         * @brief 设置显示模式
         * @param mode 显示模式
         * @return 成功返回 true
         */
        bool SetDisplayMode(DisplayMode mode);

        /**
         * @brief 获取当前显示模式
         * @return 当前显示模式
         */
        [[nodiscard]] DisplayMode GetCurrentDisplayMode() const;

        /**
         * @brief 设置分辨率
         * @param width 宽度
         * @param height 高度
         * @return 成功返回 true
         */
        bool SetResolution(int width, int height);

        /**
         * @brief 设置预设分辨率
         * @param resolution 分辨率结构体
         * @return 成功返回 true
         */
        bool SetResolutionPreset(const Resolution& resolution);

        /**
         * @brief 获取可用分辨率列表
         * @return 分辨率列表
         */
        std::vector<Resolution> GetAvailableResolutions() const;

        /**
         * @brief 获取显示信息
         * @return 显示信息
         */
        [[nodiscard]] const DisplayInfo& GetDisplayInfo() const { return m_currentDisplayInfo; }

        /**
         * @brief 处理窗口事件
         * @param event 窗口事件
         */
        void HandleWindowEvent(const SDL_WindowEvent& event);

        /**
         * @brief 处理显示器变化事件
         * @param event 显示器事件
         */
        void HandleDisplayChangeEvent(const SDL_DisplayEvent& event);

        /**
         * @brief 窗口坐标 → 游戏坐标
         * @param windowX 窗口 X 坐标
         * @param windowY 窗口 Y 坐标
         * @return 游戏坐标 (x, y)
         */
        std::pair<int, int> WindowToGameCoords(int windowX, int windowY) const;

        /**
         * @brief 游戏坐标 → 渲染坐标
         * @param gameX 游戏 X 坐标
         * @param gameY 游戏 Y 坐标
         * @return 渲染坐标 (x, y)
         */
        std::pair<int, int> GameToRenderCoords(float gameX, float gameY) const;

        /**
         * @brief 设置缩放策略
         * @param strategy 缩放策略
         */
        void SetScalingStrategy(ScalingStrategy strategy);

        /**
         * @brief 获取缩放策略
         * @return 缩放策略
         */
        [[nodiscard]] ScalingStrategy GetScalingStrategy() const;

        /**
         * @brief 保存显示设置
         * @param filePath 配置文件路径（可选）
         * @return 成功返回 true
         */
        bool SaveDisplaySettings(const std::string& filePath = "");

        /**
         * @brief 加载显示设置
         * @param filePath 配置文件路径（可选）
         * @return 成功返回 true
         */
        bool LoadDisplaySettings(const std::string& filePath = "");

        /**
         * @brief 检查是否需要重绘
         * @return 需要重绘返回 true
         */
        [[nodiscard]] bool NeedsRedraw() const { return m_needsRedraw; }

        /**
         * @brief 清除重绘标志
         */
        void ClearRedrawFlag() { m_needsRedraw = false; }

        /**
         * @brief 获取视口矩形
         * @return 视口矩形
         */
        [[nodiscard]] SDL_FRect GetViewportRect() const;

        /**
         * @brief 获取 letterbox/pillarbox 区域
         * @return 黑边区域列表
         */
        std::vector<SDL_FRect> GetLetterboxRects() const;

    private:
        SDL_Window* m_window;
        SDL_Renderer* m_renderer;

        std::unique_ptr<ResolutionManager> m_resolutionManager;
        std::unique_ptr<ViewportManager> m_viewportManager;
        std::unique_ptr<DisplaySettings> m_displaySettings;

        DisplayInfo m_currentDisplayInfo;
        bool m_needsRedraw;
        
        std::string m_defaultConfigPath;

        /**
         * @brief 应用显示模式
         * @param mode 显示模式
         * @return 成功返回 true
         */
        bool ApplyDisplayMode(DisplayMode mode);

        /**
         * @brief 更新显示信息
         */
        void UpdateDisplayInfo();

        /**
         * @brief 应用设置到窗口
         * @param settings 设置
         * @return 成功返回 true
         */
        bool ApplySettings(const DisplaySettings::Settings& settings);
    };

} // namespace Match3::Display
