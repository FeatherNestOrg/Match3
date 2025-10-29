#pragma once

#include "Scene.hpp"
#include <memory>

namespace Match3
{
    class Renderer;
    class UIManager;
    class FontRenderer;
    class GameStateManager;
    class InputManager;
    class SceneManager;

    /**
     * @brief 游戏场景
     * 
     * 显示游戏棋盘和游戏玩法
     */
    class GameScene : public Scene
    {
    public:
        GameScene(Renderer* renderer, FontRenderer* fontRenderer,
                  SceneManager* sceneManager, int windowWidth, int windowHeight);
        ~GameScene() override;

        void OnEnter() override;
        void OnExit() override;
        void Update(float deltaTime) override;
        void Render() override;
        bool HandleMouseClick(int x, int y) override;
        bool HandleMouseMove(int x, int y) override;
        bool HandleMouseDown(int x, int y) override;
        bool HandleMouseUp(int x, int y) override;
        bool HandleKeyPress(int key) override;

        [[nodiscard]] std::string GetName() const override { return "GameScene"; }

        void HandleWindowResize(int width, int height) override;

    private:
        void CreateGameUI();

        Renderer* m_renderer;
        FontRenderer* m_fontRenderer;
        SceneManager* m_sceneManager;
        std::unique_ptr<GameStateManager> m_gameState;
        std::unique_ptr<UIManager> m_uiManager;
        int m_windowWidth;
        int m_windowHeight;
    };
} // namespace Match3
