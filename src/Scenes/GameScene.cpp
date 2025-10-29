#include "GameScene.hpp"
#include "MenuScene.hpp"
#include "SceneManager.hpp"
#include "Core/Logger.hpp"
#include "Core/Config.hpp"
#include "Render/Renderer.hpp"
#include "Render/FontRenderer.hpp"
#include "Managers/GameStateManager.hpp"
#include "Input/MouseHandler.hpp"
#include "UI/UIManager.hpp"
#include "UI/Components/Button.hpp"
#include "UI/Components/Label.hpp"
#include "UI/Components/Panel.hpp"
#include <Display/DisplayManager.hpp>
#include <SDL3/SDL.h>

namespace Match3
{
    GameScene::GameScene(Renderer* renderer, FontRenderer* fontRenderer,
                         SceneManager* sceneManager, Display::DisplayManager* displayManager, int windowWidth,
                         int windowHeight)
        : m_renderer(renderer)
          , m_fontRenderer(fontRenderer)
          , m_sceneManager(sceneManager)
          , m_displayManager(displayManager)
          , m_gameState(std::make_unique<GameStateManager>(renderer))
          , m_uiManager(std::make_unique<UIManager>())
          , m_windowWidth(windowWidth)
          , m_windowHeight(windowHeight)
    {
        m_uiManager->SetFontRenderer(m_fontRenderer);
    }

    GameScene::~GameScene() = default;

    void GameScene::OnEnter()
    {
        LOG_INFO("GameScene: Entering");

        // 初始化游戏状态
        m_gameState->Initialize(Config::BOARD_ROWS, Config::BOARD_COLS, Config::GEM_TYPES);

        CreateGameUI();
    }

    void GameScene::OnExit()
    {
        LOG_INFO("GameScene: Exiting");
        m_gameState.reset();
        m_uiManager.reset();
    }

    void GameScene::Update(float deltaTime)
    {
        // 更新游戏状态
        if (m_gameState)
        {
            m_gameState->Update(deltaTime);
        }

        // 更新 UI
        if (m_uiManager)
        {
            m_uiManager->Update(deltaTime);
        }
    }

    void GameScene::Render()
    {
        // 清空屏幕
        m_renderer->Clear(Config::BG_COLOR.r, Config::BG_COLOR.g,
                          Config::BG_COLOR.b, Config::BG_COLOR.a);

        // 绘制棋盘网格
        m_renderer->SetDrawColor(Config::GRID_COLOR.r, Config::GRID_COLOR.g,
                                 Config::GRID_COLOR.b, Config::GRID_COLOR.a);

        // 绘制垂直线
        for (int col = 0; col <= Config::BOARD_COLS; ++col)
        {
            const int x = Config::BOARD_OFFSET_X + col * Config::GEM_SIZE;
            m_renderer->DrawLine(x, Config::BOARD_OFFSET_Y,
                                 x, Config::BOARD_OFFSET_Y + Config::BOARD_ROWS * Config::GEM_SIZE);
        }

        // 绘制水平线
        for (int row = 0; row <= Config::BOARD_ROWS; ++row)
        {
            const int y = Config::BOARD_OFFSET_Y + row * Config::GEM_SIZE;
            m_renderer->DrawLine(Config::BOARD_OFFSET_X, y,
                                 Config::BOARD_OFFSET_X + Config::BOARD_COLS * Config::GEM_SIZE, y);
        }

        // 渲染游戏内容
        if (m_gameState)
        {
            m_gameState->Render();
        }

        // 渲染 UI
        if (m_uiManager)
        {
            m_uiManager->Render(m_renderer);
        }

        m_renderer->Present();
    }

    bool GameScene::HandleMouseClick(int x, int y)
    {
        // 转换鼠标坐标到棋盘坐标
        auto boardPos = MouseHandler::ScreenToBoard(x, y);
        if (boardPos.has_value())
        {
            auto [row, col] = boardPos.value();
            LOG_DEBUG("Click at board position: ({}, {})", row, col);

            // 将点击传递给游戏状态管理器
            if (m_gameState)
            {
                m_gameState->HandleClick(row, col);
                return true;
            }
        }
        return false;
    }

    bool GameScene::HandleMouseMove(int x, int y)
    {
        if (m_uiManager)
        {
            return m_uiManager->HandleMouseMove(x, y);
        }
        return false;
    }

