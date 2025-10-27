#!/bin/bash

# Android 构建脚本
# 用法: ./scripts/build-android.sh [abi] [build_type]
# 示例: ./scripts/build-android.sh arm64-v8a Release

set -e

# 配置
ABI=${1:-arm64-v8a}
BUILD_TYPE=${2:-Release}
API_LEVEL=${3:-24}
LOG_LEVEL=${4:-INFO}

echo "============================================"
echo "Match-3 Android Build Script"
echo "============================================"
echo "ABI:        $ABI"
echo "Type:       $BUILD_TYPE"
echo "API Level:  $API_LEVEL"
echo "Log Level:  $LOG_LEVEL"
echo "============================================"

# 检查 NDK
if [ -z "$ANDROID_NDK_HOME" ]; then
    echo "❌ Error: ANDROID_NDK_HOME not set"
    echo "Please set it to your Android NDK path:"
    echo "  export ANDROID_NDK_HOME=/path/to/ndk"
    exit 1
fi

if [ ! -d "$ANDROID_NDK_HOME" ]; then
    echo "❌ Error: ANDROID_NDK_HOME directory does not exist: $ANDROID_NDK_HOME"
    exit 1
fi

echo "✓ Using NDK: $ANDROID_NDK_HOME"

# 构建目录
BUILD_DIR="build-android-${ABI}-${BUILD_TYPE}"
echo "✓ Build directory: $BUILD_DIR"

# 清理旧的构建（可选）
if [ "$5" == "clean" ]; then
    echo "🧹 Cleaning old build..."
    rm -rf "$BUILD_DIR"
fi

# 配置 CMake
echo "⚙️  Configuring CMake..."
cmake -B "$BUILD_DIR" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI="$ABI" \
    -DANDROID_PLATFORM="android-$API_LEVEL" \
    -DANDROID_STL=c++_shared \
    -DENABLE_CONSOLE_LOG=ON \
    -DLOG_LEVEL="$LOG_LEVEL"

# 构建
echo "🔨 Building..."
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" -j$(nproc)

echo ""
echo "✅ Build successful!"
echo ""
echo "Output:"
if [ -f "$BUILD_DIR/match-3" ]; then
    echo "  Binary: $BUILD_DIR/match-3"
elif [ -f "$BUILD_DIR/libmatch-3.so" ]; then
    echo "  Library: $BUILD_DIR/libmatch-3.so"
else
    echo "  Check $BUILD_DIR for output files"
fi

echo ""
echo "To clean and rebuild, run:"
echo "  $0 $ABI $BUILD_TYPE $API_LEVEL $LOG_LEVEL clean"
