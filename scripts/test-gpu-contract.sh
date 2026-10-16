#!/usr/bin/env bash
# GPU driver stack (`gpu` diagnostic + capability + Debian/TUXEDO repair) helper
# contract.  Static wiring plus a behavioural harness that sources the helper
# without main and stubs only the read-only target probes and the
# package-manager/bootloader transcript plumbing.  No real block device, package
# manager, GRUB or DKMS is ever touched.
# shellcheck disable=SC1090,SC2034
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="${HELPER:-$ROOT_DIR/scripts/boot-repair-helper.sh}"

[[ -x "$HELPER" ]] || { echo 'FAIL: helper is not executable' >&2; exit 1; }
bash -n "$HELPER"

fail() { printf 'FAIL: %s\n' "$*" >&2; exit 1; }

# ---------------------------------------------------------------------------
# Static wiring contracts
# ---------------------------------------------------------------------------
for fn in diagnostic_gpu gpu_hardware_evidence gpu_loaded_module_evidence \
    gpu_installed_driver_packages gpu_expected_driver gpu_driver_mismatch_evidence \
    gpu_journal_correlation gpu_dkms_build_status \
    gpu_unavailable_reason gpu_collect_evidence \
    preflight_gpu adaptive_gpu_repair \
    repair_nomodeset repair_gpu_verify \
    gpu_wrong_driver_packages gpu_correct_driver_packages gpu_conflicting_module \
    gpu_oem_nvidia_flavour_detected gpu_install_backend gpu_oem_nvidia_package_set \
    gpu_apt_nvidia_driver_set gpu_package_set_unavailable_reason; do
    grep -q "^${fn}()" "$HELPER" || fail "helper is missing ${fn}()"
done

# The 15th capability key is registered and emits through the reasons array.
grep -q 'local -a keys=(validate filesystem dpkg fixbroken aptupdate upgrade dkms display gpu initramfs efi grub extlinux bootstack kernels)' "$HELPER" \
    || fail 'the keys array does not carry the gpu capability key'
grep -q 'reasons\[gpu\]="available"' "$HELPER" \
    || fail 'diagnostic_repair_capabilities does not gate gpu through the reasons array'
grep -qE 'gpu\)[[:space:]]+diagnostic_gpu' "$HELPER" || fail 'gpu diagnostic dispatch missing'
grep -q 'gpu) gpu_interactive_menu' "$HELPER" || fail 'repair dispatch is missing gpu'
grep -q '^gpu_interactive_menu()' "$HELPER" || fail 'helper is missing gpu_interactive_menu()'
grep -q '^helper_menu_prompt()' "$HELPER" || fail 'helper is missing helper_menu_prompt()'
grep -q 'nomodeset) repair_nomodeset' "$HELPER" || fail 'repair dispatch is missing nomodeset'
grep -q 'gpu) printf' "$HELPER" || fail 'stage_rank has no gpu rank'
# Stable evidence lines are emitted verbatim (never translated).
for prefix in 'GPU hardware note:' 'GPU kernel module loaded:' 'GPU kernel module blacklisted:' \
    'GPU installed driver packages:' 'GPU expected driver:' 'GPU OEM flavour:' \
    'GPU driver mismatch:' 'GPU DKMS build status:' 'GPU journal errors:'; do
    grep -q "printf '${prefix}" "$HELPER" || fail "gpu evidence line '${prefix}' missing"
done

# ---------------------------------------------------------------------------
# Behavioural harness
# ---------------------------------------------------------------------------
sandbox="$(mktemp -d)"
cleanup_sandbox() { rm -rf -- "$sandbox"; }
trap cleanup_sandbox EXIT

fake_root="$sandbox/root"
mkdir -p "$fake_root/etc/modprobe.d" "$fake_root/boot/grub" "$fake_root/usr/bin" \
    "$fake_root/var/lib/dpkg" "$fake_root/etc/apt"

source <(sed '/^main "\$@"/d' "$HELPER")
trap - EXIT INT TERM HUP

# Read-only target probes are stubbed so availability/evidence is decided by
# fixture globals only.
profile_target_backends() {
    TARGET_DISTRO_FAMILY=debian
    TARGET_PACKAGE_MANAGERS=(apt/dpkg)
    TARGET_PACKAGE_MANAGER=apt/dpkg
    TARGET_SERVICE_MANAGERS=(systemd)
    TARGET_SERVICE_MANAGER=systemd
    TARGET_DISPLAY_BACKEND=systemd
    TARGET_INITRAMFS_BACKENDS=(initramfs-tools)
    TARGET_INITRAMFS_BACKEND=initramfs-tools
    TARGET_LOGGING_BACKEND=journald
    TARGET_BOOTLOADER_BACKEND=grub
    TARGET_ESP_MOUNT=unresolved
    TARGET_KERNEL_LAYOUT='versioned vmlinuz-* kernels'
    TARGET_REPAIR_BACKEND='Debian/APT'
}
package_stage_backends() { printf '%s\n' apt/dpkg; }
is_alpine_family() { return 1; }
target_apt_sources_present() { return 0; }
target_dpkg_package_installed() { return 1; }
target_has_executable() { return 1; }
target_has_path() { return 1; }
target_dpkg_detected() { return 1; }
target_rpm_detected() { return 1; }
target_pacman_detected() { return 1; }
target_apk_detected() { return 1; }
target_logging_backend() { printf 'journald\n'; }
target_journal_evidence_present() { return 0; }
journal_cached_window_lines() { :; }
journal_boot_relevant_filter() { cat; }
non_journald_log_fallback_ready() { return 1; }
non_systemd_raw_log_lines() { :; }
syslog_non_user_filter() { cat; }
installed_kernel_versions() { printf '%s\n' 6.8.0-x 6.5.0-old; }
running_kernel_version() { printf '6.8.0-x\n'; }
is_tuxedo_uki_layout() { return 1; }
grub2_layout_detected() { return 1; }
grub_editenv_tool() { printf '/usr/bin/grub-editenv\n'; }
grub_env_path() { printf '/boot/grub/grubenv\n'; }
grub_env_block_valid() { return 0; }
apt_transaction_reported_no_changes() { return 1; }
apt_simulation_is_safe() { return 0; }
safe_apt_install_packages() { :; }
adaptive_dkms_repair() { :; }
adaptive_initramfs_repair() { :; }
adaptive_grub_stage() { :; }
adaptive_extlinux_repair() { :; }
run_package_stage() { :; }
kernel_inventory_had_removable_kernels() { return 1; }

TARGET_ROOT="$fake_root"
RUNNING_HOST_MODE=0
SESSION_DIR="$sandbox/session"
SESSION_LOG="$sandbox/session.log"
mkdir -p "$SESSION_DIR"
: > "$SESSION_LOG"

# Fix the evidence globals directly; gpu_collect_evidence is a no-op so the
# diagnostic/gate consume exactly the fixture state.
gpu_collect_evidence() {
    GPU_EVIDENCE_COLLECTED=1
    profile_target_backends
}
# The GPU gate consumes the shared boot classification; the harness pins
# BOOT_REACHED via the fixture globals so no journal is read here.
boot_failure_classification() { BOOT_FAILURE_CLASSIFIED=1; }

