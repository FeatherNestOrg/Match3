#include "Game.hpp"
#include "Logger.hpp"
#include <SDL3/SDL_timer.h>

#include <utility>

namespace Match3
{
    Game::Game(std::string title, const int width, const int height)
        : m_title(std::move(title))
          , m_windowWidth(width)
          , m_windowHeight(height)
          , m_window(nullptr)
          , m_renderer(nullptr)
          , m_isRunning(false)
          , m_isPaused(false)
          , m_fps(0.0f)
          , m_frameTimeAccumulator(0.0f)
          , m_frameCount(0)
    {
    }

    Game::~Game()
    {
        Shutdown();
    }

    bool Game::Initialize()
    {
        LOG_INFO("Initializing Match-3 Game...");

        // 初始化 SDL
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        {
            LOG_ERROR("SDL initialization failed: {}", SDL_GetError());
            return false;
        }

        // 创建窗口
        if (!SDL_CreateWindowAndRenderer(
            m_title.c_str(),
            m_windowWidth,
            m_windowHeight,
            SDL_WINDOW_RESIZABLE,
            &m_window,
            &m_renderer))
        {
            LOG_ERROR("Window/Renderer creation failed: {}", SDL_GetError());
            return false;
        }

        LOG_INFO("Window created: {}x{}", m_windowWidth, m_windowHeight);

        // 设置渲染器混合模式
        SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

        m_isRunning = true;
        LOG_INFO("Game initialized successfully!");

        return true;
    }

    void Game::Run()
    {
        LOG_INFO("Starting game loop...");

        Uint64 lastTime = SDL_GetTicksNS();
        float accumulator = 0.0f;

        while (m_isRunning)
        {
            // 计算 delta time
            Uint64 currentTime = SDL_GetTicksNS();
            float deltaTime = static_cast<float>(currentTime - lastTime) / 1000000000.0f; // 转换为秒
            lastTime = currentTime;

            // 限制最大 delta time（避免螺旋死亡）
            if (deltaTime > 0.25f)
            {
                deltaTime = 0.25f;
            }

            accumulator += deltaTime;

            // 处理输入
            HandleEvents();

            // 固定时间步长更新
            while (accumulator >= FIXED_TIMESTEP)
            {
                if (!m_isPaused)
                {
                    Update(FIXED_TIMESTEP);
                }
                accumulator -= FIXED_TIMESTEP;
            }

            // 渲染
            Render();

            // 更新 FPS
            UpdateFPS(deltaTime);

            // 限制帧率（避免 CPU 占用过高）
            SDL_Delay(1);
        }

        LOG_INFO("Game loop ended");
    }

    void Game::Shutdown()
    {
        LOG_INFO("Shutting down game...");

        if (m_renderer)
        {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }

        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        SDL_Quit();
        LOG_INFO("Game shutdown complete");
    }

    void Game::HandleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                LOG_INFO("Quit event received");
                m_isRunning = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                {
                    LOG_INFO("ESC pressed - exiting game");
                    m_isRunning = false;
                }
                else if (event.key.key == SDLK_SPACE)
                {
                    m_isPaused = !m_isPaused;
                    LOG_INFO("Game {}", m_isPaused ? "paused" : "resumed");
                }
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                m_windowWidth = event.window.data1;
                m_windowHeight = event.window.data2;
                LOG_INFO("Window resized: {}x{}", m_windowWidth, m_windowHeight);
                break;
            default: break;
            }
        }
    }

    void Game::Update(float deltaTime)
    {
        // TODO: 这里将添加游戏逻辑更新
        // - 更新棋盘状态
        // - 处理动画
        // - 检测匹配
        // 等等...
    }

    void Game::Render()
    {
        // 清空屏幕（深色背景）
        SDL_SetRenderDrawColor(m_renderer, 20, 20, 30, 255);
        SDL_RenderClear(m_renderer);

        // TODO: 这里将添加渲染逻辑
        // - 渲染背景
        // - 渲染棋盘
        // - 渲染宝石
        // - 渲染 UI
        // 等等...

        // 临时：绘制一个简单的网格作为占位
        SDL_SetRenderDrawColor(m_renderer, 60, 60, 80, 255);

        // 垂直线
        for (int x = 0; x <= m_windowWidth; x += 80)
        {
            SDL_RenderLine(m_renderer, x, 0, x, m_windowHeight);
        }

        // 水平线
        for (int y = 0; y <= m_windowHeight; y += 80)
        {
            SDL_RenderLine(m_renderer, 0, y, m_windowWidth, y);
        }

        // 显示渲染结果
        SDL_RenderPresent(m_renderer);
    }

    void Game::UpdateFPS(float deltaTime)
    {
        m_frameTimeAccumulator += deltaTime;
        m_frameCount++;

        // 每秒更新一次 FPS
        if (m_frameTimeAccumulator >= 1.0f)
        {
            m_fps = m_frameCount / m_frameTimeAccumulator;

            LOG_DEBUG("FPS: {:.1f} | Frame Time: {:.2f}ms",
                      m_fps, (m_frameTimeAccumulator / m_frameCount) * 1000.0f);

            m_frameTimeAccumulator = 0.0f;
            m_frameCount = 0;
        }
    }
} // namespace Match3

