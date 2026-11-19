"""jfbuild (jfSW MiSTer core games, 2026-11-17): the engine's printext256()
text in the 640-pixel modes. The core shows 640x200 and 640x240 on a 4:3
screen, so a pixel is half as wide as in 320x200; the 8x8 and 4x6 fonts drawn
a pixel a column came out squeezed to half their width and could not be read
(TekWar's messages and dialogues, Jess 2026-11-17). In those modes every
font column is now drawn two pixels wide, which gives the text the shape it
has in 320x200.
usage: patch_text.py <tree with jfbuild/>  (idempotent)"""
import os, sys
root = sys.argv[1]

def edit(rel, pairs):
    p = os.path.join(root, rel); s = open(p, encoding='utf-8', errors='replace', newline='').read(); crlf = '\r\n' in s
    for old, new in pairs:
        if crlf: old, new = old.replace('\n', '\r\n'), new.replace('\n', '\r\n')
        if new in s: continue
        assert s.count(old) == 1, f'{rel}: {old[:60]!r} x{s.count(old)}'
        s = s.replace(old, new)
    open(p, 'w', encoding='utf-8', newline='').write(s); print('patched', rel)

edit('jfbuild/src/engine.c', [
    ('''	f = &textfonts[min((unsigned)fontsize, 2)];
	stx = xpos;

	for(i=0;name[i];i++)
	{
		letptr = &f->font[((int)(unsigned char)name[i])*f->cellh + f->cellyoff];
		ptr = (unsigned char *)(ylookup[ypos+f->charysiz-1]+stx+frameplace);
		for(y=f->charysiz-1;y>=0;y--)
		{
			for(x=f->charxsiz-1;x>=0;x--)
			{
				if (letptr[y]&pow2char[7-x-f->cellxoff])
					ptr[x] = (unsigned char)col;
				else if (backcol >= 0)
					ptr[x] = (unsigned char)backcol;
			}
			ptr -= bytesperline;
		}
		stx += f->charxsiz;
	}
''', '''	f = &textfonts[min((unsigned)fontsize, 2)];
	stx = xpos;

	// MiSTer: in the core's 640x200 and 640x240 modes a pixel is half as
	// wide as in 320x200 (the screen is 4:3 at every size), so a font column
	// is drawn two pixels wide to keep the text's shape
	const int xs = (xdim == 640 && ydim <= 240) ? 2 : 1;

	for(i=0;name[i];i++)
	{
		letptr = &f->font[((int)(unsigned char)name[i])*f->cellh + f->cellyoff];
		ptr = (unsigned char *)(ylookup[ypos+f->charysiz-1]+stx+frameplace);
		for(y=f->charysiz-1;y>=0;y--)
		{
			for(x=f->charxsiz-1;x>=0;x--)
			{
				if (letptr[y]&pow2char[7-x-f->cellxoff])
					ptr[x*xs] = ptr[x*xs+xs-1] = (unsigned char)col;
				else if (backcol >= 0)
					ptr[x*xs] = ptr[x*xs+xs-1] = (unsigned char)backcol;
			}
			ptr -= bytesperline;
		}
		stx += f->charxsiz*xs;
	}
'''),
])
print('text OK')
