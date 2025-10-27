# Match-3 游戏 ECS 架构实现方案

## 📋 项目现状

**已完成**：

- ✅ ECS 架构基础搭建（基于 EnTT）
- ✅ 核心游戏循环和事件处理
- ✅ 棋盘系统（初始化、宝石管理）
- ✅ 匹配检测系统
- ✅ 宝石交换系统
- ✅ 基础动画系统
- ✅ 粒子系统
- ✅ 简单渲染系统
- ✅ 状态机管理

**当前架构特点**：

- 使用 EnTT 作为 ECS 框架
- 模块化系统设计：BoardSystem、MatchDetectionSystem、SwapSystem、AnimationSystem、RenderSystem
- 组件化数据结构：Gem、Position、Velocity、Renderable、Animation 等
- 集中的 GameStateManager 管理游戏状态和系统协调
- SystemManager 统一管理所有系统的更新

---

## 🎯 阶段一：UI 系统完整实现（第 1-3 周）

### 目标

建立完整的 UI 框架，支持菜单、HUD 和游戏界面的渲染

### 为什么先做 UI

1. 不依赖复杂的游戏逻辑，可以独立开发
2. UI 系统是后续所有功能的基础
3. 能尽早获得完整的游戏体验反馈

### 任务清单

#### 1.1 UI 基础框架设计

- [ ] **创建 UI 组件基类体系**

  - 文件：`src/UI/Components/UIComponent.hpp`
  - 实现 `UIComponent` 基类（位置、大小、可见性、启用状态）
  - 实现 `UIContainer` 容器类（子组件管理）
  - 实现层级和 Z-order 管理

- [ ] **创建基础 UI 组件**

  - 文件：`src/UI/Components/`
  - `Button.hpp/cpp` - 按钮组件（普通、按下、悬停状态）
  - `Label.hpp/cpp` - 文本标签（静态文本显示）
  - `Panel.hpp/cpp` - 面板/背景（矩形容器）
  - `Slider.hpp/cpp` - 滑块（音量、进度条等）
  - `Grid.hpp/cpp` - 网格布局组件

- [ ] **实现 UI 事件系统**

  - 文件：`src/UI/Events/UIEvents.hpp`
  - 点击事件（onClick）
  - 悬停事件（onHover, onLeave）
  - 状态变化事件
  - 使用委托或观察者模式

- [ ] **实现 UI 渲染管理**

  - 文件：`src/UI/UIRenderer.hpp/cpp`
  - 将 UI 组件转换为渲染命令
  - 处理 Z-order 和深度排序
  - 优化批处理渲染

- [ ] **创建 UI 管理器**
  - 文件：`src/UI/UIManager.hpp/cpp`
  - 管理 UI 组件树
  - 处理 UI 事件分发
  - 提供快速访问 UI 的接口

#### 1.2 文字渲染支持

- [ ] **集成字体系统**

  - 评估是否使用 SDL_ttf 或自实现位图字体
  - 实现字体加载和缓存

- [ ] **实现文字渲染**
  - 文件：`src/Render/FontRenderer.hpp/cpp`
  - 支持不同字体、大小、颜色
  - 文字对齐（左、中、右）
  - 文字宽度测量（用于布局）

#### 1.3 布局系统

- [ ] **实现自动布局引擎**
  - 文件：`src/UI/Layout/LayoutManager.hpp`
  - 相对位置布局
  - 网格布局（行列自动排列）
  - 锚点布局（相对于父容器）
  - 自适应屏幕分辨率

#### 1.4 主菜单场景

- [ ] **创建主菜单 UI**

  - 文件：`src/Scenes/MenuScene.hpp/cpp`
  - 游戏标题
  - "开始游戏"按钮
  - "设置"按钮
  - "退出"按钮
  - 背景装饰

- [ ] **实现场景切换机制**
  - 文件：`src/Scenes/SceneManager.hpp/cpp`
  - 场景栈管理
  - 场景生命周期（初始化、激活、暂停、销毁）
  - 场景切换动画

### 交付物

