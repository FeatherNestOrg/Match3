#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <string>

namespace Match3
{
    class Renderer;
    class ResourceManager;
    class InputManager;
    class FontRenderer;
    class SceneManager;

    /**
     * @brief 游戏主类 - 负责游戏循环和核心系统管理
     */
    class Game
    {
    public:
        Game(std::string title, int width, int height);
        ~Game();

        // 禁止拷贝
        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;

        /**
         * @brief 初始化游戏
         * @return 成功返回 true
         */
        bool Initialize();

        /**
         * @brief 运行游戏主循环
         */
        void Run();

        /**
         * @brief 关闭游戏
         */
        void Shutdown();

        /**
         * @brief 检查游戏是否正在运行
         */
        [[nodiscard]] bool IsRunning() const { return m_isRunning; }

    private:
        /**
         * @brief 处理输入事件
         */
        void HandleEvents();

        /**
         * @brief 更新游戏逻辑
         * @param deltaTime 帧间隔时间（秒）
         */
        void Update(float deltaTime);

        /**
         * @brief 渲染游戏画面
         */
        void Render();

        /**
         * @brief 计算 FPS
         */
        void UpdateFPS(float deltaTime);

        /**
         * @brief 初始化渲染资源
         */
        bool InitializeRenderResources();

        /**
         * @brief 设置输入回调
         */
        void SetupInputCallbacks();

    private:
        std::string m_title;
        int m_windowWidth;
        int m_windowHeight;

        SDL_Window* m_window;
        SDL_Renderer* m_sdlRenderer;

        std::unique_ptr<Renderer> m_renderer;
        std::unique_ptr<ResourceManager> m_resourceManager;
        std::unique_ptr<InputManager> m_inputManager;
        std::unique_ptr<FontRenderer> m_fontRenderer;
        std::unique_ptr<SceneManager> m_sceneManager;

        bool m_isRunning;
        bool m_isPaused;

        // 性能监控
        float m_fps;
        float m_frameTimeAccumulator;
        int m_frameCount;

        // 固定时间步长（秒）
        static constexpr float TARGET_FPS = 60.0f;
        static constexpr float FIXED_TIMESTEP = 1.0f / TARGET_FPS;
    };
} // namespace Match3
