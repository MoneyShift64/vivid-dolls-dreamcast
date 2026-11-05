#!/bin/sh
# Build Vivid Dolls for the Dreamcast and make vividdolls.cdi from your own ROM.
#
#   ./build.sh            looks for nus-zsaj-0.u3 (or baserom.z64) in this folder
#   ./build.sh ROMFILE    uses the ROM you name
#   CHEAT=1 ./build.sh    testing build: holding L+R ends a round as a 99% clear
#
# Needs KallistiOS (source /opt/toolchains/dc/kos/environ.sh first) with SH4ZAM,
# and mkdcdisc on PATH.
set -e
cd "$(dirname "$0")"
ROM=$1
if [ -z "$ROM" ]; then
    for f in nus-zsaj-0.u3 baserom.z64; do
        [ -f "$f" ] && ROM=$f && break
    done
fi
if [ -z "$ROM" ] || [ ! -f "$ROM" ]; then
    echo "error: put your Vivid Dolls ROM (nus-zsaj-0.u3) in this folder, or run ./build.sh ROMFILE" >&2
    exit 1
fi
if [ -z "$KOS_BASE" ]; then
    echo "error: load KallistiOS first: source /opt/toolchains/dc/kos/environ.sh" >&2
    exit 1
fi
if [ "${CHEAT:-0}" != 0 ]; then
    make -f port/Makefile.dc CHEAT=1
    VD_FIRST_READ=build/dc-cheat/1ST_READ.BIN tools/port/make_cdi.sh "$ROM" vividdolls-cheat.cdi
    echo "Made vividdolls-cheat.cdi"
else
    make -f port/Makefile.dc
    tools/port/make_cdi.sh "$ROM" vividdolls.cdi
    echo "Made vividdolls.cdi"
fi