```
src/UI/
├── Components/
│   ├── UIComponent.hpp/cpp     # UI基类
│   ├── UIContainer.hpp/cpp     # 容器类
│   ├── Button.hpp/cpp          # 按钮
│   ├── Label.hpp/cpp           # 文本标签
│   ├── Panel.hpp/cpp           # 面板
│   ├── Slider.hpp/cpp          # 滑块
│   └── Grid.hpp/cpp            # 网格布局
├── Events/
│   └── UIEvents.hpp            # UI事件系统
├── Layout/
│   └── LayoutManager.hpp/cpp   # 布局管理
├── UIRenderer.hpp/cpp          # UI渲染
└── UIManager.hpp/cpp           # UI管理器

src/Scenes/
├── Scene.hpp/cpp               # 场景基类
├── SceneManager.hpp/cpp        # 场景管理
├── MenuScene.hpp/cpp           # 主菜单
└── GameScene.hpp/cpp           # 游戏场景（升级）

src/Render/
└── FontRenderer.hpp/cpp        # 字体渲染
```

### 验证标准

- [ ] 可正确显示主菜单
- [ ] 按钮可点击且有视觉反馈
- [ ] 文字正确显示和对齐
- [ ] 场景切换流畅
- [ ] 无内存泄漏
- [ ] UI 响应延迟 < 16ms

---

## 🚀 阶段二：批量渲染优化（第 4 周）

### 目标

优化渲染性能，实现高效的批处理渲染

### 为什么重要

1. 当前逐个渲染宝石效率低
2. 为后续大规模宝石和特效做准备
3. 改善帧率稳定性

### 任务清单

#### 2.1 渲染批处理架构

- [ ] **设计批处理系统**

  - 文件：`src/Render/BatchRenderer.hpp/cpp`
  - 按纹理分组渲染命令
  - 支持不同的顶点属性（位置、颜色、纹理坐标）
  - 动态批处理队列

- [ ] **实现顶点缓冲**

  - 使用 SDL3 的渲染 API 优化
  - 或使用低级图形 API（如需要）
  - 支持动态更新

- [ ] **创建渲染命令系统**
  - 文件：`src/Render/RenderCommand.hpp`
  - 定义不同类型的渲染命令
  - 命令队列管理
  - 命令排序和分组

#### 2.2 升级 RenderSystem

- [ ] **集成批处理到 RenderSystem**

  - 修改 `src/Systems/RenderSystem.hpp/cpp`
  - 改用批处理渲染宝石
  - 改用批处理渲染粒子
  - 改用批处理渲染 UI 元素

- [ ] **实现渲染统计和分析**
  - 记录批次数、顶点数、绘制调用数
  - 添加性能调试信息显示

#### 2.3 宝石贴图集支持

- [ ] **创建宝石精灵表**

  - 或使用现有资源
  - 定义纹理坐标映射

- [ ] **实现精灵表渲染**
  - 从贴图集中提取不同宝石
  - 支持动画帧

### 交付物

```
src/Render/
├── BatchRenderer.hpp/cpp       # 批处理渲染
├── RenderCommand.hpp           # 渲染命令
└── SpriteAtlas.hpp/cpp         # 精灵表管理

resources/
└── textures/
    └── gems_atlas.png          # 宝石贴图集
```

### 验证标准

- [ ] 渲染性能提升至少 50%
- [ ] 支持 1000+ 宝石同屏（如需要）
- [ ] 帧率稳定 60 FPS
- [ ] 渲染命令数 < 100
- [ ] 无渲染闪烁或错误

---

## 🎮 阶段三：游戏规则完整化（第 5-6 周）

### 目标

实现完整的游戏玩法和规则系统

### 任务清单

#### 3.1 计分系统

- [ ] **创建 ScoreManager**

  - 文件：`src/Game/ScoreManager.hpp/cpp`
  - 基础分数计算（3 连=30 分，4 连=50 分，5 连=100 分）
  - 连击加成（1.25x, 1.5x, 2.0x）
  - 特殊宝石奖励

- [ ] **实现分数显示更新**
  - 动画显示分数变化
  - 飘字特效（+100）

#### 3.2 特殊宝石系统

