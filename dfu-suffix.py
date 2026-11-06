#!/usr/bin/env python3
"""Minimal dfu-suffix replacement.

Appends a 16-byte DFU suffix to a firmware file, matching the layout used by
dfu-util so the STM32duino bootloader accepts the image.
"""
import argparse
import struct
import sys

DFU_SUFFIX_LENGTH = 16
DFU_SIGNATURE = b'UFD'


def add_suffix(path, vid, pid, did, bcd_device):
    with open(path, "rb") as fh:
        data = fh.read()

    if data[-DFU_SUFFIX_LENGTH:][-3:] == DFU_SIGNATURE:
        print("dfu-suffix: %s already has a suffix" % path)
        return 0

    suffix = struct.pack("<HHHH3sB", bcd_device, pid, vid, did, DFU_SIGNATURE, DFU_SUFFIX_LENGTH)
    with open(path, "wb") as fh:
        fh.write(data + suffix)

    print("dfu-suffix: appended to %s" % path)
    return 0


def main():
    ap = argparse.ArgumentParser(add_help=True)
    ap.add_argument("-v", "--vid", type=lambda x: int(x, 16), required=True)
    ap.add_argument("-p", "--pid", type=lambda x: int(x, 16), required=True)
    ap.add_argument("-d", "--did", type=lambda x: int(x, 16), default=0xFFFF)
    ap.add_argument("-b", "--bcd-device", type=lambda x: int(x, 16), default=0x0100)
    ap.add_argument("-a", "--add", metavar="FILE", required=True)
    args = ap.parse_args()

    return add_suffix(args.add, args.vid, args.pid, args.did, args.bcd_device)


if __name__ == '__main__':
    sys.exit(main())
