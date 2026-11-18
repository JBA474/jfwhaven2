# jfwhaven2

Witchaven II on JonoF's Build engine, laid out like [jonof/jfwhaven](https://github.com/jonof/jfwhaven)
so it builds against the `mister` branches of [ItsDanik/jfbuild](https://github.com/ItsDanik/jfbuild)
and [ItsDanik/jfaudiolib](https://github.com/ItsDanik/jfaudiolib) and runs as a game set of the
[jfSW MiSTer hybrid core](https://github.com/ItsDanik/jfSW_MiSTer) (8BitDo-class gamepad, rumble,
the core's OSD Cheats page).

`src/` and `CMakeLists.txt` are generated from the original DOS sources by the scripts in
`build-tools/` (see the `q_*full.sh` driver there), so the port stays reproducible from the
originals. The game data itself is not in this repository.

**Status:** playtesting and bug-fixing in progress, feedback welcome.

**Origin of the sources:** Capstone's WHAVEN2 2.0g archive as re-published by Les Bird (https://lesbird.github.io/capstone/index.html; also https://github.com/willtate/whaven2 and RTCM https://rtcm.site/knowledge-base/downloads-rtcm/src-witchaven2-source). Level fixes for LEVEL5/LEVEL13 by FGSFDS (https://rtcm.site/knowledge-base/downloads-rtcm/witchaven-patches/) belong to the game set, not here.

**License:** the Build engine parts are under Ken Silverman's Build license (BUILDLIC.TXT in jfbuild);
the port scripts are GPL-2.0-or-later. The game sources were released to hobbyists by a former
Capstone programmer (Les Bird); no commercial game data is included.

**Disclaimer:** I of course used AI. Most of the porting, building and log-level testing was done with
Claude Code (Anthropic) under my direction; the games were played on the hardware by me.

🤖 Generated with [Claude Code](https://claude.com/claude-code)
