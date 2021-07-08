#!/usr/bin/env python3

"""
Usage: ./gen-script-table.py Scripts.txt
Input files:
* https://unicode.org/Public/UCD/latest/ucd/Scripts.txt
"""

import os.path, sys

if len(sys.argv) != 2:
    sys.exit(__doc__)

files = [open(x, encoding="utf-8") for x in sys.argv[1:]]
file = open("hz-script-table.h", "w+", encoding="ascii")

file.write("#ifndef HZ_SCRIPT_TABLE_H\n")
file.write("#define HZ_SCRIPT_TABLE_H\n")

file.write("\n")
file.write("\n")
file.write("typedef struct hz_script_range_t {\n")
file.write("    const char *script_name;\n")
file.write("    hz_script_t script;\n")
file.write("    hz_unicode_t first_code;\n")
file.write("    hz_unicode_t last_code;\n")
file.write("} hz_script_range_t;\n")

file.write("\n")
file.write("static const hz_script_range_t script_ranges[] = {\n")

for line in (line for line in files[0].readlines() if not line.startswith('#') and ';' in line):
    line = line.replace('\n', '')
    parts = [x.strip() for x in line.split(";")]
    codes = parts[0]

    code_parts = codes.split('..', 2)
    if len(code_parts) == 2:
        r0 = code_parts[0]
        r1 = code_parts[1]
    else:
        r0 = code_parts[0]
        r1 = code_parts[0]

    script = (parts[1].split('#'))[0].strip()
    s = 'HZ_SCRIPT_%s' % script.upper()
    sn = script.replace('_',' ')

    file.write("    { \"%s\", %s, 0x%s, 0x%s },\n" % (sn, s, r0, r1))

file.write("};\n")

file.write("\n")

file.write("#endif /* HZ_SCRIPT_TABLE_H */")