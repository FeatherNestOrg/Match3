#include "MouseHandler.hpp"

namespace Match3
{
    std::optional<std::pair<int, int>> MouseHandler::ScreenToBoard(const int screenX, const int screenY)
    {
        // 检查是否在棋盘范围内
        if (!IsInBoardBounds(screenX, screenY))
        {
            return std::nullopt;
        }

        // 计算棋盘坐标
        const int col = (screenX - Config::BOARD_OFFSET_X) / Config::GEM_SIZE;
        const int row = (screenY - Config::BOARD_OFFSET_Y) / Config::GEM_SIZE;

        // 验证坐标有效性
        if (!IsValidBoardPosition(row, col))
        {
            return std::nullopt;
        }

        return std::make_pair(row, col);
    }

    std::pair<int, int> MouseHandler::BoardToScreen(const int row, const int col)
    {
        const int centerX = Config::BOARD_OFFSET_X + col * Config::GEM_SIZE + Config::GEM_SIZE / 2;
        const int centerY = Config::BOARD_OFFSET_Y + row * Config::GEM_SIZE + Config::GEM_SIZE / 2;
        return {centerX, centerY};
    }

    std::tuple<int, int, int, int> MouseHandler::GetGemRect(const int row, const int col)
    {
        const int x = Config::BOARD_OFFSET_X + col * Config::GEM_SIZE;
        const int y = Config::BOARD_OFFSET_Y + row * Config::GEM_SIZE;
        return {x, y, Config::GEM_SIZE, Config::GEM_SIZE};
    }

    bool MouseHandler::IsInBoardBounds(const int screenX, const int screenY)
    {
        const int boardStartX = Config::BOARD_OFFSET_X;
        const int boardStartY = Config::BOARD_OFFSET_Y;
        const int boardEndX = boardStartX + Config::BOARD_COLS * Config::GEM_SIZE;
        const int boardEndY = boardStartY + Config::BOARD_ROWS * Config::GEM_SIZE;

        return screenX >= boardStartX && screenX < boardEndX &&
            screenY >= boardStartY && screenY < boardEndY;
    }

    bool MouseHandler::IsValidBoardPosition(const int row, const int col)
    {
        return row >= 0 && row < Config::BOARD_ROWS &&
            col >= 0 && col < Config::BOARD_COLS;
    }
} // namespace Match3

