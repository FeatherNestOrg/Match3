#include "GameStateManager.hpp"
#include "Core/Logger.hpp"
#include "Core/Config.hpp"
#include <numeric>
#include <cmath>

#include "Systems/LifetimeSystem.hpp"
#include "Systems/ParticleSystem.hpp"

namespace Match3
{
    GameStateManager::GameStateManager(Renderer* renderer)
        : m_renderer(renderer)
    {
        m_factory = std::make_unique<EntityFactory>(m_registry);
        m_systemManager = std::make_unique<SystemManager>();
    }

    GameStateManager::~GameStateManager() = default;

    void GameStateManager::Initialize(int rows, int cols, int gemTypes)
    {
        LOG_INFO("GameStateManager: Initializing {}x{} with {} gem types", rows, cols, gemTypes);

        m_rows = rows;
        m_cols = cols;
        m_gemTypes = gemTypes;

        // 创建所有系统
        auto boardSystem = std::make_shared<Systems::BoardSystem>(rows, cols, *m_factory);
        auto matchSystem = std::make_shared<Systems::MatchDetectionSystem>(*boardSystem);
        auto swapSystem = std::make_shared<Systems::SwapSystem>(*boardSystem, *matchSystem);
        auto animSystem = std::make_shared<Systems::AnimationSystem>();
        auto particleSystem = std::make_shared<Systems::ParticleSystem>();
        auto lifetimeSystem = std::make_shared<Systems::LifetimeSystem>();
        auto renderSystem = std::make_shared<Systems::RenderSystem>(m_renderer);

        // 保存系统引用（快速访问）
        m_boardSystem = boardSystem.get();
        m_matchSystem = matchSystem.get();
        m_swapSystem = swapSystem.get();
        m_animSystem = animSystem.get();
        m_renderSystem = renderSystem.get(); // 保存RenderSystem引用

        // 设置回调
        m_swapSystem->SetSwapCallback([this](bool valid)
        {
            OnSwapComplete(valid);
        });

        // 添加系统到管理器（按执行顺序）
        m_systemManager->AddSystem(animSystem);
        m_systemManager->AddSystem(particleSystem);
        m_systemManager->AddSystem(lifetimeSystem);
        m_systemManager->AddSystem(boardSystem);
        m_systemManager->AddSystem(matchSystem);
        m_systemManager->AddSystem(swapSystem);

        // 将renderSystem添加到管理器但不参与UpdateAll
        // RenderSystem只在Render阶段手动调用
        m_systemManager->AddSystemNoUpdate(renderSystem);

        // 初始化棋盘
        m_boardSystem->InitializeBoard(m_registry, gemTypes);

        LOG_INFO("GameStateManager: Initialized with {} systems", m_systemManager->GetSystemCount());

        StartNewGame();
    }

    void GameStateManager::Update(float deltaTime)
    {
        // 更新状态计时器
        m_stateTimer += deltaTime;

        // 根据当前状态更新
        switch (m_currentState)
        {
        case ECSPlayState::Idle:
            UpdateIdleState(deltaTime);
            break;
        case ECSPlayState::Swapping:
            UpdateSwappingState(deltaTime);
            break;
        case ECSPlayState::Matching:
            UpdateMatchingState(deltaTime);
            break;
        case ECSPlayState::Eliminating:
            UpdateEliminatingState(deltaTime);
            break;
        case ECSPlayState::Falling:
            UpdateFallingState(deltaTime);
            break;
        case ECSPlayState::Filling:
            UpdateFillingState(deltaTime);
            break;
        case ECSPlayState::GameOver:
        case ECSPlayState::Paused:
            // 这些状态不自动更新
            break;
        }

        // 更新所有系统
        m_systemManager->UpdateAll(m_registry, deltaTime);
    }

    void GameStateManager::Render()
    {
        // 在Render阶段调用RenderSystem
        if (m_renderSystem)
        {
            m_renderSystem->Update(m_registry, 0.0f);
        }
    }

