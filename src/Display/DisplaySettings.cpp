#include "DisplaySettings.hpp"
#include "Core/Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace Match3::Display
{
    // JSON serialization helpers for enums
    NLOHMANN_JSON_SERIALIZE_ENUM(DisplayMode, {
        {DisplayMode::WINDOWED, "WINDOWED"},
        {DisplayMode::FULLSCREEN_EXCLUSIVE, "FULLSCREEN_EXCLUSIVE"},
        {DisplayMode::FULLSCREEN_BORDERLESS, "FULLSCREEN_BORDERLESS"},
    })

    NLOHMANN_JSON_SERIALIZE_ENUM(ScalingStrategy, {
        {ScalingStrategy::ASPECT_RATIO_STRICT, "ASPECT_RATIO_STRICT"},
        {ScalingStrategy::FILL_SCREEN, "FILL_SCREEN"},
        {ScalingStrategy::INTEGER_SCALE, "INTEGER_SCALE"},
        {ScalingStrategy::DYNAMIC_SCALE, "DYNAMIC_SCALE"},
    })

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

        try
        {
            json j;
            file >> j;
            file.close();

            // Parse JSON with default values
            m_settings.displayMode = j.value("displayMode", DisplayMode::WINDOWED);
            m_settings.windowWidth = j.value("windowWidth", 800);
            m_settings.windowHeight = j.value("windowHeight", 600);
            m_settings.renderWidth = j.value("renderWidth", 800);
            m_settings.renderHeight = j.value("renderHeight", 600);
            m_settings.scalingStrategy = j.value("scalingStrategy", ScalingStrategy::ASPECT_RATIO_STRICT);
            m_settings.useNativeResolution = j.value("useNativeResolution", false);
            m_settings.lastUsedMonitor = j.value("lastUsedMonitor", 0);

            LOG_INFO("Display settings loaded from: {}", filePath);
            return true;
        }
        catch (const json::exception& e)
        {
            LOG_ERROR("Failed to parse display settings file: {} - {}", filePath, e.what());
            m_settings = GetDefaultSettings();
            return false;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Exception while loading display settings: {}", e.what());
            m_settings = GetDefaultSettings();
            return false;
        }
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

        try
        {
            json j;
            j["displayMode"] = m_settings.displayMode;
            j["windowWidth"] = m_settings.windowWidth;
            j["windowHeight"] = m_settings.windowHeight;
            j["renderWidth"] = m_settings.renderWidth;
            j["renderHeight"] = m_settings.renderHeight;
            j["scalingStrategy"] = m_settings.scalingStrategy;
            j["useNativeResolution"] = m_settings.useNativeResolution;
            j["lastUsedMonitor"] = m_settings.lastUsedMonitor;

            std::ofstream file(m_configFilePath);
            if (!file.is_open())
            {
                LOG_ERROR("Could not open file for writing: {}", m_configFilePath);
                return false;
            }

            file << j.dump(2); // Pretty print with 2 space indentation
            file.close();

            LOG_INFO("Display settings saved to: {}", m_configFilePath);
            return true;
        }
        catch (const json::exception& e)
        {
            LOG_ERROR("Failed to serialize display settings: {}", e.what());
            return false;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Exception while saving display settings: {}", e.what());
            return false;
        }
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

} // namespace Match3::Display
