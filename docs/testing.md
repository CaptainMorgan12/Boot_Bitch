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

For Arch-family build and package testing, use the disposable VM when it is
available:

```bash
VM="arch-boot-repair"
sudo virsh start "$VM"
virt-viewer --connect qemu:///system "$VM"
```

The VM can exchange source trees or generated packages through
`/home/amiga/VMs/boot-repair/shared/`. Inside the VM, run
`./scripts/setup-dev-deps.sh`, `./scripts/package-arch.sh`, and
`./scripts/test-packaging-profile-contract.sh`. Package creation defaults to
`makepkg`; it does not install dependencies unless `--syncdeps` is explicitly
requested. Install the resulting package with `pacman -U` only after
the artifact and dependency list have been reviewed.

After installation, the Arch VM can exercise the native guarded backend with
the VM's disposable root (for example, `/dev/vda` and `/dev/vda2`):

```bash
/usr/libexec/boot-repair/boot-repair-helper host-diagnose /dev/vda /dev/vda2 backend
/usr/libexec/boot-repair/boot-repair-helper host-repair /dev/vda /dev/vda2 fix-broken
/usr/libexec/boot-repair/boot-repair-helper host-repair /dev/vda /dev/vda2 initramfs
/usr/libexec/boot-repair/boot-repair-helper host-repair /dev/vda /dev/vda2 grub
```

The package transaction uses a copied pacman database and cache for its trial;
repository/download errors, removals and unresolved dependencies stop the
repair before the live transaction is started. The conventional EFI repair
also verifies or creates one firmware entry for the selected vendor loader,
then reruns the duplicate-destination and BootOrder checks.
