# Witchaven II port: fresh DOS sources + JonoF's modules + the port's transformations, then a build
S=/mnt/c/Users/Jess/AppData/Local/Temp/claude/d3c88340bd34ded3/877f9e48-85c9-4030-aad6-fabad4d7e72d/scratchpad
export PATH=~/ionfury/bin:$PATH
T=~/ionfury/tier2/jfwhaven2; W2="/mnt/c/Users/Jess/Desktop/EDUKE32/Witchaven2/WHAVEN2/SRC"; JF=~/ionfury/tier2/jfwhaven/src
rm -rf $T && cp -r ~/ionfury/tier2/jfwhaven $T && rm -rf $T/build
for f in WHAVEN.C WHANI.C WHINP.C WHMENU.C WHOBJ.C WHPLR.C WHFX.C WHTAG.C READCFG.C PROFILE.C; do
  lc=$(echo $f | tr A-Z a-z); tr -d '\r' < "$W2/$f" | expand -t 4 > $T/src/$lc
done
for f in NAMES.H WHDEFS.H KEYDEFS.H JEFF.H LES.H RAF.H SNDNAMES.H WH2CNTRL.H PROFILE.H; do
  lc=$(echo $f | tr A-Z a-z); tr -d '\r' < "$W2/$f" | expand -t 4 > $T/src/$lc
done
for f in whsndmod.c whsmk.c whnet.c datascan.c datascan.h b5compat.c version.c version.h mister_port.c mister_port.h; do cp $JF/$f $T/src/$f; done
rm -f $T/src/whmsc.c $T/src/extern.h $T/src/3d.h $T/src/config.c $T/src/icorp.h
cp "$S/tier2/wh2/sos.h" "$S/tier2/wh2/sosm.h" $T/src/
# the sound header: JonoF's structures with the sequel's sound numbers
python3 - "$JF/sndmod.h" "$W2/SNDMOD.H" "$T/src/sndmod.h" <<'PY'
import sys, re
jf = open(sys.argv[1], encoding='latin1').read()
w2 = open(sys.argv[2], 'rb').read().decode('latin1').replace('\r\n', '\n')
w2s = [l for l in w2.split('\n') if re.match(r'#define\s+S_\w+', l)]
jf = re.sub(r'(?m)^#define\s+S_\w+.*\n', '', jf)
jf = jf.replace('#define  MAX_SND_LOOPS', '\n'.join(w2s) + '\n\n#define  MAX_SND_LOOPS', 1)
open(sys.argv[3], 'w', encoding='latin1', newline='\n').write(jf); print('sndmod.h:', len(w2s), 'sequel sound names')
PY
sed -i 's/kopen4load("songs", 0)/kopen4load("w_songs", 0)/' $T/src/whsndmod.c
# the DOS headers the sequel's modules include: dropped
for f in whaven.c whani.c whinp.c whmenu.c whobj.c whplr.c whfx.c whtag.c readcfg.c profile.c; do
  sed -i -E '/^#include[ \t]*<(memcheck|dos|conio|graph|malloc|windows|io|process|i86|bios|direct)\.h>/d; /^#include[ \t]*"(swift|avlib|vfx1|vesa|svga|smack|serial|rtq|netnow|mplib|lpc|jstick|hmistd|dpmi|commlib|3d|extern|pragmas)\.h"/d' $T/src/$f
done
sed -i 's|#include <sys\types.h>|#include <sys/types.h>|; s|#include <sys\stat.h>|#include <sys/stat.h>|' $T/src/*.c
cp $JF/jeff.h $T/src/jeff.h
for f in whaven.c whani.c whinp.c whmenu.c whobj.c whplr.c whfx.c whtag.c names.h whdefs.h raf.h les.h wh2cntrl.h; do sed -i -E 's/\blong\b/int/g' $T/src/$f; done
python3 "$S/tier2/wh2/make_icorp.py" $T $W2 || exit 1
python3 "$S/tier2/wh2/port_wh2.py" $T || exit 1
python3 "$S/tier2/wh2/port_wh2b.py" $T || exit 1
python3 "$S/tier2/patch_rumble.py" $T || exit 1
python3 "$S/tier2/patch_whrumble.py" $T || exit 1
python3 "$S/tier2/patch_cheats.py" $T jfwhaven2 || exit 1
python3 "$S/tier2/patch_nointro.py" $T jfwhaven2 || exit 1
python3 "$S/tier2/patch_wh2menu.py" $T || exit 1
sed -i '/src\/whmsc.c/d; /src\/config.c/d' $T/CMakeLists.txt
grep -q "src/readcfg.c" $T/CMakeLists.txt || sed -i 's|\tsrc/datascan.c|\tsrc/datascan.c\n\tsrc/readcfg.c\n\tsrc/profile.c\n\tsrc/wh2globals.c|' $T/CMakeLists.txt
sed -i 's/project(jfwhaven C)/project(jfwhaven2 C)/; s/add_executable(jfwhaven$/add_executable(jfwhaven2/; s/add_executable(jfwhaven /add_executable(jfwhaven2 /; s/jfwhaven PRIVATE/jfwhaven2 PRIVATE/g' $T/CMakeLists.txt
grep -n "add_executable\|project(" $T/CMakeLists.txt
R=~/ionfury/EDuke32_MiSTer; PREFIX=$R/build/sdl/mister/prefix
BT=Release; CF=""; [ -n "$WH2DEBUG" ] && { BT=Debug; CF="-O0 -fno-omit-frame-pointer -funwind-tables -g"; }
cmake -S $T -B $T/build -DCMAKE_BUILD_TYPE=$BT "-DCMAKE_C_FLAGS=$CF" -DCMAKE_TOOLCHAIN_FILE=$R/hybrid/toolchain/mister.cmake -DCMAKE_FIND_ROOT_PATH=$PREFIX -DCMAKE_PREFIX_PATH=$PREFIX -DMISTER_HYBRID=ON "-DCMAKE_EXE_LINKER_FLAGS=-no-pie -static-libgcc" > $T/cmake.log 2>&1 || { tail -5 $T/cmake.log; exit 1; }
cmake --build $T/build -j"$(nproc)" -- -k > $T/build.log 2>&1
echo "== errors per file"; grep -o "src/[a-z0-9_]*\.[ch]:[0-9]*:[0-9]*: error" $T/build.log | cut -d: -f1 | sort | uniq -c | sort -rn
echo "== error kinds"; grep "error:" $T/build.log | sed -E "s/.*error: //" | sort | uniq -c | sort -rn | head -50
ls -la $T/build/jfwhaven2 2>/dev/null
