#include "Logger.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace Match3
{
    std::shared_ptr<spdlog::logger> Logger::s_logger = nullptr;

    bool Logger::Initialize(const std::string& appName)
    {
        try
        {
            std::vector<spdlog::sink_ptr> sinks;

            // 1. 根据编译选项决定是否添加控制台输出
#ifdef M3_ENABLE_CONSOLE_LOG
            const auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");
            sinks.push_back(console_sink);
#endif

            // 2. 创建 .log 文件夹（如果不存在）
            const std::filesystem::path log_dir = ".log";
            if (!std::filesystem::exists(log_dir))
            {
                std::filesystem::create_directories(log_dir);
            }

            // 3. 生成带时间戳的日志文件名
            auto now = std::time(nullptr);
            auto tm = *std::localtime(&now);
            std::ostringstream oss;
            oss << log_dir.string() << "/"
                << appName << "_"
                << std::put_time(&tm, "%Y%m%d_%H%M%S")
                << ".log";

            // 4. 添加文件输出 (使用滚动文件，最大 5MB，最多保留 3 个文件)
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                oss.str(),
                1024 * 1024 * 5, // 5MB
                3 // 保留 3 个文件
            );
            file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");
            sinks.push_back(file_sink);

            // 5. 创建多 sink 日志器
            s_logger = std::make_shared<spdlog::logger>("Match3", sinks.begin(), sinks.end());

            // 6. 根据编译选项设置日志级别
#ifndef M3_LOG_LEVEL
#define M3_LOG_LEVEL "DEBUG"
#endif

            std::string log_level = M3_LOG_LEVEL;
            if (log_level == "TRACE")
            {
                s_logger->set_level(spdlog::level::trace);
            }
            else if (log_level == "DEBUG")
            {
                s_logger->set_level(spdlog::level::debug);
            }
            else if (log_level == "INFO")
            {
                s_logger->set_level(spdlog::level::info);
            }
            else if (log_level == "WARN")
            {
                s_logger->set_level(spdlog::level::warn);
            }
            else if (log_level == "ERROR")
            {
                s_logger->set_level(spdlog::level::err);
            }
            else if (log_level == "CRITICAL")
            {
                s_logger->set_level(spdlog::level::critical);
            }
            else
            {
                s_logger->set_level(spdlog::level::debug);
            }

            // 7. 启用立即刷新（确保崩溃时日志不丢失）
            s_logger->flush_on(spdlog::level::warn);

            // 8. 注册为默认日志器
            spdlog::set_default_logger(s_logger);

            s_logger->info("Logger initialized successfully");
            s_logger->info("Log level: {}", log_level);
            s_logger->info("Console output: {}",
#ifdef M3_ENABLE_CONSOLE_LOG
                           "Enabled"
#else
                           "Disabled"
#endif
            );
            s_logger->info("Log file: {}", oss.str());

            return true;
        }
        catch (const std::exception& e)
        {
            fprintf(stderr, "Failed to initialize logger: %s\n", e.what());
            return false;
        }
    }

    void Logger::Shutdown()
    {
        if (s_logger)
        {
            s_logger->info("Logger shutting down");
            s_logger->flush();
            s_logger.reset();
        }
        spdlog::shutdown();
    }
} // namespace Match3
