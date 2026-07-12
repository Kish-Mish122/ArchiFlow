#!/bin/bash
# build_linux_cross.sh - Cross-compilation script for Windows target

echo "========================================"
echo "  ArchiFlow Cross-Compilation Script"
echo "========================================"
echo ""

# Configuration
TARGET=i686-w64-mingw32
BUILD_DIR=build_cross
OUTPUT_DIR=bin/win32
LIBARCHIVE_DIR=libarchive

# Check for cross-compiler
if ! command -v $TARGET-g++ &> /dev/null; then
    echo "Error: Cross-compiler $TARGET-g++ not found."
    echo "Please install mingw-w64:"
    echo "  sudo apt-get install mingw-w64"
    exit 1
fi

echo "Cross-compiler found: $TARGET-g++"

# Create directories
mkdir -p $BUILD_DIR
mkdir -p $OUTPUT_DIR

echo "Building libarchive..."
cd $LIBARCHIVE_DIR
./configure --host=$TARGET --prefix=../$BUILD_DIR/libarchive
make -j4
make install
cd ..

echo "Compiling ArchiFlow..."
$TARGET-g++ -std=c++11 -O2 -static \
    -I$BUILD_DIR/libarchive/include \
    -L$BUILD_DIR/libarchive/lib \
    -D_WIN32_WINNT=0x0601 \
    -DUNICODE -D_UNICODE \
    -o $OUTPUT_DIR/ArchiFlow.exe \
    ArchiFlow.cpp \
    RegistryManager.cpp \
    ShellExtension.cpp \
    ConfigManager.cpp \
    ThemeManager.cpp \
    DragDropHandler.cpp \
    FileUtils.cpp \
    ArchiFlowInstaller.cpp \
    -larchive -lz -lbz2 -llzma -lcrypto \
    -lcomctl32 -lcomdlg32 -lshell32 -luxtheme \
    -mwindows -s

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================"
    echo "  Build successful!"
    echo "  Output: $OUTPUT_DIR/ArchiFlow.exe"
    echo "========================================"
else
    echo ""
    echo "========================================"
    echo "  Build failed!"
    echo "========================================"
    exit 1
fi