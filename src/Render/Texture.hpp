#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <string>

namespace Match3
{
    /**
     * @brief 纹理管理类 - 封装 SDL_Texture
     */
    class Texture
    {
    public:
        Texture() = default;
        ~Texture();

        // 禁止拷贝，允许移动
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        /**
         * @brief 从颜色创建纹理（用于简单形状）
         * @param renderer SDL 渲染器
         * @param width 宽度
         * @param height 高度
         * @param r 红色分量
         * @param g 绿色分量
         * @param b 蓝色分量
         * @param a 透明度
         * @return 成功返回 true
         */
        bool CreateFromColor(SDL_Renderer* renderer, int width, int height,
                             uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

        /**
         * @brief 渲染纹理
         * @param renderer SDL 渲染器
         * @param x X 坐标
         * @param y Y 坐标
         */
        void Render(SDL_Renderer* renderer, int x, int y) const;

        /**
         * @brief 渲染纹理（带缩放）
         * @param renderer SDL 渲染器
         * @param x X 坐标
         * @param y Y 坐标
         * @param width 目标宽度
         * @param height 目标高度
         */
        void Render(SDL_Renderer* renderer, int x, int y, int width, int height) const;

        /**
         * @brief 释放纹理
         */
        void Free();

        /**
         * @brief 检查纹理是否有效
         */
        [[nodiscard]] bool IsValid() const { return m_texture != nullptr; }

        /**
         * @brief 获取宽度
         */
        [[nodiscard]] int GetWidth() const { return m_width; }

        /**
         * @brief 获取高度
         */
        [[nodiscard]] int GetHeight() const { return m_height; }

        /**
         * @brief 设置颜色调制
         */
        void SetColorMod(uint8_t r, uint8_t g, uint8_t b);

        /**
         * @brief 设置透明度
         */
        void SetAlpha(uint8_t alpha);

    private:
        SDL_Texture* m_texture = nullptr;
        int m_width = 0;
        int m_height = 0;
    };
} // namespace Match3
