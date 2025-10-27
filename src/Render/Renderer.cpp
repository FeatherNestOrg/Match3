#include "Renderer.hpp"
#include <cmath>

namespace Match3
{
    Renderer::Renderer(SDL_Renderer* sdlRenderer)
        : m_sdlRenderer(sdlRenderer)
    {
    }

    void Renderer::Clear(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
    {
        SDL_SetRenderDrawColor(m_sdlRenderer, r, g, b, a);
        SDL_RenderClear(m_sdlRenderer);
    }

    void Renderer::Present()
    {
        SDL_RenderPresent(m_sdlRenderer);
    }

    void Renderer::SetDrawColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
    {
        SDL_SetRenderDrawColor(m_sdlRenderer, r, g, b, a);
    }

    void Renderer::FillRect(const int x, const int y, const int w, const int h)
    {
        const SDL_FRect rect = {static_cast<float>(x), static_cast<float>(y),
                                static_cast<float>(w), static_cast<float>(h)};
        SDL_RenderFillRect(m_sdlRenderer, &rect);
    }

    void Renderer::DrawRect(const int x, const int y, const int w, const int h)
    {
        const SDL_FRect rect = {static_cast<float>(x), static_cast<float>(y),
                                static_cast<float>(w), static_cast<float>(h)};
        SDL_RenderRect(m_sdlRenderer, &rect);
    }

    void Renderer::DrawLine(const int x1, const int y1, const int x2, const int y2)
    {
        SDL_RenderLine(m_sdlRenderer,
                       static_cast<float>(x1), static_cast<float>(y1),
                       static_cast<float>(x2), static_cast<float>(y2));
    }

    void Renderer::FillCircle(const int centerX, const int centerY, const int radius)
    {
        // 使用中点圆算法绘制实心圆
        for (int y = -radius; y <= radius; y++)
        {
            for (int x = -radius; x <= radius; x++)
            {
                if (x * x + y * y <= radius * radius)
                {
                    SDL_RenderPoint(m_sdlRenderer,
                                    static_cast<float>(centerX + x),
                                    static_cast<float>(centerY + y));
                }
            }
        }
    }
    
    void Renderer::FillCircleWithAlpha(const int centerX, const int centerY, const int radius,
                                       const uint8_t r, const uint8_t g, const uint8_t b, const float alpha)
    {
        const auto alphaValue = static_cast<uint8_t>(255 * alpha);
        SDL_SetRenderDrawColor(m_sdlRenderer, r, g, b, alphaValue);
        FillCircle(centerX, centerY, radius);
    }

    void Renderer::DrawCircle(const int centerX, const int centerY, const int radius)
    {
        // 使用 Bresenham 圆算法绘制空心圆
        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius;

        auto drawCirclePoints = [&](int cx, int cy, int px, int py)
        {
            SDL_RenderPoint(m_sdlRenderer, static_cast<float>(cx + px), static_cast<float>(cy + py));
            SDL_RenderPoint(m_sdlRenderer, static_cast<float>(cx - px), static_cast<float>(cy + py));
            SDL_RenderPoint(m_sdlRenderer, static_cast<float>(cx + px), static_cast<float>(cy - py));
            SDL_RenderPoint(m_sdlRenderer, static_cast<float>(cx - px), static_cast<float>(cy - py));
            SDL_RenderPoint(m_sdlRenderer, static_cast<float>(cx + py), static_cast<float>(cy + px));
            SDL_RenderPoint(m_sdlRenderer, static_cast<float>(cx - py), static_cast<float>(cy + px));
            SDL_RenderPoint(m_sdlRenderer, static_cast<float>(cx + py), static_cast<float>(cy - px));
            SDL_RenderPoint(m_sdlRenderer, static_cast<float>(cx - py), static_cast<float>(cy - px));
        };

        while (y >= x)
        {
            drawCirclePoints(centerX, centerY, x, y);
            x++;

            if (d > 0)
            {
                y--;
                d = d + 4 * (x - y) + 10;
            }
            else
            {
                d = d + 4 * x + 6;
            }
        }
    }
} // namespace Match3
