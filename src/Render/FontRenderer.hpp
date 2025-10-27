#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace Match3
{
    /**
     * @brief Text alignment options
     */
    enum class TextAlign
    {
        Left,
        Center,
        Right
    };

    /**
     * @brief Font renderer using SDL_ttf for bitmap font rendering
     */
    class FontRenderer
    {
    public:
        explicit FontRenderer(SDL_Renderer* sdlRenderer);
        ~FontRenderer();

        // Disable copy
        FontRenderer(const FontRenderer&) = delete;
        FontRenderer& operator=(const FontRenderer&) = delete;

        /**
         * @brief Initialize SDL_ttf library
         * @return true if successful
         */
        bool Initialize();

        /**
         * @brief Load a font from file
         * @param fontPath Path to the TTF font file
         * @param size Font size in points
         * @param fontId Unique identifier for this font
         * @return true if successful
         */
        bool LoadFont(const std::string& fontPath, int size, const std::string& fontId);

        /**
         * @brief Render text to screen
         * @param text Text to render
         * @param x X position
         * @param y Y position
         * @param fontId Font identifier
         * @param r Red component (0-255)
         * @param g Green component (0-255)
         * @param b Blue component (0-255)
         * @param a Alpha component (0-255)
         * @param align Text alignment
         */
        void RenderText(const std::string& text, int x, int y, const std::string& fontId,
                       uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255,
                       TextAlign align = TextAlign::Left);

        /**
         * @brief Measure text width
         * @param text Text to measure
         * @param fontId Font identifier
         * @return Width in pixels, or -1 on error
         */
        int MeasureText(const std::string& text, const std::string& fontId);

        /**
         * @brief Get text height for a font
         * @param fontId Font identifier
         * @return Height in pixels, or -1 on error
         */
        int GetTextHeight(const std::string& fontId);

        /**
         * @brief Shutdown and cleanup SDL_ttf
         */
        void Shutdown();

    private:
        SDL_Renderer* m_sdlRenderer; // Not owned
        std::unordered_map<std::string, TTF_Font*> m_fonts;
        bool m_initialized;
    };
} // namespace Match3
