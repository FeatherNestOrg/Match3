#include "MenuScene.hpp"
#include "GameScene.hpp"
#include "SettingsScene.hpp"
#include "SceneManager.hpp"
#include "Core/Logger.hpp"
#include "Core/Config.hpp"
#include "Render/Renderer.hpp"
#include "Render/FontRenderer.hpp"
#include "UI/UIManager.hpp"
#include "UI/Components/Button.hpp"
#include "UI/Components/Label.hpp"
#include "UI/Components/Panel.hpp"
#include "Display/DisplayManager.hpp"
#include <SDL3/SDL.h>

namespace Match3
{
    MenuScene::MenuScene(Renderer* renderer, FontRenderer* fontRenderer,
                         SceneManager* sceneManager, Display::DisplayManager* displayManager,
                         int windowWidth, int windowHeight)
        : m_renderer(renderer)
          , m_fontRenderer(fontRenderer)
          , m_sceneManager(sceneManager)
          , m_displayManager(displayManager)
          , m_uiManager(std::make_unique<UIManager>())
          , m_windowWidth(windowWidth)
          , m_windowHeight(windowHeight)
          , m_shouldExit(false)
    {
        m_uiManager->SetFontRenderer(m_fontRenderer);
    }

    MenuScene::~MenuScene() = default;

    void MenuScene::OnEnter()
    {
        LOG_INFO("MenuScene: Entering");
        CreateMenuUI();
    }

    void MenuScene::OnExit()
    {
        LOG_INFO("MenuScene: Exiting");
        m_uiManager.reset();
    }

    void MenuScene::Update(float deltaTime)
    {
        if (m_uiManager)
        {
            m_uiManager->Update(deltaTime);
        }

        // Handle exit flag from button callback
        if (m_shouldExit)
        {
            // We can't directly exit from here, but we can pop all scenes
            // which will cause the game to exit when the scene stack is empty
            while (m_sceneManager && !m_sceneManager->IsEmpty())
            {
                m_sceneManager->PopScene();
            }
        }
    }

    void MenuScene::Render()
    {
        // 清空屏幕（使用深色背景）
        m_renderer->Clear(20, 20, 30, 255);

        // 渲染 UI
        if (m_uiManager)
        {
            m_uiManager->Render(m_renderer);
        }

        m_renderer->Present();
    }

    bool MenuScene::HandleMouseClick(int x, int y)
    {
        // UI Manager 会处理所有 UI 交互
        // 这里只是返回 true 表示事件被处理
        return false;
    }

    bool MenuScene::HandleMouseMove(int x, int y)
    {
        if (m_uiManager)
        {
            m_uiManager->HandleMouseMove(x, y);
        }
        return false;
    }

    bool MenuScene::HandleMouseDown(int x, int y)
    {
        if (m_uiManager)
        {
            m_uiManager->HandleMouseDown(x, y);
        }
        return false;
    }

    bool MenuScene::HandleMouseUp(int x, int y)
    {
        if (m_uiManager)
        {
            m_uiManager->HandleMouseUp(x, y);
        }
        return false;
    }

    bool MenuScene::HandleKeyPress(int key)
    {
        if (key == SDLK_ESCAPE)
        {
            LOG_INFO("ESC pressed in menu - setting exit flag");
            m_shouldExit = true;
            return true;
        }
        return false;
    }

