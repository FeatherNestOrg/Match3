# 全局统一坐标系（Logical Game Coordinates）实现计划

## 目的

将项目中渲染、UI 布局和输入事件统一到一个固定的逻辑坐标系（Game coords），例如当前的 GAME_WIDTH × GAME_HEIGHT（默认 800 ×
600）。所有输入事件先被映射到逻辑坐标，场景与 UI 使用逻辑坐标进行布局和命中检测；渲染层把逻辑坐标映射为渲染像素坐标（render
coords）。此方案解决因窗口分辨率/缩放策略变化导致的鼠标位置错位问题，并使缩放策略集中在 `Display`/`ViewportManager` 层。

---

## 高层执行计划（简述）

1. 提供/增强转换与渲染辅助（Renderer 支持 Game->Render 转换或绑定转换回调）。
2. 将输入事件（SDL 原始 window pixels）通过 `DisplayManager::WindowToGameCoords` 映射为逻辑坐标并传递给场景/UI。
3. 把 UI 组件（Button、Panel、Label、等）和场景内 UI 布局语义切换成逻辑坐标。
4. 渲染时统一由 Renderer 把逻辑坐标转换为渲染坐标绘制。
5. 处理视口外（letterbox/pillarbox）点击为“场外”，避免误触。
6. 增加单元测试与回归测试，确保在不同缩放策略下行为一致。

---

## 任务清单（可分为多个 PR）

阶段 A — 基础设施（低风险）

- [ ] 在 `Renderer` 中增加 Game* API 或可注入的转换回调（Game -> Render）。
- [ ] 新增/封装转换辅助工具（TransformUtil，可选）。
- [ ] 为 `Renderer` 绑定 `DisplayManager::GameToRenderCoords`（或注入回调）。

阶段 B — 输入与事件（中等风险）

- [ ] 在 `Core/Game.cpp` 中将 SDL 鼠标事件的 window 坐标通过 `DisplayManager::WindowToGameCoords` 映射为逻辑坐标再传递。
- [ ] 改善 `ViewportManager::WindowToGameCoords` 行为：在视口外返回明确信号（例如 `std::optional<std::pair<int,int>>` 或
  `bool IsPointInViewport`）。
- [ ] 统一 `InputManager` 的回调语义为“传递逻辑坐标”。

阶段 C — UI 与场景迁移（较大改动）

- [ ] 把 `UIComponent` 及其子类（Button/Panel/Label）的位置/大小视为逻辑坐标。
- [ ] 修改这些组件的 `Render` 实现，使用 Renderer 的 Game* 方法或在渲染前把坐标转换为 render coords。
- [ ] 将 `MenuScene`、`GameScene`、`SettingsScene` 中创建 UI 的位置/尺寸从 window pixels 迁移到逻辑坐标（或使用从 window->
  game 的临时转换来逐步迁移）。

阶段 D — 鼠标/游戏逻辑适配（小）

- [ ] 把 `MouseHandler` 的输入语义更新为接受逻辑坐标。
- [ ] 确保棋盘点击/拖拽等都基于逻辑坐标（例如 `ScreenToBoard` 将接收 game coords）。

阶段 E — 视口边界与黑边处理（重要）

- [ ] 在 `WindowToGameCoords` 中对视口外点返回 `std::nullopt`（或明确的 out-of-bounds 信号）。
- [ ] 在 `Game::HandleEvents` 处理 nullopt（选择忽略或传递给 scene 作为 out-of-viewport）。

阶段 F — 清理、优化与文档（后期）

- [ ] 移除临时兼容代码路径。
- [ ] 对计算进行缓存（resize 时更新转换缓存），减少 per-frame 开销。
- [ ] 更新 README、Display 模块文档，添加开发者指南和迁移说明。

---

## 需要修改/检查的文件（初始清单）

- 核心事件与渲染绑定
    - `src/Core/Game.cpp`  （事件分发改为 Window->Game -> Scene）
    - `src/Display/DisplayManager.hpp/.cpp`（保证暴露 GameToRenderCoords/WindowToGameCoords）
    - `src/Display/ViewportManager.hpp/.cpp`（改进 WindowToGameCoords 的 out-of-viewport 行为）
