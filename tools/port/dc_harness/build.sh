#!/bin/sh
# Build the headless Dreamcast emulator used by dcrun.py / boot_test.py:
#   Flycast (libretro core, patched: serial console to a file, SH4 register dump,
#   PC-sampling profiler) + libharness.so (minimal libretro frontend, EGL surfaceless
#   + Mesa llvmpipe, GLES 3).
# Everything goes to $DC_HARNESS_DIR (default ~/vd-emu), outside the project.
# Needs: git (GitHub only), cmake, gcc/g++, Mesa EGL/GLES runtime libs. Takes ~25 min on 2 CPUs.
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
EMU=${DC_HARNESS_DIR:-$HOME/vd-emu}
mkdir -p "$EMU" && cd "$EMU"

# headers not installed on this machine (Khronos registries from GitHub)
[ -d EGL-Registry ] || git clone --depth 1 https://github.com/KhronosGroup/EGL-Registry
[ -d OpenGL-Registry ] || git clone --depth 1 https://github.com/KhronosGroup/OpenGL-Registry
mkdir -p libs
ln -sf /usr/lib/x86_64-linux-gnu/libGLESv2.so.2 libs/libGLESv2.so
ln -sf /usr/lib/x86_64-linux-gnu/libEGL.so.1 libs/libEGL.so

if [ ! -d flycast ]; then
    git clone https://github.com/flyinghead/flycast
    cd flycast
    git checkout "$(cat "$HERE/FLYCAST_COMMIT")"
    # freetype's official URL is on gitlab (not reachable): use the GitHub mirror
    git config submodule.core/deps/freetype.url https://github.com/freetype/freetype.git
    git submodule update --init --recursive
    git apply "$HERE/flycast_harness.patch"
    cd ..
fi
mkdir -p flycast/build-lr && cd flycast/build-lr
INC="-I$EMU/OpenGL-Registry/api -I$EMU/EGL-Registry/api"
cmake .. -DLIBRETRO=ON -DCMAKE_BUILD_TYPE=Release -DUSE_GLES=ON -DUSE_VULKAN=OFF -DUSE_LUA=OFF \
    -DUSE_BREAKPAD=OFF -DENABLE_LOG=ON -DCMAKE_C_FLAGS="$INC" -DCMAKE_CXX_FLAGS="$INC" \
    -DCMAKE_SHARED_LINKER_FLAGS="-L$EMU/libs"
make -j2
cd "$EMU"
mkdir -p harness
gcc -O2 -fPIC -shared -Wall -o harness/libharness.so "$HERE/harness.c" $INC \
    -I"$EMU/flycast/core/deps/libretro-common/include" /usr/lib/x86_64-linux-gnu/libEGL.so.1 -ldl
echo "OK: $EMU/flycast/build-lr/flycast_libretro.so $EMU/harness/libharness.so"
