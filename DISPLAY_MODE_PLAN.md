# Match-3 游戏 - 全屏/窗口化/调整分辨率实现方案

## 📋 一、现状分析

### 当前架构

- **窗口系统**：基于 SDL3
- **渲染管理**：集中在 `Game` 类
- **配置管理**：使用 `Config` 命名空间的静态常量
- **游戏坐标系统**：固定分辨率（800x600）

### 存在的问题

1. ❌ 窗口分辨率硬编码在 `Config::WINDOW_WIDTH/HEIGHT`
2. ❌ 游戏内容没有适配不同屏幕尺寸
3. ❌ 无法动态切换全屏/窗口模式
4. ❌ 没有处理窗口调整大小的事件
5. ❌ UI 和游戏元素使用绝对像素坐标

---

## 🎯 二、设计目标

### 2.1 功能需求

| 需求           | 说明                                   |
| -------------- | -------------------------------------- |
| **全屏支持**   | 支持独占式全屏和无边框全屏             |
| **窗口化模式** | 支持可调整大小的窗口                   |
| **分辨率切换** | 可以动态切换预定义分辨率或自定义分辨率 |
| **宽高比适配** | 使用 letterbox/pillarbox 或自适应缩放  |
| **DPI 感知**   | 处理高 DPI 显示器（Retina、4K 等）     |
| **配置持久化** | 保存用户的显示模式设置                 |

### 2.2 架构目标

- ✅ **解耦**：显示模式逻辑独立于游戏核心逻辑
- ✅ **灵活**：支持多种分辨率和宽高比
- ✅ **高效**：最小化性能开销
- ✅ **易维护**：清晰的接口和易理解的实现

---

## 🏗️ 三、整体架构设计

### 3.1 核心模块结构

```
src/Display/
├── DisplayMode.hpp              # 显示模式定义（枚举、结构体）
├── DisplayConfig.hpp            # 显示配置管理
├── DisplayManager.hpp/cpp       # 显示模式管理器（主控制中心）
├── ResolutionManager.hpp/cpp    # 分辨率管理
├── ViewportManager.hpp/cpp      # 视口和坐标变换管理
└── DisplaySettings.hpp/cpp      # 显示设置的持久化
```

### 3.2 核心类图

```
DisplayManager (核心管理器)
├── ResolutionManager         (分辨率管理)
│   ├── 预定义分辨率列表
│   ├── 当前分辨率状态
│   └── DPI 处理
├── ViewportManager           (视口和坐标变换)
│   ├── 窗口坐标 → 游戏坐标
│   ├── 游戏坐标 → 渲染坐标
│   └── letterbox/pillarbox 计算
└── DisplaySettings          (持久化存储)
    ├── 读写配置文件
    └── 用户设置缓存

Display Mode States:
├── WINDOWED (窗口化)
├── FULLSCREEN_EXCLUSIVE (独占全屏)
└── FULLSCREEN_BORDERLESS (无边框全屏)
```

---

## 📐 四、详细设计

### 4.1 显示模式枚举（DisplayMode.hpp）

```cpp
namespace Match3::Display {
    // 显示模式类型
    enum class DisplayMode {
        WINDOWED,               // 窗口化模式
        FULLSCREEN_EXCLUSIVE,   // 独占全屏
        FULLSCREEN_BORDERLESS   // 无边框全屏
    };

    // 缩放策略
    enum class ScalingStrategy {
        ASPECT_RATIO_STRICT,    // 保持宽高比，letterbox/pillarbox
        FILL_SCREEN,            // 填满屏幕，可能拉伸
        INTEGER_SCALE,          // 整数倍缩放（像素艺术游戏适用）
        DYNAMIC_SCALE           // 动态缩放，适配不同屏幕
    };

    // 显示信息结构体
    struct DisplayInfo {
        int windowWidth;        // 窗口宽度（像素）
        int windowHeight;       // 窗口高度（像素）
        int renderWidth;        // 渲染分辨率
        int renderHeight;
        int physicalWidth;      // 物理显示器宽度
        int physicalHeight;     // 物理显示器高度
        float dpiScale;         // DPI 缩放因子
        DisplayMode mode;
        ScalingStrategy strategy;
    };

    // 分辨率预设
    struct Resolution {
        int width;
        int height;
        const char* label;      // "1920x1080", "1280x720", 等
    };
}
```

