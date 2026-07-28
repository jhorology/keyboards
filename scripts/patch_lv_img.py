import sys
import re

if len(sys.argv) < 2:
    sys.exit(0)

filepath = sys.argv[1]
with open(filepath, "r") as f:
    content = f.read()

def replace_header(match):
    cf = match.group(1)
    w = int(match.group(2))
    h = int(match.group(3))
    if "ALPHA_1BIT" in cf:
        color_fmt = "LV_COLOR_FORMAT_A1"
        stride = (w + 7) // 8
    elif "INDEXED_1BIT" in cf:
        color_fmt = "LV_COLOR_FORMAT_I1"
        stride = (w + 7) // 8
    else:
        color_fmt = "LV_COLOR_FORMAT_A1"
        stride = (w + 7) // 8

    return f"""  .header.magic = LV_IMAGE_HEADER_MAGIC,
  .header.cf = {color_fmt},
  .header.flags = 0,
  .header.w = {w},
  .header.h = {h},
  .header.stride = {stride},"""

pattern = r'\s*\.header\.cf\s*=\s*(LV_IMG_CF_\w+),\s*\n\s*\.header\.always_zero\s*=\s*\d+,\s*\n\s*\.header\.reserved\s*=\s*\d+,\s*\n\s*\.header\.w\s*=\s*(\d+),\s*\n\s*\.header\.h\s*=\s*(\d+),'

new_content = re.sub(pattern, replace_header, content)

with open(filepath, "w") as f:
    f.write(new_content)
