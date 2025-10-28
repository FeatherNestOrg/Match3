# Android APK 构建说明

本目录包含用 Android Studio 创建的标准 Android 项目，用于将 Match3 游戏打包为 APK。

## 项目配置

- **Package Name**: `com.feathernest.match3`
- **Min SDK**: 24 (Android 7.0)
- **Target SDK**: 34
- **Build Tools**: Gradle 8.13, AGP 8.13.0

## 目录结构

```
android/
├── app/
│   ├── src/
│   │   └── main/
│   │       ├── java/           # Kotlin/Java 代码
│   │       ├── res/            # Android 资源文件
│   │       ├── jniLibs/        # 预编译的原生库 (.so 文件)
│   │       │   ├── armeabi-v7a/
│   │       │   ├── arm64-v8a/
│   │       │   ├── x86/
│   │       │   └── x86_64/
│   │       └── AndroidManifest.xml
│   └── build.gradle.kts        # App 模块配置
├── gradle/
│   └── wrapper/
│       └── gradle-wrapper.jar  # Gradle wrapper (已提交)
├── build.gradle.kts            # 项目级配置
├── settings.gradle.kts
└── gradlew                     # Gradle wrapper 脚本

```

## CI 构建流程

### 1. 预编译原生库

CI 首先为各个架构编译原生 C++ 代码：

```bash
# 由 build-android job 完成
cmake -B build-android-arm64-v8a \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-24
  
cmake --build build-android-arm64-v8a
```

### 2. 复制 .so 文件到 jniLibs

CI 将编译好的 `libMatch3.so` 文件复制到对应的架构目录：

```bash
cp build-android-*/src/libMatch3.so android/app/src/main/jniLibs/{abi}/
```

### 3. 构建 APK

```bash
cd android
./gradlew assembleRelease
```

生成的 APK 位于：`app/build/outputs/apk/release/app-release.apk`

## 本地开发

### 前置条件

- Android SDK (API 34)
- JDK 17
- Android NDK r26c (用于编译原生代码)

### 快速测试构建

如果你已经有编译好的 .so 文件：

```bash
# 1. 复制 .so 文件到 jniLibs
cp /path/to/libMatch3.so android/app/src/main/jniLibs/arm64-v8a/

# 2. 构建 APK
cd android
./gradlew assembleDebug

# 3. 安装到设备
./gradlew installDebug
```

### 完整构建流程

```bash
# 1. 编译各架构的原生库
for abi in armeabi-v7a arm64-v8a x86 x86_64; do
  cmake -B build-android-$abi \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=$abi \
    -DANDROID_PLATFORM=android-24
  cmake --build build-android-$abi
  cp build-android-$abi/src/libMatch3.so android/app/src/main/jniLibs/$abi/
done

# 2. 构建并安装 APK
cd android
./gradlew installRelease
```

## 常用命令

```bash
# 查看可用任务
./gradlew tasks

# 构建 Debug APK
./gradlew assembleDebug

# 构建 Release APK
./gradlew assembleRelease

# 清理构建
./gradlew clean

# 安装到连接的设备
./gradlew installDebug

# 查看依赖
./gradlew app:dependencies
```

## 配置说明

### 不使用内置 CMake

本项目配置为**不使用** Android Gradle Plugin 的内置 CMake 构建。原因：

- Match3 项目有自己的 CMake 构建系统
- 需要复杂的依赖管理（SDL3, EnTT 等）
- 跨平台构建需要统一的构建流程

因此，我们采用**预编译模式**：

1. 在外部编译 libMatch3.so
2. 将 .so 文件放入 jniLibs
3. Gradle 只负责打包 APK

### 签名配置

当前使用 Debug 签名配置。生产环境需要配置正式的签名：

```kotlin
// app/build.gradle.kts
android {
    signingConfigs {
        create("release") {
            storeFile = file("path/to/keystore.jks")
            storePassword = "***"
            keyAlias = "***"
            keyPassword = "***"
        }
    }
    buildTypes {
        release {
            signingConfig = signingConfigs.getByName("release")
        }
    }
}
```

## 故障排查

### Gradle 同步失败

```bash
# 清理并重新同步
./gradlew clean
./gradlew --refresh-dependencies
```

### 找不到 .so 文件

检查 jniLibs 目录结构：

```bash
find app/src/main/jniLibs -name "*.so"
```

### ABI 不匹配

确保 .so 文件在正确的架构目录下：

- ARM 手机 → `arm64-v8a/` 或 `armeabi-v7a/`
- x86 模拟器 → `x86_64/` 或 `x86/`

## 更多信息

- [Android 开发文档](https://developer.android.com/)
- [Gradle 用户指南](https://docs.gradle.org/)
- [Android NDK 指南](https://developer.android.com/ndk/guides)

