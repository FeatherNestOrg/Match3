#include "Game.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Render/Renderer.hpp"
#include "Render/ResourceManager.hpp"
#include "Render/FontRenderer.hpp"
#include "Input/InputManager.hpp"
#include "Scenes/SceneManager.hpp"
#include "Scenes/MenuScene.hpp"
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

        // 初始化 SDL (音频是可选的)
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            LOG_ERROR("SDL initialization failed: {}", SDL_GetError());
            return false;
        }

        // 尝试初始化音频（不强制要求）
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
        {
            LOG_WARN("Audio initialization failed: {} - continuing without audio", SDL_GetError());
        }
        else
        {
            LOG_INFO("Audio initialized successfully");
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
        LOG_INFO("Initializing SceneManager");
        m_sceneManager = std::make_unique<SceneManager>();

        // 初始化 UI 系统
        LOG_INFO("Initializing UI system");
        m_fontRenderer = std::make_unique<FontRenderer>(m_sdlRenderer);
        if (!m_fontRenderer->Initialize())
        {
            LOG_ERROR("Failed to initialize FontRenderer");
            return false;
        }

        // 加载字体
        if (!m_fontRenderer->LoadFont("resources/fonts/ZCOOLKuaiLe-Regular.ttf", 24, "default"))
        {
            LOG_ERROR("Failed to load default font");
            return false;
        }
        if (!m_fontRenderer->LoadFont("resources/fonts/ZCOOLKuaiLe-Regular.ttf", 32, "title"))
        {
            LOG_ERROR("Failed to load title font");
            return false;
        }
        if (!m_fontRenderer->LoadFont("resources/fonts/ZCOOLKuaiLe-Regular.ttf", 18, "small"))
        {
            LOG_ERROR("Failed to load small font");
            return false;
        }

        // 创建 UI 管理器
        // (现在由场景管理)

        // 创建主菜单场景
        m_sceneManager->ChangeScene(
            std::make_unique<MenuScene>(m_renderer.get(), m_fontRenderer.get(),
                                       m_sceneManager.get(), m_windowWidth, m_windowHeight));

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

        m_sceneManager.reset();
        m_fontRenderer.reset();
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

            case SDL_EVENT_KEY_DOWN:
                if (m_sceneManager)
                {
                    m_sceneManager->HandleKeyPress(event.key.key);
                }
                break;

            case SDL_EVENT_MOUSE_MOTION:
                if (m_sceneManager)
                {
                    m_sceneManager->HandleMouseMove(
                        static_cast<int>(event.motion.x),
                        static_cast<int>(event.motion.y));
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT && m_sceneManager)
                {
                    m_sceneManager->HandleMouseDown(
                        static_cast<int>(event.button.x),
                        static_cast<int>(event.button.y));
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT && m_sceneManager)
                {
                    m_sceneManager->HandleMouseUp(
                        static_cast<int>(event.button.x),
                        static_cast<int>(event.button.y));
                }
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

        // 更新场景管理器
        if (m_sceneManager)
        {
            m_sceneManager->Update(deltaTime);
            
            // 如果场景栈为空，退出游戏
            if (m_sceneManager->IsEmpty())
            {
                LOG_INFO("Scene stack is empty - exiting game");
                m_isRunning = false;
            }
        }
    }

    void Game::Render()
    {
        // 场景会自己处理清屏和渲染
        if (m_sceneManager)
        {
            m_sceneManager->Render();
        }
    }

    void Game::SetupInputCallbacks()
    {
        // Input callbacks are now handled by the SceneManager and individual scenes
        // This method is kept for compatibility but is no longer needed
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

