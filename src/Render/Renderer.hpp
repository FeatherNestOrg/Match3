#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <optional>

namespace Match3
{
    /**
     * @brief 渲染器封装类 - 提供便捷的渲染接口
     */
    class Renderer
    {
    public:
        explicit Renderer(SDL_Renderer* sdlRenderer);
        ~Renderer() = default;

        // 禁止拷贝
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        /**
         * @brief 清空屏幕
         * @param r 红色分量 (0-255)
         * @param g 绿色分量 (0-255)
         * @param b 蓝色分量 (0-255)
         * @param a 透明度 (0-255)
         */
        void Clear(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255);

        /**
         * @brief 呈现渲染结果
         */
        void Present();

        /**
         * @brief 设置绘制颜色
         */
        void SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

        /**
         * @brief 绘制实心矩形
         */
        void FillRect(int x, int y, int w, int h);

        /**
         * @brief 绘制空心矩形
         */
        void DrawRect(int x, int y, int w, int h);

        /**
         * @brief 绘制直线
         */
        void DrawLine(int x1, int y1, int x2, int y2);

        /**
         * @brief 绘制实心圆形
         */
        void FillCircle(int centerX, int centerY, int radius);
        
        /**
         * @brief 绘制实心圆形（带透明度）
         * @param alpha 透明度 (0.0-1.0)
         */
        void FillCircleWithAlpha(int centerX, int centerY, int radius,
                                 uint8_t r, uint8_t g, uint8_t b, float alpha);

        /**
         * @brief 绘制空心圆形
         */
        void DrawCircle(int centerX, int centerY, int radius);

        /**
         * @brief 获取底层 SDL 渲染器（用于高级操作）
         */
        [[nodiscard]] SDL_Renderer* GetSDLRenderer() const { return m_sdlRenderer; }

    private:
        SDL_Renderer* m_sdlRenderer; // 不拥有所有权
    };
} // namespace Match3