# ---- diagnostic emission (stable evidence lines, read-only) ----------------
GPU_HARDWARE=""
GPU_HARDWARE_NOTE="target-only"
GPU_HARDWARE_DRIVER_IN_USE=""
GPU_LOADED_MODULES="none"
GPU_BLACKLISTED_MODULES="none"
GPU_INSTALLED_PKGS="nvidia-driver@560.28.03, xserver-xorg-video-nouveau@1.0.17"
GPU_EXPECTED="nvidia"
GPU_EXPECTED_SOURCE="hardware-pciid"
GPU_OEM_FLAVOUR="tuxedo"
GPU_MISMATCH="nvidia vs nouveau"
GPU_MISMATCH_REASON="expected nvidia but the installed driver is nouveau"
GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=absent"
GPU_JOURNAL_ERRORS="7"
BOOT_REACHED="desktop"
BOOT_FAILURE_MARKERS=""
out="$(diagnostic_gpu)"
grep -q '^GPU hardware note: target-only$' <<<"$out" || fail 'GPU hardware note line wrong'
grep -q '^GPU kernel module loaded: none$' <<<"$out" || fail 'loaded-module line wrong'
grep -q '^GPU installed driver packages: nvidia-driver@560.28.03, xserver-xorg-video-nouveau@1.0.17$' <<<"$out" \
    || fail 'installed-driver-packages line wrong'
grep -q '^GPU expected driver: nvidia (source=hardware-pciid)$' <<<"$out" || fail 'expected-driver line wrong'
grep -q '^GPU OEM flavour: tuxedo$' <<<"$out" || fail 'OEM flavour line wrong'
grep -q '^GPU driver mismatch: nvidia vs nouveau (reason=expected nvidia but the installed driver is nouveau)$' <<<"$out" \
    || fail 'mismatch line wrong'
grep -q '^GPU DKMS build status: 6.8.0-x=built, 6.5.0-old=absent$' <<<"$out" || fail 'DKMS build status line wrong'
grep -q '^GPU journal errors: 7 actionable lines$' <<<"$out" || fail 'journal-errors line wrong'
# The diagnostic now surfaces the shared boot classification (the read-only
# "how far did this boot get" evidence), sourced from boot_failure_classification
# rather than a re-derivation; it no longer emits its own removed GPU boot
# reached: re-derivation.
grep -q '^Boot reached: desktop$' <<<"$out" \
    || fail 'diagnostic_gpu must emit the shared Boot reached: line'
grep -q '^Boot failure markers: none$' <<<"$out" \
    || fail 'diagnostic_gpu must emit Boot failure markers: none when clean'
if grep -q 'GPU boot reached:' <<<"$out"; then
    fail 'diagnostic_gpu must not emit a removed GPU boot reached: re-derivation'
fi
# The diagnostic never writes to the target (read-only invariant).
if [[ -n "$(find "$fake_root/etc/modprobe.d" -type f 2>/dev/null)" ]]; then
    fail 'diagnostic_gpu wrote a file into the target modprobe.d'
fi

# ---- primary/display emission (round-trip evidence, host scope) --------------
GPU_PRIMARY="10de:2b85 (10b0:f319)"
GPU_PRIMARY_DRIVER_IN_USE="nvidia"
GPU_DISPLAY_CONNECTED="connected"
out="$(diagnostic_gpu)"
grep -q '^GPU primary: 10de:2b85 (10b0:f319) \[driver-in-use=nvidia\]$' <<<"$out" \
    || fail 'diagnostic_gpu must emit GPU primary: <device> [driver-in-use=<drv>]'
grep -q '^GPU display: connected$' <<<"$out" \
    || fail 'diagnostic_gpu must emit GPU display: connected'
# An empty primary emits the none sentinel (no guessed primary).
GPU_PRIMARY=""
GPU_PRIMARY_DRIVER_IN_USE=""
out="$(diagnostic_gpu)"
grep -q '^GPU primary: none$' <<<"$out" \
    || fail 'diagnostic_gpu must emit GPU primary: none when no primary is determined'

# ---- target-mode primary/display (boot-log-derived, no host enumeration) -----
# The diagnostic emission must carry the primary evidence regardless of host
# mode: a repair target's primary comes from its own boot log, never the
# recovery host's /sys enumeration, and the round-trip must not depend on it.
RUNNING_HOST_MODE=0
GPU_PRIMARY="1002:13c0 (1462:7e59)"
GPU_PRIMARY_DRIVER_IN_USE="amdgpu"
GPU_DISPLAY_CONNECTED="not-connected"
out="$(diagnostic_gpu)"
grep -q '^GPU primary: 1002:13c0 (1462:7e59) \[driver-in-use=amdgpu\]$' <<<"$out" \
    || fail 'target-mode diagnostic must emit the boot-log-derived primary GPU'
grep -q '^GPU display: not-connected$' <<<"$out" \
    || fail 'target-mode diagnostic must emit GPU display: not-connected'
GPU_PRIMARY=""
GPU_PRIMARY_DRIVER_IN_USE=""
GPU_DISPLAY_CONNECTED=""

# ---- target-mode primary detection: display-attachment signals --------------
# A target journal with BOTH nvidia bound ("Enabling HDA controller") and amdgpu
# bound-but-no-display ("Cannot find any crtc or sizes"), and no PCI class-03
# line, must select nvidia as the primary (amdgpu is a no-display driver and is
# excluded).  The cached boot-head accessor is stubbed to return the fixture.
RUNNING_HOST_MODE=0
BOOT_REACHED="desktop"
journal_cached_boot_head_lines() {
    printf '%s\n' \
        'nvidia 0000:01:00.0: Enabling HDA controller' \
        'amdgpu 0000:79:00.0: [drm] Cannot find any crtc or sizes'
}
gpu_target_primary_from_boot_log
[[ "$GPU_PRIMARY" == "10de:0000 (0000:0000)" ]] \
    || fail "target-mode primary must select nvidia (10de:0000), not none; got '$GPU_PRIMARY'"
[[ "$GPU_PRIMARY_DRIVER_IN_USE" == nvidia ]] \
    || fail "target-mode primary driver-in-use must be nvidia; got '$GPU_PRIMARY_DRIVER_IN_USE'"
[[ "$GPU_DISPLAY_CONNECTED" == connected ]] \
    || fail "target-mode display must be connected for a desktop boot; got '$GPU_DISPLAY_CONNECTED'"
unset -f journal_cached_boot_head_lines
GPU_PRIMARY=""
GPU_PRIMARY_DRIVER_IN_USE=""
GPU_DISPLAY_CONNECTED=""
BOOT_REACHED=""

# ---- capability gate: available requires backend + derivable expected -------
# (a proven mismatch is no longer required: a healthy stack with a derivable
# driver is available for maintenance re-install/re-build).
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    : # available
else
    fail "gpu should be available with backend+mismatch+expected; got $reason"
fi

# ---- fail-closed reasons ----------------------------------------------------
# no-package-backend
package_stage_backends() { return 0; }
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    fail 'gpu must fail closed with no package backend'
else
    [[ "$reason" == reason:no-package-backend* ]] || fail "expected no-package-backend, got $reason"
fi
package_stage_backends() { printf '%s\n' apt/dpkg; }

# gpu-driver-unknown (hardware seen, no expected driver)
GPU_EXPECTED=""
GPU_HARDWARE_NOTE="target-only"
GPU_MISMATCH=""
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    fail 'gpu must fail closed with no expected driver'
else
    [[ "$reason" == reason:gpu-driver-unknown ]] || fail "expected gpu-driver-unknown, got $reason"
fi

# Healthy stack with a derivable expected driver is now AVAILABLE (the
# healthy-stack refusal was removed so maintenance re-install/re-build works).
GPU_EXPECTED="nvidia"
GPU_MISMATCH=""
BOOT_REACHED="desktop"
GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=built"
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    : # available
else
    fail "gpu must be available for a healthy stack with a derivable driver; got $reason"
fi