    void MenuScene::CreateMenuUI()
    {
        LOG_INFO("MenuScene: Creating menu UI");

        // 创建标题面板
        auto titlePanel = std::make_shared<Panel>(0, 0, m_windowWidth, 120);
        titlePanel->SetColor(30, 30, 50, 255);
        titlePanel->SetBorderEnabled(true);
        titlePanel->SetBorderColor(80, 80, 120, 255);
        titlePanel->SetId("title_panel");
        titlePanel->SetZOrder(0);
        m_uiManager->AddComponent(titlePanel);

        // 创建游戏标题
        auto titleLabel = std::make_shared<Label>(m_windowWidth / 2, 40, "消消乐", "title");
        titleLabel->SetColor(255, 255, 255, 255);
        titleLabel->SetAlignment(TextAlign::Center);
        titleLabel->SetFontRenderer(m_fontRenderer);
        titleLabel->SetId("title_label");
        titleLabel->SetZOrder(1);
        m_uiManager->AddComponent(titleLabel);

        // 计算按钮位置（居中）
        const int buttonWidth = 300;
        const int buttonHeight = 60;
        const int buttonSpacing = 80;
        const int startY = 200;
        const int centerX = m_windowWidth / 2 - buttonWidth / 2;

        // 开始游戏按钮
        auto startButton = std::make_shared<Button>(
            centerX, startY, buttonWidth, buttonHeight, "开始游戏", "default");
        startButton->SetNormalColor(60, 120, 60, 255);
        startButton->SetHoverColor(80, 150, 80, 255);
        startButton->SetPressedColor(40, 100, 40, 255);
        startButton->SetFontRenderer(m_fontRenderer);
        startButton->SetId("start_button");
        startButton->SetZOrder(2);
        startButton->SetOnClick([this]()
        {
            LOG_INFO("Start Game button clicked - switching to GameScene");
            // 切换到游戏场景
            m_sceneManager->ChangeScene(
                std::make_unique<GameScene>(m_renderer, m_fontRenderer,
                                            m_sceneManager, m_displayManager, m_windowWidth, m_windowHeight));
        });
        m_uiManager->AddComponent(startButton);

        // 设置按钮
        auto settingsButton = std::make_shared<Button>(
            centerX, startY + buttonSpacing, buttonWidth, buttonHeight, "设置", "default");
        settingsButton->SetNormalColor(80, 80, 120, 255);
        settingsButton->SetHoverColor(100, 100, 150, 255);
        settingsButton->SetPressedColor(60, 60, 100, 255);
        settingsButton->SetFontRenderer(m_fontRenderer);
        settingsButton->SetId("settings_button");
        settingsButton->SetZOrder(2);
        settingsButton->SetOnClick([this]()
        {
            LOG_INFO("Settings button clicked - opening settings scene");
            m_sceneManager->PushScene(
                std::make_unique<SettingsScene>(m_renderer, m_fontRenderer,
                                                m_sceneManager, m_displayManager,
                                                m_windowWidth, m_windowHeight));
        });
        m_uiManager->AddComponent(settingsButton);

        // 退出按钮
        auto exitButton = std::make_shared<Button>(
            centerX, startY + buttonSpacing * 2, buttonWidth, buttonHeight, "退出", "default");
        exitButton->SetNormalColor(120, 60, 60, 255);
        exitButton->SetHoverColor(150, 80, 80, 255);
        exitButton->SetPressedColor(100, 40, 40, 255);
        exitButton->SetFontRenderer(m_fontRenderer);
        exitButton->SetId("exit_button");
        exitButton->SetZOrder(2);
        exitButton->SetOnClick([this]()
        {
            LOG_INFO("Exit button clicked - setting exit flag");
            m_shouldExit = true;
        });
        m_uiManager->AddComponent(exitButton);

        // 底部信息面板
        auto infoPanel = std::make_shared<Panel>(0, m_windowHeight - 60, m_windowWidth, 60);
        infoPanel->SetColor(30, 30, 50, 255);
        infoPanel->SetBorderEnabled(true);
        infoPanel->SetBorderColor(80, 80, 120, 255);
        infoPanel->SetId("info_panel");
        infoPanel->SetZOrder(0);
        m_uiManager->AddComponent(infoPanel);

        // 信息标签
        auto infoLabel = std::make_shared<Label>(
            20, m_windowHeight - 30, "主菜单 - 按 ESC 退出", "small");
        infoLabel->SetColor(200, 200, 200, 255);
        infoLabel->SetFontRenderer(m_fontRenderer);
        infoLabel->SetId("info_label");
        infoLabel->SetZOrder(1);
        m_uiManager->AddComponent(infoLabel);

        LOG_INFO("MenuScene: Menu UI created successfully");
    }
} // namespace Match3
