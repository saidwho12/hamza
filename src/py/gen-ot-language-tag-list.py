#!/usr/bin/env python3

"""
Usage: ./gen-ot-language-list
"""

import os.path, sys

if len(sys.argv) != 2:
    sys.exit(__doc__)

files = [open(x, encoding="utf-8") for x in sys.argv[1:]]
file = open("hz-ot-language-tags-list.h", "w+", encoding="ascii")

file.write("#ifndef HZ_OT_LANGUAGE_TAGS_LIST_H\n")
file.write("#define HZ_OT_LANGUAGE_TAGS_LIST_H\n")



file.write("#endif /* HZ_OT_LANGUAGE_TAGS_LIST_H */")
