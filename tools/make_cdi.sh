#!/bin/sh
# Vivid Dolls - make a Dreamcast disc image (.cdi) from the built program and
# YOUR OWN ROM dump.
#
#   tools/make_cdi.sh ROMFILE [OUTPUT.cdi]
#
# ROMFILE: the Vivid Dolls ROM in any byte order (nus-zsaj-0.u3 chip dump,
# baserom.z64, ...). The script checks it, converts it to .z64 order, keeps the
# used first 0x23B000 bytes (the rest of the 8 MB is 0xFF padding) and puts it
# on the disc as VIVIDOLL.ROM next to build/dc/1ST_READ.BIN
# (build that first: make -f platform/dc/Makefile).
# OUTPUT defaults to build/dc/vividdolls.cdi. Set VD_CDI_PAD=1 for a padded data track
# (for burning to CD-R).
#
# Disc tool: mkdcdisc (KallistiOS toolchain, /opt/toolchains/dc/bin or PATH).
# Fallback: mkisofs/genisoimage + IP.BIN + cdi4dc, if those are installed.
# The same steps (check, convert, cut, add 1ST_READ.BIN) are what
# buildmycdi.cloud does in the browser.
set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)
ROM=$1
OUT=${2:-$ROOT/build/dc/vividdolls.cdi}
FIRST_READ=${VD_FIRST_READ:-$ROOT/build/dc/1ST_READ.BIN}   # VD_FIRST_READ: e.g. a debug build
KEEP=2338816          # 0x23B000
SHA_KEEP=eead45676f0104545eaba3abccd4e1bdd59db322
SHA_FULL=747e9420cce23ff82b5922cd91ec43272d03fa4f

if [ -z "$ROM" ] || [ ! -f "$ROM" ]; then
    echo "usage: $0 ROMFILE [OUTPUT.cdi]" >&2
    echo "  ROMFILE = your Vivid Dolls ROM (nus-zsaj-0.u3 or baserom.z64)" >&2
    exit 1
fi
if [ ! -f "$FIRST_READ" ]; then
    echo "error: $FIRST_READ not found - build it first: make -f platform/dc/Makefile" >&2
    exit 1
fi

WORK=$(mktemp -d "${TMPDIR:-/tmp}/vdcdi.XXXXXX")
trap 'rm -rf "$WORK"' EXIT INT TERM
mkdir -p "$WORK/disc"

# ---- check + convert the ROM (python3 -I: no files from the current directory are imported) ----
python3 -I - "$ROM" "$WORK/disc/VIVIDOLL.ROM" "$KEEP" "$SHA_KEEP" "$SHA_FULL" <<'EOF'
import hashlib, sys
src, dst, keep, sha_keep, sha_full = sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4], sys.argv[5]
d = bytearray(open(src, 'rb').read())
if len(d) < keep or len(d) % 4:
    sys.exit('error: %s is not a Vivid Dolls ROM (size %d)' % (src, len(d)))
head = bytes(d[:4])
if head == b'\x80\x37\x12\x40':
    kind = 'z64'
elif head == b'\x37\x80\x40\x12':
    kind = 'v64 (byte-swapped chip dump)'
    d[0::2], d[1::2] = d[1::2], d[0::2]
elif head == b'\x40\x12\x37\x80':
    kind = 'n64 (word-swapped)'
    d[0::4], d[1::4], d[2::4], d[3::4] = d[3::4], d[2::4], d[1::4], d[0::4]
else:
    sys.exit('error: %s is not an N64/Aleck64 ROM (unknown header %s)' % (src, head.hex()))
if len(d) == 8 * 1024 * 1024 and hashlib.sha1(d).hexdigest() != sha_full:
    sys.exit('error: wrong ROM (SHA-1 %s, expected %s)' % (hashlib.sha1(d).hexdigest(), sha_full))
h = hashlib.sha1(d[:keep]).hexdigest()
if h != sha_keep:
    sys.exit('error: wrong ROM (SHA-1 of the used part %s, expected %s)' % (h, sha_keep))
open(dst, 'wb').write(d[:keep])
print('ROM OK (%s), %d bytes on the disc as VIVIDOLL.ROM' % (kind, keep))
EOF

mkdir -p "$(dirname "$OUT")"

# ---- build the disc ----
MKDCDISC=$(command -v mkdcdisc 2>/dev/null || true)
[ -z "$MKDCDISC" ] && [ -x /opt/toolchains/dc/bin/mkdcdisc ] && MKDCDISC=/opt/toolchains/dc/bin/mkdcdisc
if [ -n "$MKDCDISC" ]; then
    # no data-track padding (small file for Flycast / GDEMU); VD_CDI_PAD=1 pads it
    # (files at the outer edge of a burnt CD-R read faster on a real drive)
    PAD=-N
    [ "${VD_CDI_PAD:-0}" = 1 ] && PAD=
    "$MKDCDISC" -B "$FIRST_READ" -f "$WORK/disc/VIVIDOLL.ROM" $PAD \
        -n "VIVID DOLLS" -a "VIVID DOLLS PORT" -o "$OUT" -v 1
else
    MKISOFS=$(command -v mkisofs 2>/dev/null || command -v genisoimage 2>/dev/null || true)
    CDI4DC=$(command -v cdi4dc 2>/dev/null || true)
    MAKEIP=$(command -v makeip 2>/dev/null || true)
    if [ -z "$MKISOFS" ] || [ -z "$CDI4DC" ] || [ -z "$MAKEIP" ]; then
        echo "error: no disc tool found (need mkdcdisc, or mkisofs + makeip + cdi4dc)" >&2
        exit 1
    fi
    cp "$FIRST_READ" "$WORK/disc/1ST_READ.BIN"
    (cd "$WORK" && "$MAKEIP" -g "VIVID DOLLS" IP.BIN >/dev/null)
    "$MKISOFS" -C 0,11702 -V VIVIDDOLLS -G "$WORK/IP.BIN" -r -J -l -o "$WORK/data.iso" "$WORK/disc"
    "$CDI4DC" "$WORK/data.iso" "$OUT"
fi
echo "Disc image: $OUT"