### 4.2 分辨率管理器（ResolutionManager.hpp/cpp）

```cpp
class ResolutionManager {
public:
    // 预定义分辨率列表
    static constexpr std::array<Display::Resolution, 8> PRESET_RESOLUTIONS = {
        Display::Resolution{800, 600, "800x600"},
        Display::Resolution{1024, 768, "1024x768"},
        Display::Resolution{1280, 720, "1280x720 (HD)"},
        Display::Resolution{1366, 768, "1366x768"},
        Display::Resolution{1920, 1080, "1920x1080 (FHD)"},
        Display::Resolution{2560, 1440, "2560x1440 (2K)"},
        Display::Resolution{3840, 2160, "3840x2160 (4K)"},
        Display::Resolution{1024, 1024, "Square 1024"}
    };

    // 可支持的宽高比
    static constexpr std::array<float, 4> ASPECT_RATIOS = {
        4.0f / 3.0f,    // 1.333
        16.0f / 10.0f,  // 1.6
        16.0f / 9.0f,   // 1.778
        1.0f            // 1:1 方形
    };

    ResolutionManager(SDL_Window* window);

    // 获取显示器信息
    std::vector<Display::Resolution> GetAvailableDisplayModes();
    Display::DisplayInfo GetCurrentDisplayInfo();

    // 查询分辨率
    bool IsResolutionSupported(int width, int height);
    const Display::Resolution* FindClosestResolution(float targetAspect);

    // DPI 处理
    float GetDPIScale();
    std::pair<int, int> ApplyDPIScaling(int width, int height);

private:
    SDL_Window* m_window;
    std::vector<SDL_DisplayMode> m_displayModes;
    float m_dpiScale;
};
```

### 4.3 视口管理器（ViewportManager.hpp/cpp）

```cpp
class ViewportManager {
public:
    ViewportManager();

    // 更新视口信息
    void UpdateViewport(const Display::DisplayInfo& displayInfo);

    // 坐标变换
    // 窗口坐标（鼠标输入）→ 游戏逻辑坐标
    std::pair<int, int> WindowToGameCoords(int windowX, int windowY);

    // 游戏逻辑坐标 → 渲染坐标
    std::pair<int, int> GameToRenderCoords(float gameX, float gameY);

    // 获取 letterbox/pillarbox 黑边矩形（用于清除）
    std::vector<SDL_Rect> GetLetterboxRects();

    // 宽高比计算
    float CalculateAspectRatio(int width, int height);
    bool IsAspectRatioMatch(float ar1, float ar2, float tolerance = 0.01f);

    // 缩放因子
    float GetScaleFactorX() const;
    float GetScaleFactorY() const;

private:
    // 内部坐标系统配置
    struct Viewport {
        int offsetX, offsetY;          // 相对于窗口的偏移
        int renderWidth, renderHeight;  // 实际渲染区域
        float scaleX, scaleY;           // 缩放因子
    };

    Viewport m_viewport;
    Display::ScalingStrategy m_strategy;
};
```

### 4.4 显示管理器（DisplayManager.hpp/cpp）

**主控制中心，协调各个子系统**

```cpp
class DisplayManager {
public:
    explicit DisplayManager(SDL_Window* window, SDL_Renderer* renderer);
    ~DisplayManager() = default;

    // 初始化显示系统
    bool Initialize();

    // 显示模式切换
    bool SetDisplayMode(Display::DisplayMode mode);
    Display::DisplayMode GetCurrentDisplayMode() const;

    // 分辨率操作
    bool SetResolution(int width, int height);
    bool SetResolutionPreset(const Display::Resolution& resolution);
    std::vector<Display::Resolution> GetAvailableResolutions() const;

    // 获取显示信息
    const Display::DisplayInfo& GetDisplayInfo() const;

    // 处理窗口事件
    void HandleWindowEvent(const SDL_WindowEvent& event);
    void HandleDisplayChangeEvent(const SDL_DisplayEvent& event);

    // 坐标变换服务
    std::pair<int, int> WindowToGameCoords(int windowX, int windowY) const;
    std::pair<int, int> GameToRenderCoords(float gameX, float gameY) const;

    // 缩放策略
    void SetScalingStrategy(Display::ScalingStrategy strategy);
    Display::ScalingStrategy GetScalingStrategy() const;

    // 保存/加载设置
    void SaveDisplaySettings();
    void LoadDisplaySettings();

private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    std::unique_ptr<ResolutionManager> m_resolutionManager;
    std::unique_ptr<ViewportManager> m_viewportManager;
    std::unique_ptr<DisplaySettings> m_displaySettings;

    Display::DisplayInfo m_currentDisplayInfo;
    bool m_needsRedraw;

    // 内部方法
    bool ApplyDisplayMode(Display::DisplayMode mode);
    void UpdateDisplayInfo();
};
```