# A healthy stack with unknown DKMS build state (no module name) is still
# available for maintenance.
GPU_DKMS_BUILD_STATUS="none"
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    : # available
else
    fail "gpu must be available for a healthy stack (dkms status none); got $reason"
fi

# no-gpu-hardware
GPU_EXPECTED=""
GPU_HARDWARE_NOTE="none-detected"
GPU_LOADED_MODULES="none"
GPU_INSTALLED_PKGS="none"
GPU_JOURNAL_ERRORS="0"
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    fail 'gpu must fail closed with no GPU hardware'
else
    [[ "$reason" == reason:no-gpu-hardware ]] || fail "expected no-gpu-hardware, got $reason"
fi

# alpine-nvidia-unsupported
GPU_EXPECTED="nvidia"
GPU_HARDWARE_NOTE="recovery-host"
GPU_HARDWARE="10de:24b0 (10de:24b0)"
GPU_INSTALLED_PKGS="nvidia-driver@560.28.03"
GPU_JOURNAL_ERRORS="2"
GPU_MISMATCH="nvidia vs nvidia"
is_alpine_family() { return 0; }
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    fail 'gpu must fail closed on Alpine with an nvidia driver'
else
    [[ "$reason" == reason:alpine-nvidia-unsupported ]] || fail "expected alpine-nvidia-unsupported, got $reason"
fi
is_alpine_family() { return 1; }

# gpu-hybrid-ambiguous (multiple class-03 devices)
GPU_HARDWARE_DEVICES=("10de:24b0 (10de:24b0)" "8086:3e9b (1028:087d)")
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    fail 'gpu must fail closed on an ambiguous hybrid stack'
else
    [[ "$reason" == reason:gpu-hybrid-ambiguous ]] || fail "expected gpu-hybrid-ambiguous, got $reason"
fi
GPU_HARDWARE_DEVICES=("10de:24b0 (10de:24b0)")

# hybrid with a determinable primary (two devices, primary resolved) proceeds
# past the hybrid gate — it must never be refused as gpu-hybrid-ambiguous.
GPU_HARDWARE_DEVICES=("10de:2b85 (10b0:f319)" "1002:13c0 (1462:7e59)")
GPU_PRIMARY="10de:2b85 (10b0:f319)"
GPU_PRIMARY_DRIVER_IN_USE="nvidia"
GPU_EXPECTED="nvidia"
GPU_MISMATCH="nvidia vs nouveau"
GPU_HARDWARE_NOTE="recovery-host"
GPU_INSTALLED_PKGS="xserver-xorg-video-nouveau@1.0.17"
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    : # available: the primary resolved, so the hybrid gate must not refuse
else
    [[ "$reason" != reason:gpu-hybrid-ambiguous ]] \
        || fail 'a determinable primary GPU must not be refused as gpu-hybrid-ambiguous'
fi
GPU_PRIMARY=""
GPU_PRIMARY_DRIVER_IN_USE=""
GPU_HARDWARE_DEVICES=("10de:24b0 (10de:24b0)")

# nvidia-dkms-requires-network (offline proprietary fetch)
target_apt_sources_present() { return 1; }
GPU_EXPECTED="nvidia"
GPU_MISMATCH="nvidia vs nouveau"
GPU_INSTALLED_PKGS="xserver-xorg-video-nouveau@1.0.17"
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    fail 'gpu must fail closed when proprietary nvidia needs network'
else
    [[ "$reason" == reason:nvidia-dkms-requires-network ]] || fail "expected nvidia-dkms-requires-network, got $reason"
fi
target_apt_sources_present() { return 0; }

# ---- shared classifier routing: gpu gate never calls the removed re-derivation
# gpu_unavailable_reason must consume the shared BOOT_REACHED (via
# boot_failure_classification), never the deleted gpu_boot_reached.  Poison-stub
# the old function and assert it is never invoked.
GPU_POISON_LOG="$sandbox/gpu-boot-reached-poison.log"
: > "$GPU_POISON_LOG"
gpu_boot_reached() { printf 'CALLED\n' >> "$GPU_POISON_LOG"; }
GPU_EXPECTED="nvidia"
GPU_MISMATCH=""
BOOT_REACHED="desktop"
GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=built"
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    : # available: the healthy-stack refusal was removed
else
    fail "gpu must be available for a healthy stack (poison-stub run); got $reason"
fi
[[ ! -s "$GPU_POISON_LOG" ]] \
    || fail 'gpu_unavailable_reason must not call gpu_boot_reached (use the shared classifier)'
# A non-desktop BOOT_REACHED also keeps the stage available (the healthy-stack
# refusal no longer gates on boot-reached).
BOOT_REACHED="tty"
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    : # available: a tty-only boot is actionable evidence
else
    fail "gpu should stay available when BOOT_REACHED=tty (actionable boot evidence); got $reason"
fi
unset -f gpu_boot_reached
BOOT_REACHED=""

# ---- decision-tree primitives ----------------------------------------------
# Production reality: the main process profiles the backends before dispatching
# a stage (the earlier gate calls ran inside command substitutions, so their
# backend profile did not reach this shell).
profile_target_backends
GPU_EXPECTED="nvidia"
GPU_INSTALLED_PKGS="nvidia-driver@560.28.03, xserver-xorg-video-nouveau@1.0.17, mesa-vulkan-drivers@24.0.0"
wrong="$(gpu_wrong_driver_packages)"
grep -qx 'xserver-xorg-video-nouveau' <<<"$wrong" \
    || fail "purge set must name only the wrong driver; got '$wrong'"
if grep -q 'mesa-vulkan-drivers\|nvidia-driver' <<<"$wrong"; then
    fail 'purge set must never include mesa or the correct driver'
fi
[[ "$(gpu_conflicting_module)" == nouveau ]] || fail 'conflicting module for nvidia must be nouveau'
grep -q 'nvidia-driver nvidia-kernel-dkms nvidia-utils' < <(gpu_correct_driver_packages) \
    || fail 'correct driver package set for nvidia wrong'

# ---- blacklist write (fingerprint + changed) -------------------------------
REPAIR_STATUS_OUT="$(repair_gpu_blacklist_module)"
grep -q '^Repair change status gpu:blacklist-module: changed$' <<<"$REPAIR_STATUS_OUT" \
    || fail 'blacklist write must report changed'
[[ -f "$fake_root/etc/modprobe.d/blacklist-nouveau.conf" ]] \
    || fail 'blacklist file was not written'
grep -q 'blacklist nouveau' "$fake_root/etc/modprobe.d/blacklist-nouveau.conf" \
    || fail 'blacklist file content wrong'
# A second run is byte-identical -> unchanged.
REPAIR_STATUS_OUT="$(repair_gpu_blacklist_module)"
grep -q '^Repair change status gpu:blacklist-module: unchanged|reason:gpu-blacklist-identical$' <<<"$REPAIR_STATUS_OUT" \
    || fail 'a second blacklist write must report unchanged'

# ---- nomodeset one-boot fallback -------------------------------------------
# Production reality: run_repair/run_host_repair call profile_target_backends in
# the main process before dispatching the stage, so the bootloader backend is
# set here (the earlier gate calls ran inside command substitutions).
profile_target_backends
run_chroot_try() {
    CHROOT_TRY_RC=0
    CHROOT_TRY_OUTPUT=''
    # Mutate the grubenv so the after-fingerprint differs (simulating the write).
    printf '# GRUB Environment Block' > "$TARGET_ROOT/boot/grub/grubenv"
    dd if=/dev/zero bs=1 count=1 >> "$TARGET_ROOT/boot/grub/grubenv" 2>/dev/null || true
}
: > "$fake_root/boot/grub/grubenv"
printf '# GRUB Environment Block' > "$fake_root/boot/grub/grubenv"
head -c 1024 /dev/zero >> "$fake_root/boot/grub/grubenv" 2>/dev/null || true
NOMODESET_OUT="$(repair_nomodeset)"
grep -q '^Repair change status nomodeset: changed$' <<<"$NOMODESET_OUT" \
    || { echo "nomodeset output: [$NOMODESET_OUT] TBB=[$TARGET_BOOTLOADER_BACKEND]" >&2; fail 'nomodeset must report changed after writing next_entry'; }

