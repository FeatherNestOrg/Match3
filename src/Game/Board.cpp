#include "Board.hpp"
#include "Core/Config.hpp"
#include "Core/Logger.hpp"
#include <algorithm>
#include <random>

namespace Match3
{
    Board::Board(const int rows, const int cols, const int gemTypes)
        : m_rows(rows)
          , m_cols(cols)
          , m_gemTypes(gemTypes)
    {
        m_board.resize(rows, std::vector<Gem>(cols));
    }

    bool Board::Initialize()
    {
        LOG_INFO("Initializing board {}x{} with {} gem types", m_rows, m_cols, m_gemTypes);

        // 初始化所有宝石位置
        for (int row = 0; row < m_rows; ++row)
        {
            for (int col = 0; col < m_cols; ++col)
            {
                m_board[row][col] = Gem(row, col, GemType::Empty);
                GenerateGemAt(row, col);
            }
        }

        // 确保初始没有匹配
        auto matches = DetectMatches();
        int attempts = 0;
        const int maxAttempts = 100;

        while (!matches.empty() && attempts < maxAttempts)
        {
            // 重新生成匹配的宝石
            for (const auto& match : matches)
            {
                for (const auto& [row, col] : match.positions)
                {
                    GenerateGemAt(row, col);
                }
            }
            matches = DetectMatches();
            ++attempts;
        }

        if (attempts >= maxAttempts)
        {
            LOG_WARN("Board initialization took {} attempts", attempts);
        }

        LOG_INFO("Board initialized successfully");
        return true;
    }

    void Board::Reset()
    {
        Initialize();
    }

    const Gem& Board::GetGem(const int row, const int col) const
    {
        return m_board[row][col];
    }

    Gem& Board::GetGem(const int row, const int col)
    {
        return m_board[row][col];
    }

    bool Board::SwapGems(const int row1, const int col1, const int row2, const int col2)
    {
        // 检查位置是否有效
        if (row1 < 0 || row1 >= m_rows || col1 < 0 || col1 >= m_cols ||
            row2 < 0 || row2 >= m_rows || col2 < 0 || col2 >= m_cols)
        {
            return false;
        }

        // 检查是否相邻
        if (!AreAdjacent(row1, col1, row2, col2))
        {
            return false;
        }

        // 执行交换
        std::swap(m_board[row1][col1], m_board[row2][col2]);

        // 更新位置信息
        m_board[row1][col1].SetPosition(row1, col1);
        m_board[row2][col2].SetPosition(row2, col2);

        return true;
    }

    bool Board::AreAdjacent(const int row1, const int col1, const int row2, const int col2) const
    {
        const int rowDiff = std::abs(row1 - row2);
        const int colDiff = std::abs(col1 - col2);

        // 相邻的定义：横向或纵向相差1，且不能对角线
        return (rowDiff == 1 && colDiff == 0) || (rowDiff == 0 && colDiff == 1);
    }

    std::vector<Match> Board::DetectMatches() const
    {
        return MatchDetector::DetectMatches(m_board, m_rows, m_cols);
    }

    int Board::RemoveMatches(const std::vector<Match>& matches)
    {
        int removedCount = 0;

        for (const auto& match : matches)
        {
            for (const auto& [row, col] : match.positions)
            {
                if (!m_board[row][col].IsEmpty())
                {
                    m_board[row][col].SetType(GemType::Empty);
                    m_board[row][col].SetState(GemState::Eliminating);
                    ++removedCount;
                }
            }
        }

        return removedCount;
    }

    bool Board::ApplyGravity()
    {
        bool hasMoved = false;

        // 从下往上处理每一列
        for (int col = 0; col < m_cols; ++col)
        {
            // 从底部开始向上扫描
            int writePos = m_rows - 1; // 写入位置

            for (int row = m_rows - 1; row >= 0; --row)
            {
                if (!m_board[row][col].IsEmpty())
                {
                    if (row != writePos)
                    {
                        // 移动宝石
                        m_board[writePos][col] = m_board[row][col];
                        m_board[writePos][col].SetPosition(writePos, col);
                        m_board[writePos][col].SetState(GemState::Falling);

                        // 清空原位置
                        m_board[row][col].SetType(GemType::Empty);
                        m_board[row][col].SetState(GemState::Idle);

                        hasMoved = true;
                    }
                    --writePos;
                }
            }
        }

        return hasMoved;
    }

