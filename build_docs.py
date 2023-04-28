#!/usr/bin/python3

import os
import subprocess

print(os)
print('os name: ' + os.name)
nd = os.path.join(os.path.abspath(os.sep), 'Program Files (x86)', 'Natural Docs', 'NaturalDocs.exe')
subprocess.run([nd, '-p', './nd-project','-i','./hz', '-o', 'HTML', './docs/', '--tab-width', '4', '--no-auto-group', '--rebuild'],shell=True)