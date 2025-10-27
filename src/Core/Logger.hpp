#pragma once

#define FMT_HEADER_ONLY
#define SPDLOG_FMT_EXTERNAL
#include <spdlog/spdlog.h>
#include <memory>

namespace Match3
{
    /**
     * @brief 日志管理器 - 统一管理游戏日志输出
     * 
     * 功能:
     * - 根据编译选项控制控制台输出
     * - 始终输出到 .log 文件夹
     * - 根据编译选项设置日志级别
     */
    class Logger
    {
    public:
        /**
         * @brief 初始化日志系统
         * @param appName 应用名称，用于生成日志文件名
         * @return 成功返回 true
         */
        static bool Initialize(const std::string& appName = "match3");

        /**
         * @brief 关闭日志系统
         */
        static void Shutdown();

        /**
         * @brief 获取日志器实例
         */
        static std::shared_ptr<spdlog::logger> GetLogger()
        {
            return s_logger;
        }

    private:
        static std::shared_ptr<spdlog::logger> s_logger;
    };
} // namespace Match3

// 便捷宏定义
#define LOG_TRACE(...)    ::Match3::Logger::GetLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)    ::Match3::Logger::GetLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)     ::Match3::Logger::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Match3::Logger::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Match3::Logger::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Match3::Logger::GetLogger()->critical(__VA_ARGS__)
