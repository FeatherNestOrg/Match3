#include "DisplaySettings.hpp"
#include "Core/Logger.hpp"
#include <fstream>
#include <sstream>

namespace Match3::Display
{
    DisplaySettings::DisplaySettings()
        : m_settings(GetDefaultSettings())
    {
    }

    bool DisplaySettings::LoadFromFile(const std::string& filePath)
    {
        m_configFilePath = filePath;

        std::ifstream file(filePath);
        if (!file.is_open())
        {
            LOG_WARN("Could not open display settings file: {}", filePath);
            LOG_INFO("Using default display settings");
            m_settings = GetDefaultSettings();
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json = buffer.str();
        file.close();

        if (!DeserializeFromJSON(json))
        {
            LOG_ERROR("Failed to parse display settings file: {}", filePath);
            m_settings = GetDefaultSettings();
            return false;
        }

        LOG_INFO("Display settings loaded from: {}", filePath);
        return true;
    }

    bool DisplaySettings::SaveToFile(const std::string& filePath)
    {
        if (!filePath.empty())
        {
            m_configFilePath = filePath;
        }

        if (m_configFilePath.empty())
        {
            LOG_ERROR("No config file path specified");
            return false;
        }

        std::string json = SerializeToJSON();
        
        std::ofstream file(m_configFilePath);
        if (!file.is_open())
        {
            LOG_ERROR("Could not open file for writing: {}", m_configFilePath);
            return false;
        }

        file << json;
        file.close();

        LOG_INFO("Display settings saved to: {}", m_configFilePath);
        return true;
    }

    DisplaySettings::Settings DisplaySettings::GetDefaultSettings()
    {
        Settings defaults;
        defaults.displayMode = DisplayMode::WINDOWED;
        defaults.windowWidth = 800;
        defaults.windowHeight = 600;
        defaults.renderWidth = 800;
        defaults.renderHeight = 600;
        defaults.scalingStrategy = ScalingStrategy::ASPECT_RATIO_STRICT;
        defaults.useNativeResolution = false;
        defaults.lastUsedMonitor = 0;
        return defaults;
    }

    std::string DisplaySettings::SerializeToJSON() const
    {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"displayMode\": \"" << DisplayModeToString(m_settings.displayMode) << "\",\n";
        ss << "  \"windowWidth\": " << m_settings.windowWidth << ",\n";
        ss << "  \"windowHeight\": " << m_settings.windowHeight << ",\n";
        ss << "  \"renderWidth\": " << m_settings.renderWidth << ",\n";
        ss << "  \"renderHeight\": " << m_settings.renderHeight << ",\n";
        ss << "  \"scalingStrategy\": \"" << ScalingStrategyToString(m_settings.scalingStrategy) << "\",\n";
        ss << "  \"useNativeResolution\": " << (m_settings.useNativeResolution ? "true" : "false") << ",\n";
        ss << "  \"lastUsedMonitor\": " << m_settings.lastUsedMonitor << "\n";
        ss << "}\n";
        return ss.str();
    }

    bool DisplaySettings::DeserializeFromJSON(const std::string& json)
    {
        // 简单的 JSON 解析（手动解析，避免依赖外部库）
        Settings newSettings = GetDefaultSettings();

        auto findValue = [&json](const std::string& key) -> std::string {
            size_t pos = json.find("\"" + key + "\"");
            if (pos == std::string::npos)
                return "";
            
            size_t colonPos = json.find(':', pos);
            if (colonPos == std::string::npos)
                return "";
            
            size_t valueStart = json.find_first_not_of(" \t\n\r", colonPos + 1);
            if (valueStart == std::string::npos)
                return "";
            
            size_t valueEnd;
            if (json[valueStart] == '"')
            {
                // 字符串值
                valueStart++;
                valueEnd = json.find('"', valueStart);
            }
            else
            {
                // 数字或布尔值
                valueEnd = json.find_first_of(",}\n", valueStart);
            }
            
            if (valueEnd == std::string::npos)
                return "";
            
            return json.substr(valueStart, valueEnd - valueStart);
        };

        try
        {
            std::string displayMode = findValue("displayMode");
            if (!displayMode.empty())
                newSettings.displayMode = StringToDisplayMode(displayMode);

            std::string windowWidth = findValue("windowWidth");
            if (!windowWidth.empty())
                newSettings.windowWidth = std::stoi(windowWidth);

            std::string windowHeight = findValue("windowHeight");
            if (!windowHeight.empty())
                newSettings.windowHeight = std::stoi(windowHeight);

            std::string renderWidth = findValue("renderWidth");
            if (!renderWidth.empty())
                newSettings.renderWidth = std::stoi(renderWidth);

            std::string renderHeight = findValue("renderHeight");
            if (!renderHeight.empty())
                newSettings.renderHeight = std::stoi(renderHeight);

            std::string scalingStrategy = findValue("scalingStrategy");
            if (!scalingStrategy.empty())
                newSettings.scalingStrategy = StringToScalingStrategy(scalingStrategy);

            std::string useNative = findValue("useNativeResolution");
            if (!useNative.empty())
                newSettings.useNativeResolution = (useNative == "true");

            std::string monitor = findValue("lastUsedMonitor");
            if (!monitor.empty())
                newSettings.lastUsedMonitor = std::stoi(monitor);

            m_settings = newSettings;
            return true;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Exception while parsing JSON: {}", e.what());
            return false;
        }
    }

    std::string DisplaySettings::DisplayModeToString(DisplayMode mode)
    {
        switch (mode)
        {
        case DisplayMode::WINDOWED:
            return "WINDOWED";
        case DisplayMode::FULLSCREEN_EXCLUSIVE:
            return "FULLSCREEN_EXCLUSIVE";
        case DisplayMode::FULLSCREEN_BORDERLESS:
            return "FULLSCREEN_BORDERLESS";
        default:
            return "WINDOWED";
        }
    }

    DisplayMode DisplaySettings::StringToDisplayMode(const std::string& str)
    {
        if (str == "FULLSCREEN_EXCLUSIVE")
            return DisplayMode::FULLSCREEN_EXCLUSIVE;
        if (str == "FULLSCREEN_BORDERLESS")
            return DisplayMode::FULLSCREEN_BORDERLESS;
        return DisplayMode::WINDOWED;
    }

    std::string DisplaySettings::ScalingStrategyToString(ScalingStrategy strategy)
    {
        switch (strategy)
        {
        case ScalingStrategy::ASPECT_RATIO_STRICT:
            return "ASPECT_RATIO_STRICT";
        case ScalingStrategy::FILL_SCREEN:
            return "FILL_SCREEN";
        case ScalingStrategy::INTEGER_SCALE:
            return "INTEGER_SCALE";
        case ScalingStrategy::DYNAMIC_SCALE:
            return "DYNAMIC_SCALE";
        default:
            return "ASPECT_RATIO_STRICT";
        }
    }

    ScalingStrategy DisplaySettings::StringToScalingStrategy(const std::string& str)
    {
        if (str == "FILL_SCREEN")
            return ScalingStrategy::FILL_SCREEN;
        if (str == "INTEGER_SCALE")
            return ScalingStrategy::INTEGER_SCALE;
        if (str == "DYNAMIC_SCALE")
            return ScalingStrategy::DYNAMIC_SCALE;
        return ScalingStrategy::ASPECT_RATIO_STRICT;
    }

} // namespace Match3::Display