# UKI-only layouts refuse nomodeset.
TARGET_BOOTLOADER_BACKEND='systemd-boot + UKI'
NOMODESET_OUT="$(repair_nomodeset)"
grep -q '^Repair change status nomodeset: unchanged|reason:nomodeset-uki-only$' <<<"$NOMODESET_OUT" \
    || fail 'nomodeset must refuse UKI-only layouts'

# ---- OEM/HWE nvidia flavour recognition + mismatch verdict ------------------
# A correct OEM/HWE nvidia driver (nvidia-driver-bin / nvidia-open-kernel-dkms /
# a -1tux version suffix) is the expected driver, and a correct driver whose
# DKMS module is absent is NOT a mismatch: it is a rebuild-DKMS situation.
GPU_EXPECTED=""
GPU_EXPECTED_SOURCE=""
GPU_HARDWARE=""
GPU_LOADED_MODULES="none"
GPU_INSTALLED_PKGS="nvidia-driver-bin@560.28.03-1tux1, nvidia-open-kernel-dkms@560.28.03-1tux1"
gpu_oem_nvidia_flavour_detected || fail 'the OEM/HWE nvidia flavour must be detected'
gpu_expected_driver
[[ "$GPU_EXPECTED" == nvidia ]] || fail "the OEM/HWE nvidia driver must be the expected driver; got $GPU_EXPECTED"
[[ "$GPU_EXPECTED_SOURCE" == oem-flavour ]] || fail "the OEM/HWE nvidia source must be oem-flavour; got $GPU_EXPECTED_SOURCE"
GPU_DKMS_BUILD_STATUS="6.8.0-x=absent"
gpu_driver_mismatch_evidence
[[ -z "$GPU_MISMATCH" ]] || fail "a correct driver with an absent DKMS module must not be a mismatch; got $GPU_MISMATCH"
# A wrong driver (nouveau installed) is still a mismatch.
GPU_INSTALLED_PKGS="xserver-xorg-video-nouveau@1.0.17"
gpu_driver_mismatch_evidence
[[ "$GPU_MISMATCH" == "nvidia vs nouveau" ]] || fail "nouveau installed must be a mismatch; got $GPU_MISMATCH"

# ---- free-DDX/mesa presence is never a driver guess --------------------------
# A target-mode generic all-drivers set (amdgpu+nouveau+radeon installed
# together by xserver-xorg-video-all) must NOT derive a discrete driver: with no
# hardware anchor and no nvidia evidence the expected driver stays empty and no
# mismatch is produced.
GPU_HARDWARE_DEVICES=()
GPU_HARDWARE_DRIVERS=()
GPU_HARDWARE=""
GPU_HARDWARE_DRIVER_IN_USE=""
GPU_PRIMARY=""
GPU_PRIMARY_DRIVER_IN_USE=""
GPU_HARDWARE_NOTE="target-only"
GPU_LOADED_MODULES="none"
GPU_INSTALLED_PKGS="xserver-xorg-video-amdgpu@23.0.0-1,xserver-xorg-video-nouveau@1.0.17-1,xserver-xorg-video-radeon@19.1.0-1"
GPU_EXPECTED=""
GPU_EXPECTED_SOURCE=""
gpu_expected_driver
[[ -z "$GPU_EXPECTED" ]] \
    || fail "the generic all-drivers free-DDX set must not derive a driver; got $GPU_EXPECTED (source=$GPU_EXPECTED_SOURCE)"
gpu_driver_mismatch_evidence
[[ -z "$GPU_MISMATCH" ]] || fail "no expected driver must produce no mismatch; got $GPU_MISMATCH"

# ---- virtual GPU fails closed -------------------------------------------------
# Host-mode virtio GPU: no hardware anchor maps to a driver and no nvidia-family
# package is present, so the expected driver stays empty, there is no mismatch,
# DKMS is none, and the gate fails closed with virtual-gpu-no-discrete-driver.
GPU_HARDWARE_DEVICES=("1af4:1050 (1af4:1100)")
GPU_HARDWARE_DRIVERS=("virtio_gpu")
GPU_HARDWARE="1af4:1050 (1af4:1100)"
GPU_HARDWARE_DRIVER_IN_USE="virtio_gpu"
GPU_PRIMARY="1af4:1050 (1af4:1100)"
GPU_PRIMARY_DRIVER_IN_USE="virtio_gpu"
GPU_HARDWARE_NOTE="virtual-gpu"
GPU_LOADED_MODULES="none"
GPU_JOURNAL_ERRORS="0"
GPU_INSTALLED_PKGS="none"
GPU_EXPECTED=""
GPU_EXPECTED_SOURCE=""
gpu_expected_driver
[[ -z "$GPU_EXPECTED" ]] \
    || fail "a virtual GPU with no nvidia must leave GPU_EXPECTED empty; got $GPU_EXPECTED (source=$GPU_EXPECTED_SOURCE)"
gpu_driver_mismatch_evidence
[[ -z "$GPU_MISMATCH" ]] || fail "a virtual GPU must produce no mismatch; got $GPU_MISMATCH"
gpu_dkms_build_status
[[ "$GPU_DKMS_BUILD_STATUS" == none ]] \
    || fail "a virtual GPU with no expected driver must report DKMS none; got $GPU_DKMS_BUILD_STATUS"
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    fail 'gpu must fail closed for a virtual GPU with no nvidia driver'
else
    [[ "$reason" == reason:virtual-gpu-no-discrete-driver ]] \
        || fail "expected virtual-gpu-no-discrete-driver, got $reason"
fi

# ---- amdgpu + co-present nouveau is never a mismatch -------------------------
# nouveau is legitimately co-present via the all-drivers set on non-NVIDIA
# machines; an amdgpu hardware-PCI-ID stack with nouveau installed must not be
# reported as a wrong driver.
GPU_EXPECTED="amdgpu"
GPU_HARDWARE_DEVICES=("1002:13c0 (1462:7e59)")
GPU_HARDWARE="1002:13c0 (1462:7e59)"
GPU_PRIMARY="1002:13c0 (1462:7e59)"
GPU_INSTALLED_PKGS="xserver-xorg-video-amdgpu@23.0.0-1,xserver-xorg-video-nouveau@1.0.17-1"
gpu_driver_mismatch_evidence
[[ -z "$GPU_MISMATCH" ]] \
    || fail "amdgpu + co-present nouveau must not be a mismatch; got $GPU_MISMATCH"

# ---- amdgpu + nvidia: mismatch only when no nvidia hardware is present --------
# Without nvidia hardware, an installed nvidia driver is a wrong driver.
GPU_EXPECTED="amdgpu"
GPU_HARDWARE_DEVICES=("1002:13c0 (1462:7e59)")
GPU_HARDWARE="1002:13c0 (1462:7e59)"
GPU_PRIMARY="1002:13c0 (1462:7e59)"
GPU_INSTALLED_PKGS="xserver-xorg-video-amdgpu@23.0.0-1,nvidia-driver@560.28.03"
gpu_driver_mismatch_evidence
[[ "$GPU_MISMATCH" == "amdgpu vs nvidia" ]] \
    || fail "amdgpu + nvidia with no nvidia hardware must mismatch; got $GPU_MISMATCH"
