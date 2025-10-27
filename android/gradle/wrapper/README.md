# Gradle Wrapper 说明

## 文件结构

```
gradle/
└── wrapper/
    ├── gradle-wrapper.jar      # 需要下载或在首次运行时生成
    └── gradle-wrapper.properties  # 已包含
```

## ⚠️ 缺失的 gradle-wrapper.jar

`gradle-wrapper.jar` 是一个二进制文件，无法直接在 Git 中管理。它会通过以下方式获得：

### 方法 1: CI/CD 自动处理（推荐）✅

在 GitHub Actions 中，我们使用 `gradle/gradle-build-action@v2` 自动设置 Gradle：

```yaml
- name: Setup Gradle
  uses: gradle/gradle-build-action@v2
  with:
    gradle-version: "8.4"
```

这个 action 会：

1. 自动下载 Gradle 8.4
2. 生成 `gradle-wrapper.jar`
3. 配置所有必要的文件

### 方法 2: 本地初始化（首次使用）

如果在本地没有 `gradle-wrapper.jar`，运行一次：

```bash
# macOS / Linux
gradle wrapper --gradle-version 8.4

# Windows
gradle.bat wrapper --gradle-version 8.4
```

这会生成必要的文件。

### 方法 3: 使用现有项目的文件

从其他 Android 项目复制 `gradle/wrapper/gradle-wrapper.jar`。

## 工作流优化

由于 JAR 文件很大（~8-10 MB），我们选择：

1. **在 CI/CD 中生成**：GitHub Actions 会自动处理，第一次可能较慢
2. **缓存优化**：Gradle action 会自动缓存下载的 Gradle 版本

## 提交到 Git

你有两个选择：

### 选项 A: 忽略 JAR（推荐）

```bash
# .gitignore 已配置
# gradle/wrapper/gradle-wrapper.jar 被忽略
```

优点：

- 减少仓库大小
- CI/CD 自动处理
- 避免 binary 文件冲突

### 选项 B: 提交 JAR（大仓库）

```bash
# 编辑 .gitignore，移除 gradle-wrapper.jar
# 然后 git add gradle/wrapper/gradle-wrapper.jar
```

优点：

- 完全离线可用
- 本地构建更快

## 首次构建

### 本地首次构建

```bash
cd android

# Linux / macOS
chmod +x gradlew
./gradlew assembleDebug

# Windows
gradlew.bat assembleDebug
```

第一次会下载 Gradle 和依赖项，可能需要 5-10 分钟。

### CI/CD 首次构建

在 GitHub Actions 中，`gradle/gradle-build-action@v2` 会自动处理一切。

---

## 总结

| 方案             | 速度 | 磁盘 | CI/CD   | 本地     |
| ---------------- | ---- | ---- | ------- | -------- |
| 忽略 JAR（当前） | 中   | 小   | ✅ 自动 | ⚠ 首次慢 |
| 提交 JAR         | 快   | 大   | ✅ 最快 | ✅ 最快  |

**建议**：使用当前方案（忽略 JAR），CI/CD 自动处理，本地可按需运行。

---

**⚡ 不用手动干预，工作流会自动处理！**
