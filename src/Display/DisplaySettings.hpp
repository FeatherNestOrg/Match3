#pragma once

#include "DisplayMode.hpp"
#include <string>

namespace Match3::Display
{
    /**
     * @brief 显示设置持久化类
     */
    class DisplaySettings
    {
    public:
        /**
         * @brief 显示设置数据结构
         */
        struct Settings
        {
            DisplayMode displayMode{DisplayMode::WINDOWED};
            int windowWidth{800};
            int windowHeight{600};
            int renderWidth{800};
            int renderHeight{600};
            ScalingStrategy scalingStrategy{ScalingStrategy::ASPECT_RATIO_STRICT};
            bool useNativeResolution{false}; // 使用原生分辨率（全屏）
            int lastUsedMonitor{0};          // 上次使用的显示器索引
        };

        DisplaySettings();

        /**
         * @brief 从文件加载设置
         * @param filePath 配置文件路径
         * @return 成功返回 true
         */
        bool LoadFromFile(const std::string& filePath);

        /**
         * @brief 保存设置到文件
         * @param filePath 配置文件路径
         * @return 成功返回 true
         */
        bool SaveToFile(const std::string& filePath);

        /**
         * @brief 获取当前设置
         * @return 设置数据
         */
        [[nodiscard]] Settings GetSettings() const { return m_settings; }

        /**
         * @brief 设置新的配置
         * @param settings 新设置
         */
        void SetSettings(const Settings& settings) { m_settings = settings; }

        /**
         * @brief 获取默认设置
         * @return 默认设置
         */
        static Settings GetDefaultSettings();

    private:
        Settings m_settings;
        std::string m_configFilePath;
    };

} // namespace Match3::Display