# A hybrid nvidia+amd system (nvidia hardware present) legitimately installs
# both nvidia and amdgpu: not a mismatch.
GPU_HARDWARE_DEVICES=("1002:13c0 (1462:7e59)" "10de:2b85 (10b0:f319)")
GPU_HARDWARE="1002:13c0 (1462:7e59)"
GPU_PRIMARY="1002:13c0 (1462:7e59)"
gpu_driver_mismatch_evidence
[[ -z "$GPU_MISMATCH" ]] \
    || fail "a hybrid nvidia+amd system must not be a mismatch; got $GPU_MISMATCH"

# ---- DKMS build status: in-kernel drivers report none ------------------------
# amdgpu/radeon/i915/nouveau are in-kernel and have no DKMS module: the build
# status must be none (never a spurious absent), so no DKMS rebuild or
# broken-kernel case-(c) is triggered.
for drv in amdgpu radeon i915 nouveau; do
    GPU_EXPECTED="$drv"
    gpu_dkms_build_status
    [[ "$GPU_DKMS_BUILD_STATUS" == none ]] \
        || fail "in-kernel driver $drv must report DKMS build status none; got $GPU_DKMS_BUILD_STATUS"
done

# ---- DKMS module detection: TUXEDO's nvidia-current-open is "built" ---------
# The DKMS build status must detect the actually-built module name (TUXEDO
# ships nvidia-current-open*, not a bare `nvidia.ko`), so a successfully
# rebuilt driver reports `built` for that kernel instead of `absent`.
GPU_EXPECTED="nvidia"
mkdir -p "$fake_root/usr/lib/modules/6.8.0-x/updates/dkms"
touch "$fake_root/usr/lib/modules/6.8.0-x/updates/dkms/nvidia-current-open.ko"
gpu_dkms_build_status
[[ "$GPU_DKMS_BUILD_STATUS" == "6.8.0-x=built, 6.5.0-old=absent" ]] \
    || fail "the TUXEDO nvidia-current-open module must report built (got: $GPU_DKMS_BUILD_STATUS)"

# ---- DKMS build status: a module-less driver (mesa) reports none -------------
# mesa has no kernel module, so gpu_expected_module_name is empty.  The
# per-kernel loop must not mark every installed kernel absent (which would
# wrongly offer a DKMS rebuild / kernel replace / remove in the GPU menu); the
# build status is none so the menu offers the maintenance branch, never the
# replace-kernel / remove-older-kernel branch.
GPU_EXPECTED="mesa"
GPU_MISMATCH=""
gpu_dkms_build_status
[[ "$GPU_DKMS_BUILD_STATUS" == "none" ]] \
    || fail "a module-less driver (mesa) must report DKMS build status none (got: $GPU_DKMS_BUILD_STATUS)"
# The generic mesa fallback has no concrete package set: the gate must fail
# closed (gpu-driver-unknown), never report available.
GPU_HARDWARE_NOTE="recovery-host"
GPU_INSTALLED_PKGS="mesa-vulkan-drivers@26.1.6-1"
GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    fail 'gpu must fail closed for the generic mesa fallback'
else
    [[ "$reason" == reason:gpu-driver-unknown ]] || fail "expected gpu-driver-unknown for mesa, got $reason"
fi
# The mesa menu offers maintenance actions but never the kernel replace/remove
# actions that only a failed/absent DKMS build would surface.
BOOT_REPAIR_SESSION_PROTOCOL=1
BOOT_REPAIR_SESSION_REQUEST=7
MESA_PROMPT_FILE="$sandbox/mesa-prompt.txt"
MESA_PROMPT_COUNT_FILE="$sandbox/mesa-prompt-count.txt"
: > "$MESA_PROMPT_FILE"
printf '0' > "$MESA_PROMPT_COUNT_FILE"
helper_menu_prompt() {
    local n=0
    n="$(cat "$MESA_PROMPT_COUNT_FILE" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$MESA_PROMPT_COUNT_FILE"
    printf '%s\n' "$1" >> "$MESA_PROMPT_FILE"
    HELPER_MENU_ANSWER="0"   # abort after the single maintenance prompt
    return 0
}
status_line="$(gpu_interactive_menu 2>/dev/null || true)"
[[ "$(cat "$MESA_PROMPT_COUNT_FILE")" -eq 1 ]] \
    || fail 'a module-less (mesa) stack must offer the maintenance menu exactly once'
grep -q 'Re-install the correct driver' "$MESA_PROMPT_FILE" \
    || fail 'the mesa maintenance menu must offer re-install'
if grep -q 'Replace the newest kernel\|Remove the older kernel' "$MESA_PROMPT_FILE"; then
    fail 'a module-less (mesa) stack must never offer kernel replace/remove'
fi
BOOT_REPAIR_SESSION_PROTOCOL=0
GPU_EXPECTED="nvidia"

# ---- interactive menu: healthy stack offers maintenance actions --------------
# A consistent graphics stack (no mismatch and no failed/absent DKMS build) with
# a derivable expected driver now offers maintenance actions (re-install /
# re-build) instead of returning immediately.  Prompts are recorded to files
# because the menu runs inside a command substitution (a subshell), so a counter
# variable would not survive back here.
GPU_EXPECTED="nvidia"
GPU_MISMATCH=""
GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=built"
BOOT_REPAIR_SESSION_PROTOCOL=1
BOOT_REPAIR_SESSION_REQUEST=7
GPU_PROMPT_FILE="$sandbox/gpu-prompt.txt"
GPU_PROMPT_COUNT_FILE="$sandbox/gpu-prompt-count.txt"
: > "$GPU_PROMPT_FILE"
printf '0' > "$GPU_PROMPT_COUNT_FILE"
helper_menu_prompt() {
    local n=0
    n="$(cat "$GPU_PROMPT_COUNT_FILE" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$GPU_PROMPT_COUNT_FILE"
    printf '%s\n' "$1" >> "$GPU_PROMPT_FILE"
    HELPER_MENU_ANSWER="0"
    return 0
}
status_line="$(gpu_interactive_menu 2>/dev/null || true)"
[[ "$(cat "$GPU_PROMPT_COUNT_FILE")" -eq 1 ]] \
    || fail 'a healthy GPU stack with a derivable driver must offer the maintenance menu once'
grep -q 'Re-install the correct driver' "$GPU_PROMPT_FILE" \
    || fail 'the maintenance menu must offer re-install'
grep -q 'Re-build the driver module with DKMS' "$GPU_PROMPT_FILE" \
    || fail 'the maintenance menu must offer a DKMS rebuild'
grep -q 'Re-build the initramfs and bootloader' "$GPU_PROMPT_FILE" \
    || fail 'the maintenance menu must offer an initramfs/bootloader rebuild'

# ---- interactive menu: no expected driver reports verified without prompting --
# With no derivable expected driver there is nothing to re-install or re-build:
# the menu reports the read-only verified result and returns without prompting.
GPU_EXPECTED=""
GPU_MISMATCH=""
GPU_DKMS_BUILD_STATUS="none"
: > "$GPU_PROMPT_FILE"
printf '0' > "$GPU_PROMPT_COUNT_FILE"
helper_menu_prompt() {
    local n=0
    n="$(cat "$GPU_PROMPT_COUNT_FILE" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$GPU_PROMPT_COUNT_FILE"
    printf '%s\n' "$1" >> "$GPU_PROMPT_FILE"
    HELPER_MENU_ANSWER=""
    return 1   # no expected driver must never prompt; abort the loop if it does
}
status_line="$(gpu_interactive_menu 2>/dev/null || true)"
[[ "$(cat "$GPU_PROMPT_COUNT_FILE")" -eq 0 ]] \
    || fail 'a stack with no expected driver must report verified without prompting'
