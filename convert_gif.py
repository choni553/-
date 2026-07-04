#!/usr/bin/env python3
"""Convert animated WebP to LVGL v9 BGRA"""
from PIL import Image
import os

webp = r"C:\Users\刘金芮\Desktop\super auto pets\uWN0sc21DPOULVyY8V.webp"
out_dir = r"d:\kaifa daima\lv_port_linux-release-v9.4\src\UI"

img = Image.open(webp)
frames, durs = [], []
try:
    while True:
        img.seek(len(frames))
        durs.append(img.info.get('duration', 100))
        frames.append(img.convert("RGBA"))
except EOFError:
    pass

w, h = frames[0].size
print(f"WebP: {len(frames)} frames @ {w}x{h}")

lines = ['// WebP anim BGRA', '#include "lvgl/lvgl.h"', '']
stride, dsize = w * 4, w * h * 4

for i, frame in enumerate(frames):
    raw = frame.tobytes()
    bgra = bytearray()
    for j in range(0, len(raw), 4):
        r, g, b, a = raw[j], raw[j+1], raw[j+2], raw[j+3]
        bgra.extend([b, g, r, a])
    lines.append(f'static const uint8_t gif_frame{i}_map[] = {{')
    for k in range(0, len(bgra), 8):
        lines.append('    ' + ', '.join(f'0x{x:02x}' for x in bgra[k:k+8]) + ',')
    lines.append('};')
    lines.append('')
    print(f"  Frame {i}: {len(bgra)} bytes")

for i in range(len(frames)):
    lines.append(f'const lv_image_dsc_t gif_frame{i} = {{')
    lines.append('    .header.magic = LV_IMAGE_HEADER_MAGIC,')
    lines.append(f'    .header.cf = LV_COLOR_FORMAT_ARGB8888,')
    lines.append('    .header.flags = 0,')
    lines.append(f'    .header.w = {w}, .header.h = {h},')
    lines.append(f'    .header.stride = {stride},')
    lines.append('    .header.reserved_2 = 0,')
    lines.append(f'    .data_size = {dsize},')
    lines.append(f'    .data = gif_frame{i}_map,')
    lines.append('    .reserved = NULL, .reserved_2 = NULL,')
    lines.append('};')
    lines.append('')

lines.append(f'const void * gif_anim_frames[{len(frames) + 1}] = {{')
for i in range(len(frames)):
    lines.append(f'    &gif_frame{i},')
lines.append('    NULL,')
lines.append('};')
lines.append(f'const int gif_anim_num_frames = {len(frames)};')
lines.append(f'const int gif_anim_duration = {sum(durs)};')

with open(os.path.join(out_dir, 'gif_anim.c'), 'w', encoding='utf-8') as f:
    f.write('\n'.join(lines))
print(f"Done: {os.path.getsize(os.path.join(out_dir, 'gif_anim.c'))/1024:.0f} KB")
