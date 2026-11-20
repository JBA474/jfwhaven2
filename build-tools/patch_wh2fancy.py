"""Witchaven II on the MiSTer (2026-11-19): fancyfont() and fancyfontscreen() (whmenu.c) lower-
case the string they are given in place; called with a string literal, that is a write into
read-only memory on Linux (the DOS build's literals were writable): SIGSEGV in strlwr at
0x2203ec, the crash Jess hit twice in the game. They lower-case a copy now, and the copy is
256 bytes instead of 40. usage: patch_wh2fancy.py <jfwhaven2 tree>  (idempotent)"""
import os, sys
root = sys.argv[1]
p = os.path.join(root, 'src/whmenu.c'); s = open(p, encoding='utf-8', errors='replace').read()
old = '''     strlwr(string);
     len = strlen(string);
     strcpy(temp, string);
'''
new = '''     strncpy(temp, string, sizeof(temp) - 1);   // MiSTer: a copy, the string may be a literal (read-only)
     temp[sizeof(temp) - 1] = 0;
     strlwr(temp);
     len = strlen(temp);
'''
if new in s:
    print('already'); sys.exit(0)
assert s.count(old) == 2, s.count(old)
out = ''; rest = s
for _ in range(2):
    i = rest.index(old)
    head = rest[:i]
    j = head.rindex('     char temp[40];\n')
    head = head[:j] + '     char temp[256];\n' + head[j + len('     char temp[40];\n'):]
    out += head + new; rest = rest[i + len(old):]
out += rest
open(p, 'w', encoding='utf-8', newline='\n').write(out); print('patched whmenu.c (fancyfont copies)')
