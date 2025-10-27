#include "ResourceManager.hpp"
#include "Core/Logger.hpp"

namespace Match3
{
    ResourceManager::ResourceManager(SDL_Renderer* renderer)
        : m_renderer(renderer)
    {
    }

    bool ResourceManager::CreateColorTexture(const std::string& name, const int width, const int height,
                                              const uint8_t r, const uint8_t g, const uint8_t b,
                                              const uint8_t a)
    {
        // 检查是否已存在
        if (HasTexture(name))
        {
            LOG_WARN("Texture '{}' already exists, skipping creation", name);
            return true;
        }

        auto texture = std::make_unique<Texture>();
        if (!texture->CreateFromColor(m_renderer, width, height, r, g, b, a))
        {
            LOG_ERROR("Failed to create color texture '{}'", name);
            return false;
        }

        m_textures[name] = std::move(texture);
        LOG_DEBUG("Created color texture '{}' ({}x{})", name, width, height);
        return true;
    }

    Texture* ResourceManager::GetTexture(const std::string& name)
    {
        const auto it = m_textures.find(name);
        if (it != m_textures.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    bool ResourceManager::HasTexture(const std::string& name) const
    {
        return m_textures.contains(name);
    }

    void ResourceManager::Clear()
    {
        m_textures.clear();
        LOG_INFO("All resources cleared");
    }
} // namespace Match3
