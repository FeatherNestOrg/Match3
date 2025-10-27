#include "Core/Game.hpp"
#include "Core/Config.hpp"
#include <spdlog/spdlog.h>

int main(int argc, char* argv[])
{
    // 设置日志级别
#ifdef M3_ENABLE_CONSOLE_LOG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    spdlog::info("=== Match-3 Game Starting ===");

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
            spdlog::error("Failed to initialize game");
            return 1;
        }

        // 运行游戏循环
        game.Run();

        // 关闭游戏
        game.Shutdown();
    }
    catch (const std::exception& e)
    {
        spdlog::critical("Fatal error: {}", e.what());
        return 1;
    }

    spdlog::info("=== Game Ended Successfully ===");
    return 0;
}
