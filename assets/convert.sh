#!/bin/bash

SRC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEST_DIR="$SRC_DIR/../src/assets"

# mkdir -p "$DEST_DIR"

for file in "$SRC_DIR"/*; do
    [ "$file" = "$SRC_DIR/convert.sh" ] && continue
    [ -f "$file" ] || continue

    base_name=$(basename "$file")
    ext="${base_name##*.}"
    if [ "$ext" = "ttf" ]; then
        out_name="font.ttf.h"
        c_name="font_ttf"
    else
        out_name="${base_name}.h"
        c_name="${base_name}"
    fi

    if xxd -n $c_name -i "$file" > "$DEST_DIR/$out_name"; then
        echo "converted $file"
    fi
done