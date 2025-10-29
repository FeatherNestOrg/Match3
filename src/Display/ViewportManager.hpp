#pragma once

#include "DisplayMode.hpp"
#include <SDL3/SDL.h>
#include <vector>
#include <utility>

namespace Match3::Display
{
    /**
     * @brief 视口管理器 - 负责坐标变换和视口计算
     */
    class ViewportManager
    {
    public:
        ViewportManager();

        /**
         * @brief 更新视口信息
         * @param displayInfo 显示信息
         */
        void UpdateViewport(const DisplayInfo& displayInfo);

        /**
         * @brief 窗口坐标（鼠标输入）→ 游戏逻辑坐标
         * @param windowX 窗口 X 坐标
         * @param windowY 窗口 Y 坐标
         * @return 游戏逻辑坐标 (x, y)
         */
        std::pair<int, int> WindowToGameCoords(int windowX, int windowY);

        /**
         * @brief 游戏逻辑坐标 → 渲染坐标
         * @param gameX 游戏逻辑 X 坐标
         * @param gameY 游戏逻辑 Y 坐标
         * @return 渲染坐标 (x, y)
         */
        std::pair<int, int> GameToRenderCoords(float gameX, float gameY);

        /**
         * @brief 获取 letterbox/pillarbox 黑边矩形（用于清除）
         * @return 黑边矩形列表
         */
        std::vector<SDL_FRect> GetLetterboxRects();

        /**
         * @brief 计算宽高比
         * @param width 宽度
         * @param height 高度
         * @return 宽高比
         */
        static float CalculateAspectRatio(int width, int height);

        /**
         * @brief 检查两个宽高比是否匹配
         * @param ar1 宽高比1
         * @param ar2 宽高比2
         * @param tolerance 容差
         * @return 匹配返回 true
         */
        static bool IsAspectRatioMatch(float ar1, float ar2, float tolerance = 0.01f);

        /**
         * @brief 获取水平缩放因子
         * @return X 轴缩放因子
         */
        [[nodiscard]] float GetScaleFactorX() const { return m_viewport.scaleX; }

        /**
         * @brief 获取垂直缩放因子
         * @return Y 轴缩放因子
         */
        [[nodiscard]] float GetScaleFactorY() const { return m_viewport.scaleY; }

        /**
         * @brief 获取视口矩形
         * @return 视口矩形
         */
        [[nodiscard]] SDL_FRect GetViewportRect() const;

        /**
         * @brief 设置缩放策略
         * @param strategy 缩放策略
         */
        void SetScalingStrategy(ScalingStrategy strategy);

        /**
         * @brief 获取当前缩放策略
         * @return 缩放策略
         */
        [[nodiscard]] ScalingStrategy GetScalingStrategy() const { return m_strategy; }

    private:
        /**
         * @brief 内部视口结构
         */
        struct Viewport
        {
            float offsetX{0.0f};
            float offsetY{0.0f};           // 相对于窗口的偏移
            float renderWidth{800.0f};
            float renderHeight{600.0f};   // 实际渲染区域
            float scaleX{1.0f};
            float scaleY{1.0f};           // 缩放因子
        };

        Viewport m_viewport;
        ScalingStrategy m_strategy;
        
        // 游戏逻辑分辨率（固定）
        static constexpr int GAME_WIDTH = 800;
        static constexpr int GAME_HEIGHT = 600;

        /**
         * @brief 根据缩放策略计算视口
         * @param windowWidth 窗口宽度
         * @param windowHeight 窗口高度
         */
        void CalculateViewport(int windowWidth, int windowHeight);

        /**
         * @brief 计算保持宽高比的视口（letterbox/pillarbox）
         * @param windowWidth 窗口宽度
         * @param windowHeight 窗口高度
         */
        void CalculateAspectRatioViewport(int windowWidth, int windowHeight);

        /**
         * @brief 计算填满屏幕的视口
         * @param windowWidth 窗口宽度
         * @param windowHeight 窗口高度
         */
        void CalculateFillScreenViewport(int windowWidth, int windowHeight);

        /**
         * @brief 计算整数倍缩放的视口
         * @param windowWidth 窗口宽度
         * @param windowHeight 窗口高度
         */
        void CalculateIntegerScaleViewport(int windowWidth, int windowHeight);
    };

} // namespace Match3::Display
