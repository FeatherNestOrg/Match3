# Display Module

## Overview

The Display module provides comprehensive display management for the Match-3 game, supporting multiple display modes, resolutions, and platforms including desktop and Android.

## Architecture

```
Display Module
├── DisplayMode.hpp          # Core types and enums
├── DisplayManager          # Main controller
├── ResolutionManager       # Resolution and DPI management
├── ViewportManager         # Coordinate transformation
├── DisplaySettings         # Persistent configuration
└── PlatformDisplay         # Platform-specific utilities
```

## Key Features

### 1. Display Modes
- **Windowed**: Traditional windowed mode with borders
- **Fullscreen Exclusive**: Native fullscreen with exclusive display access
- **Fullscreen Borderless**: Borderless window that covers the entire screen

### 2. Scaling Strategies
- **Aspect Ratio Strict**: Maintains aspect ratio with letterbox/pillarbox
- **Fill Screen**: Stretches to fill the screen (may distort)
- **Integer Scale**: Pixel-perfect integer scaling (ideal for pixel art)
- **Dynamic Scale**: Adaptive scaling based on screen size

### 3. Platform Support
- **Desktop (Windows/Linux/macOS)**: Full window management and resolution switching
- **Android**: Automatic detection of usable screen area, DPI awareness, safe area support
- **Cross-platform**: Unified API for all platforms

### 4. Coordinate Transformation
- Automatic conversion between window coordinates and game coordinates
- Viewport management with letterbox/pillarbox support
- DPI-aware rendering for high-resolution displays

## Usage

### Initialization

```cpp
// In Game class initialization
m_displayManager = std::make_unique<Display::DisplayManager>(m_window, m_sdlRenderer);
if (!m_displayManager->Initialize()) {
    LOG_ERROR("Failed to initialize DisplayManager");
    return false;
}

// Load saved settings
m_displayManager->LoadDisplaySettings();
```

### Changing Display Mode

```cpp
// Set to fullscreen
m_displayManager->SetDisplayMode(Display::DisplayMode::FULLSCREEN_EXCLUSIVE);

// Set to windowed
m_displayManager->SetDisplayMode(Display::DisplayMode::WINDOWED);

// Set to borderless fullscreen
m_displayManager->SetDisplayMode(Display::DisplayMode::FULLSCREEN_BORDERLESS);
```

### Changing Resolution

```cpp
// Set specific resolution
m_displayManager->SetResolution(1920, 1080);

// Get available resolutions
auto resolutions = m_displayManager->GetAvailableResolutions();
for (const auto& res : resolutions) {
    LOG_INFO("Available: {}", res.label);
}
```

### Coordinate Transformation

```cpp
// Convert window coordinates to game coordinates (for mouse events)
auto [gameX, gameY] = m_displayManager->WindowToGameCoords(windowX, windowY);

// Convert game coordinates to render coordinates
auto [renderX, renderY] = m_displayManager->GameToRenderCoords(gameX, gameY);
```

### Event Handling

```cpp
// In event loop
switch (event.type) {
    case SDL_EVENT_WINDOW_RESIZED:
    case SDL_EVENT_WINDOW_MAXIMIZED:
    case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
        m_displayManager->HandleWindowEvent(event.window);
        break;
        
    case SDL_EVENT_DISPLAY_ORIENTATION:
    case SDL_EVENT_DISPLAY_ADDED:
        m_displayManager->HandleDisplayChangeEvent(event.display);
        break;
}
```

### Settings Persistence

```cpp
// Save current settings
m_displayManager->SaveDisplaySettings("display_settings.json");

// Load settings
m_displayManager->LoadDisplaySettings("display_settings.json");
```

## Android-Specific Features

### Usable Screen Size Detection

On Android, the Display module automatically detects the usable screen area, accounting for:
- System status bar
- Navigation bar
- Virtual buttons
- Notches/cutouts

```cpp
// Get usable screen size (Android-aware)
auto [width, height] = PlatformDisplay::GetUsableScreenSize();
```

### DPI and Density Detection

```cpp
// Get device DPI
float dpi = PlatformDisplay::GetDeviceDPI();

// Get density category (ldpi, mdpi, hdpi, xhdpi, xxhdpi, xxxhdpi)
const char* density = PlatformDisplay::GetDensityCategory();
```

### Safe Area Support

```cpp
// Get safe area (accounts for notches, rounded corners, etc.)
SDL_Rect safeArea = PlatformDisplay::GetSafeArea();
```

## Configuration File Format

The display settings are saved in JSON format:

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

## Implementation Details

### Viewport Calculation

The ViewportManager calculates the viewport based on the selected scaling strategy:

1. **Aspect Ratio Strict**: Calculates letterbox/pillarbox to maintain aspect ratio
2. **Fill Screen**: Uses full window size
3. **Integer Scale**: Calculates the largest integer multiple that fits
4. **Dynamic Scale**: Adapts to window size

### Coordinate Systems

```
Window Coordinates (Input)
    ↓ [WindowToGameCoords]
Game Coordinates (Logic)
    ↓ [GameToRenderCoords]
Render Coordinates (Output)
```

### Resolution Detection

- Desktop: Uses SDL3's display mode enumeration
- Android: Uses SDL3's display bounds with platform-specific adjustments
- Fallback: Provides sensible defaults if detection fails

## Best Practices

1. **Initialize Early**: Initialize DisplayManager before creating game scenes
2. **Load Settings**: Always try to load saved settings for consistent user experience
3. **Save on Exit**: Save settings when the game closes
4. **Handle Events**: Always handle window and display events for proper responsiveness
5. **Use Coordinate Transformation**: Always convert coordinates for accurate input handling

## Future Enhancements

- [ ] Multi-monitor support
- [ ] Custom resolution presets
- [ ] Aspect ratio override options
- [ ] Advanced DPI scaling modes
- [ ] iOS platform support
- [ ] Game-specific viewport configurations

## Dependencies

- SDL3: Core display and window management
- Standard C++ filesystem: Configuration file handling

## License

Part of the Match-3 game project.
