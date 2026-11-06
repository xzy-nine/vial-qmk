import hashlib
import os
import sys


def main():
    """Emit a stable BUILD_ID for this firmware.

    Vial derives the EEPROM validity magic from BUILD_ID. A random value makes
    the magic change on every build, which wipes the user configuration stored
    in EEPROM each time the same board is reflashed. Deriving the id from the
    keyboard and keymap name instead keeps it stable across rebuilds.
    """
    key = "%s:%s" % (os.environ.get("KEYBOARD", ""), os.environ.get("KEYMAP", ""))
    digest = hashlib.sha256(key.encode("utf-8")).digest()
    value = int.from_bytes(digest[:3], "big")
    print("#define BUILD_ID ((uint32_t)0x{:08X})".format(value))


if __name__ == "__main__":
    main()
