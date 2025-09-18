#!/usr/bin/env python3
from pathlib import Path

MD = Path('docs/ComponentTraits.md')
text = MD.read_text()
lines = text.splitlines()

out_lines = []

i = 0
while i < len(lines):
    line = lines[i]
    if line.strip().startswith('```cpp'):
        # start collecting block including opening fence
        block = [line]
        i += 1
        while i < len(lines) and not lines[i].strip().startswith('```'):
            block.append(lines[i])
            i += 1
        # add the closing fence if present
        if i < len(lines):
            block.append(lines[i])
        # check if body has HIDE lines
        body = block[1:-1]
        has_hide = any(l.strip().startswith('HIDE:') for l in body)
        if not has_hide:
            # insert HIDE as first body line
            new_block = [block[0]] + ['HIDE: flecs::world world;'] + body + [block[-1]]
        else:
            new_block = block
        out_lines.extend(new_block)
        i += 1
    else:
        out_lines.append(line)
        i += 1

new_text = '\n'.join(out_lines) + ('\n' if text.endswith('\n') else '')
if new_text != text:
    MD.write_text(new_text)
    print('Updated', MD)
else:
    print('No changes required')
