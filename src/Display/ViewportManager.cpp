#include "ViewportManager.hpp"
#include "Core/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace Match3::Display
{
    ViewportManager::ViewportManager()
        : m_strategy(ScalingStrategy::ASPECT_RATIO_STRICT)
    {
        // 初始化默认视口
        m_viewport.renderWidth = GAME_WIDTH;
        m_viewport.renderHeight = GAME_HEIGHT;
        m_viewport.scaleX = 1.0f;
        m_viewport.scaleY = 1.0f;
        m_viewport.offsetX = 0.0f;
        m_viewport.offsetY = 0.0f;
    }

    void ViewportManager::UpdateViewport(const DisplayInfo& displayInfo)
    {
        m_strategy = displayInfo.strategy;
        CalculateViewport(displayInfo.windowWidth, displayInfo.windowHeight);
        
        LOG_DEBUG("Viewport updated: offset({}, {}), size({}x{}), scale({}, {})",
                  m_viewport.offsetX, m_viewport.offsetY,
                  m_viewport.renderWidth, m_viewport.renderHeight,
                  m_viewport.scaleX, m_viewport.scaleY);
    }

    std::pair<int, int> ViewportManager::WindowToGameCoords(int windowX, int windowY)
    {
        // 1. 减去视口偏移
        float viewX = static_cast<float>(windowX) - m_viewport.offsetX;
        float viewY = static_cast<float>(windowY) - m_viewport.offsetY;

        // 2. 应用逆缩放变换到游戏坐标
        int gameX = static_cast<int>(viewX / m_viewport.scaleX);
        int gameY = static_cast<int>(viewY / m_viewport.scaleY);

        // 3. 限制在游戏边界内
        gameX = std::clamp(gameX, 0, GAME_WIDTH);
        gameY = std::clamp(gameY, 0, GAME_HEIGHT);

        return {gameX, gameY};
    }

    std::pair<int, int> ViewportManager::GameToRenderCoords(float gameX, float gameY)
    {
        // 应用缩放和偏移
        int renderX = static_cast<int>(gameX * m_viewport.scaleX + m_viewport.offsetX);
        int renderY = static_cast<int>(gameY * m_viewport.scaleY + m_viewport.offsetY);

        return {renderX, renderY};
    }

    std::vector<SDL_FRect> ViewportManager::GetLetterboxRects()
    {
        std::vector<SDL_FRect> rects;

        // 左边黑边（pillarbox）
        if (m_viewport.offsetX > 0.0f)
        {
            SDL_FRect leftRect{
                0.0f,
                0.0f,
                m_viewport.offsetX,
                m_viewport.renderHeight + 2.0f * m_viewport.offsetY
            };
            rects.push_back(leftRect);
        }

        // 右边黑边（pillarbox）
        float rightEdge = m_viewport.offsetX + m_viewport.renderWidth;
        if (rightEdge > 0.0f)
        {
            SDL_FRect rightRect{
                rightEdge,
                0.0f,
                m_viewport.offsetX, // 对称的
                m_viewport.renderHeight + 2.0f * m_viewport.offsetY
            };
            rects.push_back(rightRect);
        }

        // 上边黑边（letterbox）
        if (m_viewport.offsetY > 0.0f)
        {
            SDL_FRect topRect{
                0.0f,
                0.0f,
                m_viewport.renderWidth + 2.0f * m_viewport.offsetX,
                m_viewport.offsetY
            };
            rects.push_back(topRect);
        }

        // 下边黑边（letterbox）
        float bottomEdge = m_viewport.offsetY + m_viewport.renderHeight;
        if (bottomEdge > 0.0f)
        {
            SDL_FRect bottomRect{
                0.0f,
                bottomEdge,
                m_viewport.renderWidth + 2.0f * m_viewport.offsetX,
                m_viewport.offsetY // 对称的
            };
            rects.push_back(bottomRect);
        }

        return rects;
    }

    float ViewportManager::CalculateAspectRatio(int width, int height)
    {
        if (height == 0)
        {
            return 0.0f;
        }
        return static_cast<float>(width) / static_cast<float>(height);
    }

    bool ViewportManager::IsAspectRatioMatch(float ar1, float ar2, float tolerance)
    {
        return std::abs(ar1 - ar2) <= tolerance;
    }

    SDL_FRect ViewportManager::GetViewportRect() const
    {
        return SDL_FRect{
            m_viewport.offsetX,
            m_viewport.offsetY,
            m_viewport.renderWidth,
            m_viewport.renderHeight
        };
    }

    void ViewportManager::SetScalingStrategy(ScalingStrategy strategy)
    {
        m_strategy = strategy;
    }

    void ViewportManager::CalculateViewport(int windowWidth, int windowHeight)
    {
        switch (m_strategy)
        {
        case ScalingStrategy::ASPECT_RATIO_STRICT:
            CalculateAspectRatioViewport(windowWidth, windowHeight);
            break;
        case ScalingStrategy::FILL_SCREEN:
            CalculateFillScreenViewport(windowWidth, windowHeight);
            break;
        case ScalingStrategy::INTEGER_SCALE:
            CalculateIntegerScaleViewport(windowWidth, windowHeight);
            break;
        case ScalingStrategy::DYNAMIC_SCALE:
            // 动态缩放：直接使用窗口尺寸
            m_viewport.renderWidth = static_cast<float>(windowWidth);
            m_viewport.renderHeight = static_cast<float>(windowHeight);
            m_viewport.scaleX = static_cast<float>(windowWidth) / GAME_WIDTH;
            m_viewport.scaleY = static_cast<float>(windowHeight) / GAME_HEIGHT;
            m_viewport.offsetX = 0.0f;
            m_viewport.offsetY = 0.0f;
            break;
        }
    }

    void ViewportManager::CalculateAspectRatioViewport(int windowWidth, int windowHeight)
    {
        float windowAspect = CalculateAspectRatio(windowWidth, windowHeight);
        float gameAspect = CalculateAspectRatio(GAME_WIDTH, GAME_HEIGHT);

        if (windowAspect > gameAspect)
        {
            // 窗口更宽 - 需要 pillarbox（左右黑边）
            m_viewport.renderHeight = static_cast<float>(windowHeight);
            m_viewport.renderWidth = m_viewport.renderHeight * gameAspect;
            m_viewport.offsetX = (windowWidth - m_viewport.renderWidth) / 2.0f;
            m_viewport.offsetY = 0.0f;
        }
        else
        {
            // 窗口更高 - 需要 letterbox（上下黑边）
            m_viewport.renderWidth = static_cast<float>(windowWidth);
            m_viewport.renderHeight = m_viewport.renderWidth / gameAspect;
            m_viewport.offsetX = 0.0f;
            m_viewport.offsetY = (windowHeight - m_viewport.renderHeight) / 2.0f;
        }

        // 计算缩放因子
        m_viewport.scaleX = m_viewport.renderWidth / GAME_WIDTH;
        m_viewport.scaleY = m_viewport.renderHeight / GAME_HEIGHT;
    }

    void ViewportManager::CalculateFillScreenViewport(int windowWidth, int windowHeight)
    {
        // 填满屏幕，可能会拉伸
        m_viewport.renderWidth = static_cast<float>(windowWidth);
        m_viewport.renderHeight = static_cast<float>(windowHeight);
        m_viewport.scaleX = static_cast<float>(windowWidth) / GAME_WIDTH;
        m_viewport.scaleY = static_cast<float>(windowHeight) / GAME_HEIGHT;
        m_viewport.offsetX = 0.0f;
        m_viewport.offsetY = 0.0f;
    }

    void ViewportManager::CalculateIntegerScaleViewport(int windowWidth, int windowHeight)
    {
        // 计算整数倍缩放
        int scaleX = std::max(1, windowWidth / GAME_WIDTH);
        int scaleY = std::max(1, windowHeight / GAME_HEIGHT);
        
        // 使用较小的缩放因子以确保完全可见
        int scale = std::min(scaleX, scaleY);

        m_viewport.renderWidth = static_cast<float>(GAME_WIDTH * scale);
        m_viewport.renderHeight = static_cast<float>(GAME_HEIGHT * scale);
        m_viewport.scaleX = static_cast<float>(scale);
        m_viewport.scaleY = static_cast<float>(scale);
        
        // 居中显示
        m_viewport.offsetX = (windowWidth - m_viewport.renderWidth) / 2.0f;
        m_viewport.offsetY = (windowHeight - m_viewport.renderHeight) / 2.0f;
    }

} // namespace Match3::Display
