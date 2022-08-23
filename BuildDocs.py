#!/usr/bin/python3

import os

print(os.name)
os.system("\"C:\\Program Files (x86)\\Natural Docs\\NaturalDocs.exe\" -p .\\nd-project -i .\\hz -o HTML .\\docs\\ --tab-width 4 --no-auto-group --rebuild")