#include "SettingsScene.hpp"
#include "SceneManager.hpp"
#include "Core/Logger.hpp"
#include "Render/Renderer.hpp"
#include "Render/FontRenderer.hpp"
#include "UI/UIManager.hpp"
#include "UI/Components/Button.hpp"
#include "UI/Components/Label.hpp"
#include "UI/Components/Panel.hpp"
#include "Display/DisplayManager.hpp"
#include "Display/DisplayMode.hpp"
#include <SDL3/SDL.h>

namespace Match3
{
    SettingsScene::SettingsScene(Renderer* renderer, FontRenderer* fontRenderer,
                                 SceneManager* sceneManager, Display::DisplayManager* displayManager,
                                 int windowWidth, int windowHeight)
        : m_renderer(renderer)
        , m_fontRenderer(fontRenderer)
        , m_sceneManager(sceneManager)
        , m_displayManager(displayManager)
        , m_uiManager(std::make_unique<UIManager>())
        , m_windowWidth(windowWidth)
        , m_windowHeight(windowHeight)
        , m_currentDisplayModeIndex(0)
        , m_currentScalingStrategyIndex(0)
        , m_settingsChanged(false)
    {
        m_uiManager->SetFontRenderer(m_fontRenderer);
    }

    SettingsScene::~SettingsScene() = default;

    void SettingsScene::OnEnter()
    {
        LOG_INFO("SettingsScene: Entering");
        
        // Get current settings from DisplayManager
        if (m_displayManager)
        {
            auto currentMode = m_displayManager->GetCurrentDisplayMode();
            m_currentDisplayModeIndex = static_cast<int>(currentMode);
            
            auto currentStrategy = m_displayManager->GetScalingStrategy();
            m_currentScalingStrategyIndex = static_cast<int>(currentStrategy);
        }
        
        CreateSettingsUI();
    }

    void SettingsScene::OnExit()
    {
        LOG_INFO("SettingsScene: Exiting");
        
        // Save settings if changed
        if (m_settingsChanged && m_displayManager)
        {
            LOG_INFO("Saving display settings");
            m_displayManager->SaveDisplaySettings();
        }
        
        m_uiManager.reset();
    }

    void SettingsScene::Update(float deltaTime)
    {
        if (m_uiManager)
        {
            m_uiManager->Update(deltaTime);
        }
    }

    void SettingsScene::Render()
    {
        // Clear screen with dark background
        m_renderer->Clear(20, 20, 30, 255);

        // Render UI
        if (m_uiManager)
        {
            m_uiManager->Render(m_renderer);
        }

        m_renderer->Present();
    }

    bool SettingsScene::HandleMouseClick(int x, int y)
    {
        return false;
    }

    bool SettingsScene::HandleMouseMove(int x, int y)
    {
        if (m_uiManager)
        {
            m_uiManager->HandleMouseMove(x, y);
        }
        return false;
    }

    bool SettingsScene::HandleMouseDown(int x, int y)
    {
        if (m_uiManager)
        {
            m_uiManager->HandleMouseDown(x, y);
        }
        return false;
    }

    bool SettingsScene::HandleMouseUp(int x, int y)
    {
        if (m_uiManager)
        {
            m_uiManager->HandleMouseUp(x, y);
        }
        return false;
    }

    bool SettingsScene::HandleKeyPress(int key)
    {
        if (key == SDLK_ESCAPE)
        {
            LOG_INFO("ESC pressed in settings - returning to previous scene");
            if (m_sceneManager)
            {
                m_sceneManager->PopScene();
            }
            return true;
        }
        return false;
    }