### 4.5 显示设置持久化（DisplaySettings.hpp/cpp）

```cpp
class DisplaySettings {
public:
    struct Settings {
        Display::DisplayMode displayMode;
        int windowWidth, windowHeight;
        int renderWidth, renderHeight;
        Display::ScalingStrategy scalingStrategy;
        bool useNativeResolution;   // 使用原生分辨率（全屏）
    };

    DisplaySettings();

    // 文件操作
    bool LoadFromFile(const std::string& filePath);
    bool SaveToFile(const std::string& filePath);

    // 访问器
    Settings GetSettings() const { return m_settings; }
    void SetSettings(const Settings& settings) { m_settings = settings; }

    // 默认值
    static Settings GetDefaultSettings();

private:
    Settings m_settings;
    std::string m_configFilePath;

    // JSON 序列化/反序列化
    std::string SerializeToJSON() const;
    bool DeserializeFromJSON(const std::string& json);
};
```

---

## 🔄 五、集成到现有架构

### 5.1 修改 Game 类

```cpp
// Game.hpp 中添加

private:
    std::unique_ptr<Display::DisplayManager> m_displayManager;
    Display::DisplayInfo m_lastDisplayInfo;

    void HandleDisplayConfigChange();
    void OnWindowResized(int newWidth, int newHeight);
    void OnDisplayModeChanged(Display::DisplayMode newMode);
```

### 5.2 事件流更改

```
SDL Event
    ↓
Game::HandleEvents()
    ├─ DisplayManager::HandleWindowEvent()
    │   ├─ 更新 DisplayInfo
    │   ├─ 更新 ViewportManager
    │   └─ 触发回调
    └─ 其他事件处理
```

### 5.3 坐标系统变更

**旧系统（硬编码）**：

```cpp
// 直接使用 SDL 坐标
int mouseX = event.motion.x;
int mouseY = event.motion.y;
```

**新系统（适配）**：

```cpp
// 使用 DisplayManager 进行坐标变换
auto [gameX, gameY] = m_displayManager->WindowToGameCoords(
    event.motion.x, event.motion.y
);
```

---

## 🎮 六、使用示例

### 6.1 初始化

```cpp
// Game.cpp Initialize()

m_displayManager = std::make_unique<Display::DisplayManager>(
    m_window, m_sdlRenderer
);

if (!m_displayManager->Initialize()) {
    LOG_ERROR("Failed to initialize DisplayManager");
    return false;
}

// 加载保存的显示设置
m_displayManager->LoadDisplaySettings();
```

### 6.2 事件处理

```cpp
void Game::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_MAXIMIZED:
            case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
                m_displayManager->HandleWindowEvent(event.window);
                break;

            case SDL_EVENT_MOUSE_MOTION:
                // 使用转换后的坐标
                auto [gameX, gameY] = m_displayManager->WindowToGameCoords(
                    static_cast<int>(event.motion.x),
                    static_cast<int>(event.motion.y)
                );
                m_inputManager->UpdateMousePosition(gameX, gameY);
                break;
        }
    }
}
```

### 6.3 菜单集成

```cpp
// MenuScene.cpp 中显示显示设置菜单

void MenuScene::ShowDisplaySettings() {
    auto& displayMgr = GetDisplayManager();

    // 显示模式按钮
    if (m_windowedButton->IsClicked()) {
        displayMgr->SetDisplayMode(Display::DisplayMode::WINDOWED);
    }
    if (m_fullscreenButton->IsClicked()) {
        displayMgr->SetDisplayMode(Display::DisplayMode::FULLSCREEN_EXCLUSIVE);
    }

    // 分辨率选择
    for (const auto& resolution : displayMgr->GetAvailableResolutions()) {
        if (m_resolutionDropdown->SelectItem(resolution.label)) {
            displayMgr->SetResolutionPreset(resolution);
        }
    }

    // 保存设置
    if (m_applyButton->IsClicked()) {
        displayMgr->SaveDisplaySettings();
    }
}
```

