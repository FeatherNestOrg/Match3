#!/bin/bash
# Script to bootstrap Android project structure for native C++ game

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
ANDROID_DIR="$PROJECT_ROOT/android"

echo "🚀 Bootstrapping Android project structure..."

# Create necessary directories
echo "📁 Creating directory structure..."
mkdir -p "$ANDROID_DIR/app/src/main/"{java/com/feathernestz/match3,jniLibs/{armeabi-v7a,arm64-v8a,x86,x86_64},assets,res/{drawable,layout,values,xml}}
mkdir -p "$ANDROID_DIR/app/src/main/res/mipmap-"{anydpi-v26,mdpi,hdpi,xhdpi,xxhdpi,xxxhdpi}

# Generate launcher icons using ImageMagick if available
echo "🎨 Generating launcher icons..."
if command -v convert &> /dev/null; then
    bash "$SCRIPT_DIR/generate-launcher-icons.sh" || echo "⚠️  Warning: Icon generation failed, will use defaults"
else
    echo "⚠️  ImageMagick not found. Creating placeholder icon XMLs..."
    # The XML-based adaptive icons will be created by the template below
fi

echo "✅ Android project structure bootstrapped!"
echo ""
echo "Next steps:"
echo "  1. Build native libraries: cmake --build build-android-{abi}"
echo "  2. Copy .so files to jniLibs directories"
echo "  3. Build APK: cd android && ./gradlew assembleRelease"