    void SettingsScene::CreateSettingsUI()
    {
        LOG_INFO("SettingsScene: Creating settings UI");

        // Create title panel
        auto titlePanel = std::make_shared<Panel>(0, 0, m_windowWidth, 100);
        titlePanel->SetColor(30, 30, 50, 255);
        titlePanel->SetBorderEnabled(true);
        titlePanel->SetBorderColor(80, 80, 120, 255);
        titlePanel->SetId("title_panel");
        titlePanel->SetZOrder(0);
        m_uiManager->AddComponent(titlePanel);

        // Create title
        auto titleLabel = std::make_shared<Label>(m_windowWidth / 2, 30, "游戏设置", "title");
        titleLabel->SetColor(255, 255, 255, 255);
        titleLabel->SetAlignment(TextAlign::Center);
        titleLabel->SetFontRenderer(m_fontRenderer);
        titleLabel->SetId("title_label");
        titleLabel->SetZOrder(1);
        m_uiManager->AddComponent(titleLabel);

        // Settings panel
        const int panelX = 50;
        const int panelY = 120;
        const int panelWidth = m_windowWidth - 100;
        const int panelHeight = m_windowHeight - 220;

        auto settingsPanel = std::make_shared<Panel>(panelX, panelY, panelWidth, panelHeight);
        settingsPanel->SetColor(25, 25, 40, 255);
        settingsPanel->SetBorderEnabled(true);
        settingsPanel->SetBorderColor(60, 60, 90, 255);
        settingsPanel->SetId("settings_panel");
        settingsPanel->SetZOrder(0);
        m_uiManager->AddComponent(settingsPanel);

        // Display settings section
        int yOffset = panelY + 30;
        const int labelX = panelX + 30;
        const int buttonX = panelX + 250;
        const int buttonWidth = 200;
        const int buttonHeight = 50;
        const int rowSpacing = 70;

        // Section label
        auto displayLabel = std::make_shared<Label>(labelX, yOffset, "显示设置", "default");
        displayLabel->SetColor(200, 200, 255, 255);
        displayLabel->SetAlignment(TextAlign::Left);
        displayLabel->SetFontRenderer(m_fontRenderer);
        displayLabel->SetId("display_section_label");
        displayLabel->SetZOrder(1);
        m_uiManager->AddComponent(displayLabel);

        yOffset += 50;

        // Display mode label and buttons
        auto modeLabel = std::make_shared<Label>(labelX, yOffset + 15, "显示模式:", "default");
        modeLabel->SetColor(180, 180, 180, 255);
        modeLabel->SetAlignment(TextAlign::Left);
        modeLabel->SetFontRenderer(m_fontRenderer);
        modeLabel->SetId("mode_label");
        modeLabel->SetZOrder(1);
        m_uiManager->AddComponent(modeLabel);

        // Get current display mode for button text
        const char* displayModeNames[] = {"窗口", "全屏", "无边框全屏"};
        auto currentModeText = displayModeNames[m_currentDisplayModeIndex];

        auto modeButton = std::make_shared<Button>(
            buttonX, yOffset, buttonWidth, buttonHeight, currentModeText, "default");
        modeButton->SetNormalColor(60, 80, 120, 255);
        modeButton->SetHoverColor(80, 100, 140, 255);
        modeButton->SetPressedColor(40, 60, 100, 255);
        modeButton->SetFontRenderer(m_fontRenderer);
        modeButton->SetId("mode_button");
        modeButton->SetZOrder(2);
        modeButton->SetOnClick([this, modeButton, displayModeNames]()
        {
            // Cycle through display modes
            m_currentDisplayModeIndex = (m_currentDisplayModeIndex + 1) % 3;
            modeButton->SetText(displayModeNames[m_currentDisplayModeIndex]);
            
            // Apply immediately
            auto newMode = static_cast<Display::DisplayMode>(m_currentDisplayModeIndex);
            if (m_displayManager && m_displayManager->SetDisplayMode(newMode))
            {
                m_settingsChanged = true;
                UpdateDisplayInfo();
                LOG_INFO("Display mode changed to: {}", displayModeNames[m_currentDisplayModeIndex]);
            }
        });
        m_uiManager->AddComponent(modeButton);

        yOffset += rowSpacing;

        // Scaling strategy label and buttons
        auto scalingLabel = std::make_shared<Label>(labelX, yOffset + 15, "缩放策略:", "default");
        scalingLabel->SetColor(180, 180, 180, 255);
        scalingLabel->SetAlignment(TextAlign::Left);
        scalingLabel->SetFontRenderer(m_fontRenderer);
        scalingLabel->SetId("scaling_label");
        scalingLabel->SetZOrder(1);
        m_uiManager->AddComponent(scalingLabel);

        const char* scalingStrategyNames[] = {"保持比例", "填充屏幕", "整数缩放", "动态缩放"};
        auto currentScalingText = scalingStrategyNames[m_currentScalingStrategyIndex];

        auto scalingButton = std::make_shared<Button>(
            buttonX, yOffset, buttonWidth, buttonHeight, currentScalingText, "default");
        scalingButton->SetNormalColor(60, 80, 120, 255);
        scalingButton->SetHoverColor(80, 100, 140, 255);
        scalingButton->SetPressedColor(40, 60, 100, 255);
        scalingButton->SetFontRenderer(m_fontRenderer);
        scalingButton->SetId("scaling_button");
        scalingButton->SetZOrder(2);
        scalingButton->SetOnClick([this, scalingButton, scalingStrategyNames]()
        {
            // Cycle through scaling strategies
            m_currentScalingStrategyIndex = (m_currentScalingStrategyIndex + 1) % 4;
            scalingButton->SetText(scalingStrategyNames[m_currentScalingStrategyIndex]);
            
            // Apply immediately
            auto newStrategy = static_cast<Display::ScalingStrategy>(m_currentScalingStrategyIndex);
            if (m_displayManager)
            {
                m_displayManager->SetScalingStrategy(newStrategy);
                m_settingsChanged = true;
                LOG_INFO("Scaling strategy changed to: {}", scalingStrategyNames[m_currentScalingStrategyIndex]);
            }
        });
        m_uiManager->AddComponent(scalingButton);

        yOffset += rowSpacing;

        // Display info label
        auto infoLabel = std::make_shared<Label>(labelX, yOffset, "", "small");
        infoLabel->SetColor(150, 150, 150, 255);
        infoLabel->SetAlignment(TextAlign::Left);
        infoLabel->SetFontRenderer(m_fontRenderer);
        infoLabel->SetId("info_label");
        infoLabel->SetZOrder(1);
        m_uiManager->AddComponent(infoLabel);

        // Update display info
        UpdateDisplayInfo();

        // Bottom buttons
        const int bottomButtonY = m_windowHeight - 80;
        const int backButtonWidth = 200;
        const int backButtonX = m_windowWidth / 2 - backButtonWidth / 2;

        // Back button
        auto backButton = std::make_shared<Button>(
            backButtonX, bottomButtonY, backButtonWidth, 50, "返回", "default");
        backButton->SetNormalColor(80, 80, 120, 255);
        backButton->SetHoverColor(100, 100, 150, 255);
        backButton->SetPressedColor(60, 60, 100, 255);
        backButton->SetFontRenderer(m_fontRenderer);
        backButton->SetId("back_button");
        backButton->SetZOrder(2);
        backButton->SetOnClick([this]()
        {
            LOG_INFO("Back button clicked - returning to previous scene");
            if (m_sceneManager)
            {
                m_sceneManager->PopScene();
            }
        });
        m_uiManager->AddComponent(backButton);
    }

    void SettingsScene::UpdateDisplayInfo()
    {
        if (!m_displayManager)
        {
            return;
        }

        auto displayInfo = m_displayManager->GetDisplayInfo();
        
        // Update info label
        auto infoLabel = std::dynamic_pointer_cast<Label>(m_uiManager->GetComponentById("info_label"));
        if (infoLabel)
        {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), 
                     "当前分辨率: %dx%d | DPI 缩放: %.2f",
                     displayInfo.windowWidth, displayInfo.windowHeight, displayInfo.dpiScale);
            infoLabel->SetText(buffer);
        }
    }

    void SettingsScene::ApplySettings()
    {
        if (!m_displayManager || !m_settingsChanged)
        {
            return;
        }

        // Settings are applied immediately when changed
        // Just save them here
        m_displayManager->SaveDisplaySettings();
        m_settingsChanged = false;
        LOG_INFO("Settings applied and saved");
    }

} // namespace Match3