grep -q '^Repair change status gpu: unchanged|reason:gpu-verified$' <<<"$status_line" \
    || fail 'the no-expected-driver stack must report gpu-verified'

# ---- interactive menu: iterative loop re-collects and adapts ----------------
# A mismatched stack offers the driver swap; running it (simulated) leaves a
# consistent stack, so the next iteration reports gpu-verified without prompting
# a second time.
GPU_EXPECTED="nvidia"
GPU_MISMATCH="nvidia vs nouveau"
GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=absent"
: > "$GPU_PROMPT_FILE"
printf '0' > "$GPU_PROMPT_COUNT_FILE"
GPU_MENU_ANSWER="1"
repair_gpu_driver_swap() {
    GPU_MISMATCH=""
    GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=built"
}
helper_menu_prompt() {
    local n=0
    n="$(cat "$GPU_PROMPT_COUNT_FILE" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$GPU_PROMPT_COUNT_FILE"
    printf '%s\n' "$1" >> "$GPU_PROMPT_FILE"
    HELPER_MENU_ANSWER="$GPU_MENU_ANSWER"
    return 0
}
status_line="$(gpu_interactive_menu 2>/dev/null || true)"
[[ "$(cat "$GPU_PROMPT_COUNT_FILE")" -eq 1 ]] \
    || fail 'the mismatched GPU stack must prompt once for the driver swap'
grep -q 'Purge wrong driver, install correct, rebuild all' "$GPU_PROMPT_FILE" \
    || fail 'the mismatched GPU menu must offer the driver swap'
grep -q '^Found:$' "$GPU_PROMPT_FILE" \
    || fail 'the GPU menu must begin with a Found: evidence summary'
grep -q 'driver mismatch: nvidia vs nouveau' "$GPU_PROMPT_FILE" \
    || fail 'the GPU menu summary must name the driver mismatch'
grep -q 'DKMS build status: 6.8.0-x=built, 6.5.0-old=absent' "$GPU_PROMPT_FILE" \
    || fail 'the GPU menu summary must name the DKMS build status'
grep -q '0) Abort' "$GPU_PROMPT_FILE" \
    || fail 'the GPU menu must use the 0) Abort abort line'
if grep -q 'Choice:' "$GPU_PROMPT_FILE"; then
    fail 'the GPU menu must not append a Choice: prompt suffix'
fi
grep -q '^Repair change status gpu: unchanged|reason:gpu-verified$' <<<"$status_line" \
    || fail 'the GPU swap must terminate by reporting gpu-verified'

# ---- interactive menu: 0 aborts and reports the cancelled status -------------
GPU_EXPECTED="nvidia"
GPU_MISMATCH="nvidia vs nouveau"
GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=absent"
: > "$GPU_PROMPT_FILE"
printf '0' > "$GPU_PROMPT_COUNT_FILE"
GPU_MENU_ANSWER="0"
helper_menu_prompt() {
    local n=0
    n="$(cat "$GPU_PROMPT_COUNT_FILE" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$GPU_PROMPT_COUNT_FILE"
    printf '%s\n' "$1" >> "$GPU_PROMPT_FILE"
    HELPER_MENU_ANSWER="$GPU_MENU_ANSWER"
    return 0
}
status_line="$(gpu_interactive_menu 2>/dev/null || true)"
[[ "$(cat "$GPU_PROMPT_COUNT_FILE")" -eq 1 ]] \
    || fail 'answering 0 must abort after a single prompt'
grep -q '^Repair change status gpu: unchanged|reason:gpu-aborted$' <<<"$status_line" \
    || fail 'answering 0 (Abort) must run no action and report gpu-aborted (unchanged)'

# ---- interactive menu: nomodeset is non-terminal (loop re-presents) ----------
# Choosing nomodeset does not repair the driver, so the loop must re-collect and
# re-present the (still mismatched) menu rather than terminating.
GPU_EXPECTED="nvidia"
GPU_MISMATCH="nvidia vs nouveau"
GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=absent"
: > "$GPU_PROMPT_FILE"
printf '0' > "$GPU_PROMPT_COUNT_FILE"
helper_menu_prompt() {
    local n=0
    n="$(cat "$GPU_PROMPT_COUNT_FILE" 2>/dev/null || true)"
    n=$((n + 1))
    printf '%s' "$n" > "$GPU_PROMPT_COUNT_FILE"
    printf '%s\n' "$1" >> "$GPU_PROMPT_FILE"
    if (( n >= 2 )); then
        HELPER_MENU_ANSWER="0"   # abort on the re-prompt
    else
        HELPER_MENU_ANSWER="2"   # nomodeset (non-terminal)
    fi
    return 0
}
status_line="$(gpu_interactive_menu 2>/dev/null || true)"
[[ "$(cat "$GPU_PROMPT_COUNT_FILE")" -eq 2 ]] \
    || fail 'nomodeset is non-terminal: the GPU menu must re-present afterwards'
grep -q 'Purge wrong driver, install correct, rebuild all' "$GPU_PROMPT_FILE" \
    || fail 'the re-presented menu must still offer the driver swap after nomodeset'
BOOT_REPAIR_SESSION_PROTOCOL=0

# ---- backend-aware correct-driver package selection -------------------------
# The correct driver set is selected from the detected package backend
# (TARGET_PACKAGE_MANAGERS) and the OEM flavour, never from a hardcoded Debian
# set; when no concrete set can be derived it fails closed with a named reason.

# Fedora/rpm
TARGET_PACKAGE_MANAGERS=(rpm)
GPU_EXPECTED="nvidia"
GPU_INSTALLED_PKGS="xorg-x11-drv-nouveau@1.0.17"
grep -q 'akmod-nvidia xorg-x11-drv-nvidia' < <(gpu_correct_driver_packages) \
    || fail 'Fedora nvidia correct set must be akmod-nvidia xorg-x11-drv-nvidia'
GPU_EXPECTED="amdgpu"
grep -q 'xorg-x11-drv-amdgpu mesa-dri-drivers' < <(gpu_correct_driver_packages) \
    || fail 'Fedora amdgpu correct set must be xorg-x11-drv-amdgpu mesa-dri-drivers'

# Arch/pacman
TARGET_PACKAGE_MANAGERS=(pacman)
GPU_EXPECTED="nvidia"
grep -q '^nvidia nvidia-utils$' < <(gpu_correct_driver_packages) \
    || fail 'Arch nvidia correct set must be nvidia nvidia-utils'
GPU_EXPECTED="amdgpu"
grep -q 'xf86-video-amdgpu mesa' < <(gpu_correct_driver_packages) \
    || fail 'Arch amdgpu correct set must be xf86-video-amdgpu mesa'

# Alpine/apk: no proprietary NVIDIA DKMS in main -> fail closed with a reason.
TARGET_PACKAGE_MANAGERS=(apk)
GPU_EXPECTED="nvidia"
GPU_INSTALLED_PKGS="none"
out="$(gpu_correct_driver_packages)"
[[ -z "$out" ]] || fail 'Alpine nvidia correct set must fail closed (empty)'
[[ "$(gpu_package_set_unavailable_reason)" == alpine-nvidia-unsupported ]] \
    || fail "Alpine nvidia must fail closed with alpine-nvidia-unsupported; got $(gpu_package_set_unavailable_reason)"
GPU_EXPECTED="amdgpu"
grep -q 'xf86-video-amdgpu mesa-dri-gallium' < <(gpu_correct_driver_packages) \
    || fail 'Alpine amdgpu correct set must be xf86-video-amdgpu mesa-dri-gallium'

