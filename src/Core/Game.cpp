#include "Game.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Render/Renderer.hpp"
#include "Render/ResourceManager.hpp"
#include "Managers/GameStateManager.hpp"
#include "Input/InputManager.hpp"
#include "Input/MouseHandler.hpp"
#include <SDL3/SDL_timer.h>

#include <utility>

namespace Match3
{
    Game::Game(std::string title, const int width, const int height)
        : m_title(std::move(title))
          , m_windowWidth(width)
          , m_windowHeight(height)
          , m_window(nullptr)
          , m_sdlRenderer(nullptr)
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
            &m_sdlRenderer))
        {
            LOG_ERROR("Window/Renderer creation failed: {}", SDL_GetError());
            return false;
        }

        LOG_INFO("Window created: {}x{}", m_windowWidth, m_windowHeight);

        // 设置渲染器混合模式
        SDL_SetRenderDrawBlendMode(m_sdlRenderer, SDL_BLENDMODE_BLEND);

        // 创建渲染系统
        m_renderer = std::make_unique<Renderer>(m_sdlRenderer);
        // 创建输入管理器
        m_inputManager = std::make_unique<InputManager>();

        m_resourceManager = std::make_unique<ResourceManager>(m_sdlRenderer);

        // 初始化渲染资源
        if (!InitializeRenderResources())
        {
            LOG_ERROR("Failed to initialize render resources");
            return false;
        }

        // 创建游戏状态管理器
        LOG_INFO("Initializing GameStateManager");
        m_gameState = std::make_unique<GameStateManager>(m_renderer.get());
        m_gameState->Initialize(Config::BOARD_ROWS, Config::BOARD_COLS, Config::GEM_TYPES);

        // 设置输入回调
        SetupInputCallbacks();

        m_isRunning = true;
        LOG_INFO("Game initialized successfully!");

        return true;
    }

    bool Game::InitializeRenderResources()
    {
        LOG_INFO("Initializing render resources...");

        // 创建宝石纹理（使用配置中定义的颜色）
        for (int i = 0; i < Config::GEM_TYPES; ++i)
        {
            const auto& [r, g, b, a] = Config::GEM_COLORS[i];
            const std::string textureName = "gem_" + std::to_string(i);

            if (!m_resourceManager->CreateColorTexture(
                textureName,
                Config::GEM_SIZE, Config::GEM_SIZE,
                r, g, b, a))
            {
                LOG_ERROR("Failed to create gem texture {}", i);
                return false;
            }
        }

        LOG_INFO("Render resources initialized successfully");
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

        m_gameState.reset();
        m_inputManager.reset();
        m_resourceManager.reset();
        m_renderer.reset();

        if (m_sdlRenderer)
        {
            SDL_DestroyRenderer(m_sdlRenderer);
            m_sdlRenderer = nullptr;
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
            // 传递事件给输入管理器
            if (m_inputManager)
            {
                m_inputManager->HandleEvent(event);
            }

            // 处理系统事件
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                LOG_INFO("Quit event received");
                m_isRunning = false;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                m_windowWidth = event.window.data1;
                m_windowHeight = event.window.data2;
                LOG_INFO("Window resized: {}x{}", m_windowWidth, m_windowHeight);
                break;

            default:
                break;
            }
        }
    }

    void Game::Update(const float deltaTime)
    {
        // 更新输入管理器
        if (m_inputManager)
        {
            m_inputManager->Update();
        }

        // 更新游戏状态管理器
        if (m_gameState)
        {
            m_gameState->Update(deltaTime);
        }
    }

    void Game::Render()
    {
        // 清空屏幕（使用配置中定义的背景色）
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

        // 渲染游戏内容（宝石等）
        if (m_gameState)
        {
            m_gameState->Render();
        }

        // 显示渲染结果
        m_renderer->Present();
    }

    void Game::SetupInputCallbacks()
    {
        // 设置鼠标点击回调
        m_inputManager->SetMouseClickCallback(
            [this](const int x, const int y, const InputManager::MouseButton button)
            {
                if (button == InputManager::MouseButton::Left && !m_isPaused)
                {
                    // 使用 MouseHandler 转换坐标
                    auto boardPos = MouseHandler::ScreenToBoard(x, y);
                    if (boardPos.has_value())
                    {
                        auto [row, col] = boardPos.value();
                        LOG_DEBUG("Click at board position: ({}, {})", row, col);

                        // 将点击传递给游戏状态管理器
                        if (m_gameState)
                        {
                            m_gameState->HandleClick(row, col);
                        }
                    }
                }
            });

        // 设置键盘按下回调
        m_inputManager->SetKeyDownCallback(
            [this](SDL_Keycode key)
            {
                if (key == SDLK_ESCAPE)
                {
                    LOG_INFO("ESC pressed - exiting game");
                    m_isRunning = false;
                }
                else if (key == SDLK_SPACE)
                {
                    m_isPaused = !m_isPaused;
                    LOG_INFO("Game {}", m_isPaused ? "paused" : "resumed");
                }
                else if (key == SDLK_R)
                {
                    LOG_INFO("Restarting game...");
                }
                else if (key == SDLK_H)
                {
                    // 显示提示
                    LOG_INFO("Hint requested");
                    // TODO: 实现提示功能
                }
            });
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