- [ ] **设计特殊宝石类型**

  - 4 连水平/垂直炸弹
  - 5 连彩色炸弹
  - L/T 形爆炸

- [ ] **创建特殊宝石组件**

  - 文件：`src/Components/SpecialGem.hpp`
  - 特殊宝石类型字段
  - 效果范围标记

- [ ] **实现特殊宝石生成逻辑**

  - 在 BoardSystem 中检测消除时生成
  - 根据匹配类型判断

- [ ] **实现特殊宝石效果**
  - 文件：`src/Systems/SpecialGemSystem.hpp/cpp`
  - 炸弹效果（范围消除）
  - 彩色炸弹效果（消除指定颜色）
  - 涟漪效果动画

#### 3.3 关卡系统

- [ ] **创建关卡管理器**

  - 文件：`src/Game/LevelManager.hpp/cpp`
  - 关卡数据结构
  - 难度等级
  - 目标设置

- [ ] **实现关卡目标**

  - 分数目标
  - 移动限制
  - 特定宝石消除目标

- [ ] **创建关卡数据文件**
  - 文件：`resources/data/levels.json`
  - JSON 格式关卡配置

#### 3.4 游戏结束状态

- [ ] **实现游戏胜利逻辑**

  - 目标达成判定
  - 胜利界面（显示得分、星级等）

- [ ] **实现游戏失败逻辑**

  - 无可用移动判定
  - 失败界面

- [ ] **实现失败检测系统**
  - 文件：`src/Systems/HintSystem.hpp/cpp`
  - 检测无可用匹配
  - 自动给出提示

### 交付物

```
src/Game/
├── ScoreManager.hpp/cpp        # 计分管理
├── LevelManager.hpp/cpp        # 关卡管理
└── SpecialGemEffects.hpp/cpp   # 特殊宝石效果

src/Systems/
├── SpecialGemSystem.hpp/cpp    # 特殊宝石系统
└── HintSystem.hpp/cpp          # 提示系统

resources/data/
└── levels.json                 # 关卡配置
```

### 验证标准

- [ ] 分数计算正确
- [ ] 特殊宝石正确生成和工作
- [ ] 关卡正确加载
- [ ] 胜利/失败条件正确判定
- [ ] 提示系统有效

---

## 🎨 阶段四：游戏 HUD 和菜单界面（第 7 周）

### 目标

实现完整的游戏内界面和菜单系统

### 任务清单

#### 4.1 游戏 HUD

- [ ] **创建游戏 HUD 面板**

  - 文件：`src/UI/Panels/GameHUD.hpp/cpp`
  - 分数显示
  - 目标进度显示
  - 剩余移动次数
  - 当前等级标签

- [ ] **实现 HUD 更新系统**
  - 实时同步游戏数据
  - 分数变化动画
  - 目标进度动画

#### 4.2 暂停菜单

- [ ] **创建暂停菜单 UI**

  - 文件：`src/Scenes/PauseScene.hpp/cpp`
  - 半透明背景遮罩
  - "继续"按钮
  - "主菜单"按钮
  - "设置"按钮

- [ ] **实现暂停机制**
  - 修改 GameStateManager 支持暂停状态
  - 暂停时停止系统更新（除 UI 外）
  - 音乐暂停

#### 4.3 胜利/失败界面

- [ ] **创建结果界面**

  - 文件：`src/Scenes/ResultScene.hpp/cpp`
  - 胜利/失败提示
  - 最终得分显示
  - 星级显示（1-3 星）
  - 按钮：重试、下一关、主菜单

- [ ] **实现结果统计**
  - 根据得分等级给予星级
  - 统计用时、移动数等

#### 4.4 设置菜单

- [ ] **创建设置菜单 UI**

  - 文件：`src/Scenes/SettingsScene.hpp/cpp`
  - 音量滑块（背景音乐、音效）
  - 难度选择
  - 分辨率选择
  - 返回按钮

- [ ] **实现设置持久化**
  - 文件：`src/Utils/ConfigManager.hpp/cpp`
  - 保存到配置文件
  - 应用程序启动时加载

### 交付物

