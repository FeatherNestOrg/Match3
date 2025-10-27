#pragma once

#include "Texture.hpp"
#include <SDL3/SDL.h>
#include <ankerl/unordered_dense.h>
#include <memory>
#include <string>

namespace Match3
{
    /**
     * @brief 资源管理器 - 管理游戏中的所有资源
     */
    class ResourceManager
    {
    public:
        explicit ResourceManager(SDL_Renderer* renderer);
        ~ResourceManager() = default;

        // 禁止拷贝
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        /**
         * @brief 创建纯色纹理并缓存
         * @param name 纹理名称
         * @param width 宽度
         * @param height 高度
         * @param r 红色分量
         * @param g 绿色分量
         * @param b 蓝色分量
         * @param a 透明度
         * @return 成功返回 true
         */
        bool CreateColorTexture(const std::string& name, int width, int height,
                                uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

        /**
         * @brief 获取纹理
         * @param name 纹理名称
         * @return 纹理指针，不存在则返回 nullptr
         */
        Texture* GetTexture(const std::string& name);

        /**
         * @brief 检查纹理是否存在
         */
        [[nodiscard]] bool HasTexture(const std::string& name) const;

        /**
         * @brief 清空所有资源
         */
        void Clear();

    private:
        SDL_Renderer* m_renderer; // 不拥有所有权
        ankerl::unordered_dense::map<std::string, std::unique_ptr<Texture>> m_textures;
    };
} // namespace Match3
