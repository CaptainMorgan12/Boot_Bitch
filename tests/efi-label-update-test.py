#!/usr/bin/env python3
import os
import struct
import subprocess
import tempfile
import uuid
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PART = "11111111-2222-3333-4444-555555555555"
GUID = uuid.UUID(PART).bytes_le
VARIABLE = "Boot0008-8be4df61-93ca-11d2-aa0d-00e098032b8c"


def make_variable(label: str) -> bytes:
    hard_drive = (
        struct.pack("<BBH", 4, 1, 42)
        + struct.pack("<IQQ", 1, 2048, 100000)
        + GUID
        + b"\x02\x02"
    )
    path_text = "\\EFI\\BOOT\\BOOTX64.EFI".encode("utf-16-le") + b"\0\0"
    file_path = struct.pack("<BBH", 4, 4, 4 + len(path_text)) + path_text
    end = struct.pack("<BBH", 0x7F, 0xFF, 4)
    device_path = hard_drive + file_path + end
    description = label.encode("utf-16-le") + b"\0\0"
    option = struct.pack("<IH", 7, len(device_path)) + description + device_path
    return b"\x07\x00\x00\x00" + option


with tempfile.TemporaryDirectory(prefix="boot-repair-efi-label-") as temp:
    efivarfs = Path(temp)
    variable = efivarfs / VARIABLE
    original = make_variable("UEFI OS")
    variable.write_bytes(original)
    backup = efivarfs / "backup" / "Boot0008.bin"
    subprocess.run(
        [
            "python3",
            str(ROOT / "scripts/boot-repair-efi-label.py"),
            "--bootnum",
            "0008",
            "--partuuid",
            PART,
            "--loader",
            r"\EFI\BOOT\BOOTX64.EFI",
            "--label",
            "UEFI OS Example NVMe 1TB",
            "--backup",
            str(backup),
        ],
        check=True,
        env={**os.environ, "EFI_LABEL_EFIVARFS": str(efivarfs)},
    )
    updated = variable.read_bytes()
    assert updated != original
    assert "UEFI OS Example NVMe 1TB".encode("utf-16-le") in updated
    assert backup.read_bytes() == original
print("PASS: EFI label updater preserves and verifies EFI_LOAD_OPTION destinations")