- 渲染层
    - `src/Render/Renderer.hpp/.cpp`（新增 Game* 绘制 API 或注入 DisplayManager 的转换回调）
    - `src/Render/FontRenderer.hpp/.cpp`（支持以逻辑坐标渲染文本）
- UI 层
    - `src/UI/UIManager.hpp/.cpp`
    - `src/UI/Components/UIComponent.hpp/.cpp`
    - `src/UI/Components/Button.*`, `Panel.*`, `Label.*` 等
- 输入/鼠标
    - `src/Input/InputManager.cpp`（回调语义文档或改造）
    - `src/Input/MouseHandler.hpp/.cpp`（接受逻辑坐标）
- 场景
    - `src/Scenes/MenuScene.*`, `GameScene.*`, `SettingsScene.*`（UI 布局迁移）
- 配套测试
    - 新增 `tests/ViewportManagerTests`（可选）

---

## 关键设计决策与契约（必须在 PR 文档中强调）

1. 坐标契约
    - 输入：SDL 事件里是 window pixel coords -> `DisplayManager::WindowToGameCoords(windowX,windowY)` -> 返回
      `std::optional<gameX,gameY>`（game coords）或 `nullopt` 表示视口外。
    - Scene/UI：`HandleMouse*(gameX, gameY)` 接受逻辑坐标（范围通常在 [0,GAME_WIDTH) × [0,GAME_HEIGHT)）。
    - 渲染：Renderer Game* API 接受逻辑坐标并内部调用 `DisplayManager::GameToRenderCoords` 做映射，然后绘制。

2. 视口外点击
    - 明确不要 silent-clamp（不可把视口外点击喂给边界位置），而是返回 nullopt 或由 `Game` 层判断并忽略或特殊处理。

3. 过渡兼容
    - 在迁移期间给 `UI` 层提供兼容构造（接受 window pixels 并即时转换为 game coords），方便分阶段改造而不一次性修改大量场景代码。

---

## 验证与质量门（每个 PR 的检查项）

- 构建：`cmake` 成功配置并编译（Debug/Release）。
- 单元：`ViewportManager` 的映射单元测试覆盖（Window->Game, Game->Render, 视口外检测）。
- 手动 smoke 测试：运行游戏并验证以下用例
    1. 不同 scaling 策略（ASPECT_RATIO_STRICT、FILL_SCREEN、INTEGER_SCALE、DYNAMIC_SCALE）下的 UI hover/click 行为与视觉对齐。
    2. 在 letterbox/pillarbox 黑边上点击不会触发 UI 或棋盘。
    3. 拖拽与连续点击不会因为坐标系切换而产生断裂或漂移。
- 性能：对帧率影响微小（若发现回归则缓存转换结果或减少 per-element 浮点运算）。

---

## PR 划分建议（每个 PR 小且可回滚）

- PR #1（Infra）: Renderer Game* API 或转换回调 + DisplayManager 绑定 + 基础单元测试。
- PR #2（Input）: Game.cpp 修改事件分发（Window -> Game），ViewportWindowToGame 行为改进 + input callback 语义统一。
- PR #3（UI Pilot）: 将 `MenuScene`、`GameScene` 的部分组件迁移到逻辑坐标（使用兼容函数或直接改造），并验证交互。
- PR #4（UI 全面迁移）: 迁移剩余 UI 组件与场景，移除兼容层。
- PR #5（优化与文档）: 性能优化、像素对齐调整、更新 README。

---

## 本地构建与快速验证命令（示例）

在仓库根目录：

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j
# 运行可执行（可执行名视 CMake 配置而定）
./Match3
```

---

## 注意事项与风险评估

- 风险：UI/Scene 位置改动量较大，容易引入视觉回归。通过分阶段迁移与兼容路径把风险最小化。
- DPI 与高分屏：`ResolutionManager`/`ViewportManager` 已有 DPI 支持，迁移过程中必须保持 `SDL_GetCurrentRenderOutputSize` 与
  DPI 缩放一致。
- 像素对齐问题：整数缩放策略下需特别处理 round/floor 保持像素对齐，避免半像素模糊。

---