```
src/UI/Panels/
└── GameHUD.hpp/cpp            # 游戏HUD

src/Scenes/
├── PauseScene.hpp/cpp         # 暂停场景
├── ResultScene.hpp/cpp        # 结果场景
└── SettingsScene.hpp/cpp      # 设置场景

src/Utils/
└── ConfigManager.hpp/cpp      # 配置管理
```

### 验证标准

- [ ] HUD 信息显示准确实时
- [ ] 菜单切换流畅
- [ ] 设置可正确保存和加载
- [ ] UI 在不同分辨率下布局正确
- [ ] 无 UI 操作卡顿

---

## 🔊 阶段五：音频系统（第 8 周）

### 目标

集成音效和背景音乐，提供完整的音频体验

### 任务清单

#### 5.1 音频管理器

- [ ] **创建 AudioManager**

  - 文件：`src/Audio/AudioManager.hpp/cpp`
  - 支持背景音乐播放
  - 支持音效播放（支持多声道）
  - 音量控制（主音量、音乐、音效分别控制）
  - 静音功能

- [ ] **实现音频缓存**
  - 预加载常用音效
  - 支持动态加载

#### 5.2 游戏音效

- [ ] **实现交换音效**

  - 宝石交换声音
  - 非法交换声音

- [ ] **实现消除音效**

  - 3 连消除音
  - 4 连消除音（不同音色）
  - 5 连消除音（不同音色）
  - 连击音效

- [ ] **实现特效音效**
  - 炸弹爆炸音
  - 特殊宝石激活音
  - UI 点击音

#### 5.3 背景音乐

- [ ] **创建音乐管理**
  - 文件：`src/Audio/MusicManager.hpp/cpp`
  - 菜单背景音乐
  - 游戏背景音乐
  - 胜利音乐
  - 失败音乐
  - 音乐淡入淡出过渡

#### 5.4 集成到游戏

- [ ] **在游戏事件时触发音效**
  - 修改各系统和 GameStateManager
  - 调用 AudioManager 播放音效

### 交付物

```
src/Audio/
├── AudioManager.hpp/cpp        # 音频管理
└── MusicManager.hpp/cpp        # 音乐管理

resources/audio/
├── music/
│   ├── menu_bg.ogg
│   ├── game_bg.ogg
│   ├── victory.ogg
│   └── defeat.ogg
└── sfx/
    ├── swap.ogg
    ├── match3.ogg
    ├── match4.ogg
    ├── match5.ogg
    ├── bomb.ogg
    ├── ui_click.ogg
    └── invalid_move.ogg
```

### 验证标准

- [ ] 音乐循环播放正常
- [ ] 音效触发准确无延迟
- [ ] 音量可正确调节
- [ ] 音频不会导致游戏卡顿
- [ ] 无音频爆音或失真

---

## ⚡ 阶段六：性能优化与高级功能（第 9-10 周）

### 目标

优化游戏性能，实现高级游戏特性

### 任务清单

#### 6.1 渲染性能优化

- [ ] **实现视锥剔除**
  - 只渲染可见区域的元素
- [ ] **实现动态图集管理**
  - 根据使用频率动态切换贴图集
- [ ] **实现 LOD（细节级别）系统**

  - 远处物体简化渲染

- [ ] **添加渲染统计和性能分析**
  - 显示 FPS、批次数、顶点数
  - 识别性能瓶颈

#### 6.2 内存优化

- [ ] **实现对象池**

  - 文件：`src/Utils/ObjectPool.hpp`
  - 粒子对象池
  - 动画对象池
  - 临时组件对象池

- [ ] **优化容器选择**

  - 使用 unordered_dense 替代 std::unordered_map
  - 使用 std::vector 代替链表

- [ ] **内存泄漏检测**
  - 使用 AddressSanitizer 检测
  - 定期检查内存使用

#### 6.3 输入系统完善

- [ ] **创建高级输入管理**

  - 文件：`src/Input/AdvancedInputManager.hpp/cpp`
  - 支持键盘、鼠标、触摸
  - 输入缓冲（处理同时输入）
  - 输入检测范围优化

- [ ] **实现拖拽交换**
  - 点击并拖动宝石进行交换

