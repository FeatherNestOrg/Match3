#include "GameState.hpp"
#include "Core/Config.hpp"
#include "Core/Logger.hpp"

namespace Match3
{
    GameState::GameState()
        : m_playState(PlayState::Idle)
          , m_score(0)
          , m_moves(0)
          , m_combo(0)
          , m_selectedRow(-1)
          , m_selectedCol(-1)
          , m_stateTimer(0.0f)
    {
    }

    bool GameState::Initialize()
    {
        LOG_INFO("Initializing game state...");

        // 创建棋盘
        m_board = std::make_unique<Board>(
            Config::BOARD_ROWS,
            Config::BOARD_COLS,
            Config::GEM_TYPES
        );

        if (!m_board->Initialize())
        {
            LOG_ERROR("Failed to initialize board");
            return false;
        }

        StartNewGame();
        LOG_INFO("Game state initialized successfully");
        return true;
    }

    void GameState::Update(const float deltaTime)
    {
        if (m_playState == PlayState::Paused || m_playState == PlayState::GameOver)
        {
            return;
        }

        m_stateTimer += deltaTime;

        switch (m_playState)
        {
        case PlayState::Idle:
            // 等待玩家输入
            break;

        case PlayState::Swapping:
            // 等待交换动画完成
            if (m_stateTimer >= Config::SWAP_DURATION)
            {
                m_playState = PlayState::Matching;
                m_stateTimer = 0.0f;
            }
            break;

        case PlayState::Matching:
            // 检测并处理匹配
            if (m_stateTimer >= Config::MATCH_DELAY)
            {
                ProcessMatches();
            }
            break;

        case PlayState::Eliminating:
            // 等待消除动画完成
            if (m_stateTimer >= Config::ELIMINATION_DURATION)
            {
                m_playState = PlayState::Falling;
                m_stateTimer = 0.0f;

                // 应用重力
                m_board->ApplyGravity();
            }
            break;

        case PlayState::Falling:
            // 等待下落动画完成
            if (m_stateTimer >= Config::FALL_DURATION)
            {
                m_playState = PlayState::Filling;
                m_stateTimer = 0.0f;

                // 填充空位
                m_board->FillEmptySlots();
            }
            break;

        case PlayState::Filling:
            // 填充完成后，检查是否有新的匹配
            if (m_stateTimer >= Config::FALL_DURATION)
            {
                auto matches = m_board->DetectMatches();

                if (!matches.empty())
                {
                    // 有新匹配，继续连击
                    m_playState = PlayState::Matching;
                    m_stateTimer = 0.0f;
                }
                else
                {
                    // 没有新匹配，回到闲置状态
                    m_playState = PlayState::Idle;
                    m_combo = 0;
                    m_stateTimer = 0.0f;

                    // 检查是否还有可移动的宝石
                    CheckGameOver();
                }
            }
            break;

        default:
            break;
        }
    }

    void GameState::HandleClick(const int row, const int col)
    {
        // 只在闲置状态处理点击
        if (m_playState != PlayState::Idle)
        {
            return;
        }

        // 检查位置是否有效
        if (row < 0 || row >= Config::BOARD_ROWS || col < 0 || col >= Config::BOARD_COLS)
        {
            return;
        }

        if (!HasSelection())
        {
            // 第一次选择
            SelectGem(row, col);
        }
        else
        {
            // 第二次选择
            if (row == m_selectedRow && col == m_selectedCol)
            {
                // 点击同一个宝石，取消选择
                ClearSelection();
            }
            else
            {
                // 尝试交换
                if (!TrySwap(row, col))
                {
                    // 交换失败，重新选择
                    ClearSelection();
                    SelectGem(row, col);
                }
            }
        }
    }

    void GameState::StartNewGame()
    {
        LOG_INFO("Starting new game...");

        m_score = 0;
        m_moves = 0;
        m_combo = 0;
        m_playState = PlayState::Idle;
        m_stateTimer = 0.0f;

        ClearSelection();

        LOG_INFO("New game started");
    }

    void GameState::Reset()
    {
        m_board->Reset();
        StartNewGame();
    }

    void GameState::SelectGem(const int row, const int col)
    {
        m_selectedRow = row;
        m_selectedCol = col;
        m_board->GetGem(row, col).SetState(GemState::Selected);

        LOG_DEBUG("Selected gem at ({}, {})", row, col);
    }

    void GameState::ClearSelection()
    {
        if (HasSelection())
        {
            m_board->GetGem(m_selectedRow, m_selectedCol).SetState(GemState::Idle);
            m_selectedRow = -1;
            m_selectedCol = -1;
        }
    }

    bool GameState::TrySwap(const int row, const int col)
    {
        // 检查是否相邻
        if (!m_board->AreAdjacent(m_selectedRow, m_selectedCol, row, col))
        {
            LOG_DEBUG("Gems are not adjacent");
            return false;
        }

        // 执行交换
        if (m_board->SwapGems(m_selectedRow, m_selectedCol, row, col))
        {
            // 检查是否产生匹配
            auto matches = m_board->DetectMatches();

            if (!matches.empty())
            {
                // 有匹配，交换成功
                LOG_INFO("Swap successful, {} matches found", matches.size());

                ClearSelection();
                m_playState = PlayState::Swapping;
                m_stateTimer = 0.0f;
                ++m_moves;

                return true;
            }
            else
            {
                // 没有匹配，交换回来
                LOG_DEBUG("No matches after swap, reverting");
                m_board->SwapGems(row, col, m_selectedRow, m_selectedCol);
                return false;
            }
        }

        return false;
    }

    void GameState::ProcessMatches()
    {
        auto matches = m_board->DetectMatches();

        if (matches.empty())
        {
            m_playState = PlayState::Idle;
            m_combo = 0;
            m_stateTimer = 0.0f;
            return;
        }

        // 增加连击
        ++m_combo;

        // 计算总匹配数
        int totalMatched = 0;
        for (const auto& match : matches)
        {
            totalMatched += static_cast<int>(match.positions.size());
        }

        LOG_INFO("Found {} matches with {} gems (Combo: {})", matches.size(), totalMatched, m_combo);

        // 移除匹配的宝石
        m_board->RemoveMatches(matches);

        // 更新分数
        UpdateScore(totalMatched);

        // 进入消除状态
        m_playState = PlayState::Eliminating;
        m_stateTimer = 0.0f;
    }

    void GameState::UpdateScore(const int matchedGems)
    {
        // 基础分数
        int points = matchedGems * Config::BASE_SCORE;

        // 连击倍数
        if (m_combo > 1)
        {
            points = static_cast<int>(points * (1.0f + (m_combo - 1) * Config::COMBO_MULTIPLIER));
        }

        m_score += points;
        LOG_INFO("Score: {} (+{}), Combo: {}", m_score, points, m_combo);
    }

    void GameState::CheckGameOver()
    {
        if (!m_board->HasPossibleMoves())
        {
            LOG_WARN("No more possible moves - Game Over!");
            m_playState = PlayState::GameOver;
        }
    }
} // namespace Match3

