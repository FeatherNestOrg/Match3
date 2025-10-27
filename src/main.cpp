#include "Core/Game.hpp"
#include "Core/Config.hpp"
#include "Core/Logger.hpp"

int main(int argc, char* argv[])
{
    // 初始化日志系统
    if (!Match3::Logger::Initialize("match3"))
    {
        return 1;
    }

    LOG_INFO("=== Match-3 Game Starting ===");

    try
    {
        // 创建游戏实例
        Match3::Game game(
            Match3::Config::WINDOW_TITLE,
            Match3::Config::WINDOW_WIDTH,
            Match3::Config::WINDOW_HEIGHT
        );

        // 初始化游戏
        if (!game.Initialize())
        {
            LOG_ERROR("Failed to initialize game");
            Match3::Logger::Shutdown();
            return 1;
        }

        // 运行游戏循环
        game.Run();

        // 关闭游戏
        game.Shutdown();
    }
    catch (const std::exception& e)
    {
        LOG_CRITICAL("Fatal error: {}", e.what());
        Match3::Logger::Shutdown();
        return 1;
    }

    LOG_INFO("=== Game Ended Successfully ===");
    Match3::Logger::Shutdown();
    return 0;
}
