#!/bin/bash

# Check if the user provided a module name
if [ -z "$1" ]; then
    echo "Usage: ./create_module.sh <module_name>"
    exit 1
fi

MODULE_NAME=$(echo "$1" | tr '[:upper:]' '[:lower:]')
UPPER_MODULE=$(echo "$MODULE_NAME" | tr '[:lower:]' '[:upper:]')

# Define paths matching your project directory layout
MODULE_DIR="src/c/$MODULE_NAME"
C_FILE="$MODULE_DIR/$MODULE_NAME.c"
H_FILE="$MODULE_DIR/$MODULE_NAME.h"

# 1. Create the directory structure safely
mkdir -p "$MODULE_DIR"
mkdir -p build

# 2. Generate the boilerplate header file
cat << EOF > "$H_FILE"
#ifndef ${UPPER_MODULE}_H
#define ${UPPER_MODULE}_H

#endif
EOF

# 3. Generate the boilerplate source file
cat << EOF > "$C_FILE"
#include "${MODULE_NAME}.h"
EOF

echo "✓ Created directory: $MODULE_DIR"
echo "✓ Created header:    $H_FILE"
echo "✓ Created source:    $C_FILE"

# 4. Update build.sh right before the linker command runs (macOS Safe)
if [ -f "build.sh" ]; then
    COMPILE_CMD="i686-elf-gcc -ffreestanding -c $C_FILE -o build/${MODULE_NAME}.o"
    
    # Check if the command already exists to prevent duplication
    if ! grep -q "$COMPILE_CMD" build.sh; then
        # Use awk to inject the compilation step right before the linker command execution
        awk -v cmd="$COMPILE_CMD" '
            /i686-elf-ld/ { print cmd; print $0; next }
            { print }
        ' build.sh > build.sh.tmp && mv build.sh.tmp build.sh
        chmod +x build.sh
        echo "✓ Added compilation step to build.sh"
    else
        echo "ℹ Compilation step already exists in build.sh"
    fi
else
    echo "⚠️ build.sh not found. Skipping file modification."
fi

# 5. Update linker.ld inside the INPUT(...) block (macOS Safe)
if [ -f "linker.ld" ]; then
    OBJ_FILE="build/${MODULE_NAME}.o"
    
    # Check if the object file is already specified in the linker file
    if ! grep -q "$OBJ_FILE" linker.ld; then
        # Use awk to inject the object path right after the opening INPUT( block
        awk -v obj="$OBJ_FILE" '
            /INPUT\(/ { print $0; print "    " obj; next }
            { print }
        ' linker.ld > linker.ld.tmp && mv linker.ld.tmp linker.ld
        echo "✓ Added $OBJ_FILE to INPUT() in linker.ld"
    else
        echo "ℹ $OBJ_FILE already exists in INPUT() inside linker.ld"
    fi
else
    echo "⚠️ linker.ld not found. Skipping file modification."
fi

echo "Module '$MODULE_NAME' generated and integrated successfully!"