    bool GameScene::HandleMouseDown(int x, int y)
    {
        if (m_uiManager)
        {
            return m_uiManager->HandleMouseDown(x, y);
        }
        return false;
    }

    bool GameScene::HandleMouseUp(int x, int y)
    {
        // First, let UI handle the event
        if (m_uiManager && m_uiManager->HandleMouseUp(x, y))
        {
            // UI consumed the event
            return true;
        }

        // UI didn't consume the event, check if it's a board click
        return HandleMouseClick(x, y);
    }

    bool GameScene::HandleKeyPress(int key)
    {
        if (key == SDLK_ESCAPE)
        {
            LOG_INFO("ESC pressed in game - returning to menu");
            m_sceneManager->ChangeScene(
                std::make_unique<MenuScene>(m_renderer, m_fontRenderer,
                                            m_sceneManager, m_displayManager, m_windowWidth, m_windowHeight));
            return true;
        }
        else if (key == SDLK_R)
        {
            LOG_INFO("R pressed - restarting game");
            m_gameState->Initialize(Config::BOARD_ROWS, Config::BOARD_COLS, Config::GEM_TYPES);
            return true;
        }
        return false;
    }

    void GameScene::CreateGameUI()
    {
        LOG_INFO("GameScene: Creating game UI");

        // 创建顶部 HUD 面板
        auto hudPanel = std::make_shared<Panel>(0, 0, m_windowWidth, 80);
        hudPanel->SetColor(40, 40, 60, 255);
        hudPanel->SetBorderEnabled(true);
        hudPanel->SetBorderColor(100, 100, 150, 255);
        hudPanel->SetId("hud_panel");
        hudPanel->SetZOrder(0);
        m_uiManager->AddComponent(hudPanel);

        // 创建分数标签（左侧）
        auto scoreLabel = std::make_shared<Label>(20, 30, "分数: 0", "default");
        scoreLabel->SetColor(255, 255, 255, 255);
        scoreLabel->SetFontRenderer(m_fontRenderer);
        scoreLabel->SetId("score_label");
        scoreLabel->SetZOrder(1);
        m_uiManager->AddComponent(scoreLabel);

        // 创建返回菜单按钮（右上角）
        auto menuButton = std::make_shared<Button>(
            m_windowWidth - 150, 15, 130, 50, "菜单", "default");
        menuButton->SetNormalColor(80, 80, 120, 255);
        menuButton->SetHoverColor(100, 100, 150, 255);
        menuButton->SetPressedColor(60, 60, 100, 255);
        menuButton->SetFontRenderer(m_fontRenderer);
        menuButton->SetId("menu_button");
        menuButton->SetZOrder(2);
        menuButton->SetOnClick([this]()
        {
            LOG_INFO("Menu button clicked - returning to menu");
            m_sceneManager->ChangeScene(
                std::make_unique<MenuScene>(m_renderer, m_fontRenderer,
                                            m_sceneManager, m_displayManager, m_windowWidth, m_windowHeight));
        });
        m_uiManager->AddComponent(menuButton);

        // 创建底部信息面板
        auto infoPanel = std::make_shared<Panel>(0, m_windowHeight - 60, m_windowWidth, 60);
        infoPanel->SetColor(40, 40, 60, 255);
        infoPanel->SetBorderEnabled(true);
        infoPanel->SetBorderColor(100, 100, 150, 255);
        infoPanel->SetId("info_panel");
        infoPanel->SetZOrder(0);
        m_uiManager->AddComponent(infoPanel);

        // 创建信息标签
        auto infoLabel = std::make_shared<Label>(
            20, m_windowHeight - 30, "游戏中 - ESC 返回菜单 | R 重新开始", "small");
        infoLabel->SetColor(200, 200, 200, 255);
        infoLabel->SetFontRenderer(m_fontRenderer);
        infoLabel->SetId("info_label");
        infoLabel->SetZOrder(1);
        m_uiManager->AddComponent(infoLabel);

        LOG_INFO("GameScene: Game UI created successfully");
    }

    void GameScene::HandleWindowResize(int width, int height)
    {
        LOG_INFO("GameScene: Handling window resize to {}x{}", width, height);
        m_windowWidth = width;
        m_windowHeight = height;
        
        // Recreate UI with new dimensions
        m_uiManager.reset();
        m_uiManager = std::make_unique<UIManager>();
        m_uiManager->SetFontRenderer(m_fontRenderer);
        CreateGameUI();
    }

} // namespace Match3
