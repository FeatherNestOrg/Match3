#include "MatchDetector.hpp"
#include "Core/Config.hpp"

namespace Match3
{
    std::vector<Match> MatchDetector::DetectMatches(
        const std::vector<std::vector<Gem>>& board,
        const int rows,
        const int cols)
    {
        std::vector<Match> allMatches;

        // 检测横向匹配
        auto horizontalMatches = DetectHorizontalMatches(board, rows, cols);
        allMatches.insert(allMatches.end(), horizontalMatches.begin(), horizontalMatches.end());

        // 检测纵向匹配
        auto verticalMatches = DetectVerticalMatches(board, rows, cols);
        allMatches.insert(allMatches.end(), verticalMatches.begin(), verticalMatches.end());

        return allMatches;
    }

    bool MatchDetector::HasMatchAt(
        const std::vector<std::vector<Gem>>& board,
        const int row,
        const int col,
        const int rows,
        const int cols)
    {
        if (row < 0 || row >= rows || col < 0 || col >= cols)
            return false;

        const auto& gem = board[row][col];
        if (gem.IsEmpty())
            return false;

        const GemType type = gem.GetType();

        // 检测横向匹配（当前位置左右）
        int horizontalCount = 1;

        // 向左检测
        for (int c = col - 1; c >= 0 && board[row][c].GetType() == type; --c)
            ++horizontalCount;
        // 向右检测
        for (int c = col + 1; c < cols && board[row][c].GetType() == type; ++c)
            ++horizontalCount;

        if (horizontalCount >= Config::MIN_MATCH_COUNT)
            return true;

        // 检测纵向匹配（当前位置上下）
        int verticalCount = 1;

        // 向上检测
        for (int r = row - 1; r >= 0 && board[r][col].GetType() == type; --r)
            ++verticalCount;

        // 向下检测
        for (int r = row + 1; r < rows && board[r][col].GetType() == type; ++r)
            ++verticalCount;

        return verticalCount >= Config::MIN_MATCH_COUNT;
    }

    std::vector<Match> MatchDetector::DetectHorizontalMatches(
        const std::vector<std::vector<Gem>>& board,
        const int rows,
        const int cols)
    {
        std::vector<Match> matches;

        for (int row = 0; row < rows; ++row)
        {
            int matchStart = 0;
            GemType currentType = board[row][0].GetType();
            int matchLength = 1;

            for (int col = 1; col <= cols; ++col)
            {
                const GemType nextType = (col < cols) ? board[row][col].GetType() : GemType::Empty;

                if (nextType == currentType && !board[row][col - 1].IsEmpty())
                {
                    ++matchLength;
                }
                else
                {
                    // 检查是否形成匹配
                    if (matchLength >= Config::MIN_MATCH_COUNT && currentType != GemType::Empty)
                    {
                        Match match(currentType, true);
                        for (int i = matchStart; i < matchStart + matchLength; ++i)
                        {
                            match.positions.emplace_back(row, i);
                        }
                        matches.push_back(match);
                    }

                    // 重置计数
                    matchStart = col;
                    currentType = nextType;
                    matchLength = 1;
                }
            }
        }

        return matches;
    }

    std::vector<Match> MatchDetector::DetectVerticalMatches(
        const std::vector<std::vector<Gem>>& board,
        const int rows,
        const int cols)
    {
        std::vector<Match> matches;

        for (int col = 0; col < cols; ++col)
        {
            int matchStart = 0;
            GemType currentType = board[0][col].GetType();
            int matchLength = 1;

            for (int row = 1; row <= rows; ++row)
            {
                const GemType nextType = (row < rows) ? board[row][col].GetType() : GemType::Empty;

                if (nextType == currentType && !board[row - 1][col].IsEmpty())
                {
                    ++matchLength;
                }
                else
                {
                    // 检查是否形成匹配
                    if (matchLength >= Config::MIN_MATCH_COUNT && currentType != GemType::Empty)
                    {
                        Match match(currentType, false);
                        for (int i = matchStart; i < matchStart + matchLength; ++i)
                        {
                            match.positions.emplace_back(i, col);
                        }
                        matches.push_back(match);
                    }

                    // 重置计数
                    matchStart = row;
                    currentType = nextType;
                    matchLength = 1;
                }
            }
        }

        return matches;
    }
} // namespace Match3
