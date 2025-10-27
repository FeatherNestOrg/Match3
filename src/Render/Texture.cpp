#include "Texture.hpp"
#include "Core/Logger.hpp"

namespace Match3
{
    Texture::~Texture()
    {
        Free();
    }

    Texture::Texture(Texture&& other) noexcept
        : m_texture(other.m_texture)
          , m_width(other.m_width)
          , m_height(other.m_height)
    {
        other.m_texture = nullptr;
        other.m_width = 0;
        other.m_height = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            Free();

            m_texture = other.m_texture;
            m_width = other.m_width;
            m_height = other.m_height;

            other.m_texture = nullptr;
            other.m_width = 0;
            other.m_height = 0;
        }
        return *this;
    }

    bool Texture::CreateFromColor(SDL_Renderer* renderer, const int width, const int height,
                                   const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
    {
        Free();

        // 创建目标纹理
        m_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_TARGET, width, height);
        if (!m_texture)
        {
            LOG_ERROR("Failed to create texture: {}", SDL_GetError());
            return false;
        }

        m_width = width;
        m_height = height;

        // 设置混合模式
        SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);

        // 使用渲染目标填充颜色
        SDL_Texture* previousTarget = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, m_texture);

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderClear(renderer);

        SDL_SetRenderTarget(renderer, previousTarget);

        return true;
    }

    void Texture::Render(SDL_Renderer* renderer, const int x, const int y) const
    {
        if (!m_texture) return;

        const SDL_FRect destRect = {
            static_cast<float>(x), static_cast<float>(y),
            static_cast<float>(m_width), static_cast<float>(m_height)
        };
        SDL_RenderTexture(renderer, m_texture, nullptr, &destRect);
    }

    void Texture::Render(SDL_Renderer* renderer, const int x, const int y, const int width, const int height) const
    {
        if (!m_texture) return;

        const SDL_FRect destRect = {
            static_cast<float>(x), static_cast<float>(y),
            static_cast<float>(width), static_cast<float>(height)
        };
        SDL_RenderTexture(renderer, m_texture, nullptr, &destRect);
    }

    void Texture::Free()
    {
        if (m_texture)
        {
            SDL_DestroyTexture(m_texture);
            m_texture = nullptr;
            m_width = 0;
            m_height = 0;
        }
    }

    void Texture::SetColorMod(const uint8_t r, const uint8_t g, const uint8_t b)
    {
        if (m_texture)
        {
            SDL_SetTextureColorMod(m_texture, r, g, b);
        }
    }

    void Texture::SetAlpha(const uint8_t alpha)
    {
        if (m_texture)
        {
            SDL_SetTextureAlphaMod(m_texture, alpha);
        }
    }
} // namespace Match3