#### 6.4 高级特性

- [ ] **实现天气/环境系统**

  - 文件：`src/Game/EnvironmentSystem.hpp/cpp`
  - 随机环境效果影响游戏
  - 背景动画

- [ ] **实现连击动画和特效**

  - 高倍数连击时显示 COMBO 提示
  - 粒子爆炸增强

- [ ] **实现进度保存**
  - 文件：`src/Utils/SaveSystem.hpp/cpp`
  - 保存当前游戏进度
  - 支持多个存档槽位

### 交付物

```
src/Utils/
├── ObjectPool.hpp              # 对象池
└── SaveSystem.hpp/cpp          # 存档系统

src/Game/
└── EnvironmentSystem.hpp/cpp   # 环境系统

src/Input/
└── AdvancedInputManager.hpp/cpp # 高级输入管理
```

### 验证标准

- [ ] 帧率稳定 60 FPS
- [ ] 内存占用 < 150MB
- [ ] 无明显性能波动
- [ ] 对象池有效减少 GC
- [ ] 输入响应延迟 < 5ms

---

## 🎁 阶段七：高级内容和拓展（第 11-12 周）

### 目标

添加高级游戏特性和内容

### 任务清单

#### 7.1 多关卡系统

- [ ] **创建关卡进度系统**

  - 文件：`src/Game/ProgressManager.hpp/cpp`
  - 记录已完成关卡
  - 管理解锁状态

- [ ] **实现关卡选择界面**

  - 显示所有关卡网格
  - 显示完成状态和星级
  - 关卡信息预览

- [ ] **创建大量关卡**
  - 至少 50+ 个关卡
  - 逐步增加难度
  - 多样化目标设置

#### 7.2 挑战模式

- [ ] **创建时间限制模式**
  - 固定时间内得分最多
- [ ] **创建移动限制模式**

  - 固定移动次数内达到目标

- [ ] **创建无限模式**
  - 无目标，无限玩，直到无法操作

#### 7.3 成就和排行榜

- [ ] **实现成就系统**

  - 文件：`src/Game/AchievementSystem.hpp/cpp`
  - 定义各种成就
  - 成就解锁和显示

- [ ] **实现本地排行榜**
  - 文件：`src/Game/Leaderboard.hpp/cpp`
  - 保存高分记录
  - 排行榜显示界面

#### 7.4 每日挑战

- [ ] **创建每日挑战系统**
  - 文件：`src/Game/DailyChallengeManager.hpp/cpp`
  - 生成每日特殊挑战
  - 跟踪完成状态
  - 奖励系统

#### 7.5 动态难度调整

- [ ] **实现自适应难度**
  - 根据玩家表现调整难度
  - 跟踪平均胜率
  - 根据胜率调整宝石类型数量等

### 交付物

```
src/Game/
├── ProgressManager.hpp/cpp     # 进度管理
├── AchievementSystem.hpp/cpp   # 成就系统
├── Leaderboard.hpp/cpp         # 排行榜
└── DailyChallengeManager.hpp/cpp # 每日挑战

src/UI/Scenes/
├── LevelSelectScene.hpp/cpp    # 关卡选择
├── ChallengeScene.hpp/cpp      # 挑战模式
└── LeaderboardScene.hpp/cpp    # 排行榜界面

resources/data/
└── challenges.json             # 挑战配置
```

### 验证标准

- [ ] 所有模式可正常进行
- [ ] 成就系统正确触发
- [ ] 排行榜正确保存和显示
- [ ] 每日挑战每天更新
- [ ] 难度调整平衡有效

---

## 🧪 阶段八：测试和优化（第 13 周）

### 目标

全面测试和优化游戏

### 任务清单

#### 8.1 单元测试

- [ ] **测试关键系统**
  - 文件：`tests/`
  - 匹配检测算法测试
  - 分数计算测试
  - 关卡加载测试

#### 8.2 集成测试

- [ ] **测试系统协作**
  - 测试完整游戏流程
  - 测试状态转换
  - 测试菜单导航

#### 8.3 性能测试和剖析

- [ ] **运行性能测试**

  - 测试帧率稳定性
  - 测试内存泄漏
  - 测试长时间运行

