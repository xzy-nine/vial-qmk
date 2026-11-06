#!/usr/bin/env python3
"""qmk launcher for the vial-qmk checkout."""
import os
import sys

QMK_HOME = r"E:\GitHubCode\01Main\KeyBrorad\vial-qmk"
os.environ.setdefault('ORIG_CWD', os.getcwd())
os.environ['QMK_HOME'] = QMK_HOME
sys.path.insert(0, os.path.join(QMK_HOME, "lib", "python"))

from milc import cli  # noqa: E402
import qmk.cli  # noqa: E402,F401

if __name__ == '__main__':
    cli()
