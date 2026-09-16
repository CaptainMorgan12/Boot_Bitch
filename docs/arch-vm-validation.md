# Arch VM validation

The disposable `arch-boot-repair` VM uses `/dev/vda` as its running Arch disk and
`/dev/vdb` as the alternate repair disk. The host share is exposed in the guest
as `/host`; the current test bundle is `/host/boot-repair-test`.

## Install the current helper in the guest

```bash
cd /host/boot-repair-test
sudo install -Dm755 boot-repair-bin /usr/bin/boot-repair
sudo install -Dm755 scripts/boot-repair-helper.sh \\
  /usr/libexec/boot-repair/boot-repair-helper
```

## Running-host checks

```bash
sudo /usr/libexec/boot-repair/boot-repair-helper \\
  host-diagnose /dev/vda /dev/vda2 backend
sudo /usr/libexec/boot-repair/boot-repair-helper \\
  host-repair /dev/vda /dev/vda2 fix-broken
sudo /usr/libexec/boot-repair/boot-repair-helper \\
  host-repair /dev/vda /dev/vda2 initramfs
sudo /usr/libexec/boot-repair/boot-repair-helper \\
  host-repair /dev/vda /dev/vda2 grub
```

The package stage is expected to refuse when pacman cannot download repository
metadata or packages. A refusal is a safety pass: no target package database or
installed packages are changed. The initramfs and GRUB stages must complete a
trial first, then apply and verify their generated artifacts.

For the alternate repair disk, boot the VM from the recovery image or the
alternate disk using `boot-disk.sh vdb`, then run the target-scoped `diagnose`
and `repair` commands only after confirming the root component with `lsblk`.
Never select the running system as a repair target.