- [ ] **生成性能报告**
  - 记录关键指标
  - 识别优化机会

#### 8.4 兼容性测试

- [ ] **测试不同分辨率**
  - 1280x720, 1920x1080, 3840x2160 等
- [ ] **测试不同输入方式**
  - 键盘、鼠标、触摸

#### 8.5 用户体验测试

- [ ] **收集反馈**

  - 游戏难度反馈
  - UI 可用性反馈
  - 性能反馈

- [ ] **根据反馈调整**
  - 平衡游戏难度
  - 改进 UI 设计
  - 优化性能

### 交付物

```
tests/
├── MatchDetectionTests.cpp
├── ScoreCalculationTests.cpp
├── LevelLoadingTests.cpp
└── IntegrationTests.cpp

docs/
├── PerformanceReport.md
├── UserFeedback.md
└── OptimizationGuide.md
```

### 验证标准

- [ ] 所有单元测试通过
- [ ] 所有集成测试通过
- [ ] 帧率稳定 60 FPS（所有分辨率）
- [ ] 无内存泄漏
- [ ] 响应时间 < 16ms
- [ ] 用户反馈满意度 > 80%

---

## 📦 最终项目结构

```
Match3/
├── CMakeLists.txt
├── IMPLEMENTATION_PLAN.md
├── README.md
├── src/
│   ├── main.cpp
│   ├── Core/
│   │   ├── Game.hpp/cpp
│   │   ├── Logger.hpp/cpp
│   │   └── Config.hpp
│   ├── Components/
│   │   ├── Common.hpp           # Position, Velocity, Renderable
│   │   ├── Gem.hpp              # 宝石组件
│   │   ├── Animation.hpp        # 动画组件
│   │   ├── Particle.hpp         # 粒子组件
│   │   └── SpecialGem.hpp       # 特殊宝石组件
│   ├── Systems/
│   │   ├── System.hpp           # 基类
│   │   ├── BoardSystem.hpp/cpp
│   │   ├── MatchDetectionSystem.hpp/cpp
│   │   ├── SwapSystem.hpp/cpp
│   │   ├── AnimationSystem.hpp/cpp
│   │   ├── ParticleSystem.hpp/cpp
│   │   ├── RenderSystem.hpp/cpp
│   │   ├── SpecialGemSystem.hpp/cpp
│   │   ├── HintSystem.hpp/cpp
│   │   └── LifetimeSystem.hpp/cpp
│   ├── Managers/
│   │   ├── GameStateManager.hpp/cpp
│   │   ├── SystemManager.hpp/cpp
│   │   ├── AudioManager.hpp/cpp
│   │   ├── MusicManager.hpp/cpp
│   │   └── SceneManager.hpp/cpp
│   ├── Factories/
│   │   └── EntityFactory.hpp/cpp
│   ├── Game/
│   │   ├── Board.hpp/cpp
│   │   ├── Gem.hpp/cpp
│   │   ├── MatchDetector.hpp/cpp
│   │   ├── GameState.hpp/cpp
│   │   ├── ScoreManager.hpp/cpp
│   │   ├── LevelManager.hpp/cpp
│   │   ├── ProgressManager.hpp/cpp
│   │   ├── AchievementSystem.hpp/cpp
│   │   ├── Leaderboard.hpp/cpp
│   │   ├── DailyChallengeManager.hpp/cpp
│   │   └── EnvironmentSystem.hpp/cpp
│   ├── Input/
│   │   ├── InputManager.hpp/cpp
│   │   └── AdvancedInputManager.hpp/cpp
│   ├── Render/
│   │   ├── Renderer.hpp/cpp
│   │   ├── Texture.hpp/cpp
│   │   ├── BatchRenderer.hpp/cpp
│   │   ├── RenderCommand.hpp
│   │   ├── FontRenderer.hpp/cpp
│   │   ├── ResourceManager.hpp/cpp
│   │   └── SpriteAtlas.hpp/cpp
│   ├── UI/
│   │   ├── Components/
│   │   │   ├── UIComponent.hpp/cpp
│   │   │   ├── UIContainer.hpp/cpp
│   │   │   ├── Button.hpp/cpp
│   │   │   ├── Label.hpp/cpp
│   │   │   ├── Panel.hpp/cpp
│   │   │   ├── Slider.hpp/cpp
│   │   │   └── Grid.hpp/cpp
│   │   ├── Events/
│   │   │   └── UIEvents.hpp
│   │   ├── Layout/
│   │   │   └── LayoutManager.hpp/cpp
│   │   ├── Panels/
│   │   │   └── GameHUD.hpp/cpp
│   │   ├── UIRenderer.hpp/cpp
│   │   └── UIManager.hpp/cpp
│   ├── Scenes/
│   │   ├── Scene.hpp/cpp
│   │   ├── SceneManager.hpp/cpp
│   │   ├── MenuScene.hpp/cpp
│   │   ├── GameScene.hpp/cpp
│   │   ├── PauseScene.hpp/cpp
│   │   ├── ResultScene.hpp/cpp
│   │   ├── SettingsScene.hpp/cpp
│   │   ├── LevelSelectScene.hpp/cpp
│   │   ├── ChallengeScene.hpp/cpp
│   │   └── LeaderboardScene.hpp/cpp
│   ├── Audio/
│   │   ├── AudioManager.hpp/cpp
│   │   └── MusicManager.hpp/cpp
│   ├── Utils/
│   │   ├── ObjectPool.hpp
│   │   ├── ConfigManager.hpp/cpp
│   │   ├── SaveSystem.hpp/cpp
│   │   └── GridUtils.hpp
│   └── CMakeLists.txt
├── tests/
│   ├── MatchDetectionTests.cpp
│   ├── ScoreCalculationTests.cpp
│   ├── LevelLoadingTests.cpp
│   └── IntegrationTests.cpp
├── resources/
│   ├── textures/
│   │   ├── gems_atlas.png
│   │   ├── ui_elements.png
│   │   └── backgrounds/
│   ├── audio/
│   │   ├── music/
│   │   │   ├── menu_bg.ogg
│   │   │   ├── game_bg.ogg
│   │   │   ├── victory.ogg
│   │   │   └── defeat.ogg
│   │   └── sfx/
│   │       ├── swap.ogg
│   │       ├── match3.ogg
│   │       ├── match4.ogg
│   │       ├── match5.ogg
│   │       ├── bomb.ogg
│   │       ├── ui_click.ogg
│   │       └── invalid_move.ogg
│   ├── fonts/
│   │   └── game_font.ttf
│   └── data/
│       ├── levels.json
│       ├── challenges.json
│       └── config.json
├── docs/
│   ├── ARCHITECTURE.md          # 架构文档
│   ├── ECS_GUIDE.md             # ECS使用指南
│   ├── PerformanceReport.md     # 性能报告
│   ├── API.md                   # API文档
│   └── DEVELOPER_GUIDE.md       # 开发指南
└── cmake/
    └── Modules/
        ├── FindEnTT.cmake
        ├── FindSDL3.cmake
        ├── FindSpdlog.cmake
        └── FindUnorderedDense.cmake
```