---

## 📊 七、实现阶段规划

### Phase 1: 基础框架（1-2 周）

- [ ] 创建 Display 模块基础文件
- [ ] 实现 DisplayMode、Resolution 等基础类型
- [ ] 实现 ResolutionManager 基本功能
- [ ] 实现 ViewportManager 坐标变换

### Phase 2: 核心管理器（2-3 周）

- [ ] 实现完整的 DisplayManager
- [ ] 集成到 Game 类
- [ ] 处理窗口事件
- [ ] 实现坐标变换系统

### Phase 3: 持久化和配置（1-2 周）

- [ ] 实现 DisplaySettings（JSON 序列化）
- [ ] 加载/保存配置文件
- [ ] 处理 DPI 感知

### Phase 4: UI 集成（2-3 周）

- [ ] 创建显示设置菜单 UI
- [ ] 集成到主菜单
- [ ] 测试各种分辨率和模式
- [ ] 性能优化

### Phase 5: 测试和优化（1-2 周）

- [ ] 多屏幕测试
- [ ] 高 DPI 显示器测试
- [ ] 性能分析
- [ ] Bug 修复

---

## 🔍 八、关键技术点

### 8.1 SDL3 显示模式 API

```cpp
// 获取显示器数量
int displayCount = SDL_GetNumVideoDisplays();

// 获取显示模式
SDL_DisplayMode** modes;
int modeCount = SDL_GetFullscreenDisplayModes(displayIndex, &modes);

// 切换全屏
SDL_SetWindowFullscreen(window, true);

// 无边框全屏
SDL_SetWindowBordered(window, false);
SDL_SetWindowMaximized(window, true);

// DPI 处理
float dpiScale = SDL_GetWindowDisplayScale(window);
```

### 8.2 Letterbox/Pillarbox 实现

```cpp
// 计算适配矩形
SDL_Rect CalculateViewport(int windowW, int windowH,
                           int gameW, int gameH) {
    float windowAspect = (float)windowW / windowH;
    float gameAspect = (float)gameW / gameH;

    if (windowAspect > gameAspect) {
        // 宽边框（pillarbox）
        int height = windowH;
        int width = (int)(windowH * gameAspect);
        int x = (windowW - width) / 2;
        return {x, 0, width, height};
    } else {
        // 高边框（letterbox）
        int width = windowW;
        int height = (int)(windowW / gameAspect);
        int y = (windowH - height) / 2;
        return {0, y, width, height};
    }
}
```

### 8.3 坐标变换矩阵

```
窗口坐标 (mouseX, mouseY)
    ↓ [减去偏移]
视口坐标 (viewX, viewY)
    ↓ [应用缩放]
游戏逻辑坐标 (gameX, gameY)
    ↓ [应用 DPI]
渲染坐标 (renderX, renderY)
```

---

## ⚙️ 九、配置文件格式

### display_settings.json

```json
{
  "displayMode": "WINDOWED",
  "windowWidth": 1280,
  "windowHeight": 720,
  "renderWidth": 1280,
  "renderHeight": 720,
  "scalingStrategy": "ASPECT_RATIO_STRICT",
  "useNativeResolution": false,
  "lastUsedMonitor": 0
}
```

---

## 🚀 十、性能考虑

1. **减少重绘**：只在分辨率改变时重新计算 viewport
2. **GPU 优化**：使用 SDL 的视口功能而非 CPU 端缩放
3. **缓存**：缓存常用的坐标变换结果
4. **批处理**：合并显示设置变更的渲染命令

## 📚 十二、参考资源

- SDL3 官方文档：https://wiki.libsdl.org/SDL3
- 游戏显示模式最佳实践：Game Engine Architecture
- DPI 感知指南：https://docs.microsoft.com/en-us/windows/win32/hidpi/high-dpi-desktop-application-development-on-windows
