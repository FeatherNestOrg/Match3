#include "EntityFactory.hpp"
#include "Core/Config.hpp"
#include "Core/Logger.hpp"
#include <random>
#include <cmath>

namespace Match3 {

EntityFactory::EntityFactory(entt::registry& registry)
    : m_registry(registry)
{
}

entt::entity EntityFactory::CreateGem(int row, int col, Components::GemType type)
{
    auto entity = m_registry.create();
    
    // 计算世界坐标
    const float x = Config::BOARD_OFFSET_X + col * Config::GEM_SIZE + Config::GEM_SIZE / 2.0f;
    const float y = Config::BOARD_OFFSET_Y + row * Config::GEM_SIZE + Config::GEM_SIZE / 2.0f;
    
    // 添加基础组件
    m_registry.emplace<Components::Position>(entity, x, y);
    m_registry.emplace<Components::GridPosition>(entity, row, col);
    m_registry.emplace<Components::Gem>(entity, type);
    
    // 添加渲染组件
    const auto& color = Config::GEM_COLORS[static_cast<int>(type)];
    m_registry.emplace<Components::Renderable>(entity,
        Config::GEM_SIZE / 2 - Config::GEM_MARGIN,
        color.r, color.g, color.b, color.a);
    
    return entity;
}

entt::entity EntityFactory::CreateParticle(float x, float y, 
                                            float vx, float vy,
                                            uint8_t r, uint8_t g, uint8_t b, uint8_t a,
                                            float size,
                                            float lifetime)
{
    auto entity = m_registry.create();
    
    // 添加组件
    m_registry.emplace<Components::Position>(entity, x, y);
    m_registry.emplace<Components::Velocity>(entity, vx, vy);
    m_registry.emplace<Components::Renderable>(entity, 
        static_cast<int>(size), r, g, b, a);
    m_registry.emplace<Components::Particle>(entity, size);
    m_registry.emplace<Components::Lifetime>(entity, lifetime);
    
    return entity;
}

void EntityFactory::CreateExplosion(float x, float y,
                                     uint8_t r, uint8_t g, uint8_t b,
                                     int particleCount,
                                     float spreadSpeed)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> speedDist(spreadSpeed * 0.5f, spreadSpeed * 1.5f);
    std::uniform_real_distribution<float> sizeDist(3.0f, 8.0f);
    std::uniform_real_distribution<float> lifetimeDist(0.3f, 0.8f);
    
    for (int i = 0; i < particleCount; ++i) {
        const float angle = angleDist(gen);
        const float speed = speedDist(gen);
        const float vx = std::cos(angle) * speed;
        const float vy = std::sin(angle) * speed;
        const float size = sizeDist(gen);
        const float lifetime = lifetimeDist(gen);
        
        CreateParticle(x, y, vx, vy, r, g, b, 255, size, lifetime);
    }
    
    LOG_DEBUG("Created explosion at ({}, {}) with {} particles", x, y, particleCount);
}

std::vector<entt::entity> EntityFactory::CreateBoard(int rows, int cols, int gemTypes)
{
    std::vector<entt::entity> entities;
    entities.reserve(rows * cols);
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> typeDist(0, gemTypes - 1);
    
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            const auto type = static_cast<Components::GemType>(typeDist(gen));
            auto entity = CreateGem(row, col, type);
            entities.push_back(entity);
        }
    }
    
    LOG_INFO("Created board: {}x{} = {} gems", rows, cols, entities.size());
    return entities;
}

} // namespace Match3
