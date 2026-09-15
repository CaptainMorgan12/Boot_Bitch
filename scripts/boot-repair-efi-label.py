#!/usr/bin/env python3
"""Safely replace the description in one EFI_LOAD_OPTION.

efibootmgr can create a labelled entry, but its -L option does not edit an
existing Boot#### variable.  This small helper changes only the UTF-16
description after validating the variable's partition GUID and file path.
"""

import argparse
import os
import pathlib
import struct
import sys
import uuid

EFI_GLOBAL = "8be4df61-93ca-11d2-aa0d-00e098032b8c"


def fail(message: str) -> None:
    print(f"EFI label update failed: {message}", file=sys.stderr)
    raise SystemExit(1)


def norm_path(value: str) -> str:
    return value.replace("/", "\\").casefold().lstrip("\\")


def parse_option(raw: bytes, expected_part: str, expected_loader: str):
    if len(raw) < 12:
        fail("EFI variable is shorter than an EFI_LOAD_OPTION")
    # efivarfs prepends the four-byte variable attributes.  The remaining
    # bytes are the EFI_LOAD_OPTION itself.
    option = raw[4:]
    load_attributes, path_length = struct.unpack_from("<IH", option, 0)
    del load_attributes  # preserved verbatim; this tool never changes it
    pos = 6
    end = None
    while pos + 1 < len(option):
        if option[pos:pos + 2] == b"\0\0":
            end = pos
            break
        pos += 2
    if end is None or end == 6:
        fail("EFI description is missing or malformed")
    try:
        old_label = option[6:end].decode("utf-16-le", errors="strict")
    except UnicodeDecodeError:
        fail("EFI description is not valid UTF-16")
    path_start = end + 2
    path_end = path_start + path_length
    if path_end > len(option) or path_length < 4:
        fail("EFI device path list is outside the EFI variable")

    expected_guid = uuid.UUID(expected_part).bytes_le
    found_guid = False
    found_loader = False
    pos = path_start
    while pos + 4 <= path_end:
        node_type, subtype, node_length = struct.unpack_from("<BBH", option, pos)
        if node_length < 4 or pos + node_length > path_end:
            fail("EFI device path contains an invalid node length")
        node = option[pos:pos + node_length]
        if node_type == 0x04 and subtype == 0x01 and node_length >= 42:
            # HARD_DRIVE_DEVICE_PATH: GPT signature starts at byte 24.
            if node[41] == 0x02 and node[40] == 0x02:
                if node[24:40] == expected_guid:
                    found_guid = True
        elif node_type == 0x04 and subtype == 0x04 and node_length >= 6:
            try:
                path = node[4:-2].decode("utf-16-le", errors="strict")
            except UnicodeDecodeError:
                fail("EFI file path is not valid UTF-16")
            if norm_path(path) == norm_path(expected_loader):
                found_loader = True
        if node_type == 0x7F and subtype == 0xFF:
            break
        pos += node_length

    if not found_guid:
        fail(f"Boot entry does not point to selected ESP PARTUUID {expected_part}")
    if not found_loader:
        fail(f"Boot entry does not point to selected loader {expected_loader}")
    return old_label, end


def encode_label(label: str) -> bytes:
    if not label or "\0" in label or len(label) > 240:
        fail("new EFI label is empty, contains NUL, or is too long")
    try:
        encoded = label.encode("utf-16-le", errors="strict")
    except UnicodeEncodeError:
        fail("new EFI label is not valid UTF-16")
    return encoded + b"\0\0"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--bootnum", required=True)
    parser.add_argument("--partuuid", required=True)
    parser.add_argument("--loader", required=True)
    parser.add_argument("--label", required=True)
    parser.add_argument("--backup", required=True)
    args = parser.parse_args()
    if not args.bootnum.isdigit() or len(args.bootnum) != 4:
        fail("Boot number must be exactly four decimal digits")
    try:
        partuuid = str(uuid.UUID(args.partuuid))
    except ValueError:
        fail("selected ESP PARTUUID is invalid")

    root = os.environ.get("EFI_LABEL_EFIVARFS", "/sys/firmware/efi/efivars")
    if root == "/sys/firmware/efi/efivars" and os.geteuid() != 0:
        fail("EFI variable updates require root")
    variable = pathlib.Path(root) / f"Boot{args.bootnum.upper()}-{EFI_GLOBAL}"
    try:
        original = variable.read_bytes()
    except OSError as exc:
        fail(f"cannot read {variable}: {exc}")
    old_label, description_end = parse_option(original, partuuid, args.loader)
    replacement = encode_label(args.label)
    option = original[4:]
    updated = original[:4] + option[:6] + replacement + option[description_end + 2:]
    if len(updated) != len(original) + len(replacement) - (description_end + 2 - 6):
        fail("internal EFI description size calculation failed")
    backup = pathlib.Path(args.backup)
    backup.parent.mkdir(mode=0o700, parents=True, exist_ok=True)
    if not backup.exists():
        backup.write_bytes(original)
        os.chmod(backup, 0o600)
    try:
        fd = os.open(variable, os.O_WRONLY)
        try:
            written = os.write(fd, updated)
        finally:
            os.close(fd)
        if written != len(updated):
            raise OSError("short EFI variable write")
        check = variable.read_bytes()
        new_label, _ = parse_option(check, partuuid, args.loader)
        if new_label != args.label:
            raise OSError(f"firmware returned label {new_label!r}")
    except OSError as exc:
        try:
            fd = os.open(variable, os.O_WRONLY)
            try:
                os.write(fd, original)
            finally:
                os.close(fd)
        except OSError:
            pass
        fail(str(exc))
    print(f"EFI label changed for Boot{args.bootnum.upper()}: {old_label!r} -> {args.label!r}")
    return 0


if __name__ == "__main__":
    main()