# OEM/HWE nvidia flavour (apt/dpkg): reuse the proven installed OEM set.
TARGET_PACKAGE_MANAGERS=(apt/dpkg)
GPU_EXPECTED="nvidia"
GPU_INSTALLED_PKGS="nvidia-driver-bin@560.28.03-1tux1, nvidia-open-kernel-dkms@560.28.03-1tux1, nvidia-kernel-common@1"
out="$(gpu_correct_driver_packages)"
grep -qx 'nvidia-driver-bin' <<<"$out" || fail 'OEM set must reuse nvidia-driver-bin'
grep -qx 'nvidia-open-kernel-dkms' <<<"$out" || fail 'OEM set must reuse nvidia-open-kernel-dkms'
grep -qx 'nvidia-kernel-common' <<<"$out" || fail 'OEM set must reuse nvidia-kernel-common'

# No package backend: fail closed with a named reason.
TARGET_PACKAGE_MANAGERS=()
GPU_EXPECTED="nvidia"
GPU_INSTALLED_PKGS="none"
out="$(gpu_correct_driver_packages)"
[[ -z "$out" ]] || fail 'no package backend must fail closed (empty)'
[[ "$(gpu_package_set_unavailable_reason)" == no-package-backend ]] \
    || fail "no package backend must fail closed with no-package-backend; got $(gpu_package_set_unavailable_reason)"

# ---- Fedora nvidia (rpm/dnf5) maintenance path -----------------------------
# A Fedora nvidia target (akmod-nvidia + xorg-x11-drv-nvidia installed) must
# resolve the expected driver from the rpm package names (source
# backend-recommends), open the capability gate (never the apt/dpkg-only
# nvidia-dkms-requires-network refusal), and surface the rpm driver packages in
# the installed-driver summary.
GPU_HARDWARE=""
GPU_HARDWARE_NOTE="recovery-host"
GPU_HARDWARE_DEVICES=("10de:24b0 (10de:24b0)")
GPU_PRIMARY=""
GPU_PRIMARY_DRIVER_IN_USE=""
GPU_LOADED_MODULES="none"
GPU_JOURNAL_ERRORS="0"
GPU_MISMATCH=""
GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=built"
GPU_INSTALLED_PKGS="akmod-nvidia@580.88.02-1.fc44,xorg-x11-drv-nvidia@580.88.02-1.fc44"
GPU_EXPECTED=""
GPU_EXPECTED_SOURCE=""
TARGET_PACKAGE_MANAGERS=(rpm)
gpu_install_backend() { printf 'rpm\n'; return 0; }
target_apt_sources_present() { return 1; }
is_alpine_family() { return 1; }

gpu_expected_driver
[[ "$GPU_EXPECTED" == nvidia ]] \
    || fail "Fedora nvidia (akmod-nvidia/xorg-x11-drv-nvidia) must resolve GPU_EXPECTED=nvidia; got $GPU_EXPECTED"
[[ "$GPU_EXPECTED_SOURCE" == backend-recommends ]] \
    || fail "Fedora nvidia source must be backend-recommends; got $GPU_EXPECTED_SOURCE"

GPU_EVIDENCE_COLLECTED=0
if reason="$(gpu_unavailable_reason)"; then
    : # available
else
    [[ "$reason" != reason:nvidia-dkms-requires-network ]] \
        || fail 'Fedora nvidia must not hit the apt/dpkg-only nvidia-dkms-requires-network refusal'
    fail "Fedora nvidia must be available; got $reason"
fi

summary="$(gpu_installed_driver_summary)"
grep -q 'akmod-nvidia@580.88.02-1.fc44' <<<"$summary" \
    || fail "Fedora nvidia summary must include akmod-nvidia; got '$summary'"
grep -q 'xorg-x11-drv-nvidia@580.88.02-1.fc44' <<<"$summary" \
    || fail "Fedora nvidia summary must include xorg-x11-drv-nvidia; got '$summary'"

# ---- Arch nvidia (pacman) bare-package detection ---------------------------
# A bare `nvidia` package (Arch pacman, no -driver/-dkms/-open suffix) must
# resolve the expected driver through the nvidia package family (source
# backend-recommends), and surface the bare `nvidia` token in the installed
# driver summary.
GPU_HARDWARE=""
GPU_HARDWARE_DEVICES=()
GPU_PRIMARY=""
GPU_PRIMARY_DRIVER_IN_USE=""
GPU_LOADED_MODULES="none"
GPU_JOURNAL_ERRORS="0"
GPU_MISMATCH=""
GPU_DKMS_BUILD_STATUS="6.8.0-x=built, 6.5.0-old=built"
GPU_INSTALLED_PKGS="nvidia@560.28.03-1"
GPU_EXPECTED=""
GPU_EXPECTED_SOURCE=""
TARGET_PACKAGE_MANAGERS=(pacman)
gpu_install_backend() { printf 'pacman\n'; return 0; }
target_apt_sources_present() { return 1; }
is_alpine_family() { return 1; }

gpu_expected_driver
[[ "$GPU_EXPECTED" == nvidia ]] \
    || fail "Arch nvidia (bare nvidia package) must resolve GPU_EXPECTED=nvidia; got $GPU_EXPECTED"
[[ "$GPU_EXPECTED_SOURCE" == backend-recommends ]] \
    || fail "Arch nvidia source must be backend-recommends; got $GPU_EXPECTED_SOURCE"

summary="$(gpu_installed_driver_summary)"
grep -q 'nvidia@560.28.03-1' <<<"$summary" \
    || fail "Arch nvidia summary must include the bare nvidia token; got '$summary'"

# ---- follow-on steps: rebuild-dkms rebuilds initramfs after DKMS -----------
# repair_gpu_rebuild_dkms rebuilds the driver module via DKMS and must then
# rebuild the initramfs so the new module lands in the initrd.  A module
# rebuild does not change the GRUB config, so it must not over-add grub.
# Calls are recorded to files because the sub-actions run inside command
# substitutions (a subshell), so a counter variable would not survive back here.
profile_target_backends
GPU_FOLLOWON_LOG="$sandbox/gpu-followon.log"
: > "$GPU_FOLLOWON_LOG"
adaptive_dkms_repair() { printf 'dkms\n' >> "$GPU_FOLLOWON_LOG"; }
adaptive_initramfs_repair() { printf 'initramfs\n' >> "$GPU_FOLLOWON_LOG"; }
adaptive_grub_stage() { printf 'grub\n' >> "$GPU_FOLLOWON_LOG"; }
adaptive_extlinux_repair() { printf 'extlinux\n' >> "$GPU_FOLLOWON_LOG"; }
REPAIR_STATUS_OUT="$(repair_gpu_rebuild_dkms)"
grep -q '^Repair change status gpu:rebuild-dkms: changed$' <<<"$REPAIR_STATUS_OUT" \
    || fail 'rebuild-dkms must report changed'
[[ "$(cat "$GPU_FOLLOWON_LOG")" == $'dkms\ninitramfs' ]] \
    || fail "rebuild-dkms follow-on order wrong: $(tr '\n' ',' < "$GPU_FOLLOWON_LOG")"