    void GameStateManager::HandleClick(int row, int col)
    {
        // 只在Idle状态处理点击
        if (m_currentState != ECSPlayState::Idle)
        {
            return;
        }

        if (!m_boardSystem->IsValidPosition(row, col))
        {
            return;
        }

        if (!m_hasSelection)
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
                TrySwap(row, col);
            }
        }
    }

    void GameStateManager::StartNewGame()
    {
        LOG_INFO("GameStateManager: Starting new game");

        m_score = 0;
        m_moves = 0;
        m_combo = 0;

        ClearSelection();
        SetState(ECSPlayState::Idle);

        LOG_INFO("GameStateManager: New game started");
    }

    void GameStateManager::Reset()
    {
        LOG_INFO("GameStateManager: Resetting game");

        // 清除所有实体
        m_registry.clear();

        // 重新初始化棋盘
        m_boardSystem->InitializeBoard(m_registry, m_gemTypes);

        StartNewGame();
    }

    void GameStateManager::UpdateIdleState(float dt)
    {
        // 在Idle状态，等待玩家输入
        // 这里可以添加提示系统、无操作超时等逻辑
    }

    void GameStateManager::UpdateSwappingState(float dt)
    {
        // 等待交换动画完成
        // SwapSystem会通过回调通知完成
    }

    void GameStateManager::UpdateMatchingState(float dt)
    {
        // 检测匹配
        auto matches = m_matchSystem->DetectMatches(m_registry);

        if (matches.empty())
        {
            // 无匹配，回到Idle
            LOG_DEBUG("GameStateManager: No matches found, back to Idle");
            m_combo = 0;
            SetState(ECSPlayState::Idle);
        }
        else
        {
            // 有匹配，进入消除状态
            LOG_INFO("GameStateManager: Found {} matches with {} gems (Combo: {})",
                     matches.size(),
                     std::accumulate(matches.begin(), matches.end(), 0,
                         [](int sum, const Systems::MatchGroup& m) { return sum + m.gems.size(); }),
                     m_combo + 1);

            ++m_combo;

            // 标记匹配的宝石
            m_matchSystem->MarkMatches(m_registry, matches);

            // 计算得分
            int totalGems = 0;
            for (const auto& match : matches)
            {
                totalGems += match.gems.size();
            }
            AddScore(totalGems);

            // 开始消除动画
            for (const auto& match : matches)
            {
                for (auto entity : match.gems)
                {
                    // 检查实体是否有效
                    if (!m_registry.valid(entity))
                    {
                        continue;
                    }

                    // 添加消除动画（缩放到0 + 淡出）
                    // 使用emplace_or_replace避免重复添加导致崩溃
                    m_registry.emplace_or_replace<Components::ScaleAnimation>(entity,
                                                                              Config::ELIMINATION_DURATION, 1.0f, 0.0f,
                                                                              Components::EasingType::InQuad);

                    m_registry.emplace_or_replace<Components::FadeAnimation>(entity,
                                                                             Config::ELIMINATION_DURATION, 1.0f, 0.0f,
                                                                             Components::EasingType::Linear);

                    // 添加旋转动画
                    m_registry.emplace_or_replace<Components::RotationAnimation>(entity,
                        Config::ELIMINATION_DURATION, 0.0f, 6.28f,
                        Components::EasingType::Linear);
                }
            }

            SetState(ECSPlayState::Eliminating);
        }
    }

    void GameStateManager::UpdateEliminatingState(float dt)
    {
        // 检查消除动画是否完成
        // 如果所有Matched的宝石都没有动画组件，说明动画完成
        auto view = m_registry.view<Components::Matched>();

        bool allAnimationsComplete = true;
        for (auto entity : view)
        {
            if (m_registry.all_of<Components::ScaleAnimation>(entity) ||
                m_registry.all_of<Components::FadeAnimation>(entity) ||
                m_registry.all_of<Components::RotationAnimation>(entity))
            {
                allAnimationsComplete = false;
                break;
            }
        }

        if (allAnimationsComplete)
        {
            // 销毁匹配的宝石
            std::vector<entt::entity> toDestroy;
            for (auto entity : view)
            {
                toDestroy.push_back(entity);
            }

            for (auto entity : toDestroy)
            {
                // 从网格中移除（将位置设为null）
                if (m_registry.all_of<Components::GridPosition>(entity))
                {
                    const auto& gridPos = m_registry.get<Components::GridPosition>(entity);
                    // BoardSystem会在下次RebuildGridIndex时更新
                }

                m_registry.destroy(entity);
            }

            LOG_DEBUG("GameStateManager: Eliminated {} gems", toDestroy.size());

            // 重建网格索引
            m_boardSystem->RebuildGridIndex(m_registry);

            // 进入下落状态
            SetState(ECSPlayState::Falling);
        }
    }

    void GameStateManager::UpdateFallingState(float dt)
    {
        // 应用重力
        const int movedCount = m_boardSystem->ApplyGravity(m_registry);

        if (movedCount > 0)
        {
            LOG_DEBUG("GameStateManager: Applied gravity, moved {} gems", movedCount);

            // 为移动的宝石添加下落动画
            auto view = m_registry.view<Components::Position, Components::GridPosition>();
            for (auto entity : view)
            {
                if (!m_registry.valid(entity)) continue;

                const auto& pos = view.get<Components::Position>(entity);
                const auto& gridPos = view.get<Components::GridPosition>(entity);

                // 计算目标位置
                const float targetY = Config::BOARD_OFFSET_Y + gridPos.row * Config::GEM_SIZE
                    + Config::GEM_SIZE / 2.0f;

                // 如果位置不匹配，添加动画
                if (std::abs(pos.y - targetY) > 1.0f)
                {
                    m_registry.emplace_or_replace<Components::TweenAnimation>(entity,
                                                                              Config::FALL_DURATION,
                                                                              pos.x, pos.y,
                                                                              pos.x, targetY,
                                                                              Components::EasingType::OutBounce);
                }
            }
        }

        // 直接进入填充状态
        SetState(ECSPlayState::Filling);
    }

    void GameStateManager::UpdateFillingState(float dt)
    {
        // 填充空位
        const int filledCount = m_boardSystem->FillEmptySlots(m_registry, m_gemTypes);

        if (filledCount > 0)
        {
            LOG_DEBUG("GameStateManager: Filled {} empty slots", filledCount);

            // 为新宝石添加出现动画
            auto view = m_registry.view<Components::Position, Components::GridPosition,
                                        Components::Renderable>();
            for (auto entity : view)
            {
                if (!m_registry.valid(entity)) continue;

                auto& render = view.get<Components::Renderable>(entity);

                // 检查是否是新宝石（alpha=0, scale=0）
                if (render.a == 0 && render.scale < 0.1f)
                {
                    const auto& gridPos = view.get<Components::GridPosition>(entity);
                    const auto& pos = view.get<Components::Position>(entity);

                    // 计算目标位置
                    const float targetY = Config::BOARD_OFFSET_Y + gridPos.row * Config::GEM_SIZE
                        + Config::GEM_SIZE / 2.0f;

                    // 添加下落动画（使用emplace_or_replace避免崩溃）
                    m_registry.emplace_or_replace<Components::TweenAnimation>(entity,
                                                                              Config::FALL_DURATION,
                                                                              pos.x, pos.y,
                                                                              pos.x, targetY,
                                                                              Components::EasingType::OutBounce);

                    // 添加淡入和缩放动画
                    m_registry.emplace_or_replace<Components::FadeAnimation>(entity,
                                                                             Config::FALL_DURATION * 0.5f,
                                                                             0.0f, 1.0f,
                                                                             Components::EasingType::OutQuad);

                    m_registry.emplace_or_replace<Components::ScaleAnimation>(entity,
                                                                              Config::FALL_DURATION * 0.5f,
                                                                              0.0f, 1.0f,
                                                                              Components::EasingType::OutBack);
                }
            }
        }

        // 重新检测匹配
        SetState(ECSPlayState::Matching);
    }

    void GameStateManager::SetState(ECSPlayState newState)
    {
        if (m_currentState != newState)
        {
            LOG_DEBUG("GameStateManager: State change {} -> {}",
                      static_cast<int>(m_currentState), static_cast<int>(newState));
            m_currentState = newState;
            m_stateTimer = 0.0f;
        }
    }

    void GameStateManager::SelectGem(int row, int col)
    {
        m_selectedRow = row;
        m_selectedCol = col;
        m_hasSelection = true;

        // 添加选中动画（脉冲效果）
        auto entity = m_boardSystem->GetGemAt(row, col);
        if (entity != entt::null && m_registry.valid(entity))
        {
            // 使用emplace_or_replace避免重复添加
            m_registry.emplace_or_replace<Components::Selected>(entity, 2.0f, 0.3f);
            m_registry.emplace_or_replace<Components::PulseAnimation>(entity, 2.0f, 0.2f);
        }

        LOG_DEBUG("GameStateManager: Selected gem at ({}, {})", row, col);
    }

    void GameStateManager::ClearSelection()
    {
        if (m_hasSelection)
        {
            // 移除选中动画
            auto entity = m_boardSystem->GetGemAt(m_selectedRow, m_selectedCol);
            if (entity != entt::null && m_registry.valid(entity))
            {
                // 安全移除组件（只有存在时才移除）
                if (m_registry.all_of<Components::Selected>(entity))
                {
                    m_registry.remove<Components::Selected>(entity);
                }
                if (m_registry.all_of<Components::PulseAnimation>(entity))
                {
                    // 移除PulseAnimation前，重置scale为1.0
                    if (m_registry.all_of<Components::Renderable>(entity))
                    {
                        auto& render = m_registry.get<Components::Renderable>(entity);
                        render.scale = 1.0f;
                    }
                    m_registry.remove<Components::PulseAnimation>(entity);
                }
            }

            m_selectedRow = -1;
            m_selectedCol = -1;
            m_hasSelection = false;
        }
    }

    void GameStateManager::TrySwap(int row, int col)
    {
        // 保存当前选中的实体（因为交换后位置会变化）
        auto selectedEntity = m_boardSystem->GetGemAt(m_selectedRow, m_selectedCol);

        // 请求交换
        const bool accepted = m_swapSystem->RequestSwap(m_registry,
                                                        m_selectedRow, m_selectedCol,
                                                        row, col);

        if (accepted)
        {
            // 交换被接受，需要清除选中实体的动画（而不是位置上的实体）
            if (selectedEntity != entt::null && m_registry.valid(selectedEntity))
            {
                if (m_registry.all_of<Components::Selected>(selectedEntity))
                {
                    m_registry.remove<Components::Selected>(selectedEntity);
                }
                if (m_registry.all_of<Components::PulseAnimation>(selectedEntity))
                {
                    // 重置scale为1.0
                    if (m_registry.all_of<Components::Renderable>(selectedEntity))
                    {
                        auto& render = m_registry.get<Components::Renderable>(selectedEntity);
                        render.scale = 1.0f;
                    }
                    m_registry.remove<Components::PulseAnimation>(selectedEntity);
                }
            }

            // 清除选择状态
            m_selectedRow = -1;
            m_selectedCol = -1;
            m_hasSelection = false;

            // 进入交换状态
            SetState(ECSPlayState::Swapping);

            ++m_moves;
        }
        else
        {
            // 交换失败（不相邻等），清除选择并重新选择
            ClearSelection();
            SelectGem(row, col);
        }
    }

    void GameStateManager::ProcessMatches()
    {
        SetState(ECSPlayState::Matching);
    }

    void GameStateManager::AddScore(int matchCount)
    {
        // 基础分数：每个宝石10分
        int baseScore = matchCount * 10;

        // 连击加成
        int comboBonus = m_combo * 50;

        int totalScore = baseScore + comboBonus;
        m_score += totalScore;

        LOG_INFO("GameStateManager: Score: {} (+{}), Combo: {}", m_score, totalScore, m_combo);
    }

    void GameStateManager::OnSwapComplete(bool valid)
    {
        if (valid)
        {
            LOG_INFO("GameStateManager: Swap successful, checking matches");
            // 交换有效，检测匹配
            SetState(ECSPlayState::Matching);
        }
        else
        {
            LOG_INFO("GameStateManager: Swap invalid, reverted");
            // 交换无效，已回退，清除所有选中动画
            // 注意：此时选择状态已经被清除，需要清理所有残留的动画组件
            ClearAllSelectionAnimations();
            SetState(ECSPlayState::Idle);
        }
    }

    void GameStateManager::ClearAllSelectionAnimations()
    {
        // 清除所有Selected组件
        auto selectedView = m_registry.view<Components::Selected>();
        std::vector<entt::entity> toRemove;
        for (auto entity : selectedView)
        {
            toRemove.push_back(entity);
        }
        for (auto entity : toRemove)
        {
            if (m_registry.valid(entity))
            {
                m_registry.remove<Components::Selected>(entity);
            }
        }

        // 清除所有PulseAnimation组件，并重置scale
        auto pulseView = m_registry.view<Components::PulseAnimation>();
        toRemove.clear();
        for (auto entity : pulseView)
        {
            toRemove.push_back(entity);
        }
        for (auto entity : toRemove)
        {
            if (m_registry.valid(entity))
            {
                // 重置scale为1.0，避免宝石大小异常
                if (m_registry.all_of<Components::Renderable>(entity))
                {
                    auto& render = m_registry.get<Components::Renderable>(entity);
                    render.scale = 1.0f;
                }
                m_registry.remove<Components::PulseAnimation>(entity);
            }
        }

        LOG_DEBUG("GameStateManager: Cleared all selection animations");
    }
} // namespace Match3
