#pragma once

#include "Scene.hpp"
#include <memory>

namespace Match3
{
    class Renderer;
    class UIManager;
    class FontRenderer;
    class SceneManager;

    /**
     * @brief 主菜单场景
     * 
     * 显示游戏标题、开始游戏按钮、设置按钮、退出按钮
     */
    class MenuScene : public Scene
    {
    public:
        MenuScene(Renderer* renderer, FontRenderer* fontRenderer, 
                  SceneManager* sceneManager, int windowWidth, int windowHeight);
        ~MenuScene() override;

        void OnEnter() override;
        void OnExit() override;
        void Update(float deltaTime) override;
        void Render() override;
        bool HandleMouseClick(int x, int y) override;
        bool HandleMouseMove(int x, int y) override;
        bool HandleMouseDown(int x, int y) override;
        bool HandleMouseUp(int x, int y) override;
        bool HandleKeyPress(int key) override;

        [[nodiscard]] std::string GetName() const override { return "MenuScene"; }

    private:
        void CreateMenuUI();

        Renderer* m_renderer;
        FontRenderer* m_fontRenderer;
        SceneManager* m_sceneManager;
        std::unique_ptr<UIManager> m_uiManager;
        int m_windowWidth;
        int m_windowHeight;
        bool m_shouldExit;
    };
} // namespace Match3
