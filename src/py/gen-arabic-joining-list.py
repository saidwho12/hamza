#!/usr/bin/env python3

"""
Usage: ./gen-arabic-joining-table.py ArabicShaping.txt Scripts.txt
Input files:
* https://unicode.org/Public/UCD/latest/ucd/ArabicShaping.txt
* https://unicode.org/Public/UCD/latest/ucd/Scripts.txt
"""

import os.path, sys

if len(sys.argv) != 3:
    sys.exit(__doc__)

files = [open(x, encoding="utf-8") for x in sys.argv[1:]]
file = open("hz-ot-shape-complex-arabic-joining-list.h", "w+", encoding="ascii")

file.write("#ifndef HZ_OT_SHAPE_COMPLEX_ARABIC_JOINING_LIST_H\n")
file.write("#define HZ_OT_SHAPE_COMPLEX_ARABIC_JOINING_LIST_H\n")

file.write("\n")
file.write("static const hz_arabic_joining_entry_t hz_arabic_joining_list[] = {\n")

for line in (line for line in files[0].readlines() if not line.startswith('#') and ';' in line):
    line = line.replace('\n', '')
    parts = [x.strip() for x in line.split(";")]
    code = parts[0]
    name = parts[1]
    joining_type = 'JOINING_TYPE_%s' % parts[2].replace(' ', '_')
    joining_group = ''
    if parts[3] == 'No_Joining_Group':
        joining_group = 'NO_JOINING_GROUP'
    else:
        joining_group = 'JOINING_GROUP_%s' % parts[3].replace(' ', '_')

    file.write("    { 0x%s, \"%s\", %s | %s },\n" % (code, name, joining_type, joining_group))

file.write("};\n")

file.write("\n")

file.write("#endif /* HZ_OT_SHAPE_COMPLEX_ARABIC_JOINING_LIST_H */")