---

## 📊 阶段时间表

| 阶段 | 任务            | 预计时间 | 优先级    |
| ---- | --------------- | -------- | --------- |
| 1    | UI 系统完整实现 | 3 周     | ⭐⭐⭐ 高 |
| 2    | 批量渲染优化    | 1 周     | ⭐⭐⭐ 高 |
| 3    | 游戏规则完整化  | 2 周     | ⭐⭐⭐ 高 |
| 4    | HUD 和菜单      | 1 周     | ⭐⭐ 中   |
| 5    | 音频系统        | 1 周     | ⭐⭐ 中   |
| 6    | 性能优化        | 2 周     | ⭐⭐⭐ 高 |
| 7    | 高级内容        | 2 周     | ⭐ 低     |
| 8    | 测试优化        | 1 周     | ⭐⭐⭐ 高 |

**总预计时间**: 13 周

---

## 🏗️ 架构设计原则

### ECS 模式的优势利用

1. **解耦**：系统相互独立，便于维护和扩展
2. **组合**：通过组件组合创建复杂实体
3. **数据驱动**：所有游戏状态由数据（组件）驱动
4. **性能**：便于缓存优化和批处理

### 系统设计指导

```
每个系统职责单一：
- BoardSystem: 棋盘初始化、布局管理
- MatchDetectionSystem: 匹配检测（纯计算）
- SwapSystem: 宝石交换逻辑和动画
- AnimationSystem: 所有实体动画更新
- ParticleSystem: 粒子系统
- RenderSystem: 将组件转换为渲染指令
- SpecialGemSystem: 特殊宝石效果
- HintSystem: 无匹配检测和提示

系统间通信：
- 通过组件通信（读写共享数据）
- GameStateManager 协调系统执行顺序
- 状态机控制游戏流程
```

