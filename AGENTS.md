# AGENTS.md

Boot Bitch (CMake project `BootRepair`): Qt6 Widgets GUI plus a privileged Bash backend for guarded Linux boot diagnostics and repair. Only these paths are in scope; ignore other `~/Projects` entries:

- `~/Projects/Boot_Repair` — canonical source (this directory)
- `~/Projects/Boot_Bitch-github` — local staging mirror for GitHub releases (no local `.git`)
- `~/VMs/boot-repair` — Arch libvirt test rig

## Build, test, package

- Full local build + staged-install validation (+ `.deb` on Debian-family hosts): `scripts/build.sh`
- CI-equivalent checks: `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build && QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure && bash -n scripts/*.sh`
- UI tests fail headless without `QT_QPA_PLATFORM=offscreen`. Single test: `ctest --test-dir build -R boot-repair-ui-read-only --output-on-failure`; contract tests are plain `scripts/test-*.sh` and run standalone.
- Known trap: ctest test 5 / `scripts/test-packaging-profile-contract.sh` fails whenever `Development/release-*/` contains a built package — `install.sh` finds the artifact and skips the guidance text the test greps for. Not a regression; move the artifact out to verify.
- Known trap: some `Development/build-*` dirs are pinned to other source trees (e.g. `build-gated-final` → `Boot_Bitch-github-fixed`). Check `CMAKE_HOME_DIRECTORY` in `CMakeCache.txt` or use a fresh build dir.
- `scripts/package-arch.sh` must run as an unprivileged user on an Arch host (makepkg). `scripts/setup-dev-deps.sh` covers debian/arch/rpm/suse.

## Architecture

- `src/MainWindow.cpp` — GUI; `scripts/boot-repair-helper.sh` — the only privileged backend, installed to `/usr/libexec/boot-repair/boot-repair-helper` (pkexec; dispatch `repair|host-repair|diagnose|host-diagnose|validate|... <disk> <root> [args]`).
- Gating contract: diagnostics emit stable `Repair tool <key>: available` / `unavailable|<reason>` lines (keys: validate dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub bootstack). `MainWindow::repairToolAvailable()` treats the cached diagnostic log as the single source of truth, fails closed, and must disable both the individual tool and the Settings/Full Repair stage. Backend runtime preflights always stay.
- Backend families: Debian/APT and Arch/pacman. Arch package changes are one sandboxed full pacman transaction (`-Syyuw` preflight; no partial metadata refresh); other families are diagnostics-only.
- Version is parsed from `CMakeLists.txt` (`project(BootRepair VERSION ...)`); bump it there.
- Session logs: one canonical file per run under `QStandardPaths::AppDataLocation/logs` (override with `BOOT_REPAIR_LOG_DIR` for tests), created when a scope is identified, with SCOPE markers per host/target and visible timestamps; prior sessions are listed read-only in the Logs tab. `diagnostics/autoRefreshStale` (default on) re-runs Run All after invalidating operations when a privileged session is active; it never prompts for authorization by itself.

## VM validation (Arch)

- Domain `arch-boot-repair` on `qemu:///system`; `~/VMs/boot-repair/shared/` is virtiofs-mounted in the guest as `/host`.
- `vda` is the VM's running host (protected; build/install/test the tool there). `vdb` is the disposable Arch repair target — never repair `vda`.
- Workflow: sync source to `shared/boot-repair-test/`, build in the guest as unprivileged `amiga` with `scripts/package-arch.sh`, then `sudo pacman -U Development/build-arch-package/boot-bitch-<ver>-1-x86_64.pkg.tar.zst`.
- Headless control: `virsh -c qemu:///system qemu-agent-command arch-boot-repair '{"execute":"guest-exec",...}'`. Boot-disk selection: `~/VMs/boot-repair/boot-disk.sh vda|vdb` (VM must be shut off; uses `sudo virsh`).
- Before/after every repair test assert `findmnt -no SOURCE /` is `/dev/vda2` and `/dev/vdb2` is unmounted. Example: `/usr/libexec/boot-repair/boot-repair-helper repair /dev/vdb /dev/vdb2 fix-broken`.

## Release workflow

- `Development/` is disposable (logs, build dirs, release captures); never store source there and never sync it into packages.
- Fix in `Development/` → validate in the Arch VM → capture verified artifacts, `release-notes-<ver>.md` and `SHA256SUMS` under `Development/release-<ver>/` → stage changelog/release files in `Boot_Bitch-github` → publish per `docs/publishing.md` (`gh release`). Push to GitHub only after explicit approval; never from the VM.
- `docs/testing.md` and `docs/publishing.md` hold the full validated procedures.
