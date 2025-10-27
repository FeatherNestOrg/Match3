#include "Gem.hpp"
#include <random>

namespace Match3
{
    Gem::Gem(const int row, const int col, const GemType type)
        : m_row(row)
          , m_col(col)
          , m_type(type)
          , m_state(GemState::Idle)
    {
    }

    void Gem::Reset()
    {
        m_type = GemType::Empty;
        m_state = GemState::Idle;
    }

    void Gem::RandomizeType(const int maxType)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, maxType - 1);
        m_type = static_cast<GemType>(dis(gen));
        m_state = GemState::Idle;
    }
} // namespace Match3