### 组件设计指导

```
组件应该是数据容器，不含逻辑：
✅ Position { x, y }
✅ Velocity { vx, vy }
✅ Animation { startTime, duration, ... }
❌ UpdateAnimation() 方法

组件应该尽可能小和简洁。
```

---

## 💡 开发建议

### 最佳实践

1. **按优先级实现**：优先完成高优先级阶段
2. **模块化开发**：每个模块独立测试
3. **增量集成**：逐步集成新模块
4. **频繁提交**：每天至少提交一次
5. **代码审查**：定期审查代码质量

### 调试技巧

```cpp
// 添加调试日志
M3_LOG_INFO("Event: {}", event_name);

// 可视化调试
#ifdef M3_DEBUG_DRAW
    renderer->DrawGrid();      // 显示网格
    renderer->DrawColliders(); // 显示碰撞体
#endif

// 性能分析
ScopedTimer timer("SystemName");
// ... 代码 ...
// 自动输出执行时间
```

### 性能目标

- **FPS**: 60 FPS（稳定）
- **内存**: < 200 MB（含所有资源）
- **启动时间**: < 3 秒
- **响应时间**: < 16 ms
- **批处理效率**: 90%+ 三角形在批次中

---

## 📚 关键参考资源

### ECS 学习

- EnTT 官方文档: https://github.com/skypjack/entt
- Match-3 ECS 架构示例
- 游戏编程模式

### 渲染优化

- 批处理技术
- 纹理图集管理
- 视锥剔除算法

### UI 框架设计

- 游戏 UI 最佳实践
- 响应式布局系统
- 事件系统设计

---

## ✅ 完成检查清单

### 每个阶段完成后

- [ ] 所有计划任务完成
- [ ] 代码编译无错误和警告
- [ ] 单元测试通过
- [ ] 集成测试通过
- [ ] 无内存泄漏（使用 ASan）
- [ ] 性能达标
- [ ] 代码风格统一
- [ ] 有意义的 Git 提交
- [ ] 代码注释完善
- [ ] 文档更新

### 发布前最终检查

- [ ] 所有功能正常工作
- [ ] 游戏性能稳定
- [ ] 用户界面友好
- [ ] 没有明显 Bug
- [ ] 美工资源完整
- [ ] 音频正常播放
- [ ] 多分辨率测试通过
- [ ] 不同输入方式测试通过
- [ ] 文档完整

---

## 🎮 游戏玩法总结

**最终游戏特性**：

- ✅ 标准 8x8 棋盘
- ✅ 6 种宝石类型
- ✅ 鼠标/触摸交互
- ✅ 匹配检测和消除
- ✅ 宝石下落和填充
- ✅ 连锁反应
- ✅ 特殊宝石（炸弹、彩色炸弹等）
- ✅ 分数系统和连击
- ✅ 关卡系统（50+ 关卡）
- ✅ 多种游戏模式（经典、时间挑战、移动限制）
- ✅ 成就和排行榜
- ✅ 音效和音乐
- ✅ 流畅动画
- ✅ 高性能渲染

---

**预计总开发时间**: 13 周（每周 20-30 小时）

**难度**: ⭐⭐⭐⭐ (中高等，基于已完成的 ECS 基础)

**适合**: 有 C++ 和游戏开发经验的开发者

**祝开发顺利！** 🎮✨
