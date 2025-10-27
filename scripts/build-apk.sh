#!/bin/bash

# Match3 Android APK Build Script
# 使用方式: ./scripts/build-apk.sh [apk_type]
# 示例: ./scripts/build-apk.sh release

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
ANDROID_DIR="$PROJECT_ROOT/android"

# 配置
APK_TYPE="${1:-release}"
BUILD_TYPE="Release"
LOG_LEVEL="INFO"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Match3 Android APK Builder ===${NC}"
echo "Project root: $PROJECT_ROOT"
echo "Android project: $ANDROID_DIR"
echo "APK type: $APK_TYPE"
echo ""

# 检查依赖
check_requirements() {
    echo -e "${YELLOW}Checking requirements...${NC}"
    
    # 检查 Java
    if ! command -v java &> /dev/null; then
        echo -e "${RED}Error: Java not found. Please install JDK 11 or later.${NC}"
        exit 1
    fi
    
    java_version=$(java -version 2>&1 | head -n 1)
    echo "✓ Java: $java_version"
    
    # 检查 Gradle
    if [ ! -x "$ANDROID_DIR/gradlew" ]; then
        echo -e "${YELLOW}Setting up Gradle wrapper...${NC}"
        chmod +x "$ANDROID_DIR/gradlew"
    fi
    
    echo "✓ Gradle wrapper ready"
    echo ""
}

# 编译原生库
build_native_libs() {
    echo -e "${YELLOW}Building native libraries...${NC}"
    
    local abis=("arm64-v8a" "armeabi-v7a" "x86" "x86_64")
    
    for abi in "${abis[@]}"; do
        echo ""
        echo "Building for $abi..."
        
        local build_dir="$PROJECT_ROOT/build-android-$abi"
        
        if [ -d "$build_dir" ]; then
            echo "✓ Build directory found: $build_dir"
            
            # 检查是否已有 .so 文件
            if [ -f "$build_dir/src/libMatch3.so" ]; then
                echo "✓ libMatch3.so already built"
            else
                echo "Building..."
                cmake --build "$build_dir" --config $BUILD_TYPE -j$(nproc)
            fi
        else
            echo "Configure CMake for $abi..."
            cmake -B "$build_dir" \
                -G Ninja \
                -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
                -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
                -DANDROID_ABI=$abi \
                -DANDROID_PLATFORM=android-24 \
                -DANDROID_STL=c++_shared \
                -DENABLE_CONSOLE_LOG=OFF \
                -DLOG_LEVEL=$LOG_LEVEL
            
            echo "Building..."
            cmake --build "$build_dir" --config $BUILD_TYPE -j$(nproc)
        fi
        
        # 复制 .so 到 jniLibs
        local lib_dir="$ANDROID_DIR/app/src/main/jniLibs/$abi"
        mkdir -p "$lib_dir"
        
        if [ -f "$build_dir/src/libMatch3.so" ]; then
            cp "$build_dir/src/libMatch3.so" "$lib_dir/"
            echo "✓ Copied libMatch3.so to $lib_dir"
        else
            echo -e "${RED}Error: libMatch3.so not found in $build_dir${NC}"
            exit 1
        fi
    done
    
    echo ""
    echo "✓ All native libraries built successfully"
}

# 复制游戏资源
copy_assets() {
    echo -e "${YELLOW}Copying game assets...${NC}"
    
    local assets_dir="$ANDROID_DIR/app/src/main/assets"
    mkdir -p "$assets_dir"
    
    if [ -d "$PROJECT_ROOT/resources" ]; then
        cp -r "$PROJECT_ROOT/resources"/* "$assets_dir/" 2>/dev/null || true
        echo "✓ Assets copied"
    else
        echo "⚠ No resources directory found"
    fi
    
    echo ""
}

# 构建 APK
build_apk() {
    echo -e "${YELLOW}Building APK...${NC}"
    
    cd "$ANDROID_DIR"
    
    case "$APK_TYPE" in
        debug)
            echo "Building debug APK..."
            ./gradlew assembleDebug
            local apk_output="app/build/outputs/apk/debug/app-debug.apk"
            ;;
        release)
            echo "Building release APK..."
            ./gradlew assembleRelease
            local apk_output="app/build/outputs/apk/release/app-release-unsigned.apk"
            ;;
        *)
            echo -e "${RED}Unknown APK type: $APK_TYPE${NC}"
            echo "Usage: $0 [debug|release]"
            exit 1
            ;;
    esac
    
    if [ -f "$apk_output" ]; then
        local apk_size=$(du -h "$apk_output" | cut -f1)
        echo -e "${GREEN}✓ APK built successfully${NC}"
        echo "Output: $apk_output"
        echo "Size: $apk_size"
        return 0
    else
        echo -e "${RED}Error: APK not found at $apk_output${NC}"
        return 1
    fi
}

# 主流程
main() {
    check_requirements
    
    # 仅在需要时编译原生库
    if [ -z "$SKIP_NATIVE" ]; then
        build_native_libs
    fi
    
    copy_assets
    build_apk
    
    echo ""
    echo -e "${GREEN}=== Build completed successfully! ===${NC}"
}

main "$@"
