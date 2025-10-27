#include "FontRenderer.hpp"
#include "Core/Logger.hpp"

namespace Match3
{
    FontRenderer::FontRenderer(SDL_Renderer* sdlRenderer)
        : m_sdlRenderer(sdlRenderer)
          , m_initialized(false)
    {
    }

    FontRenderer::~FontRenderer()
    {
        Shutdown();
    }

    bool FontRenderer::Initialize()
    {
        if (m_initialized)
        {
            LOG_WARN("FontRenderer already initialized");
            return true;
        }

        if (!TTF_Init())
        {
            LOG_ERROR("Failed to initialize SDL_ttf: {}", SDL_GetError());
            return false;
        }

        m_initialized = true;
        LOG_INFO("FontRenderer initialized successfully");
        return true;
    }

    bool FontRenderer::LoadFont(const std::string& fontPath, int size, const std::string& fontId)
    {
        if (!m_initialized)
        {
            LOG_ERROR("FontRenderer not initialized. Call Initialize() first");
            return false;
        }

        // Check if font already loaded
        if (m_fonts.find(fontId) != m_fonts.end())
        {
            LOG_WARN("Font '{}' already loaded", fontId);
            return true;
        }

        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), size);
        if (!font)
        {
            LOG_ERROR("Failed to load font '{}' from {}: {}", fontId, fontPath, SDL_GetError());
            return false;
        }

        m_fonts[fontId] = font;
        LOG_INFO("Loaded font '{}' (size: {}) from {}", fontId, size, fontPath);
        return true;
    }

    void FontRenderer::RenderText(const std::string& text, int x, int y, const std::string& fontId,
                                  uint8_t r, uint8_t g, uint8_t b, uint8_t a,
                                  TextAlign align)
    {
        if (text.empty())
            return;

        auto it = m_fonts.find(fontId);
        if (it == m_fonts.end())
        {
            LOG_ERROR("Font '{}' not found. Load it first with LoadFont()", fontId);
            return;
        }

        TTF_Font* font = it->second;

        // Create surface from text
        SDL_Color color = {r, g, b, a};
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), text.length(), color);
        if (!surface)
        {
            LOG_ERROR("Failed to render text: {}", SDL_GetError());
            return;
        }

        // Create texture from surface
        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_sdlRenderer, surface);
        if (!texture)
        {
            LOG_ERROR("Failed to create texture from text: {}", SDL_GetError());
            SDL_DestroySurface(surface);
            return;
        }

        // Calculate position based on alignment
        int textWidth = surface->w;
        int textHeight = surface->h;

        int renderX = x;
        switch (align)
        {
        case TextAlign::Center:
            renderX = x - textWidth / 2;
            break;
        case TextAlign::Right:
            renderX = x - textWidth;
            break;
        case TextAlign::Left:
        default:
            renderX = x;
            break;
        }

        // Render texture
        SDL_FRect destRect = {
            static_cast<float>(renderX),
            static_cast<float>(y),
            static_cast<float>(textWidth),
            static_cast<float>(textHeight)
        };

        SDL_RenderTexture(m_sdlRenderer, texture, nullptr, &destRect);

        // Cleanup
        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }

    int FontRenderer::MeasureText(const std::string& text, const std::string& fontId)
    {
        if (text.empty())
            return 0;

        auto it = m_fonts.find(fontId);
        if (it == m_fonts.end())
        {
            LOG_ERROR("Font '{}' not found", fontId);
            return -1;
        }

        int width = 0;
        int height = 0;
        if (!TTF_GetStringSize(it->second, text.c_str(), text.length(), &width, &height))
        {
            LOG_ERROR("Failed to measure text: {}", SDL_GetError());
            return -1;
        }

        return width;
    }

    int FontRenderer::GetTextHeight(const std::string& fontId)
    {
        auto it = m_fonts.find(fontId);
        if (it == m_fonts.end())
        {
            LOG_ERROR("Font '{}' not found", fontId);
            return -1;
        }

        return TTF_GetFontHeight(it->second);
    }

    void FontRenderer::Shutdown()
    {
        if (!m_initialized)
            return;

        // Free all fonts
        for (auto& [fontId, font] : m_fonts)
        {
            if (font)
            {
                TTF_CloseFont(font);
                LOG_DEBUG("Closed font '{}'", fontId);
            }
        }
        m_fonts.clear();

        TTF_Quit();
        m_initialized = false;
        LOG_INFO("FontRenderer shut down");
    }
} // namespace Match3
