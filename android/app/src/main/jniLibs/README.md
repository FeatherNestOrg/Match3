# JNI Libraries Directory

这个目录用于存放预编译的原生库文件（.so 文件）。

## 目录结构

- `armeabi-v7a/` - ARM 32位架构
- `arm64-v8a/` - ARM 64位架构
- `x86/` - x86 32位架构（模拟器）
- `x86_64/` - x86 64位架构（模拟器）

## CI 构建流程

在 CI 构建时，预编译的 `libMatch3.so` 文件会被自动复制到对应的架构目录中。

例如：

```
jniLibs/
├── armeabi-v7a/
│   └── libMatch3.so
├── arm64-v8a/
│   └── libMatch3.so
├── x86/
│   └── libMatch3.so
└── x86_64/
    └── libMatch3.so
```

## 本地开发

如果需要本地测试 APK 构建，需要先编译原生库：

```bash
# 编译 ARM64
cmake -B build-android-arm64-v8a \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-24

cmake --build build-android-arm64-v8a

# 复制 .so 文件
cp build-android-arm64-v8a/src/libMatch3.so android/app/src/main/jniLibs/arm64-v8a/

# 然后构建 APK
cd android
./gradlew assembleRelease
```

