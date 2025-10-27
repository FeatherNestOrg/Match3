#!/bin/bash

# 构建所有 Android ABI 版本
# 用法: ./scripts/build-all-android.sh [build_type]

set -e

BUILD_TYPE=${1:-Release}
LOG_LEVEL=${2:-INFO}

echo "============================================"
echo "Building Match-3 for all Android ABIs"
echo "============================================"
echo "Build Type: $BUILD_TYPE"
echo "Log Level:  $LOG_LEVEL"
echo "============================================"
echo ""

ABIS=("armeabi-v7a" "arm64-v8a" "x86" "x86_64")

for ABI in "${ABIS[@]}"; do
    echo ""
    echo "🚀 Building for $ABI..."
    echo ""
    ./scripts/build-android.sh "$ABI" "$BUILD_TYPE" 24 "$LOG_LEVEL"
    echo ""
    echo "✅ $ABI build complete"
    echo "----------------------------------------"
done

echo ""
echo "🎉 All Android builds complete!"
echo ""
echo "Build outputs:"
for ABI in "${ABIS[@]}"; do
    BUILD_DIR="build-android-${ABI}-${BUILD_TYPE}"
    if [ -d "$BUILD_DIR" ]; then
        echo "  - $BUILD_DIR/"
    fi
done
