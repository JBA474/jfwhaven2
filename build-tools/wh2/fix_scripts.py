import sys, os
S = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
p = os.path.join(S, 'tier2', 'wh2', 'port_wh2.py')
s = open(p, encoding='utf-8').read()
anchor = "    s = re.sub(r'(?m)^[ \\t]*(cls80x25|setvmode|tprintf)\\([^;]*\\);[ \\t]*\\n', '', s)\n    save(name, s)"
assert anchor in s, 'anchor missing'
extra = (
    "    s = re.sub(r'\\bshutdown\\s*\\(', 'shutdowngame(', s)\n"
    "    s = re.sub(r'itoa\\(([^,]+), ([^,]+), 10\\)', r'sprintf(\\2, \"%d\", \\1)', s)\n"
    "    save(name, s)")
s = s.replace(anchor, anchor.replace("    save(name, s)", extra))
open(p, 'w', encoding='utf-8', newline='\n').write(s)

p = os.path.join(S, 'q_wh2full.sh')
s = open(p, encoding='utf-8').read()
s = s.replace("sed -i -E 's/long/int/g'", "sed -i -E 's/\\blong\\b/int/g'")
open(p, 'w', encoding='utf-8', newline='\n').write(s)
print('fixed')