# ---- follow-on steps: replace-kernel rebuilds initramfs + bootloader --------
# repair_gpu_replace_kernel reinstalls the managed OEM/HWE kernel/headers, then
# rebuilds DKMS, and must then rebuild the initramfs + bootloader config
# (repair_gpu_rebuild_boot) so the reinstalled kernel's initrd and menu are
# regenerated.
: > "$GPU_FOLLOWON_LOG"
kernel_inventory_collect() { :; }
apt_managed_kernel_metapackage() { printf 'linux-image-tuxedo\n'; }
safe_apt_install_packages() { printf 'reinstall\n' >> "$GPU_FOLLOWON_LOG"; }
kernel_inventory_recollect_after_install() { printf 'recollect\n' >> "$GPU_FOLLOWON_LOG"; }
REPAIR_STATUS_OUT="$(repair_gpu_replace_kernel)"
grep -q '^Repair change status gpu:replace-kernel: changed$' <<<"$REPAIR_STATUS_OUT" \
    || fail 'replace-kernel must report changed'
[[ "$(cat "$GPU_FOLLOWON_LOG")" == $'reinstall\nrecollect\ndkms\ninitramfs\ngrub' ]] \
    || fail "replace-kernel follow-on order wrong: $(tr '\n' ',' < "$GPU_FOLLOWON_LOG")"

# ---- follow-on steps: remove-older-kernel recollects before the regen -------
# repair_gpu_remove_older_kernel removes the superseded kernel(s) via the guarded
# kernel-cleanup stage, and must re-collect the post-removal inventory before
# the initramfs + bootloader regen so the regen never sees the just-removed
# kernel from the pre-removal baseline.
: > "$GPU_FOLLOWON_LOG"
repair_kernel_prepare_inventory() {
    KERNEL_REMOVABLE=(6.5.0-old)
    KERNEL_RUNNING=6.8.0-x
    KERNEL_LATEST=6.8.0-x
}
kernel_inventory_had_removable_kernels() { return 0; }
kernel_inventory_recollect_after_removal() { printf 'recollect\n' >> "$GPU_FOLLOWON_LOG"; }
REPAIR_STATUS_OUT="$(repair_gpu_remove_older_kernel)"
grep -q '^Repair change status gpu:remove-older-kernel: changed$' <<<"$REPAIR_STATUS_OUT" \
    || fail 'remove-older-kernel must report changed'
[[ "$(cat "$GPU_FOLLOWON_LOG")" == $'recollect\ninitramfs\ngrub' ]] \
    || fail "remove-older-kernel follow-on order wrong: $(tr '\n' ',' < "$GPU_FOLLOWON_LOG")"

# ---- GPU purge: apt/dpkg path (unchanged body) -----------------------------
# The apt/dpkg arm of repair_gpu_purge_wrong_driver is the original
# simulation-first apt-get remove --purge body: it must still simulate through
# run_selected_chroot, then apply through run_chroot_try_display_bounded.
touch "$fake_root/usr/bin/apt-get"
chmod +x "$fake_root/usr/bin/apt-get"
gpu_install_backend() { printf 'apt/dpkg\n'; return 0; }
gpu_backup_state() { :; }
gpu_restore_state() { :; }
apt_apply_log="$sandbox/gpu-apt-apply.log"
: > "$apt_apply_log"
run_selected_chroot() {
    printf '%s\n' \
        'Reading package lists...' \
        'Remv xserver-xorg-video-nouveau [1.0.17]' \
        '0 upgraded, 0 newly installed, 1 to remove and 0 not upgraded.'
    return 0
}
run_chroot_try_display_bounded() {
    local label="$1"; shift
    printf 'apply:%s\n' "$*" >> "$apt_apply_log"
    CHROOT_TRY_RC=0
    CHROOT_TRY_OUTPUT=''
    return 0
}
GPU_EXPECTED="nvidia"
GPU_INSTALLED_PKGS="nvidia-driver@560.28.03, xserver-xorg-video-nouveau@1.0.17"
REPAIR_STATUS_OUT="$(repair_gpu_purge_wrong_driver)"
grep -q '^Repair change status gpu:purge-wrong-driver: changed$' <<<"$REPAIR_STATUS_OUT" \
    || fail "apt/dpkg GPU purge must report changed: $REPAIR_STATUS_OUT"
grep -q 'apply:apt-get -y remove --purge xserver-xorg-video-nouveau' "$apt_apply_log" \
    || fail "apt/dpkg GPU purge must apply apt-get remove --purge: $(tr '\n' ',' < "$apt_apply_log")"

# ---- GPU purge: pacman path (backend-aware) --------------------------------
# On a pacman target the purge must route through the guarded pacman removal
# (sandboxed simulate-then-apply), never the apt-get body.
gpu_install_backend() { printf 'pacman\n'; return 0; }
arch_pacman_prepare_sandbox() { :; }
pacman_apply_log="$sandbox/gpu-pacman-apply.log"
: > "$pacman_apply_log"
arch_pacman_transaction_try() {
    local label="$1"; shift
    printf 'simulate:%s\n' "$*" >> "$pacman_apply_log"
    ARCH_PACMAN_TRY_OUTPUT='Packages (1) xf86-video-nouveau-1.0.17-1'
    ARCH_PACMAN_TRY_RC=0
    return 0
}
run_chroot_try_display() {
    local label="$1"; shift
    printf 'apply:%s\n' "$*" >> "$pacman_apply_log"
    CHROOT_TRY_RC=0
    CHROOT_TRY_OUTPUT=''
    return 0
}
GPU_EXPECTED="nvidia"
GPU_INSTALLED_PKGS="xf86-video-nouveau@1.0.17-1"
REPAIR_STATUS_OUT="$(repair_gpu_purge_wrong_driver)"
grep -q '^Repair change status gpu:purge-wrong-driver: changed$' <<<"$REPAIR_STATUS_OUT" \
    || fail "pacman GPU purge must report changed: $REPAIR_STATUS_OUT"
grep -q 'simulate:.*-R --print xf86-video-nouveau' "$pacman_apply_log" \
    || fail "pacman GPU purge must simulate pacman -R --print first: $(tr '\n' ',' < "$pacman_apply_log")"
grep -q 'apply:pacman --noconfirm -R xf86-video-nouveau' "$pacman_apply_log" \
    || fail "pacman GPU purge must apply pacman -R: $(tr '\n' ',' < "$pacman_apply_log")"

# No wrong driver -> unchanged|reason:gpu-no-wrong-driver, with no pacman calls.
: > "$pacman_apply_log"
GPU_INSTALLED_PKGS="nvidia@560.28.03"
REPAIR_STATUS_OUT="$(repair_gpu_purge_wrong_driver)"
grep -q '^Repair change status gpu:purge-wrong-driver: unchanged|reason:gpu-no-wrong-driver$' <<<"$REPAIR_STATUS_OUT" \
    || fail "pacman GPU purge must report gpu-no-wrong-driver with no wrong driver: $REPAIR_STATUS_OUT"
[[ ! -s "$pacman_apply_log" ]] \
    || fail 'no pacman transaction may run when there is no wrong driver'

# A cascade (a Packages row naming an unrequested package) is refused fail-closed.
arch_pacman_transaction_try() {
    local label="$1"; shift
    ARCH_PACMAN_TRY_OUTPUT='Packages (2) xf86-video-nouveau-1.0.17-1 xf86-video-ati-1.0.0-1'
    ARCH_PACMAN_TRY_RC=0
    return 0
}
GPU_INSTALLED_PKGS="xf86-video-nouveau@1.0.17-1"
set +e
cascade_out="$(repair_gpu_purge_wrong_driver 2>&1)"
cascade_rc=$?
set -e
[[ "$cascade_rc" -ne 0 ]] \
    || fail 'a cascade (unrequested package) GPU purge must be refused fail-closed'
grep -q 'failed Boot Bitch safety checks' <<<"$cascade_out" \
    || fail "the cascade refusal must name the safety check: $cascade_out"

echo 'gpu-contract: PASS'
