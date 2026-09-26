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


def run_updater(efivarfs: Path, label: str, backup: Path) -> subprocess.CompletedProcess:
    return subprocess.run(
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
            label,
            "--backup",
            str(backup),
        ],
        capture_output=True,
        text=True,
        env={**os.environ, "EFI_LABEL_EFIVARFS": str(efivarfs)},
    )


with tempfile.TemporaryDirectory(prefix="boot-repair-efi-label-") as temp:
    efivarfs = Path(temp)
    variable = efivarfs / VARIABLE
    original = make_variable("UEFI OS")
    variable.write_bytes(original)
    backup = efivarfs / "backup" / "Boot0008.bin"
    result = run_updater(efivarfs, "UEFI OS Example NVMe 1TB", backup)
    assert result.returncode == 0, result.stderr
    updated = variable.read_bytes()
    assert updated != original
    assert "UEFI OS Example NVMe 1TB".encode("utf-16-le") in updated
    assert backup.read_bytes() == original

    # A symlinked backup path must be refused before anything is written, and
    # the symlink target must stay untouched.
    symlink_target = efivarfs / "innocent.bin"
    symlink_target.write_bytes(b"untouched")
    symlink = efivarfs / "backup-symlink.bin"
    symlink.symlink_to(symlink_target)
    result = run_updater(efivarfs, "UEFI OS via symlink", symlink)
    assert result.returncode != 0
    assert "non-regular path" in result.stderr
    assert symlink_target.read_bytes() == b"untouched"

    # A non-regular (directory) backup path must be refused as well.
    backup_dir = efivarfs / "backup-dir"
    backup_dir.mkdir()
    result = run_updater(efivarfs, "UEFI OS via dir", backup_dir)
    assert result.returncode != 0
    assert "non-regular path" in result.stderr

    # An existing regular backup is the pristine original: a later run must
    # keep it byte-identical instead of overwriting it.
    backup2 = efivarfs / "backup2" / "Boot0008.bin"
    variable.write_bytes(make_variable("UEFI OS"))
    result = run_updater(efivarfs, "First label", backup2)
    assert result.returncode == 0, result.stderr
    first_backup = backup2.read_bytes()
    result = run_updater(efivarfs, "Second label", backup2)
    assert result.returncode == 0, result.stderr
    assert backup2.read_bytes() == first_backup
    assert "Second label".encode("utf-16-le") in variable.read_bytes()

# Rollback honesty: when the restore write also fails, the tool must say so on
# stderr and in the failure message instead of swallowing it. A read-only
# variable simulates the write failure without root; skip when running as
# root (root bypasses the permission bit).
if os.geteuid() != 0:
    with tempfile.TemporaryDirectory(prefix="boot-repair-efi-label-") as temp:
        efivarfs = Path(temp)
        variable = efivarfs / VARIABLE
        original = make_variable("UEFI OS")
        variable.write_bytes(original)
        backup = efivarfs / "backup" / "Boot0008.bin"
        result = run_updater(efivarfs, "New label", backup)
        assert result.returncode == 0, result.stderr
        variable.chmod(0o444)
        result = run_updater(efivarfs, "Another label", backup)
        assert result.returncode != 0
        assert "ROLLBACK ALSO FAILED; original bytes remain in" in result.stderr
        assert str(backup) in result.stderr
        assert backup.read_bytes() == original

print("PASS: EFI label updater preserves, verifies and backs up EFI_LOAD_OPTION destinations safely")
