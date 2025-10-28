#!/bin/bash
# Script to generate Android launcher icons using ImageMagick

set -e

RES_DIR="android/app/src/main/res"
BG_COLOR="#3DDC84"
TEXT_COLOR="white"

# Create mipmap directories
for density in mdpi hdpi xhdpi xxhdpi xxxhdpi; do
    mkdir -p "${RES_DIR}/mipmap-${density}"
done

# Generate icons for each density
declare -A sizes=( ["mdpi"]=48 ["hdpi"]=72 ["xhdpi"]=96 ["xxhdpi"]=144 ["xxxhdpi"]=192 )

for density in "${!sizes[@]}"; do
    size=${sizes[$density]}
    dir="${RES_DIR}/mipmap-${density}"

    # Create launcher icon
    convert -size ${size}x${size} xc:${BG_COLOR} \
        -fill ${TEXT_COLOR} \
        -gravity center \
        -pointsize $((size/2)) \
        -font DejaVu-Sans-Bold \
        -annotate +0+0 "M3" \
        "${dir}/ic_launcher.png"

    # Create round launcher icon (same for now)
    cp "${dir}/ic_launcher.png" "${dir}/ic_launcher_round.png"

    echo "Created ${density} icons (${size}x${size})"
done

echo "All launcher icons created successfully!"