    int Board::FillEmptySlots()
    {
        int filledCount = 0;

        for (int row = 0; row < m_rows; ++row)
        {
            for (int col = 0; col < m_cols; ++col)
            {
                if (m_board[row][col].IsEmpty())
                {
                    m_board[row][col].RandomizeType(m_gemTypes);
                    m_board[row][col].SetPosition(row, col);
                    m_board[row][col].SetState(GemState::Falling);
                    ++filledCount;
                }
            }
        }

        return filledCount;
    }

    bool Board::HasPossibleMoves() const
    {
        // 尝试所有可能的交换
        for (int row = 0; row < m_rows; ++row)
        {
            for (int col = 0; col < m_cols; ++col)
            {
                // 尝试向右交换
                if (col < m_cols - 1)
                {
                    if (WouldSwapCreateMatch(row, col, row, col + 1))
                    {
                        return true;
                    }
                }

                // 尝试向下交换
                if (row < m_rows - 1)
                {
                    if (WouldSwapCreateMatch(row, col, row + 1, col))
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    std::optional<std::tuple<int, int, int, int>> Board::GetHint() const
    {
        // 查找第一个可以产生匹配的移动
        for (int row = 0; row < m_rows; ++row)
        {
            for (int col = 0; col < m_cols; ++col)
            {
                // 尝试向右交换
                if (col < m_cols - 1)
                {
                    if (WouldSwapCreateMatch(row, col, row, col + 1))
                    {
                        return std::make_tuple(row, col, row, col + 1);
                    }
                }

                // 尝试向下交换
                if (row < m_rows - 1)
                {
                    if (WouldSwapCreateMatch(row, col, row + 1, col))
                    {
                        return std::make_tuple(row, col, row + 1, col);
                    }
                }
            }
        }

        return std::nullopt;
    }

    void Board::GenerateGemAt(const int row, const int col)
    {
        std::vector<GemType> availableTypes;

        // 收集所有可用类型
        for (int i = 0; i < m_gemTypes; ++i)
        {
            availableTypes.push_back(static_cast<GemType>(i));
        }

        // 排除可能导致初始匹配的类型
        std::vector<GemType> excludeTypes;

        // 检查左边两个宝石
        if (col >= 2 &&
            m_board[row][col - 1].GetType() == m_board[row][col - 2].GetType() &&
            !m_board[row][col - 1].IsEmpty())
        {
            excludeTypes.push_back(m_board[row][col - 1].GetType());
        }

        // 检查上边两个宝石
        if (row >= 2 && m_board[row - 1][col].GetType() == m_board[row - 2][col].GetType() &&
            !m_board[row - 1][col].IsEmpty())
        {
            excludeTypes.push_back(m_board[row - 1][col].GetType());
        }

        // 移除被排除的类型
        availableTypes.erase(
            std::remove_if(availableTypes.begin(), availableTypes.end(),
                           [&excludeTypes](GemType type)
                           {
                               return std::find(excludeTypes.begin(), excludeTypes.end(), type) != excludeTypes.end();
                           }),
            availableTypes.end()
        );

        // 如果没有可用类型，使用所有类型
        if (availableTypes.empty())
        {
            for (int i = 0; i < m_gemTypes; ++i)
            {
                availableTypes.push_back(static_cast<GemType>(i));
            }
        }

        // 随机选择一个类型
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, static_cast<int>(availableTypes.size()) - 1);

        const GemType selectedType = availableTypes[dis(gen)];
        m_board[row][col].SetType(selectedType);
        m_board[row][col].SetState(GemState::Idle);
    }

    bool Board::WouldSwapCreateMatch(const int row1, const int col1, const int row2, const int col2) const
    {
        // 创建临时棋盘副本
        auto tempBoard = m_board;

        // 执行交换
        std::swap(tempBoard[row1][col1], tempBoard[row2][col2]);
        tempBoard[row1][col1].SetPosition(row1, col1);
        tempBoard[row2][col2].SetPosition(row2, col2);

        // 检查两个位置是否有匹配
        const bool hasMatch1 = MatchDetector::HasMatchAt(tempBoard, row1, col1, m_rows, m_cols);
        const bool hasMatch2 = MatchDetector::HasMatchAt(tempBoard, row2, col2, m_rows, m_cols);

        return hasMatch1 || hasMatch2;
    }
} // namespace Match3

