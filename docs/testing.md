# Testing Boot Bitch

The default test suite is safe to run on a workstation, container, or CI
runner. It builds the Qt application and runs the offscreen UI tests plus the
shell contract tests; these tests do not mount disks, unlock LUKS volumes, or
modify a target.

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
bash -n scripts/*.sh
```

Use a disposable virtual machine for integration testing. Boot the VM from a
live or recovery image, attach a second virtual disk containing a disposable
Debian/Ubuntu installation, and run Boot Bitch against that second disk. Do
not select the VM's running system disk as a repair target. Snapshots and a
throwaway disk make rollback testing repeatable.

Containers are useful for compiler, UI, AppStream, and helper-contract checks.
They do not provide a safe model for EFI NVRAM, real LUKS unlocks, or target
boot failures. Testing those paths in a container would require privileged
device and mount access; use a VM instead.

The GitHub Actions workflow runs the safe build and test set on Ubuntu. The
packaging scripts can then create a Debian package in an external build
directory for inspection before release.
