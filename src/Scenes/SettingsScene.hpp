#pragma once

#include "Scene.hpp"
#include <memory>
#include <vector>

namespace Match3
{
    class Renderer;
    class UIManager;
    class FontRenderer;
    class SceneManager;

    namespace Display
    {
        struct Resolution;
        class DisplayManager;
    }

    /**
     * @brief 设置场景
     * 
     * 显示游戏设置，包括显示配置、音频等
     */
    class SettingsScene : public Scene
    {
    public:
        SettingsScene(Renderer* renderer, FontRenderer* fontRenderer,
                      SceneManager* sceneManager, Display::DisplayManager* displayManager,
                      int windowWidth, int windowHeight);
        ~SettingsScene() override;

        void OnEnter() override;
        void OnExit() override;
        void Update(float deltaTime) override;
        void Render() override;
        bool HandleMouseClick(int x, int y) override;
        bool HandleMouseMove(int x, int y) override;
        bool HandleMouseDown(int x, int y) override;
        bool HandleMouseUp(int x, int y) override;
        bool HandleKeyPress(int key) override;

        [[nodiscard]] std::string GetName() const override { return "SettingsScene"; }

        void HandleWindowResize(int width, int height) override;

    private:
        void CreateSettingsUI();
        void UpdateDisplayInfo();
        void ApplySettings();

        Renderer* m_renderer;
        FontRenderer* m_fontRenderer;
        SceneManager* m_sceneManager;
        Display::DisplayManager* m_displayManager; // Not owned
        std::unique_ptr<UIManager> m_uiManager;
        int m_windowWidth;
        int m_windowHeight;

        // Current settings state
        int m_currentDisplayModeIndex;
        int m_currentScalingStrategyIndex;
        int m_currentResolutionIndex;
        std::vector<Display::Resolution> m_availableResolutions;
        bool m_settingsChanged;
    };
} // namespace Match3
