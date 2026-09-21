#!/usr/bin/env bash
set -Eeuo pipefail

# The helper is a root process.  Never resolve operational commands through a
# caller-controlled PATH (including direct invocation outside pkexec).
PATH='/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin'
export PATH

# Privileged backend for Boot Bitch. This helper intentionally exposes only a
# small, whitelisted command surface. The chroot shell is an explicit,
# user-confirmed escape hatch scoped to the selected target; every request
# still repeats target protection checks here even if the GUI already did so.

PROGRAM_NAME="boot-repair-helper"
SELF_PATH="$(readlink -f -- "$0")"
STATE_ROOT="${BOOT_REPAIR_STATE_ROOT:-/run/boot-repair}"
SESSION_HELPER_COPY=""
SESSION_DIR=""
MOUNT_BASE=""
TARGET_ROOT=""
TARGET_DISK=""
ROOT_DEVICE=""
ROOT_CANONICAL=""
TEMP_MAPPER_ALIASES=()
SESSION_OWNED_MAPPERS=()
TARGET_OS_ID=""
TARGET_OS_LIKE=""
TARGET_PRETTY=""
TARGET_DISTRO_FAMILY=""
# Every backend below is discovered from read-only target filesystem evidence.
# The distribution family is only an ordering/wording hint; it never decides
# whether a backend is available.  TARGET_PACKAGE_MANAGER stays the primary
# (native-first) manager for labels; TARGET_PACKAGE_MANAGERS holds every
# detected manager in deterministic native-first order.
TARGET_PACKAGE_MANAGER=""
TARGET_PACKAGE_MANAGERS=()
TARGET_INITRAMFS_BACKEND=""
TARGET_INITRAMFS_BACKENDS=()
TARGET_BOOTLOADER_BACKEND=""
TARGET_SERVICE_MANAGER=""
TARGET_SERVICE_MANAGERS=()
TARGET_DISPLAY_BACKEND=""
TARGET_LOGGING_BACKEND=""
TARGET_ESP_MOUNT=""
TARGET_KERNEL_LAYOUT=""
TARGET_REPAIR_BACKEND=""
TARGET_SUBVOL=""
ARCH_PACMAN_SANDBOX=""
ARCH_PACMAN_DB_REL=""
ARCH_PACMAN_CACHE_REL=""
ARCH_PACMAN_LOG_REL=""
MOUNTS=()
TARGET_DATA_MOUNTS=()
SESSION_LOG=""
TEMP_TARGET_PATHS=()
EFI_ESP_SOURCE=""
EFI_ESP_FSTYPE=""
EFI_HOST_ESP_SOURCE=""
EFI_HOST_ESP_PARTUUID=""
EFI_HOST_ESP_MOUNT=""
EFI_TARGET_ESP_PARTUUID=""
EFI_GRUB_INSTALL_PATH=""
EFI_BOOTLOADER_ID=""
SNAPSHOT_TOP=""
TARGET_WRITE_INTENT=0
CURRENT_STAGE=""
RUNNING_HOST_MODE=0
HOST_COMMAND_GUARD=0
HOST_COMMAND_GUARD_DIR=""
DIAGNOSTIC_SCOPE="Repair Target"
# A combined Run All report emits the shared read-only capability preamble once
# at the top instead of once per diagnostic section. Individual diagnostics
# keep this at 0 so their output stays self-contained.
REPORT_SHARED_PREAMBLE=0

# Display-manager selection is discovered from the target's existing
# display-manager.service link and installed units.  Keep this state scoped to
# one helper invocation so graphical-login repair never silently switches a
# target from its configured login manager to SDDM.
DISPLAY_MANAGER_SERVICE=""
DISPLAY_MANAGER_PACKAGE=""
DISPLAY_MANAGER_LABEL=""
DISPLAY_MANAGER_UNIT_REL=""
# True when the display-manager preflight had to install a package; the offline
# link state alone cannot prove "unchanged" after a package write.
DISPLAY_MANAGER_PACKAGE_WORK=false

# ---------------------------------------------------------------------------
# Logging, failure handling and shared session/evidence helpers
# ---------------------------------------------------------------------------
log() { printf '[%s] %s\n' "$(date '+%H:%M:%S')" "$*"; }
fail()
{
    if [[ -n "$CURRENT_STAGE" ]]; then
        log "ERROR: stage '$CURRENT_STAGE' failed: $*" >&2
    else
        log "ERROR: $*" >&2
    fi
    exit 1
}
need() { command -v "$1" >/dev/null 2>&1 || fail "Required host command not found: $1"; }

# Evidence-driven diagnostics invalidation.  Every successful modifying repair
# action ends with exactly one stable status line so the GUI can decide whether
# the cached read-only diagnostics must be regenerated:
#
#   Repair change status <tool-key>: changed
#   Repair change status <tool-key>: unchanged|<reason>
#
# "changed" is the fail-safe default: a tool may report "unchanged" only after
# proving that no write happened (identical artifact hashes, an apply step that
# reported no work, or state that was already correct).  A failed action emits
# no line at all and the GUI invalidates conservatively.
#
# Multi-backend stages run several guarded backends inside one stage.  The
# dispatcher sets REPAIR_CHANGE_STATUS_COLLECT=1 so each backend's status is
# recorded instead of printed; the dispatcher then emits exactly one combined
# line for the stage (see run_package_stage).
REPAIR_CHANGE_STATUS_COLLECT=0
REPAIR_CHANGE_STATUS_COLLECTED=()

repair_change_status()
{
    local key="$1" state="$2"
    if (( REPAIR_CHANGE_STATUS_COLLECT == 1 )); then
        REPAIR_CHANGE_STATUS_COLLECTED+=("$key"$'\t'"$state")
        return 0
    fi
    printf 'Repair change status %s: %s\n' "$key" "$state"
    if [[ -n "$SESSION_LOG" ]]; then
        printf 'Repair change status %s: %s\n' "$key" "$state" >> "$SESSION_LOG" 2>/dev/null || true
    fi
}

# Byte fingerprint of one boot artifact.  A missing file is represented
# explicitly so a repair that only creates it is still detected as changed.
repair_file_fingerprint()
{
    local path="$1" digest
    if [[ -s "$path" ]]; then
        digest="$(sha256sum "$path" 2>/dev/null | awk '{print $1}')"
        printf '%s\n' "${digest:-unreadable}"
    else
        printf 'missing\n'
    fi
}

# Fingerprint every initramfs image the initramfs repair may rewrite.  The
# images are identified by kernel/module inventory rather than by directory
# scan so a rebuild that only updates timestamps is recognized as unchanged.
# The image naming is selected from the detected initramfs backend, not from
# the distribution family: mkinitfs uses flavor-named images (initramfs-lts),
# mkinitcpio uses initramfs-*.img and initramfs-tools/dracut use initrd.img-*.
initramfs_image_fingerprint()
{
    local kver path digest pair
    command -v sha256sum >/dev/null 2>&1 || { printf 'no-sha256sum\n'; return 0; }
    if [[ "${TARGET_INITRAMFS_BACKEND:-}" == mkinitfs ]]; then
        # Alpine initramfs images have no .img suffix (initramfs-lts, -virt,
        # -edge).  The image inventory probe is BusyBox-safe.
        while IFS= read -r path; do
            [[ -n "$path" ]] || continue
            digest="$(sha256sum "$TARGET_ROOT/boot/$path" 2>/dev/null | awk '{print $1}')"
            printf '/boot/%s %s\n' "$path" "${digest:-unreadable}"
        done < <(alpine_initramfs_images)
        return 0
    fi
    if [[ "${TARGET_INITRAMFS_BACKEND:-}" == mkinitcpio ]]; then
        while IFS= read -r path; do
            [[ -n "$path" ]] || continue
            digest="$(sha256sum "$path" 2>/dev/null | awk '{print $1}')"
            printf '%s %s\n' "${path#"$TARGET_ROOT"}" "${digest:-unreadable}"
        done < <(find "$TARGET_ROOT/boot" -maxdepth 1 -type f -name 'initramfs-*.img' -size +0c 2>/dev/null | LC_ALL=C sort)
        return 0
    fi
    if [[ "${TARGET_INITRAMFS_BACKEND:-}" == dracut ]]; then
        # Fedora/dracut images are version-named and paired with the module
        # directory inventory, so a missing image is reported explicitly and
        # the rescue pair (-0-rescue-*) is never part of the fingerprint.
        while IFS= read -r pair; do
            [[ -n "$pair" ]] || continue
            kver="${pair%% *}"
            [[ -n "$kver" ]] || continue
            path="$TARGET_ROOT/boot/initramfs-$kver.img"
            if [[ -s "$path" ]]; then
                digest="$(sha256sum "$path" 2>/dev/null | awk '{print $1}')"
                printf '/boot/initramfs-%s.img %s\n' "$kver" "${digest:-unreadable}"
            else
                printf '/boot/initramfs-%s.img missing\n' "$kver"
            fi
        done < <(rpm_kernel_pairs_readonly)
        return 0
    fi
    while IFS= read -r kver; do
        [[ -n "$kver" ]] || continue
        path="$TARGET_ROOT/boot/initrd.img-$kver"
        if [[ -s "$path" ]]; then
            digest="$(sha256sum "$path" 2>/dev/null | awk '{print $1}')"
            printf '/boot/initrd.img-%s %s\n' "$kver" "${digest:-unreadable}"
        else
            printf '/boot/initrd.img-%s missing\n' "$kver"
        fi
    done < <(installed_kernel_versions)
    return 0
}

# Fingerprint the EFI boot artifacts and the current firmware entry state so an
# EFI/UKI repair that produced byte-identical files and NVRAM can report
# unchanged instead of trusting command output.
efi_boot_artifact_fingerprint()
{
    local esp_root="$TARGET_ROOT${TARGET_ESP_MOUNT:-/boot/efi}" path digest
    command -v sha256sum >/dev/null 2>&1 || { printf 'no-sha256sum\n'; return 0; }
    if [[ -d "$esp_root/EFI" ]]; then
        while IFS= read -r path; do
            [[ -n "$path" ]] || continue
            digest="$(sha256sum "$path" 2>/dev/null | awk '{print $1}')"
            printf '%s %s\n' "${path#"$esp_root"}" "${digest:-unreadable}"
        done < <(find "$esp_root/EFI" -type f \( -iname '*.efi' -o -iname 'grub.cfg' \) 2>/dev/null | LC_ALL=C sort)
    fi
    # EFI-stub systems keep the kernel and initramfs at the ESP root; include
    # them so a stub-stage reconciliation reports from the same artifact set.
    if [[ -d "$esp_root" ]]; then
        while IFS= read -r path; do
            [[ -n "$path" ]] || continue
            digest="$(sha256sum "$path" 2>/dev/null | awk '{print $1}')"
            printf '%s %s\n' "${path#"$esp_root"}" "${digest:-unreadable}"
        done < <(find "$esp_root" -maxdepth 1 -type f \( -name 'vmlinuz-*' -o -name 'initramfs-*' \) 2>/dev/null | LC_ALL=C sort)
    fi
    if command -v efibootmgr >/dev/null 2>&1; then
        efibootmgr -v 2>/dev/null | grep -E '^(Boot[0-9A-F]{4}|BootOrder:|BootCurrent:|Timeout:)' || true
    fi
    return 0
}

# APT apply/simulation output proves "no work" only when the transaction
# summary reports nothing to do and no package/configuration action ran.
apt_transaction_reported_no_changes()
{
    local output="$1"
    grep -Eiq '^[[:space:]]*(Inst|Remv|Conf)[[:space:]]' <<<"$output" && return 1
    grep -Eiq '(Setting up|Unpacking|Preparing to unpack|Processing triggers for) ' <<<"$output" && return 1
    grep -Eq '^0 upgraded, 0 newly installed, 0 to remove' <<<"$output"
}

# True when dpkg has any package in a desired-install state that is not fully
# installed (unpacked, half-configured, triggers pending, reinstall required).
dpkg_configuration_pending()
{
    local status_output rc
    set +e
    status_output="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
        dpkg-query -W -f='${db:Status-Abbrev}\n' 2>/dev/null)"
    rc=$?
    set -e
    (( rc == 0 )) || return 0
    awk '$0 ~ /^ii( |$)/ { next } $0 ~ /^i/ { found=1 } END { exit found ? 0 : 1 }' <<<"$status_output"
}

# Create and validate the root-owned, mode-0700 private state directory that
# holds per-request session directories and the authenticated helper copy.
# Fails when the directory or any ancestor is a symlink or not root-owned and
# private, so a caller cannot swap an attacker-controlled ancestor underneath.
ensure_state_root()
{
    local owner mode ancestor mode_value
    [[ "$STATE_ROOT" == /* && "$STATE_ROOT" != *$'\n'* && "$STATE_ROOT" != *$'\r'* ]] \
        || fail "State directory must be an absolute path without line breaks."
    mkdir -p -- "$STATE_ROOT"
    [[ ! -L "$STATE_ROOT" ]] || fail "State directory must not be a symbolic link: $STATE_ROOT"
    owner="$(stat -c '%u' -- "$STATE_ROOT" 2>/dev/null || true)"
    [[ "$owner" == "0" ]] || fail "State directory is not root-owned: $STATE_ROOT"
    chmod 0700 -- "$STATE_ROOT"
    mode="$(stat -c '%a' -- "$STATE_ROOT" 2>/dev/null || true)"
    [[ "$mode" == "700" ]] || fail "Unable to secure state directory permissions: $STATE_ROOT"
    # Every parent must also be root-owned and non-writable by group/others;
    # this prevents a caller from swapping an attacker-controlled ancestor
    # between validation and the privileged helper copy.
    ancestor="$STATE_ROOT"
    while [[ "$ancestor" != "/" ]]; do
        owner="$(stat -c '%u' -- "$ancestor" 2>/dev/null || true)"
        mode="$(stat -c '%a' -- "$ancestor" 2>/dev/null || true)"
        mode_value=$((8#$mode))
        [[ "$owner" == "0" ]] && (( (mode_value & 022) == 0 )) \
            || fail "State directory ancestor is not root-owned and private: $ancestor"
        ancestor="$(dirname -- "$ancestor")"
    done
}

usage()
{
    cat <<USAGE
Usage:
  $PROGRAM_NAME unlock       <target-disk> <luks-device>
  $PROGRAM_NAME validate     <target-disk> <root-device>
  $PROGRAM_NAME diagnose     <target-disk> <root-device> <diagnostic|all>
  $PROGRAM_NAME config-read  <target-disk> <root-device> <config-key>
  $PROGRAM_NAME config-write <target-disk> <root-device> <config-key> <content>
  $PROGRAM_NAME snapshots    <target-disk> <root-device> <list|inspect|plan|rollback> [snapshot-id]
  $PROGRAM_NAME host-snapshots <host-disk> <root-device> <list|inspect|plan|rollback> [snapshot-id|@rollback-before-<stamp>]
  $PROGRAM_NAME repair       <target-disk> <root-device> <stage> [stage ...]
  $PROGRAM_NAME host-repair  <host-disk> <root-device> <stage> [stage ...]
  $PROGRAM_NAME host-reboot  <host-disk> <root-device>
  $PROGRAM_NAME host-validate <host-disk> <root-device>
  $PROGRAM_NAME host-diagnose <host-disk> <root-device> <diagnostic|all>
  $PROGRAM_NAME host-default <host-disk> <root-device>
  $PROGRAM_NAME host-shell   <host-disk> <root-device> <command>
  $PROGRAM_NAME fs-inspect   <disk> <root-device>
  $PROGRAM_NAME fs-repair    <disk> <root-device> <device> <mode>
  $PROGRAM_NAME host-fs-inspect <host-disk> <root-device>
  $PROGRAM_NAME host-fs-repair  <host-disk> <root-device> <device> <mode>
  $PROGRAM_NAME shell        <target-disk> <root-device> <command>
  $PROGRAM_NAME browse-target <target-disk> <root-device> <absolute-directory>
  $PROGRAM_NAME copy-preview <target-disk> <root-device> <direction> <ownership> <approval> <destination> <source> [source ...]
  $PROGRAM_NAME copy         <target-disk> <root-device> <direction> <ownership> <approval> <destination> <source> [source ...]

Copy directions:
  host-to-repair   Copy host paths into the selected repair system
  repair-to-host   Recover repair-system paths to a safe host destination

Ownership policies:
  smart      Preserve numeric ownership only when identities map consistently;
             otherwise assign copied content to the destination directory owner
  preserve   Preserve source numeric UID/GID exactly

Approval:
  normal         Ordinary target destination
  sensitive-ok   Explicit GUI confirmation for target system paths such as /etc or /boot

Diagnostics:
  environment backend boot boot-evidence kernel grub uki display errors usage fstab btrfs mapper luks report all

Config keys (guarded target read/write): fstab crypttab grub-defaults grub-config sddm gdm3 lightdm greetd ly initramfs

Snapshots:
  list              Enumerate Btrfs root snapshots read-only
  inspect <id>      Inspect one Btrfs root snapshot read-only
  plan <id>         Validate and show a transactional rollback plan read-only
  rollback <id>     Promote a writable copy to @, reconcile boot stack, auto-revert on failure
  Host actions use the same verbs with host-snapshots against the running host.
  The selected Snapper @ snapshot is promoted to @ with a name-preserving
  transaction, the running @ is preserved as the @rollback-before-* undo point
  and a reboot is required before the running host starts the promoted root.
  An existing @rollback-before-* name is accepted as the rollback target.
  host-reboot schedules a reboot only after the explicit GUI confirmation.

File system repair:
  fs-inspect       Resolve root, /boot, ESP and /home filesystems and run
                   read-only check tools (never writes)
  fs-repair        Run the selected filesystem's repair mode after scope,
                   mount-state and tool preflights
  Modes: check, repair, rescue, scrub (filesystem-dependent)
  Offline tools refuse mounted filesystems; btrfs scrub and zpool scrub are
  online modes and require a mounted filesystem.  The running host root is
  never repaired offline.  f2fs is repair-only (no read-only check mode);
  btrfs check --repair is an explicitly confirmed last resort.

Stages:
  dpkg-configure   Complete interrupted dpkg configuration
  fix-broken       Repair broken APT package dependencies
  apt-update       Refresh APT package metadata
  apt-upgrade      Upgrade installed packages
  dkms             Preflight headers/DKMS, correct known gaps, then rebuild modules
  display-manager  Detect installed/configured display manager then restore offline graphical login
  initramfs        Trial-build, then create/update and verify initramfs images
  efi              Preflight and repair TUXEDO UKI or conventional GRUB EFI
  boot-stack       Adaptive initramfs + EFI/UKI + GRUB reconciliation
  grub             Trial-generate, then regenerate and verify GRUB configuration
  extlinux         Regenerate /boot/extlinux.conf via guarded update-extlinux

Stage mode hints:
  --post-efi       The caller already ran the EFI/UKI stage for this scope in
                   the same plan/session.  boot-stack then skips its second
                   UKI/EFI rebuild and the duplicate GRUB regeneration while
                   still validating mapper/crypttab and reconciling initramfs.
                   The helper never infers this from the stage list.

Shell:
  shell            Execute one reviewed command as root inside a fresh target chroot
  host-shell       Execute one reviewed command as root on the running host (no chroot)

LUKS unlock is supported for non-host target devices. The passphrase is read
from standard input and is never accepted as a command-line argument. File copy
uses rsync without --delete and independently validates host/target containment.
Modifying repair stages use a transaction-specific backend preflight. Debian/
Ubuntu uses APT/dpkg; Arch uses a sandboxed full pacman transaction, mkinitcpio,
and its detected EFI/GRUB layout; Alpine uses simulation-first apk transactions,
OpenRC runlevel repair, mkinitfs and guarded extlinux configuration
regeneration. Unsupported package or boot layouts remain hard-gated. EFI
bootloader reinstall is an explicit stage and is never selected implicitly.
Read-only diagnostics also profile Arch-family targets (pacman, initramfs
generator, GRUB/systemd-boot/UKI layout, ESP mount and kernel naming). Arch
modifying stages are enabled only where the corresponding guarded preflight
supports the detected layout.
Host maintenance is a separate native-running-system path. It accepts all
repair stages supported by the detected backend with the same stage-specific
checks; unsupported package or boot stages are rejected before any write.
Host validation and diagnostics are read-only. Running-host Snapper @
snapshots are available through host-snapshots in the Snapshots tab; the
chroot shell and file-copy workflows remain separate target tools.
USAGE
}

target_path_is_mounted_rw()
{
    local path="$1" options
    [[ -e "$path" ]] || return 1
    options="$(findmnt -rn -o OPTIONS --target "$path" 2>/dev/null | head -n1 || true)"
    [[ -n "$options" ]] || return 1
    case ",$options," in
        *,rw,*) return 0 ;;
        *) return 1 ;;
    esac
}

# EXIT trap for every helper invocation: when this request crossed the
# read-write boundary, append the session log into the repaired system, remove
# request-scoped target paths and mapper aliases, unmount helper-owned mounts,
# close session-owned LUKS mappings and delete the session directory.  Always
# exits with the status captured on entry so the original failure is preserved.
cleanup()
{
    local rc=$?
    set +e

    # Read-only diagnostics, validation and rollback preflight must remain
    # genuinely read-only.  Append the helper log into the repaired system only
    # after this request deliberately crossed the read-write boundary, and only
    # while the target log path is still backed by a rw mount.
    if (( TARGET_WRITE_INTENT == 1 )) \
       && [[ -n "$SESSION_LOG" && -f "$SESSION_LOG" && -n "$TARGET_ROOT" && -d "$TARGET_ROOT/var/log" ]] \
       && target_path_is_mounted_rw "$TARGET_ROOT/var/log"; then
        {
            printf '\n===== Boot Bitch session %s =====\n' "$(date --iso-8601=seconds 2>/dev/null || date)"
            cat "$SESSION_LOG"
        } >> "$TARGET_ROOT/var/log/boot-repair-session.log" 2>/dev/null || true
    fi

    # Request-scoped helper files must be removed while the target is still
    # mounted.  This also covers an interrupted UKI builder before its normal
    # post-command cleanup runs.
    for target_path in "${TEMP_TARGET_PATHS[@]:-}"; do
        [[ -n "$target_path" && ( -e "$target_path" || -L "$target_path" ) ]] \
            && rm -rf -- "$target_path" 2>/dev/null || true
    done

    for (( idx=${#MOUNTS[@]}-1; idx>=0; --idx )); do
        mountpoint="${MOUNTS[$idx]}"
        if mountpoint -q "$mountpoint" 2>/dev/null; then
            umount "$mountpoint" 2>/dev/null || umount -l "$mountpoint" 2>/dev/null || true
        fi
    done

    # Compatibility aliases are created only for the lifetime of one helper
    # request.  They make target crypttab/grub/initramfs tooling resolve the
    # mapper name the installed OS expects without renaming a live dm-crypt
    # mapping (renaming a mounted mapper leaves stale mount-source strings).
    for alias_path in "${TEMP_MAPPER_ALIASES[@]:-}"; do
        [[ -n "$alias_path" && -L "$alias_path" ]] && rm -f -- "$alias_path" 2>/dev/null || true
    done

    # A long-lived authorized session keeps LUKS mappings that *Boot Bitch*
    # opened available between requests. Close only those mappings when the
    # session ends (File -> Lock Administrator Session or app exit). Mappings
    # that were already open before Boot Bitch attached to them are never
    # added to this list and therefore are never closed here.
    if command -v cryptsetup >/dev/null 2>&1; then
        for (( idx=${#SESSION_OWNED_MAPPERS[@]}-1; idx>=0; --idx )); do
            mapper_name="${SESSION_OWNED_MAPPERS[$idx]}"
            [[ -n "$mapper_name" ]] || continue
            cryptsetup status "$mapper_name" >/dev/null 2>&1 || continue
            cryptsetup close "$mapper_name" >/dev/null 2>&1 || true
        done
    fi

    [[ -n "$SESSION_DIR" ]] && rm -rf -- "$SESSION_DIR" 2>/dev/null || true
    [[ -n "$SESSION_HELPER_COPY" ]] && rm -f -- "$SESSION_HELPER_COPY" 2>/dev/null || true
    exit "$rc"
}
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM
trap 'exit 129' HUP

# ---------------------------------------------------------------------------
# Block-device identity, mapper aliases and target protection
# ---------------------------------------------------------------------------
is_block_device()
{
    [[ -b "$1" ]]
}

canonical_block()
{
    local dev="$1"
    is_block_device "$dev" || return 1
    readlink -f -- "$dev"
}

# Canonicalize an existing path without requiring GNU realpath's -e flag.  A
# base Alpine recovery host ships BusyBox realpath (no -e); readlink -f is
# available in both BusyBox and GNU coreutils, so fall back to it after
# proving that the resolved path exists.  Callers rely on the fail-closed
# behavior for missing paths, so the existence check is mandatory.
realpath_existing()
{
    local path="$1" resolved
    if resolved="$(realpath -e -- "$path" 2>/dev/null)" && [[ -n "$resolved" ]]; then
        printf '%s\n' "$resolved"
        return 0
    fi
    resolved="$(readlink -f -- "$path" 2>/dev/null)" || return 1
    [[ -n "$resolved" && -e "$resolved" ]] || return 1
    printf '%s\n' "$resolved"
}

# Join the mounted target root with an absolute path inside the target.  A
# running-host root is "/" and would otherwise produce "//boot/efi"; BusyBox
# mountpoint does not normalize the double slash, so the join is explicit.
target_path()
{
    local suffix="$1"
    if [[ "$TARGET_ROOT" == "/" ]]; then
        printf '%s\n' "$suffix"
    else
        printf '%s\n' "$TARGET_ROOT$suffix"
    fi
}

mapper_aliases_for_device()
{
    local dev="$1" canonical alias resolved name
    canonical="$(canonical_block "$dev")" || return 1

    for alias in /dev/mapper/*; do
        [[ -e "$alias" || -L "$alias" ]] || continue
        name="$(basename -- "$alias")"
        [[ "$name" == "control" ]] && continue
        resolved="$(readlink -f -- "$alias" 2>/dev/null || true)"
        [[ "$resolved" == "$canonical" ]] && printf '%s\n' "$alias"
    done
}

preferred_block_path()
{
    local raw="$1" canonical="$2" alias raw_name
    local -a aliases=()

    # Only preserve an explicitly supplied mapper name when it already follows
    # the installed-system convention.  Recovery tools often open a LUKS
    # volume under a private name (for example boot-repair-*).  Mounting the
    # target through such a name leaks it into mountinfo and can later break
    # grub-probe or cryptsetup-initramfs if that recovery-only name disappears.
    # For foreign/private mapper names, mount through the always-present dm-N
    # node and create a request-scoped alias matching the target's crypttab
    # after /etc becomes visible.
    if [[ "$raw" == /dev/mapper/* && -b "$raw" ]]; then
        raw_name="$(basename -- "$raw")"
        if [[ "$raw_name" == luks-* ]]; then
            printf '%s\n' "$raw"
            return 0
        fi
    fi

    mapfile -t aliases < <(mapper_aliases_for_device "$canonical" 2>/dev/null | sort)
    for alias in "${aliases[@]}"; do
        [[ "$(basename -- "$alias")" == luks-* ]] || continue
        printf '%s\n' "$alias"
        return 0
    done

    # Do not select an arbitrary mapper alias here.  /dev/dm-N is a resolvable
    # block node for the lifetime of this helper request and avoids persisting
    # another tool's naming policy into the target chroot mount metadata.
    printf '%s\n' "$canonical"
}

find_crypt_mapper_for_device()
{
    local device="$1" canonical alias name existing_device
    canonical="$(canonical_block "$device")" || return 1
    command -v cryptsetup >/dev/null 2>&1 || return 1

    for alias in /dev/mapper/*; do
        [[ -e "$alias" || -L "$alias" ]] || continue
        name="$(basename -- "$alias")"
        [[ "$name" == "control" ]] && continue
        existing_device="$(cryptsetup status "$name" 2>/dev/null \
            | awk -F: '$1 ~ /^[[:space:]]*device$/ {gsub(/^[[:space:]]+|[[:space:]]+$/, "", $2); print $2; exit}')"
        [[ -n "$existing_device" ]] || continue
        existing_device="$(readlink -f -- "$existing_device" 2>/dev/null || true)"
        if [[ "$existing_device" == "$canonical" ]]; then
            printf '%s\n' "$alias"
            return 0
        fi
    done
    return 1
}

crypt_backing_device()
{
    local dev="$1" canonical kname type slave
    local -a slaves=()
    canonical="$(canonical_block "$dev")" || return 1
    type="$(lsblk -ndo TYPE "$canonical" 2>/dev/null | head -n1 || true)"
    [[ "$type" == "crypt" ]] || return 1
    kname="$(lsblk -ndo KNAME "$canonical" 2>/dev/null | head -n1 || true)"
    [[ -n "$kname" ]] || return 1

    if [[ -d "/sys/class/block/$kname/slaves" ]]; then
        while IFS= read -r slave; do
            [[ -b "/dev/$slave" ]] && slaves+=("$(canonical_block "/dev/$slave")")
        done < <(find "/sys/class/block/$kname/slaves" -mindepth 1 -maxdepth 1 -printf '%f\n' 2>/dev/null | sort -u)
    fi
    ((${#slaves[@]} == 1)) || return 1
    printf '%s\n' "${slaves[0]}"
}

# Create a request-scoped /dev/mapper/<name> symlink for the installed
# crypttab's expected mapper name when the live mapper has a different
# recovery-only name.  Aliases are removed by cleanup() and never rename a
# mounted mapping.  Returns without creating anything when no alias is needed.
prepare_mapper_compatibility_aliases()
{
    local backing name source key options resolved alias existing
    [[ -f "$TARGET_ROOT/etc/crypttab" ]] || return 0

    backing="$(crypt_backing_device "$ROOT_CANONICAL" 2>/dev/null || true)"
    [[ -n "$backing" ]] || return 0

    while read -r name source key options _rest; do
        [[ -n "$name" && "${name:0:1}" != "#" ]] || continue
        [[ "$name" =~ ^[[:alnum:]_.+-]+$ ]] || continue
        [[ -n "$source" ]] || continue
        resolved="$(resolve_fstab_source "$source")"
        [[ -n "$resolved" && -b "$resolved" ]] || continue
        resolved="$(canonical_block "$resolved")" || continue
        [[ "$resolved" == "$backing" ]] || continue

        alias="/dev/mapper/$name"
        if [[ -e "$alias" || -L "$alias" ]]; then
            existing="$(readlink -f -- "$alias" 2>/dev/null || true)"
            [[ "$existing" == "$ROOT_CANONICAL" ]] \
                || fail "Target expects mapper '$name', but $alias resolves to a different device."
            log "Mapper compatibility: $alias already resolves to $ROOT_CANONICAL" | tee -a "$SESSION_LOG"
            return 0
        fi

        ln -s -- "$ROOT_CANONICAL" "$alias"
        TEMP_MAPPER_ALIASES+=("$alias")
        log "Mapper compatibility: temporary $alias -> $ROOT_CANONICAL" | tee -a "$SESSION_LOG"
        return 0
    done < "$TARGET_ROOT/etc/crypttab"
}

# Return every physical top-level disk backing a block component.  lsblk's
# inverse-dependency view is important here: device-mapper nodes (dm-crypt,
# LVM, etc.) are holders of their underlying partition and often have no
# useful PKNAME when queried directly.  The earlier PKNAME-only walk therefore
# misclassified a successfully unlocked /dev/mapper/boot-repair-* target as an
# unrelated device.  Multiple physical disks are retained so RAID/multi-device
# stacks are rejected instead of guessed about.
top_disks_for()
{
    local dev disk
    local -a disks=()
    dev="$(canonical_block "$1")" || return 1

    mapfile -t disks < <(
        lsblk -srnpo NAME,TYPE "$dev" 2>/dev/null |
            awk '$2 == "disk" {print $1}' |
            while IFS= read -r disk; do
                canonical_block "$disk" 2>/dev/null || true
            done |
            awk 'NF' |
            sort -u
    )

    if ((${#disks[@]} > 0)); then
        printf '%s\n' "${disks[@]}"
        return 0
    fi

    # Conservative fallback for unusual util-linux/device-mapper combinations.
    # Follow PKNAME where available, then sysfs slave links (the kernel's source
    # of truth for dm/md dependency edges).  A partition's sysfs parent is used
    # only when neither source reports a dependency.
    _top_disks_for_sysfs "$dev"
}

_top_disks_for_sysfs()
{
    local dev current kname parent sys_path parent_name
    local -a parents=()
    local -A seen=()

    dev="$(canonical_block "$1")" || return 1
    current="$dev"

    while :; do
        [[ -n "${seen[$current]:-}" ]] && return 1
        seen[$current]=1
        parents=()

        while IFS= read -r parent; do
            [[ -n "$parent" ]] && parents+=("/dev/$parent")
        done < <(lsblk -ndo PKNAME "$current" 2>/dev/null | awk 'NF' | sort -u)

        kname="$(lsblk -ndo KNAME "$current" 2>/dev/null | head -n1 || true)"
        if [[ -n "$kname" && -d "/sys/class/block/$kname/slaves" ]]; then
            while IFS= read -r parent; do
                [[ -n "$parent" ]] && parents+=("/dev/$parent")
            done < <(find "/sys/class/block/$kname/slaves" -mindepth 1 -maxdepth 1 -printf '%f\n' 2>/dev/null | sort -u)
        fi

        # Partition fallback: /sys/class/block/<part> resolves below the parent
        # disk directory.  This keeps the safety check working even if PKNAME is
        # unavailable for a particular util-linux build.
        if ((${#parents[@]} == 0)) && [[ -n "$kname" && -f "/sys/class/block/$kname/partition" ]]; then
            sys_path="$(readlink -f -- "/sys/class/block/$kname" 2>/dev/null || true)"
            parent_name="$(basename -- "$(dirname -- "$sys_path")")"
            [[ -n "$parent_name" && -b "/dev/$parent_name" ]] && parents+=("/dev/$parent_name")
        fi

        if ((${#parents[@]} == 0)); then
            printf '%s\n' "$current"
            return 0
        fi

        mapfile -t parents < <(printf '%s\n' "${parents[@]}" | sort -u)
        if ((${#parents[@]} > 1)); then
            for parent in "${parents[@]}"; do
                _top_disks_for_sysfs "$parent"
            done | sort -u
            return 0
        fi

        current="$(canonical_block "${parents[0]}")" || return 1
    done
}

same_single_top_disk()
{
    local a="$1" b="$2"
    mapfile -t a_tops < <(top_disks_for "$a" | sort -u)
    mapfile -t b_tops < <(top_disks_for "$b" | sort -u)
    ((${#a_tops[@]} == 1 && ${#b_tops[@]} == 1)) || return 1
    [[ "${a_tops[0]}" == "${b_tops[0]}" ]]
}

# Refuse every write when the selected target disk backs a running-system
# mountpoint (/ or /boot or /boot/efi).  Checks lsblk mountpoints first, then
# resolves each live system mount back to its top-level disk.
assert_target_not_host()
{
    local target="$1" source source_dev mp
    mapfile -t target_tops < <(top_disks_for "$target" | sort -u)
    ((${#target_tops[@]} == 1)) || fail "Target disk resolves through a multi-device stack; refusing to guess."

    # First ask lsblk directly about the selected tree. This catches ordinary
    # block-backed host roots even when findmnt formats the source unusually.
    if lsblk -nrpo MOUNTPOINTS "${target_tops[0]}" 2>/dev/null \
        | tr ' ' '\n' \
        | grep -Fxq -e / -e /boot -e /boot/efi; then
        fail "Selected target ${target_tops[0]} contains a running-system mountpoint; refusing all writes."
    fi

    for mp in / /boot /boot/efi /efi; do
        source="$(findmnt -rn -o SOURCE --target "$mp" 2>/dev/null | head -n1 || true)"
        [[ -n "$source" ]] || continue
        # findmnt may represent Btrfs subvolumes as /dev/XYZ[/subvol]. Strip
        # the bracketed subvolume suffix before resolving the backing device.
        source="${source%%\[*}"
        source_dev="$(readlink -f -- "$source" 2>/dev/null || true)"
        [[ -b "$source_dev" ]] || continue
        mapfile -t host_tops < <(top_disks_for "$source_dev" | sort -u)
        for host_top in "${host_tops[@]}"; do
            if [[ "$host_top" == "${target_tops[0]}" ]]; then
                fail "Selected target ${target_tops[0]} backs the currently running system; refusing all writes."
            fi
        done
    done
}

# Prove that the supplied host disk and root component back the currently
# running root filesystem and that every live /boot, /boot/efi and /efi mount
# resolves to that same disk before native host maintenance is allowed.
assert_target_is_running_host()
{
    local target="$1" root_device="$2" root_source source_dev mp boot_source boot_dev
    local -a target_tops=() root_tops=() boot_tops=()

    mapfile -t target_tops < <(top_disks_for "$target" | sort -u)
    ((${#target_tops[@]} == 1)) || fail "Host disk resolves through a multi-device stack; refusing to guess."

    # The host path is intentionally native: it must prove that the supplied
    # disk is the physical backing disk of the currently running root and that
    # the selected ESP belongs to that same disk. This prevents a caller from
    # turning a mounted data disk into a privileged write target.
    root_source="$(findmnt -rn -o SOURCE --target / 2>/dev/null | head -n1 || true)"
    root_source="${root_source%%\[*}"
    [[ -n "$root_source" ]] || fail "Unable to identify the currently running root filesystem."
    root_source="$(canonical_block "$root_source" 2>/dev/null || true)"
    [[ -n "$root_source" ]] || fail "The currently running root is not backed by a block device."
    mapfile -t root_tops < <(top_disks_for "$root_source" | sort -u)
    [[ ${#root_tops[@]} -eq 1 && "${root_tops[0]}" == "${target_tops[0]}" ]] \
        || fail "Selected host disk does not back the currently running root filesystem."

    root_device="$(canonical_block "$root_device" 2>/dev/null || true)"
    [[ -n "$root_device" && "$root_device" == "$root_source" ]] \
        || fail "Supplied root component does not match the currently running root filesystem."

    for mp in /boot /boot/efi /efi; do
        boot_source="$(findmnt -rn -o SOURCE --target "$mp" 2>/dev/null \
            | awk '/^\/dev\// {print; exit}' || true)"
        [[ -n "$boot_source" ]] || continue
        boot_source="${boot_source%%\[*}"
        boot_dev="$(canonical_block "$boot_source" 2>/dev/null || true)"
        [[ -n "$boot_dev" ]] || fail "The running host $mp mount is not backed by a block device."
        mapfile -t boot_tops < <(top_disks_for "$boot_dev" | sort -u)
        [[ ${#boot_tops[@]} -eq 1 && "${boot_tops[0]}" == "${target_tops[0]}" ]] \
            || fail "Running host $mp resolves outside the selected host disk; refusing native boot maintenance."
    done
}

# ---------------------------------------------------------------------------
# fstab/crypttab resolution and guarded target mount preparation
# ---------------------------------------------------------------------------
resolve_fstab_source()
{
    local spec
    spec="$(fstab_unescape "$1")"
    case "$spec" in
        UUID=*) blkid -U "${spec#UUID=}" 2>/dev/null || true ;;
        PARTUUID=*) blkid -t "PARTUUID=${spec#PARTUUID=}" -o device 2>/dev/null | head -n1 ;;
        LABEL=*) blkid -L "${spec#LABEL=}" 2>/dev/null || true ;;
        PARTLABEL=*) blkid -t "PARTLABEL=${spec#PARTLABEL=}" -o device 2>/dev/null | head -n1 ;;
        /dev/*) readlink -f -- "$spec" 2>/dev/null || true ;;
        *) printf '%s' "" ;;
    esac
}

fstab_entry_for_mountpoint()
{
    local mountpoint_name="$1"
    [[ -f "$TARGET_ROOT/etc/fstab" ]] || return 0
    awk -v mp="$mountpoint_name" '
        /^[[:space:]]*#/ { next }
        NF >= 4 && $2 == mp { print $1 "\t" $3 "\t" $4; exit }
    ' "$TARGET_ROOT/etc/fstab"
}

mount_recorded()
{
    local source="$1" destination="$2"; shift 2
    mkdir -p -- "$destination"
    mount "$@" -- "$source" "$destination"
    MOUNTS+=("$destination")
}

# Populate a private writable /dev tmpfs from the recovery host's device tree.
# Device nodes, symlinks (for example /dev/disk/by-uuid) and directories are
# copied so chroot tools see the same devices as before, but every write stays
# on the private tmpfs and never reaches the running host's /dev.  The host's
# nested /dev mounts (pts, shm, mqueue, hugepages) are recreated as plain
# directories: a guarded package transaction needs a writable /dev, not ptys,
# and avoiding nested mounts keeps the EXIT cleanup a single unmount.  The
# source directory is overridable for the shell contract test.
populate_writable_dev()
{
    local destination="$1" source="${2:-/dev}" entry name
    local -a entries=()
    local -a essential_nodes=(
        'null c 1 3 666'
        'zero c 1 5 666'
        'full c 1 7 666'
        'random c 1 8 666'
        'urandom c 1 9 666'
        'tty c 5 0 666'
        'console c 5 1 600'
    )

    shopt -s nullglob
    entries=("$source"/*)
    shopt -u nullglob
    mkdir -p -- "$destination"
    for entry in "${entries[@]}"; do
        name="${entry##*/}"
        case "$name" in
            pts|shm|mqueue|hugepages) continue ;;
        esac
        cp -a "$entry" "$destination/$name" 2>/dev/null || true
    done
    mkdir -p -- "$destination/pts" "$destination/shm"
    chmod 0755 -- "$destination/pts" 2>/dev/null || true
    chmod 1777 -- "$destination/shm" 2>/dev/null || true
    for name in mqueue hugepages; do
        [[ -d "$source/$name" ]] || continue
        mkdir -p -- "$destination/$name"
        chmod --reference="$source/$name" "$destination/$name" 2>/dev/null || true
    done
    for entry in "${essential_nodes[@]}"; do
        name="${entry%% *}"
        [[ -e "$destination/$name" ]] && continue
        read -r name type major minor mode <<<"$entry"
        mknod -m "$mode" "$destination/$name" "$type" "$major" "$minor" 2>/dev/null || true
    done
    for name in fd stdin stdout stderr; do
        [[ -e "$destination/$name" || -L "$destination/$name" ]] && continue
        case "$name" in
            fd) ln -s /proc/self/fd "$destination/fd" 2>/dev/null || true ;;
            stdin) ln -s /proc/self/fd/0 "$destination/stdin" 2>/dev/null || true ;;
            stdout) ln -s /proc/self/fd/1 "$destination/stdout" 2>/dev/null || true ;;
            stderr) ln -s /proc/self/fd/2 "$destination/stderr" 2>/dev/null || true ;;
        esac
    done
}

mount_special()
{
    local kind="$1" source="$2" destination="$3" root_real parent_real
    root_real="$(realpath_existing "$TARGET_ROOT" 2>/dev/null)" \
        || fail "Unable to resolve target root before mounting $kind."
    [[ "$destination" == "$TARGET_ROOT/"* && ! -L "$destination" ]] \
        || fail "Refusing to mount $kind through an unsafe target path: $destination"
    parent_real="$(realpath_existing "$(dirname -- "$destination")" 2>/dev/null)" \
        || fail "Unable to resolve target mount parent: $destination"
    path_within "$parent_real" "$root_real" \
        || fail "Target mount parent escapes the selected root: $destination"
    mkdir -p -- "$destination"
    [[ ! -L "$destination" ]] || fail "Refusing to mount $kind through a target symlink: $destination"
    case "$kind" in
        rbind)
            mount --rbind "$source" "$destination"
            mount --make-rslave "$destination"
            ;;
        rbind-ro)
            mount --rbind "$source" "$destination"
            mount --make-rslave "$destination"
            # Keep target-side repair tools from modifying the host's device
            # or sysfs trees.  Recursive bind remount applies to nested mounts.
            mount -o remount,bind,ro,rec "$destination"
            ;;
        dev-rw)
            # Writable private /dev for target chroots.  rpm/dnf5 payloads
            # that own /dev (for example Fedora's filesystem package) cannot
            # be unpacked onto the read-only recovery-host /dev bind, but a
            # read-write bind would let a package transaction modify the
            # running host's device tree.  A populated tmpfs keeps the chroot
            # functional while every write stays on the private mount.
            mount -t tmpfs -o mode=755,nosuid,size=64M tmpfs "$destination"
            populate_writable_dev "$destination"
            log "Target /dev: private writable tmpfs populated from the recovery host; the running host /dev is not modified." | tee -a "$SESSION_LOG"
            ;;
        tmpfs)
            mount -t tmpfs -o mode=755,nosuid,nodev,noexec,size=64M tmpfs "$destination"
            ;;
        proc)
            mount -t proc proc "$destination"
            ;;
        *) fail "Internal mount type error: $kind" ;;
    esac
    MOUNTS+=("$destination")
}

mount_target_resolver()
{
    local target_link="$TARGET_ROOT/etc/resolv.conf" link destination root_real
    [[ -r /etc/resolv.conf ]] || return 0
    [[ -e "$target_link" || -L "$target_link" ]] || return 0

    # Resolve the target's configured resolver path after /run has been
    # isolated. Bind the live recovery-host resolver there so apt and other
    # network tools work in the chroot without writing persistent target data.
    if [[ -L "$target_link" ]]; then
        link="$(readlink -- "$target_link")"
        if [[ "$link" == /* ]]; then
            destination="$TARGET_ROOT$link"
        else
            destination="$(dirname -- "$target_link")/$link"
        fi
    else
        destination="$target_link"
    fi
    root_real="$(realpath_existing "$TARGET_ROOT" 2>/dev/null || true)"
    [[ -n "$root_real" ]] || return 0
    mkdir -p -- "$(dirname -- "$destination")"
    [[ -e "$destination" ]] || touch -- "$destination"
    path_within "$(realpath -m -- "$destination")" "$root_real" \
        || fail "Target resolver path escapes the selected root: $destination"
    mount --bind /etc/resolv.conf "$destination"
    mount -o remount,bind,ro "$destination"
    MOUNTS+=("$destination")
    log "Bound recovery-host resolver into target chroot (temporary, read-only)" | tee -a "$SESSION_LOG"
}

# Mount the target's fstab entry for /boot, /boot/efi or /efi at its target
# path, recording it in MOUNTS.  requested_mode is ro or rw.  A same-device
# /boot entry is skipped unless it is a distinct Btrfs subvolume, and an
# existing helper-recorded mount is only remounted when rw is requested.
mount_target_boot_entry()
{
    local mp="$1" requested_mode="${2:-rw}" entry spec fstype options resolved dest mount_options
    entry="$(fstab_entry_for_mountpoint "$mp")"
    [[ -n "$entry" ]] || return 0
    IFS=$'\t' read -r spec fstype options <<< "$entry"

    resolved="$(resolve_fstab_source "$spec")"
    [[ -n "$resolved" && -b "$resolved" ]] || fail "Unable to resolve fstab entry for $mp: $spec"
    same_single_top_disk "$TARGET_DISK" "$resolved" || fail "$mp resolves outside the selected target disk: $resolved"

    dest="$TARGET_ROOT$mp"
    [[ "$dest" == "$TARGET_ROOT/"* && ! -L "$dest" ]] \
        || fail "Refusing to mount target $mp through an unsafe path."
    local root_real dest_parent_real
    root_real="$(realpath_existing "$TARGET_ROOT" 2>/dev/null)" \
        || fail "Unable to resolve target root before mounting $mp."
    dest_parent_real="$(realpath_existing "$(dirname -- "$dest")" 2>/dev/null)" \
        || fail "Unable to resolve target mount parent for $mp."
    path_within "$dest_parent_real" "$root_real" \
        || fail "Target mount parent escapes the selected root for $mp."
    if mountpoint -q "$dest" 2>/dev/null; then
        if [[ "$requested_mode" == "rw" ]]; then
            local recorded_mount=false recorded_path
            for recorded_path in "${MOUNTS[@]}"; do
                if [[ "$recorded_path" == "$dest" ]]; then
                    recorded_mount=true
                    break
                fi
            done
            [[ "$recorded_mount" == true ]] || fail "Unexpected existing mount at target $mp; refusing to change its mode."
            log "Remounting target $mp read-write"
            mount -o remount,rw "$dest"
        fi
        return 0
    fi

    # If /boot is simply a directory on the root filesystem, mounting the same
    # root device over it would hide the real directory. A separate same-device
    # mount is only needed for a Btrfs subvolume entry.
    if [[ "$(readlink -f -- "$resolved")" == "$(readlink -f -- "$ROOT_DEVICE")" ]]; then
        if [[ "$fstype" != "btrfs" || ",$options," != *,subvol=* && ",$options," != *,subvolid=* ]]; then
            log "$mp lives on the root filesystem; no separate mount required"
            return 0
        fi
    fi

    [[ "$requested_mode" == "ro" || "$requested_mode" == "rw" ]] || fail "Internal mount mode error: $requested_mode"
    mount_options="$requested_mode"
    if [[ "$requested_mode" == "ro" ]]; then
        case "$fstype" in
            ext2|ext3|ext4) mount_options+=",noload" ;;
            xfs) mount_options+=",norecovery" ;;
        esac
    fi
    if [[ "$fstype" == "btrfs" ]]; then
        IFS=',' read -ra option_parts <<< "$options"
        for option in "${option_parts[@]}"; do
            case "$option" in
                subvol=*|subvolid=*) mount_options+=",$option" ;;
            esac
        done
    fi
    log "Mounting target $mp from $resolved"
    mount_recorded "$resolved" "$dest" -o "$mount_options"
}

fstab_unescape()
{
    local value="$1"
    value="${value//\\040/ }"
    value="${value//\\011/$'\t'}"
    value="${value//\\043/#}"
    # Some TUXEDO fstab entries escape @ in Btrfs subvolume names.
    value="${value//\\@/@}"
    value="${value//\\134/\\}"
    printf '%s\n' "$value"
}

current_btrfs_subvol()
{
    local options option value
    local -a option_parts=()
    options="$(findmnt -rn -o OPTIONS --target "$MOUNT_BASE" 2>/dev/null || true)"
    IFS=',' read -ra option_parts <<< "$options"
    for option in "${option_parts[@]}"; do
        case "$option" in
            subvol=*)
                value="${option#subvol=}"
                value="${value#/}"
                [[ -n "$value" ]] && printf '%s\n' "$value"
                return 0
                ;;
        esac
    done
    return 1
}

# Mount every same-filesystem Btrfs fstab subvolume below the target root
# (for example @home, @var@log) in depth order so a chroot cannot write into
# hidden mountpoint directories inside @.  The recovery mode (ro/rw) overrides
# any fstab ro/rw option; system directories are skipped.
mount_target_btrfs_subvolumes()
{
    local requested_mode="$1" _depth spec mountpoint_name fstype options resolved dest mount_options option filtered_options
    local -a option_parts=()

    [[ "$requested_mode" == "ro" || "$requested_mode" == "rw" ]] \
        || fail "Internal Btrfs subvolume mount mode error: $requested_mode"
    [[ -f "$TARGET_ROOT/etc/fstab" ]] || return 0
    [[ "$(lsblk -ndo FSTYPE "$ROOT_CANONICAL" 2>/dev/null | head -n1 || true)" == "btrfs" ]] || return 0

    log "Mounting target Btrfs fstab subvolumes ($requested_mode)" | tee -a "$SESSION_LOG"

    while IFS=$'\t' read -r _depth spec mountpoint_name fstype options; do
        [[ -n "$spec" && -n "$mountpoint_name" ]] || continue
        mountpoint_name="$(fstab_unescape "$mountpoint_name")"
        options="$(fstab_unescape "$options")"
        [[ "$mountpoint_name" == /* ]] || continue
        case "$mountpoint_name" in
            /|/boot|/boot/efi|/efi|/dev|/dev/*|/proc|/proc/*|/sys|/sys/*|/run|/run/*) continue ;;
        esac

        resolved="$(resolve_fstab_source "$spec")"
        [[ -n "$resolved" && -b "$resolved" ]] || {
            log "WARNING: skipping unresolved Btrfs fstab entry $mountpoint_name -> $spec" | tee -a "$SESSION_LOG"
            continue
        }
        resolved="$(canonical_block "$resolved")" || continue
        [[ "$resolved" == "$ROOT_CANONICAL" ]] || continue

        dest="$TARGET_ROOT$mountpoint_name"
        if mountpoint -q "$dest" 2>/dev/null; then
            continue
        fi

        mount_options="$options"
        [[ -n "$mount_options" && "$mount_options" != "defaults" ]] || mount_options=""
        # The recovery mode is authoritative even when fstab contains ro/rw.
        IFS=',' read -ra option_parts <<< "$mount_options"
        filtered_options=""
        for option in "${option_parts[@]}"; do
            [[ -n "$option" ]] || continue
            case "$option" in
                ro|rw|auto|noauto|nofail|_netdev|x-systemd.*) continue ;;
            esac
            filtered_options+="${filtered_options:+,}$option"
        done
        mount_options="$requested_mode${filtered_options:+,$filtered_options}"

        mkdir -p -- "$dest"
        log "Mounting target $mountpoint_name from $resolved ($requested_mode)" | tee -a "$SESSION_LOG"
        mount_recorded "$resolved" "$dest" -o "$mount_options"
        TARGET_DATA_MOUNTS+=("$dest")
    done < <(
        awk '
            /^[[:space:]]*#/ { next }
            NF >= 4 && $3 == "btrfs" && $2 != "/" {
                depth=$2; n=gsub("/", "/", depth);
                print n "\t" $1 "\t" $2 "\t" $3 "\t" $4
            }
        ' "$TARGET_ROOT/etc/fstab" | sort -n -k1,1
    )
}

remount_target_data_rw()
{
    local path
    for path in "${TARGET_DATA_MOUNTS[@]:-}"; do
        [[ -n "$path" && -d "$path" ]] || continue
        mountpoint -q "$path" 2>/dev/null || continue
        log "Remounting target data subvolume ${path#"$TARGET_ROOT"} read-write" | tee -a "$SESSION_LOG"
        mount -o remount,rw "$path"
    done
}

find_btrfs_root()
{
    local base="$1" candidate
    [[ -f "$base/etc/os-release" ]] && { printf '%s\n' "$base"; return 0; }

    while IFS= read -r candidate; do
        [[ -f "$candidate/etc/os-release" ]] || continue
        printf '%s\n' "$candidate"
        return 0
    done < <(find "$base" -mindepth 1 -maxdepth 3 \( -type f -o -type l \) -path '*/etc/os-release' -printf '%h\n' 2>/dev/null | sed 's#/etc$##')

    return 1
}

# Linux-capable root filesystem types, mirroring the GUI's
# preferredRepairNode() candidate rule: ext2/3/4, xfs, btrfs and f2fs. Swap,
# ESP vfat, LUKS containers, ntfs/exfat and unformatted partitions can never
# host an installed Linux root and are therefore never fallback candidates.
is_linux_root_fstype()
{
    case "$1" in
        ext2|ext3|ext4|xfs|btrfs|f2fs) return 0 ;;
    esac
    return 1
}

# Candidate partitions of one target disk for the os-release confirmation
# fallback. Linux-capable filesystem types only; the selected component and
# partitions that are already mounted are excluded. Output is
# "<bytes>\t<hint>\t<path>\t<fstype>" so the caller can order largest first
# with root-label hints as the deterministic tie-break.
target_root_candidates()
{
    local disk="$1" exclude="$2" record key value rest name fstype size type label partlabel hint mountpoints
    while IFS= read -r record; do
        [[ -n "$record" ]] || continue
        name=""; fstype=""; size=""; type=""; label=""; partlabel=""
        rest="$record"
        while [[ "$rest" =~ ([A-Z]+)=\"([^\"]*)\" ]]; do
            key="${BASH_REMATCH[1]}"
            value="${BASH_REMATCH[2]}"
            case "$key" in
                NAME) name="$value" ;;
                FSTYPE) fstype="$value" ;;
                SIZE) size="$value" ;;
                TYPE) type="$value" ;;
                LABEL) label="$value" ;;
                PARTLABEL) partlabel="$value" ;;
            esac
            rest="${rest#*"${BASH_REMATCH[0]}"}"
        done
        [[ "$type" == "part" ]] || continue
        [[ -n "$name" && "$name" != "$exclude" ]] || continue
        is_linux_root_fstype "$fstype" || continue
        [[ "$size" =~ ^[0-9]+$ ]] || continue
        # A partition already mounted anywhere must not be probed or reused.
        mountpoints="$(lsblk -nro MOUNTPOINTS "$name" 2>/dev/null | tr -d '[:space:]')"
        [[ -n "$mountpoints" ]] && continue
        hint=0
        label="$(printf '%s %s' "$label" "$partlabel" | tr '[:upper:]' '[:lower:]')"
        [[ "$label" == *root* ]] && hint=2
        [[ "$label" == *boot* && "$label" != *root* ]] && hint=-1
        printf '%s\t%s\t%s\t%s\n' "$size" "$hint" "$name" "$fstype"
    done < <(lsblk -P -b -p -o NAME,FSTYPE,SIZE,TYPE,LABEL,PARTLABEL "$disk" 2>/dev/null)
}

# True when a read-only probe mount of one Linux-capable component exposes
# /etc/os-release, the installed-root evidence. The probe mount is always
# removed again; a mount failure is not evidence and returns 1.
component_has_os_release()
{
    local dev="$1" fstype="$2" probe_dir probe_options="ro" found=1
    case "$fstype" in
        ext2|ext3|ext4) probe_options="ro,noload" ;;
        xfs) probe_options="ro,norecovery" ;;
    esac
    probe_dir="$(mktemp -d "$SESSION_DIR/root-probe.XXXXXX")" || return 1
    if ! mount_recorded "$dev" "$probe_dir" -o "$probe_options" 2>/dev/null; then
        rmdir "$probe_dir" 2>/dev/null || true
        return 1
    fi
    [[ -f "$probe_dir/etc/os-release" ]] && found=0
    umount "$probe_dir" 2>/dev/null || true
    rmdir "$probe_dir" 2>/dev/null || true
    return "$found"
}

# Confirms the selected root component: when it carries /etc/os-release this
# function is not called. Otherwise it probes the other Linux-capable
# partitions of the same target disk, largest first, and prints the first one
# with os-release evidence. On failure it prints a comma-separated candidate
# list and returns 1 so the caller can name every candidate in the refusal.
resolve_target_root_component()
{
    local disk="$1" exclude="$2" entry size hint name fstype candidates="" resolved=""
    local -a ordered=()

    mapfile -t ordered < <(target_root_candidates "$disk" "$exclude" | sort -rn -k1,1 -k2,2)
    for entry in "${ordered[@]:-}"; do
        [[ -n "$entry" ]] || continue
        IFS=$'\t' read -r size hint name fstype <<< "$entry"
        candidates+="${candidates:+, }$name ($fstype)"
    done
    for entry in "${ordered[@]:-}"; do
        [[ -n "$entry" ]] || continue
        IFS=$'\t' read -r size hint name fstype <<< "$entry"
        if component_has_os_release "$name" "$fstype"; then
            resolved="$name"
            break
        fi
    done

    if [[ -z "$resolved" ]]; then
        printf '%s\n' "$candidates"
        return 1
    fi
    printf '%s\n' "$resolved"
    return 0
}

read_target_os()
{
    [[ -f "$TARGET_ROOT/etc/os-release" ]] || fail "Mounted filesystem does not contain /etc/os-release; root selection is not confirmed."

    TARGET_OS_ID="$(awk -F= '$1=="ID" {gsub(/^"|"$/, "", $2); print tolower($2); exit}' "$TARGET_ROOT/etc/os-release")"
    TARGET_OS_LIKE="$(awk -F= '$1=="ID_LIKE" {gsub(/^"|"$/, "", $2); print tolower($2); exit}' "$TARGET_ROOT/etc/os-release")"
    TARGET_PRETTY="$(awk -F= '$1=="PRETTY_NAME" {sub(/^[^=]*=/, ""); gsub(/^"|"$/, ""); print; exit}' "$TARGET_ROOT/etc/os-release")"
    [[ -n "$TARGET_PRETTY" ]] || TARGET_PRETTY="${TARGET_OS_ID:-Unknown Linux}"
}

is_debian_family()
{
    [[ "$TARGET_OS_ID" =~ ^(debian|ubuntu|tuxedo|linuxmint|pop)$ ]] || [[ " $TARGET_OS_LIKE " == *" debian "* ]] || [[ " $TARGET_OS_LIKE " == *" ubuntu "* ]]
}

is_arch_family()
{
    [[ "$TARGET_OS_ID" =~ ^(arch|manjaro|endeavouros|garuda|artix)$ ]] \
        || [[ " $TARGET_OS_LIKE " == *" arch "* ]]
}

is_alpine_family()
{
    [[ "$TARGET_OS_ID" == alpine ]] \
        || [[ " $TARGET_OS_LIKE " == *" alpine "* ]]
}

target_has_executable()
{
    local path
    for path in "$@"; do
        [[ -x "$TARGET_ROOT$path" ]] && return 0
    done
    return 1
}

target_has_path()
{
    local path
    for path in "$@"; do
        [[ -e "$TARGET_ROOT$path" ]] && return 0
    done
    return 1
}

# Read-only Debian package-state evidence from the target dpkg database.  The
# target is not chrooted while the backend profile is built, so this reads the
# database file directly instead of invoking dpkg-query inside the target.
target_dpkg_package_installed()
{
    local pkg="$1" status_file="$TARGET_ROOT/var/lib/dpkg/status"
    [[ -f "$status_file" ]] || return 1
    awk -v pkg="$pkg" '
        BEGIN { RS = ""; FS = "\n"; found = 0 }
        {
            name = ""; state = ""
            for (i = 1; i <= NF; i++) {
                if ($i ~ /^Package: /) name = substr($i, 10)
                else if ($i ~ /^Status: /) state = substr($i, 9)
            }
            if (name == pkg && state == "install ok installed") { found = 1; exit }
        }
        END { exit(found ? 0 : 1) }
    ' "$status_file"
}

# Read-only apk package-state evidence for an Alpine target.  Native host
# maintenance queries the live apk; a non-running target is inspected through
# its installed database file so no target command is executed.
target_apk_package_installed()
{
    local pkg="$1" db="$TARGET_ROOT/lib/apk/db/installed"
    [[ -n "$pkg" ]] || return 1
    if (( RUNNING_HOST_MODE == 1 )) && command -v apk >/dev/null 2>&1; then
        apk info -e "$pkg" >/dev/null 2>&1
        return $?
    fi
    [[ -f "$db" ]] || return 1
    awk -v pkg="$pkg" '$0 == "P:" pkg { found = 1; exit } END { exit(found ? 0 : 1) }' "$db"
}

# Debian-family initramfs generator evidence.  initramfs-tools is considered
# installed only with real package evidence: the dpkg database, its package
# data directory, or its configuration directory.  Loose executables alone are
# not sufficient, because dracut and vendor tools can ship compatible stubs.
target_initramfs_tools_installed()
{
    target_dpkg_package_installed initramfs-tools \
        || target_dpkg_package_installed initramfs-tools-core \
        || target_has_path /usr/share/initramfs-tools /etc/initramfs-tools
}

# dracut counts as installed/configured only with generator evidence.  A lone
# /usr/bin/dracut binary (for example a leftover from dracut-core pulled in as
# a dependency) is not enough to override a real initramfs-tools installation.
target_dracut_installed()
{
    target_has_path /usr/lib/dracut /etc/dracut.conf /etc/dracut.conf.d \
        || target_dpkg_package_installed dracut
}

# Read-only OpenRC probe for Alpine-family targets: the service manager
# executable or its init.d/runlevels tree is enough evidence, and nothing is
# executed.
alpine_openrc_present()
{
    target_has_executable /sbin/openrc /usr/sbin/openrc /usr/bin/openrc \
        || target_has_path /etc/init.d /etc/runlevels
}

# Read-only Alpine display-manager probe.  OpenRC services are configured
# through /etc/init.d and /etc/conf.d, not through systemd units or the
# display-manager.service alias.  Prints the detected service name.
alpine_display_manager_present()
{
    local candidate
    for candidate in lightdm slim gdm sddm lxdm; do
        if target_has_executable "/etc/init.d/$candidate" \
            || target_has_path "/etc/conf.d/$candidate"; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done
    return 1
}

# Enumerate the OpenRC init scripts that declare "provide display-manager".
# This is the OpenRC equivalent of the systemd display-manager.service alias;
# it is read-only and never executes a target service.
alpine_display_manager_services()
{
    local script
    [[ -d "$TARGET_ROOT/etc/init.d" ]] || return 0
    for script in "$TARGET_ROOT"/etc/init.d/*; do
        [[ -f "$script" ]] || continue
        if grep -Eq '^[[:space:]]*provide[[:space:]]+[^#]*display-manager([[:space:]]|$)' \
            "$script" 2>/dev/null; then
            basename -- "$script"
        fi
    done
}

# Print every OpenRC runlevel in which the service is enabled.  The enabled
# state is read from the /etc/runlevels/* symlinks, so a non-running repair
# target is inspected without executing target commands.
alpine_service_enabled_runlevels()
{
    local service="$1" runlevel_dir
    [[ -d "$TARGET_ROOT/etc/runlevels" ]] || return 0
    for runlevel_dir in "$TARGET_ROOT"/etc/runlevels/*; do
        [[ -d "$runlevel_dir" ]] || continue
        # OpenRC runlevel entries are symlinks to /etc/init.d/<service>; for a
        # non-running target the absolute link target only resolves inside the
        # target root, so the link itself is the enabled-state evidence.
        [[ -e "$runlevel_dir/$service" || -L "$runlevel_dir/$service" ]] || continue
        basename -- "$runlevel_dir"
    done
}

# Read-only apk package state for an Alpine target.  The running host is
# queried through apk itself; a non-running target is inspected through its
# installed database file so no target command is executed.  Prints
# "installed" or "not installed".
alpine_package_state()
{
    local package="$1" db="$TARGET_ROOT/lib/apk/db/installed"
    [[ -n "$package" ]] || { printf 'unknown'; return 0; }
    if (( RUNNING_HOST_MODE == 1 )) && command -v apk >/dev/null 2>&1; then
        if apk info -e "$package" >/dev/null 2>&1; then
            printf 'installed'
        else
            printf 'not installed'
        fi
        return 0
    fi
    if [[ -f "$db" ]] && awk -v pkg="$package" \
        '$0 == "P:" pkg { found = 1; exit } END { exit(found ? 0 : 1) }' "$db"; then
        printf 'installed'
    else
        printf 'not installed'
    fi
}

# Alpine kernel and initramfs image names under /boot (vmlinuz-lts, -virt,
# -edge or versioned flavors and their initramfs-* counterparts).  Read-only;
# used by the backend profile, capability evidence and kernel diagnostics.
alpine_kernel_images()
{
    [[ -d "$TARGET_ROOT/boot" ]] || return 0
    # sed is used instead of GNU find -printf so the probe also works with the
    # BusyBox find shipped by a base Alpine system.
    find "$TARGET_ROOT/boot" -maxdepth 1 \( -type f -o -type l \) -name 'vmlinuz-*' \
        -print 2>/dev/null | sed 's#.*/##' | sort -V || true
}

alpine_initramfs_images()
{
    [[ -d "$TARGET_ROOT/boot" ]] || return 0
    find "$TARGET_ROOT/boot" -maxdepth 1 \( -type f -o -type l \) -name 'initramfs-*' \
        -print 2>/dev/null | sed 's#.*/##' | sort -V || true
}

# Pair every installed Alpine kernel module directory with its /boot images.
# Real Alpine kernel packages write the flavor with a leading dash (the
# kernel-suffix file contains "-lts\n", "-virt\n" or "-edge\n"), so the suffix
# is normalized by stripping every leading dash before the boot images are
# resolved; a suffix written without the dash keeps working.  Kernels with an
# empty or absent kernel-suffix fall back to the module directory name only
# when a matching vmlinuz-<kver> exists, otherwise they are skipped (fail
# closed).  Prints
# "<kver> <suffix> /boot/vmlinuz-<suffix> /boot/initramfs-<suffix>".
# BusyBox find is used, so GNU -printf is deliberately avoided.
alpine_kernel_pairs()
{
    local root candidate kver suffix
    local -a roots=(/lib/modules /usr/lib/modules)
    for root in "${roots[@]}"; do
        [[ -d "$TARGET_ROOT$root" ]] || continue
        for candidate in "$TARGET_ROOT$root"/*; do
            [[ -d "$candidate" ]] || continue
            kver="$(basename -- "$candidate")"
            [[ "$kver" =~ ^[[:alnum:]][[:alnum:].+_-]*$ ]] || continue
            suffix=""
            if [[ -r "$candidate/kernel-suffix" ]]; then
                suffix="$(tr -d '[:space:]' < "$candidate/kernel-suffix" 2>/dev/null | head -c 64 || true)"
                # "-lts" -> "lts", "-virt" -> "virt", "--edge" -> "edge".
                while [[ "$suffix" == -* ]]; do
                    suffix="${suffix#-}"
                done
            fi
            if [[ -n "$suffix" && -f "$TARGET_ROOT/boot/vmlinuz-$suffix" ]]; then
                printf '%s %s /boot/vmlinuz-%s /boot/initramfs-%s\n' \
                    "$kver" "$suffix" "$suffix" "$suffix"
            elif [[ -f "$TARGET_ROOT/boot/vmlinuz-$kver" ]]; then
                printf '%s %s /boot/vmlinuz-%s /boot/initramfs-%s\n' \
                    "$kver" "$kver" "$kver" "$kver"
            fi
        done
    done | sort -V -u
}

# ---------------------------------------------------------------------------
# Alpine EFI backend probes (read-only target evidence)
# ---------------------------------------------------------------------------
# True when the recovery host booted through UEFI firmware.  Indirected so the
# contract tests can exercise both firmware modes without touching the host.
alpine_efi_firmware_available()
{
    [[ -d /sys/firmware/efi ]]
}

# Alpine EFI loader path probes.  Each one mirrors a mandatory preflight check
# of the guarded Alpine GRUB EFI stage so capability evidence and repair
# preflight cannot drift apart.
alpine_grub_install_present()
{
    target_has_executable /usr/sbin/grub-install /usr/bin/grub-install
}

alpine_grub_module_dir_present()
{
    target_has_path /usr/lib/grub/x86_64-efi
}

alpine_grub_config_tool_present()
{
    target_has_executable /usr/sbin/grub-mkconfig /usr/bin/grub-mkconfig \
        /usr/sbin/update-grub /usr/bin/update-grub
}

# Classify the detected Alpine EFI boot path from target evidence only:
#   grub         - the detected bootloader backend is GRUB (UEFI or BIOS)
#   efi-stub     - no GRUB; vmlinuz-* and initramfs-* live at the ESP root
#   syslinux-efi - the ESP carries EFI/syslinux/syslinux.efi
#   none         - no recognised EFI boot path
alpine_efi_backend()
{
    local esp_root
    # Profile in this shell (not a command substitution) so the detected
    # bootloader and ESP globals are visible to the caller afterwards.
    profile_target_backends
    if [[ "${TARGET_BOOTLOADER_BACKEND:-}" == grub ]]; then
        printf 'grub\n'
        return 0
    fi
    if [[ "${TARGET_ESP_MOUNT:-unresolved}" == unresolved ]]; then
        printf 'none\n'
        return 0
    fi
    esp_root="$TARGET_ROOT$TARGET_ESP_MOUNT"
    if [[ -f "$esp_root/EFI/syslinux/syslinux.efi" ]]; then
        printf 'syslinux-efi\n'
        return 0
    fi
    # A kernel at the ESP root is the defining EFI-stub artifact.  The stage
    # and capability probes separately require the initramfs image so a broken
    # stub layout fails closed with a precise reason instead of "no EFI path".
    if compgen -G "$esp_root/vmlinuz-*" >/dev/null 2>&1; then
        printf 'efi-stub\n'
        return 0
    fi
    printf 'none\n'
}

# ESP-root kernel and initramfs image names used by the EFI-stub layout.
alpine_efi_esp_kernel_images()
{
    local esp_root
    esp_root="$(profile_esp_root 2>/dev/null || true)"
    [[ -n "$esp_root" ]] || return 0
    find "$esp_root" -maxdepth 1 -type f -name 'vmlinuz-*' -print 2>/dev/null \
        | sed 's#.*/##' | LC_ALL=C sort -V || true
}

alpine_efi_esp_initramfs_images()
{
    local esp_root
    esp_root="$(profile_esp_root 2>/dev/null || true)"
    [[ -n "$esp_root" ]] || return 0
    find "$esp_root" -maxdepth 1 -type f -name 'initramfs-*' -print 2>/dev/null \
        | sed 's#.*/##' | LC_ALL=C sort -V || true
}

# Firmware entries on one ESP PARTUUID whose loader is an EFI-stub kernel
# (vmlinuz-*) and whose loader file actually exists at the ESP root.  Read-only;
# used by the Alpine EFI-stub capability evidence and reconciliation preflight.
alpine_efi_stub_entry_ids_for_partuuid()
{
    local partuuid="${1,,}" line part loader relative id
    local esp_root
    command -v efibootmgr >/dev/null 2>&1 || return 1
    [[ -n "$partuuid" ]] || return 1
    esp_root="$(profile_esp_root 2>/dev/null || true)"
    [[ -n "$esp_root" ]] || return 1
    while IFS= read -r line; do
        part="$(efi_entry_partuuid_line "$line")"
        [[ "$part" == "$partuuid" ]] || continue
        loader="$(efi_entry_loader_line "$line" || true)"
        [[ "${loader##*\\}" == vmlinuz-* ]] || continue
        relative="${loader//\\//}"
        [[ -f "$esp_root$relative" ]] || continue
        id="$(efi_entry_id_line "$line")"
        [[ -n "$id" ]] && printf '%s\n' "${id^^}"
    done < <(efibootmgr -v 2>/dev/null | sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p')
}

target_distro_family()
{
    if is_debian_family; then
        printf '%s\n' "debian"
    elif is_arch_family; then
        printf '%s\n' "arch"
    elif is_alpine_family; then
        printf '%s\n' "alpine"
    elif [[ "$TARGET_OS_ID" =~ ^(fedora|rhel|rocky|almalinux)$ ]] \
        || [[ " $TARGET_OS_LIKE " == *" fedora "* ]] \
        || [[ " $TARGET_OS_LIKE " == *" rhel "* ]]; then
        printf '%s\n' "fedora"
    elif [[ "$TARGET_OS_ID" =~ ^(opensuse|opensuse-tumbleweed|suse)$ ]] \
        || [[ " $TARGET_OS_LIKE " == *" suse "* ]]; then
        printf '%s\n' "suse"
    else
        printf '%s\n' "unknown"
    fi
}

# ---------------------------------------------------------------------------
# Backend probes (read-only target evidence, never distribution-family gates)
# ---------------------------------------------------------------------------
# A "detected" backend has enough evidence to be considered in use on the
# target; a "ready" backend additionally passes the mandatory prerequisites its
# guarded stage re-checks at run time.  The family is never consulted here.

target_dpkg_detected()
{
    target_has_executable /usr/bin/dpkg /usr/sbin/dpkg /bin/dpkg \
        /usr/bin/dpkg-query /usr/sbin/dpkg-query /bin/dpkg-query \
        || [[ -f "$TARGET_ROOT/var/lib/dpkg/status" ]]
}

target_dpkg_ready()
{
    [[ -f "$TARGET_ROOT/var/lib/dpkg/status" ]] \
        && target_has_executable /usr/bin/dpkg /usr/sbin/dpkg /bin/dpkg
}

target_apt_sources_present()
{
    [[ -s "$TARGET_ROOT/etc/apt/sources.list" ]] && return 0
    compgen -G "$TARGET_ROOT/etc/apt/sources.list.d/*.list" >/dev/null 2>&1 && return 0
    compgen -G "$TARGET_ROOT/etc/apt/sources.list.d/*.sources" >/dev/null 2>&1 && return 0
    return 1
}

target_apt_detected()
{
    target_has_executable /usr/bin/apt-get /usr/sbin/apt-get /bin/apt-get \
        || target_apt_sources_present
}

target_apt_ready()
{
    target_has_executable /usr/bin/apt-get /usr/sbin/apt-get /bin/apt-get \
        && target_dpkg_ready
}

target_apk_detected()
{
    target_has_executable /sbin/apk /usr/sbin/apk /usr/bin/apk \
        || [[ -f "$TARGET_ROOT/lib/apk/db/installed" ]] \
        || [[ -f "$TARGET_ROOT/etc/apk/world" ]]
}

target_pacman_detected()
{
    target_has_executable /usr/bin/pacman /usr/bin/pacman-static \
        || [[ -d "$TARGET_ROOT/var/lib/pacman" ]] \
        || [[ -f "$TARGET_ROOT/etc/pacman.conf" ]]
}

target_rpm_detected()
{
    target_has_executable /usr/bin/rpm /bin/rpm /usr/sbin/rpm \
        || [[ -d "$TARGET_ROOT/var/lib/rpm" ]] \
        || [[ -d "$TARGET_ROOT/usr/lib/sysimage/rpm" ]]
}

# A usable rpm database is the sqlite backend rpm 4.16+/6 uses.  A legacy-only
# tree (Berkeley DB `Packages`) fails closed instead of being handed to dnf5,
# which requires the sqlite rpmdb.
rpm_database_present()
{
    [[ -s "$TARGET_ROOT/usr/lib/sysimage/rpm/rpmdb.sqlite" ]] \
        || [[ -s "$TARGET_ROOT/var/lib/rpm/rpmdb.sqlite" ]]
}

# dnf4 ships /usr/bin/dnf4 -> dnf-3; the guarded backend only supports dnf5.
rpm_dnf4_present()
{
    target_has_executable /usr/bin/dnf4 /usr/sbin/dnf4 /bin/dnf4 \
        /usr/bin/dnf-3 /usr/sbin/dnf-3 /bin/dnf-3
}

# Resolve the guarded dnf5 command from target evidence: the explicit dnf5
# binary when present, otherwise a `dnf` symlink that resolves to dnf5.  dnf4
# and dnf-3 are never selected.
rpm_dnf_tool()
{
    local dnf link
    if target_has_executable /usr/bin/dnf5 /usr/sbin/dnf5 /bin/dnf5; then
        printf 'dnf5\n'
        return 0
    fi
    for dnf in /usr/bin/dnf /usr/sbin/dnf /bin/dnf; do
        [[ -x "$TARGET_ROOT$dnf" ]] || continue
        link="$(readlink "$TARGET_ROOT$dnf" 2>/dev/null || true)"
        if [[ "$(basename -- "$link")" == dnf5 ]]; then
            printf 'dnf\n'
            return 0
        fi
    done
    return 1
}

# Count enabled repository sections in the target's /etc/yum.repos.d files.
# A section without an explicit `enabled=` line is enabled by dnf's default.
rpm_enabled_repo_count()
{
    local file repo_count count=0
    while IFS= read -r file; do
        [[ -n "$file" ]] || continue
        repo_count="$(awk '
            /^[[:space:]]*\[[^]]+\]/ {
                if (in_section && enabled) count++
                in_section = 1
                enabled = 1
                next
            }
            in_section && /^[[:space:]]*enabled[[:space:]]*=/ {
                value = $0
                sub(/^[^=]*=/, "", value)
                gsub(/[[:space:]]/, "", value)
                value = tolower(value)
                if (value == "0" || value == "false" || value == "no" || value == "off") enabled = 0
            }
            END { if (in_section && enabled) count++; print count + 0 }
        ' "$file" 2>/dev/null || true)"
        [[ "$repo_count" =~ ^[0-9]+$ ]] || repo_count=0
        count=$(( count + repo_count ))
    done < <(find "$TARGET_ROOT/etc/yum.repos.d" -maxdepth 1 -type f -name '*.repo' 2>/dev/null | LC_ALL=C sort)
    printf '%s\n' "$count"
}

rpm_repositories_present()
{
    (( $(rpm_enabled_repo_count) > 0 ))
}

# A ready rpm backend additionally has the sqlite rpmdb and a dnf5 command
# (explicit binary or a dnf symlink resolving to dnf5).  Repository
# prerequisites are checked by the per-stage preflight and the capability
# reason, mirroring target_apt_ready.
target_rpm_ready()
{
    target_has_executable /usr/bin/rpm /bin/rpm /usr/sbin/rpm \
        && rpm_database_present \
        && rpm_dnf_tool >/dev/null
}

# Compatibility alias for the research/plan name.
target_dnf5_ready()
{
    target_rpm_ready
}

# Read-only rpm package-state evidence.  The target is chrooted only while a
# modifying stage runs; capability probes use the file-level checks above.
target_rpm_package_installed()
{
    local package="$1"
    [[ -n "$package" ]] || return 1
    target_has_executable /usr/bin/rpm /bin/rpm /usr/sbin/rpm || return 1
    run_selected_chroot /usr/bin/env LC_ALL=C PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
        rpm -q --qf '%{NAME}\n' "$package" 2>/dev/null | grep -Fxq "$package"
}

# Deterministic native-first package-manager ordering.  The family is only an
# ordering hint: every entry is detected from target evidence and every stage
# probes the detected managers for applicability.
target_package_manager_backend_order()
{
    case "${TARGET_DISTRO_FAMILY:-unknown}" in
        arch)   printf '%s\n' pacman apt/dpkg apk rpm ;;
        alpine) printf '%s\n' apk apt/dpkg pacman rpm ;;
        fedora) printf '%s\n' rpm apt/dpkg apk pacman ;;
        *)      printf '%s\n' apt/dpkg apk pacman rpm ;;
    esac
}

target_package_manager_backend_detected()
{
    case "$1" in
        apt/dpkg) target_apt_detected ;;
        apk) target_apk_detected ;;
        pacman) target_pacman_detected ;;
        rpm) target_rpm_detected ;;
        *) return 1 ;;
    esac
}

target_systemd_present()
{
    if (( RUNNING_HOST_MODE == 1 )) && [[ -d /run/systemd/system ]]; then
        return 0
    fi
    target_has_executable /usr/lib/systemd/systemd /lib/systemd/systemd \
        /usr/bin/systemctl /bin/systemctl
}

openrc_present()
{
    target_has_executable /sbin/openrc /usr/sbin/openrc /usr/bin/openrc \
        || target_has_executable /sbin/rc-update /usr/sbin/rc-update /usr/bin/rc-update \
            /sbin/rc-service /usr/sbin/rc-service /usr/bin/rc-service \
        || target_has_path /etc/runlevels
}

# Compatibility alias kept for callers/tests that grew around the Alpine-only
# name; the probe itself is service-manager evidence, not family evidence.
alpine_openrc_present()
{
    openrc_present
}

target_sysvinit_present()
{
    target_has_path /etc/init.d \
        && ! target_systemd_present \
        && ! openrc_present
}

target_service_manager_backends()
{
    target_systemd_present && printf 'systemd\n'
    openrc_present && printf 'OpenRC\n'
    target_sysvinit_present && printf 'sysvinit\n'
    return 0
}

# Print the backend that owns the target's graphical login, in evidence order:
# a configured systemd display-manager alias/unit, an OpenRC service that
# provides display-manager, or a sysvinit script.
target_display_manager_backend()
{
    local systemd_dm=""
    if [[ -L "$TARGET_ROOT/etc/systemd/system/display-manager.service" ]]; then
        printf 'systemd\n'
        return 0
    fi
    if [[ -f "$TARGET_ROOT/usr/lib/systemd/system/graphical.target" || -f "$TARGET_ROOT/lib/systemd/system/graphical.target" ]]; then
        systemd_dm="$(systemd_display_manager_present 2>/dev/null || true)"
        if [[ -n "$systemd_dm" ]]; then
            printf 'systemd\n'
            return 0
        fi
    fi
    if [[ -n "$(alpine_display_manager_services)" || -n "$(alpine_display_manager_present 2>/dev/null || true)" ]]; then
        printf 'OpenRC\n'
        return 0
    fi
    if target_has_path /etc/init.d/lightdm /etc/init.d/slim /etc/init.d/gdm \
        /etc/init.d/gdm3 /etc/init.d/sddm /etc/init.d/lxdm; then
        printf 'sysvinit\n'
        return 0
    fi
    printf 'none\n'
}

target_initramfs_backend_detected()
{
    case "$1" in
        mkinitfs)
            target_has_executable /sbin/mkinitfs /usr/sbin/mkinitfs /usr/bin/mkinitfs \
                || target_has_path /etc/mkinitfs ;;
        mkinitcpio)
            target_has_executable /usr/bin/mkinitcpio /usr/sbin/mkinitcpio \
                || target_has_path /etc/mkinitcpio.conf /etc/mkinitcpio.d ;;
        initramfs-tools)
            target_initramfs_tools_installed ;;
        dracut)
            target_dracut_installed \
                || target_has_executable /usr/bin/dracut /usr/sbin/dracut ;;
        booster)
            target_has_executable /usr/bin/booster /usr/lib/booster/booster ;;
        *) return 1 ;;
    esac
}

# Deterministic native-first initramfs ordering.  The family is only a hint;
# the repair dispatch supports mkinitfs, mkinitcpio and initramfs-tools and
# fails closed for every other detected backend.
target_initramfs_backend_order()
{
    case "${TARGET_DISTRO_FAMILY:-unknown}" in
        arch)   printf '%s\n' mkinitcpio mkinitfs dracut initramfs-tools booster ;;
        alpine) printf '%s\n' mkinitfs mkinitcpio initramfs-tools dracut booster ;;
        debian) printf '%s\n' initramfs-tools dracut booster mkinitfs mkinitcpio ;;
        fedora) printf '%s\n' dracut initramfs-tools mkinitfs mkinitcpio booster ;;
        *)      printf '%s\n' mkinitfs mkinitcpio initramfs-tools dracut booster ;;
    esac
}

# A persistent journal directory alone is not evidence: systemd creates
# /var/log/journal even when it holds no journal files, and journalctl then
# answers "No journal files were found."  Require at least one journal file for
# a mounted target; the running host keeps the live journalctl path (volatile
# /run/log/journal is valid there).
target_journal_evidence_present()
{
    local dir="$TARGET_ROOT/var/log/journal" file
    [[ -d "$dir" ]] || return 1
    (( ${RUNNING_HOST_MODE:-0} == 1 )) && return 0
    for file in "$dir"/*/*.journal "$dir"/*/*.journal~ "$dir"/*.journal; do
        [[ -f "$file" ]] && return 0
    done
    return 1
}

# Print the detected logging source.  journald is authoritative when its
# persistent journal exists; otherwise the probe names the plain-text source
# (syslog-ng, BusyBox syslog, /var/log/messages|syslog, dmesg) so diagnostics
# never blame the recovery host for a target that simply has no journal.
target_logging_backend()
{
    if (( RUNNING_HOST_MODE == 1 )); then
        if command -v journalctl >/dev/null 2>&1 && [[ -d /var/log/journal ]]; then
            printf 'journald\n'
            return 0
        fi
    elif target_journal_evidence_present && command -v journalctl >/dev/null 2>&1; then
        printf 'journald\n'
        return 0
    fi
    if [[ -f "$TARGET_ROOT/etc/syslog-ng/syslog-ng.conf" || -f "$TARGET_ROOT/etc/syslog-ng.conf" ]] \
        || target_has_executable /usr/sbin/syslog-ng /sbin/syslog-ng /usr/bin/syslog-ng; then
        printf 'syslog-ng\n'
        return 0
    fi
    if target_has_executable /sbin/syslogd /usr/sbin/syslogd /usr/bin/syslogd \
        || target_has_path /etc/init.d/syslog /etc/init.d/syslogd /etc/conf.d/syslog; then
        printf 'busybox-syslog\n'
        return 0
    fi
    if [[ -r "$TARGET_ROOT/var/log/messages" || -r "$TARGET_ROOT/var/log/syslog" ]]; then
        printf 'messages\n'
        return 0
    fi
    if (( RUNNING_HOST_MODE == 1 )) && command -v dmesg >/dev/null 2>&1; then
        printf 'dmesg\n'
        return 0
    fi
    if [[ -r "$TARGET_ROOT/var/log/dmesg" ]]; then
        printf 'dmesg\n'
        return 0
    fi
    printf 'none\n'
}

non_journald_log_fallback_ready()
{
    if (( RUNNING_HOST_MODE == 1 )) && command -v dmesg >/dev/null 2>&1; then
        return 0
    fi
    case "$(target_logging_backend)" in
        syslog-ng|busybox-syslog|messages|dmesg) return 0 ;;
        *) return 1 ;;
    esac
}

# Compatibility alias: the fallback is "any non-journal logging source", not
# an OpenRC-only concept.
openrc_log_fallback_ready()
{
    non_journald_log_fallback_ready
}

# Human-readable name of the detected plain-text logging source for
# diagnostic section labels.
non_journald_log_source_label()
{
    case "$(target_logging_backend)" in
        syslog-ng) printf 'syslog-ng' ;;
        busybox-syslog) printf 'BusyBox syslog' ;;
        messages) printf 'syslog' ;;
        dmesg) printf 'dmesg' ;;
        *) printf 'syslog' ;;
    esac
}

# Join arguments with ", " for human-readable backend lists.
join_comma()
{
    local first=1 item
    for item in "$@"; do
        [[ -n "$item" ]] || continue
        if (( first )); then
            printf '%s' "$item"
            first=0
        else
            printf ', %s' "$item"
        fi
    done
}

# The family-native package manager is a wording/ordering hint only.
native_package_manager_for_family()
{
    case "${TARGET_DISTRO_FAMILY:-unknown}" in
        arch) printf 'pacman\n' ;;
        alpine) printf 'apk\n' ;;
        debian) printf 'apt/dpkg\n' ;;
        fedora) printf 'rpm\n' ;;
        *) printf 'none\n' ;;
    esac
}

# Human-readable repair-capability label.  The family is a wording hint only:
# the label names the native profile when its backends were actually detected
# and otherwise describes the detected backend set.
target_repair_backend_label()
{
    if [[ "$TARGET_DISTRO_FAMILY" == arch && "$TARGET_PACKAGE_MANAGER" == pacman ]]; then
        printf '%s\n' "Arch profile — guarded pacman/mkinitcpio/GRUB/EFI repairs when transaction preflights pass"
    elif [[ "$TARGET_DISTRO_FAMILY" == debian && "$TARGET_PACKAGE_MANAGER" == "apt/dpkg" ]]; then
        printf '%s\n' "Debian/APT profile — existing guarded modifying backend"
    elif [[ "$TARGET_DISTRO_FAMILY" == alpine && "$TARGET_PACKAGE_MANAGER" == apk ]]; then
        printf '%s\n' "Alpine profile — guarded apk/OpenRC/mkinitfs/extlinux repairs when their stage-specific preflights pass"
    elif [[ "$TARGET_DISTRO_FAMILY" == fedora && "$TARGET_PACKAGE_MANAGER" == rpm ]]; then
        printf '%s\n' "Fedora profile — guarded rpm/dnf5/dracut repairs when their stage-specific preflights pass"
    else
        printf 'Detected backends — package: %s; service: %s; initramfs: %s; bootloader: %s\n' \
            "$(join_comma "${TARGET_PACKAGE_MANAGERS[@]}")" "${TARGET_SERVICE_MANAGER:-unknown}" \
            "${TARGET_INITRAMFS_BACKEND:-unknown}" "${TARGET_BOOTLOADER_BACKEND:-unknown}"
    fi
}

# Populate the target profile globals (family hint, detected package managers,
# service manager, display backend, initramfs backends, bootloader backend,
# logging source, ESP mount candidate, kernel layout, repair capability) from
# read-only filesystem evidence.  Shared by capability evidence, backend
# diagnostics and the repair preflights; never modifies the target.
profile_target_backends()
{
    local family="${TARGET_DISTRO_FAMILY:-}" esp_path backend
    [[ -n "$family" ]] || family="$(target_distro_family)"
    TARGET_DISTRO_FAMILY="$family"

    TARGET_PACKAGE_MANAGERS=()
    while IFS= read -r backend; do
        [[ -n "$backend" ]] || continue
        if target_package_manager_backend_detected "$backend"; then
            TARGET_PACKAGE_MANAGERS+=("$backend")
        fi
    done < <(target_package_manager_backend_order)
    if ((${#TARGET_PACKAGE_MANAGERS[@]} > 0)); then
        TARGET_PACKAGE_MANAGER="${TARGET_PACKAGE_MANAGERS[0]}"
    else
        TARGET_PACKAGE_MANAGER="unknown"
    fi

    TARGET_SERVICE_MANAGERS=()
    while IFS= read -r backend; do
        [[ -n "$backend" ]] || continue
        TARGET_SERVICE_MANAGERS+=("$backend")
    done < <(target_service_manager_backends)
    if ((${#TARGET_SERVICE_MANAGERS[@]} > 0)); then
        TARGET_SERVICE_MANAGER="${TARGET_SERVICE_MANAGERS[0]}"
    else
        TARGET_SERVICE_MANAGER="unknown"
    fi

    TARGET_DISPLAY_BACKEND="$(target_display_manager_backend)"

    TARGET_INITRAMFS_BACKENDS=()
    while IFS= read -r backend; do
        [[ -n "$backend" ]] || continue
        if target_initramfs_backend_detected "$backend"; then
            TARGET_INITRAMFS_BACKENDS+=("$backend")
        fi
    done < <(target_initramfs_backend_order)
    if ((${#TARGET_INITRAMFS_BACKENDS[@]} > 0)); then
        TARGET_INITRAMFS_BACKEND="${TARGET_INITRAMFS_BACKENDS[0]}"
    else
        TARGET_INITRAMFS_BACKEND="unknown"
    fi

    TARGET_LOGGING_BACKEND="$(target_logging_backend)"

    if target_has_path /boot/grub/grub.cfg \
        || target_has_executable /usr/sbin/grub-mkconfig /usr/bin/grub-mkconfig \
        || target_has_executable /usr/sbin/update-grub /usr/bin/update-grub; then
        TARGET_BOOTLOADER_BACKEND="grub"
    elif target_has_path /boot/grub2/grub.cfg \
        || target_has_executable /usr/sbin/grub2-mkconfig /usr/bin/grub2-mkconfig \
        || target_has_executable /usr/sbin/grub2-install /usr/bin/grub2-install \
        || target_has_path /boot/grub2/i386-pc; then
        # Fedora/RHEL grub2 layout.  Checked before the /boot/loader branch:
        # Fedora's /boot/loader is the GRUB2 BLS directory, not systemd-boot.
        TARGET_BOOTLOADER_BACKEND="grub"
    elif target_has_path /boot/extlinux.conf /boot/syslinux/syslinux.cfg /boot/syslinux.cfg \
            /etc/update-extlinux.conf /boot/syslinux/ldlinux.sys; then
        TARGET_BOOTLOADER_BACKEND="syslinux/extlinux"
    elif target_has_path /efi/loader /efi/loader/loader.conf /boot/loader/loader.conf \
            /boot/efi/loader/loader.conf /boot/EFI/systemd /efi/EFI/systemd \
        || target_has_executable /usr/bin/bootctl; then
        if target_has_path /efi/EFI/Linux /boot/efi/EFI/Linux /boot/EFI/Linux; then
            TARGET_BOOTLOADER_BACKEND="systemd-boot + UKI"
        else
            TARGET_BOOTLOADER_BACKEND="systemd-boot"
        fi
    elif target_has_path /efi/EFI/Linux /boot/efi/EFI/Linux /boot/EFI/Linux; then
        TARGET_BOOTLOADER_BACKEND="generic UKI"
    else
        TARGET_BOOTLOADER_BACKEND="unknown EFI loader"
    fi

    TARGET_ESP_MOUNT=""
    local esp_dir
    for esp_path in /efi /boot/efi /boot; do
        esp_dir="$(target_path "$esp_path")"
        if mountpoint -q "$esp_dir" 2>/dev/null \
            && findmnt -rn -o FSTYPE --target "$esp_dir" 2>/dev/null \
                | grep -Eiq '^(vfat|fat|fat16|fat32|msdos)$'; then
            TARGET_ESP_MOUNT="$esp_path"
            break
        fi
    done
    if [[ -z "$TARGET_ESP_MOUNT" ]]; then
        for esp_path in /efi /boot/efi /boot; do
            esp_dir="$(target_path "$esp_path")"
            if [[ -d "$esp_dir/EFI" || -d "$esp_dir/loader" ]]; then
                TARGET_ESP_MOUNT="$esp_path"
                break
            fi
        done
    fi
    [[ -n "$TARGET_ESP_MOUNT" ]] || TARGET_ESP_MOUNT="unresolved"

    if [[ "$TARGET_INITRAMFS_BACKEND" == mkinitfs ]]; then
        if [[ -n "$(alpine_kernel_images)" ]]; then
            TARGET_KERNEL_LAYOUT="Alpine named kernels (vmlinuz-lts/virt/edge)"
        else
            TARGET_KERNEL_LAYOUT="no conventional vmlinuz files detected"
        fi
    elif find "$TARGET_ROOT/boot" -maxdepth 1 -type f -name 'vmlinuz-linux*' -print -quit 2>/dev/null | grep -q .; then
        TARGET_KERNEL_LAYOUT="Arch-style named kernels (vmlinuz-linux*)"
    elif find "$TARGET_ROOT/boot" -maxdepth 1 -type f -name 'vmlinuz-*' -print -quit 2>/dev/null | grep -q .; then
        TARGET_KERNEL_LAYOUT="versioned vmlinuz-* kernels"
    else
        TARGET_KERNEL_LAYOUT="no conventional vmlinuz files detected"
    fi

    TARGET_REPAIR_BACKEND="$(target_repair_backend_label)"
}

profile_esp_root()
{
    profile_target_backends
    [[ "$TARGET_ESP_MOUNT" != unresolved ]] || return 1
    printf '%s\n' "$TARGET_ROOT$TARGET_ESP_MOUNT"
}

# Identify and mount the selected repair target at MOUNT_BASE.  mode ro mounts
# only the root (and locates/mounts the Btrfs root subvolume); mode rw also
# mounts /boot, the ESP, /dev, /proc, /sys, /run and the resolver.  Sets
# TARGET_ROOT, TARGET_SUBVOL and SESSION_LOG after all safety checks pass.
prepare_target()
{
    local mode="$1" fstype mounted_root raw_target raw_root mount_mode discovered_subvol="" root_mount_options
    local selected_component="" selected_fstype="" fallback_root=""
    need lsblk
    need findmnt
    need mount
    need umount
    need blkid
    need readlink

    [[ "$mode" == "ro" || "$mode" == "rw" ]] || fail "Internal target mount mode error: $mode"
    raw_target="$TARGET_DISK"
    raw_root="$ROOT_DEVICE"
    TARGET_DISK="$(canonical_block "$raw_target")" || fail "Target disk is not a block device: $raw_target"
    ROOT_CANONICAL="$(canonical_block "$raw_root")" || fail "Root component is not a block device: $raw_root"
    ROOT_DEVICE="$(preferred_block_path "$raw_root" "$ROOT_CANONICAL")"

    assert_target_not_host "$TARGET_DISK"
    same_single_top_disk "$TARGET_DISK" "$ROOT_CANONICAL" || fail "Selected root component does not belong exclusively to target disk."

    fstype="$(lsblk -ndo FSTYPE "$ROOT_CANONICAL" 2>/dev/null | head -n1)"
    [[ "$fstype" != "crypto_LUKS" ]] || fail "The selected root component is still LUKS-encrypted. Unlock it first, then refresh and select the mapped Linux filesystem."
    [[ -n "$fstype" ]] || fail "No filesystem was detected on the selected root component."

    SESSION_DIR="$(mktemp -d "$STATE_ROOT/session.XXXXXX")"
    MOUNT_BASE="$SESSION_DIR/mount"
    SESSION_LOG="$SESSION_DIR/session.log"
    mkdir -p "$MOUNT_BASE"
    touch "$SESSION_LOG"
    TARGET_DATA_MOUNTS=()

    log "Protected host check: PASS" | tee -a "$SESSION_LOG"
    log "Target disk: $TARGET_DISK" | tee -a "$SESSION_LOG"
    log "Root component: $ROOT_DEVICE ($fstype)" | tee -a "$SESSION_LOG"

    mount_mode="$mode"
    root_mount_options="$mount_mode"
    if [[ "$mode" == "ro" ]]; then
        case "$fstype" in
            ext2|ext3|ext4) root_mount_options+=",noload" ;;
            xfs) root_mount_options+=",norecovery" ;;
        esac
    fi
    mount_recorded "$ROOT_DEVICE" "$MOUNT_BASE" -o "$root_mount_options"
    TARGET_ROOT="$MOUNT_BASE"
    TARGET_SUBVOL=""

    if [[ "$fstype" == "btrfs" ]]; then
        discovered_subvol="$(current_btrfs_subvol 2>/dev/null || true)"
        [[ -n "$discovered_subvol" ]] && TARGET_SUBVOL="$discovered_subvol"
    fi

    if [[ ! -f "$TARGET_ROOT/etc/os-release" && "$fstype" == "btrfs" ]]; then
        # Mount the top-level tree only long enough to identify the installed
        # Linux root. Then remount that subvolume directly at MOUNT_BASE. This
        # is critical for chroot tools: / must be the selected root mount, not
        # merely a directory inside a subvolid=5 mount.
        umount "$MOUNT_BASE"
        MOUNTS=()
        mount_recorded "$ROOT_DEVICE" "$MOUNT_BASE" -o "$root_mount_options,subvolid=5"
        mounted_root="$(find_btrfs_root "$MOUNT_BASE" || true)"
        if [[ -n "$mounted_root" ]]; then
            TARGET_ROOT="$mounted_root"
            TARGET_SUBVOL="${TARGET_ROOT#"$MOUNT_BASE"/}"
            [[ -n "$TARGET_SUBVOL" && "$TARGET_SUBVOL" != "$TARGET_ROOT" ]] \
                || fail "Unable to determine the Btrfs root subvolume path."

            # /etc is visible now, so create any request-scoped mapper alias the
            # installed crypttab expects before the final root mount is created.
            read_target_os
            prepare_mapper_compatibility_aliases
            ROOT_DEVICE="$(preferred_block_path "$raw_root" "$ROOT_CANONICAL")"

            umount "$MOUNT_BASE"
            MOUNTS=()
            mount_recorded "$ROOT_DEVICE" "$MOUNT_BASE" -o "$mode,subvol=$TARGET_SUBVOL"
            TARGET_ROOT="$MOUNT_BASE"
        else
            # No subvolume carries os-release. Fall through to the same-disk
            # component fallback below instead of refusing immediately.
            umount "$MOUNT_BASE"
            MOUNTS=()
            TARGET_ROOT="$MOUNT_BASE"
            TARGET_SUBVOL=""
        fi
    fi

    if [[ ! -f "$TARGET_ROOT/etc/os-release" ]]; then
        # The committed component is not a confirmed installed Linux root.
        # Before refusing, probe the other Linux-capable partitions of the
        # same target disk (largest first, swap/ESP/crypto excluded) and adopt
        # the first one whose read-only mount exposes /etc/os-release. This is
        # the privileged confirmation fallback for a component the GUI could
        # not verify while running unprivileged.
        if mountpoint -q "$MOUNT_BASE" 2>/dev/null; then
            umount "$MOUNT_BASE"
            MOUNTS=()
        fi
        selected_component="$ROOT_DEVICE"
        selected_fstype="$fstype"
        fallback_root="$(resolve_target_root_component "$TARGET_DISK" "$ROOT_CANONICAL")" || {
            fail "Selected component $selected_component ($selected_fstype) does not contain /etc/os-release and no other Linux-capable partition on $TARGET_DISK qualifies. Candidates: ${fallback_root:-none}."
        }
        [[ -n "$fallback_root" ]] || fail "Internal root-component resolution error."
        fstype="$(lsblk -ndo FSTYPE "$fallback_root" 2>/dev/null | head -n1 || true)"
        [[ -n "$fstype" ]] || fail "No filesystem was detected on the resolved root component $fallback_root."
        ROOT_CANONICAL="$(canonical_block "$fallback_root")" || fail "Resolved root component is not a block device: $fallback_root"
        ROOT_DEVICE="$(preferred_block_path "$fallback_root" "$ROOT_CANONICAL")"
        log "Root component fallback: selected component $selected_component ($selected_fstype) lacks /etc/os-release; resolved $ROOT_DEVICE ($fstype) from $TARGET_DISK." | tee -a "$SESSION_LOG"

        root_mount_options="$mode"
        if [[ "$mode" == "ro" ]]; then
            case "$fstype" in
                ext2|ext3|ext4) root_mount_options+=",noload" ;;
                xfs) root_mount_options+=",norecovery" ;;
            esac
        fi
        mount_recorded "$ROOT_DEVICE" "$MOUNT_BASE" -o "$root_mount_options"
        TARGET_ROOT="$MOUNT_BASE"
        TARGET_SUBVOL=""
    fi

    read_target_os
    prepare_mapper_compatibility_aliases
    log "Detected target OS: $TARGET_PRETTY" | tee -a "$SESSION_LOG"
    log "Target root mount: $TARGET_ROOT (subvolume=${TARGET_SUBVOL:-default/none})" | tee -a "$SESSION_LOG"

    # Mount same-filesystem Btrfs fstab subvolumes such as @home, @root,
    # @var@log and @.snapshots. Without these, a recovery chroot could write
    # into the hidden mountpoint directories inside @ instead of the installed
    # system's actual subvolumes.
    mount_target_btrfs_subvolumes "$mode"

    if [[ "$mode" == "rw" ]]; then
        mount_target_boot_entry "/boot"
        mount_target_boot_entry "/boot/efi"
        mount_target_boot_entry "/efi"
        mount_special dev-rw none "$TARGET_ROOT/dev"
        mount_special proc proc "$TARGET_ROOT/proc"
        mount_special rbind-ro /sys "$TARGET_ROOT/sys"
        mount_special tmpfs none "$TARGET_ROOT/run"
        mount_target_resolver
    fi
}

# Prepare native running-host maintenance without mounting anything: prove the
# supplied disk/root back the live system, optionally require a Debian-family
# backend (require_debian=yes) and writable live root/boot/ESP mounts
# (require_rw=yes), then detect the running host ESP.
prepare_running_host()
{
    local raw_target="$1" raw_root="$2" require_rw="${3:-no}" fstype

    need lsblk
    need findmnt
    need blkid
    need readlink

    TARGET_DISK="$(canonical_block "$raw_target" 2>/dev/null || true)"
    [[ -n "$TARGET_DISK" ]] || fail "Host disk is not a block device: $raw_target"
    ROOT_CANONICAL="$(canonical_block "$raw_root" 2>/dev/null || true)"
    [[ -n "$ROOT_CANONICAL" ]] || fail "Host root component is not a block device: $raw_root"
    ROOT_DEVICE="$(preferred_block_path "$raw_root" "$ROOT_CANONICAL")"

    assert_target_is_running_host "$TARGET_DISK" "$ROOT_CANONICAL"
    fstype="$(lsblk -ndo FSTYPE "$ROOT_CANONICAL" 2>/dev/null | head -n1 || true)"
    [[ "$fstype" != "crypto_LUKS" && -n "$fstype" ]] \
        || fail "The running host root is not an unlocked filesystem."

    # Native host maintenance deliberately does not mount, remount or bind
    # recovery filesystems. Existing command runners use chroot /, while every
    # path resolves to the live system guarded by the identity check above.
    SESSION_DIR="$(mktemp -d "$STATE_ROOT/session.XXXXXX")"
    MOUNT_BASE="/"
    TARGET_ROOT="/"
    TARGET_SUBVOL=""
    TARGET_DATA_MOUNTS=()
    SESSION_LOG="$SESSION_DIR/session.log"
    touch "$SESSION_LOG"

    if [[ "$fstype" == "btrfs" ]]; then
        TARGET_SUBVOL="$(current_btrfs_subvol 2>/dev/null || true)"
    fi
    read_target_os
    EFI_ESP_SOURCE=""
    EFI_ESP_FSTYPE=""
    detect_mounted_esp || true
    if [[ "$require_rw" == yes ]]; then
        target_path_is_mounted_rw / || fail "The running host root filesystem is not writable. Repair from another system instead."
        target_path_is_mounted_rw /boot || fail "The running host /boot filesystem is not writable."
        if [[ -n "$EFI_ESP_SOURCE" && -n "$TARGET_ESP_MOUNT" ]]; then
            target_path_is_mounted_rw "$TARGET_ESP_MOUNT" || fail "The running host EFI System Partition is not writable."
        fi
        TARGET_WRITE_INTENT=1
    fi
    RUNNING_HOST_MODE=1

    log "Running-host identity and boot-mount check: PASS" | tee -a "$SESSION_LOG"
    log "Host disk: $TARGET_DISK" | tee -a "$SESSION_LOG"
    log "Host root component: $ROOT_DEVICE ($fstype)" | tee -a "$SESSION_LOG"
    log "Host root mount: / (subvolume=${TARGET_SUBVOL:-default/none})" | tee -a "$SESSION_LOG"
    log "Host EFI System Partition: ${EFI_ESP_SOURCE:-not detected} (${EFI_ESP_FSTYPE:-unknown})${TARGET_ESP_MOUNT:+ mounted at $TARGET_ESP_MOUNT}" | tee -a "$SESSION_LOG"
}

# Remount the already-confirmed target root read-write and install the chroot
# mounts (boot entries, data subvolumes, /dev, /proc, /sys, /run, resolver).
promote_target_rw()
{
    [[ -n "$MOUNT_BASE" && -d "$MOUNT_BASE" ]] || fail "Internal target mount is not prepared."
    TARGET_WRITE_INTENT=1
    log "Remounting confirmed target root read-write" | tee -a "$SESSION_LOG"
    mount -o remount,rw "$MOUNT_BASE"
    remount_target_data_rw
    mount_target_boot_entry "/boot"
    mount_target_boot_entry "/boot/efi"
    mount_target_boot_entry "/efi"
    mount_special dev-rw none "$TARGET_ROOT/dev"
    mount_special proc proc "$TARGET_ROOT/proc"
    mount_special rbind-ro /sys "$TARGET_ROOT/sys"
    mount_special tmpfs none "$TARGET_ROOT/run"
    # prepare_target is deliberately read-only during repair preflight.  The
    # resolver bind therefore cannot be installed until this promotion step;
    # without it apt-update inside the repair chroot sees the target's stale
    # systemd-resolved path and fails DNS resolution.
    mount_target_resolver
}

# Promote only the target root and its Btrfs data subvolumes to rw for File
# Copy; used after all read-only destination safety checks have passed.
promote_target_data_rw()
{
    [[ -n "$MOUNT_BASE" && -d "$MOUNT_BASE" ]] || fail "Internal target mount is not prepared."
    TARGET_WRITE_INTENT=1
    log "Remounting confirmed target filesystem read-write for file copy" | tee -a "$SESSION_LOG"
    mount -o remount,rw "$MOUNT_BASE"
    remount_target_data_rw
}

# ---------------------------------------------------------------------------
# File Copy: virtual path validation, ownership mapping and verified transfer
# ---------------------------------------------------------------------------
validate_virtual_path()
{
    local path="$1" component
    [[ "$path" == /* ]] || fail "Repair-system paths must be absolute: $path"
    [[ "$path" != *$'\n'* && "$path" != *$'\r'* && "$path" != *$'\t'* ]] || fail "Paths containing line breaks or tabs are not supported."
    [[ "$path" != "/" ]] || fail "The repair-system root directory cannot be used as a File Copy source or destination. Choose a more specific path."

    IFS='/' read -ra components <<< "$path"
    for component in "${components[@]}"; do
        [[ "$component" != ".." ]] || fail "Parent-directory traversal is not accepted in repair-system paths: $path"
    done
}

# File Copy intentionally treats repair-system paths as a virtual namespace
# until the guarded helper mounts the selected target.  The folder browser
# uses the same namespace, but it must also be able to inspect / itself.
validate_browse_virtual_path()
{
    local path="$1" component
    [[ "$path" == /* ]] || fail "Repair-system paths must be absolute: $path"
    [[ "$path" != *$'\n'* && "$path" != *$'\r'* && "$path" != *$'\t'* ]] \
        || fail "Paths containing line breaks or tabs are not supported."

    IFS='/' read -ra components <<< "$path"
    for component in "${components[@]}"; do
        [[ "$component" != ".." ]] \
            || fail "Parent-directory traversal is not accepted in repair-system paths: $path"
    done
}

browse_target_directory()
{
    local virtual_path="$1" candidate root_real candidate_real entry name encoded
    validate_browse_virtual_path "$virtual_path"
    need find
    need base64

    # Every browse request gets its own temporary read-only target mount.  No
    # target path is left mounted when this request exits, including on error.
    prepare_target ro
    maybe_mount_target_path "$virtual_path" ro

    candidate="$TARGET_ROOT$virtual_path"
    [[ -d "$candidate" && ! -L "$candidate" ]] \
        || fail "Repair-system folder does not exist: $virtual_path"
    root_real="$(realpath_existing "$TARGET_ROOT")" \
        || fail "Unable to resolve mounted target root."
    candidate_real="$(realpath_existing "$candidate")" \
        || fail "Unable to resolve repair-system folder: $virtual_path"
    path_within "$candidate_real" "$root_real" \
        || fail "Repair-system folder escapes the selected target through a symlink: $virtual_path"

    # Emit only immediate real directories.  Names are base64-encoded so a
    # valid directory containing whitespace, tabs or newlines remains one
    # unambiguous protocol record for the unprivileged Qt browser.
    while IFS= read -r -d '' entry; do
        name="${entry##*/}"
        encoded="$(printf '%s' "$name" | base64 | tr -d '\n')"
        printf 'BROWSE_ENTRY\t%s\n' "$encoded"
    done < <(find "$candidate_real" -mindepth 1 -maxdepth 1 -type d ! -type l -print0)
}

path_within()
{
    local child="$1" parent="$2"
    [[ "$parent" == / && "$child" == /* ]] && return 0
    [[ "$child" == "$parent" || "$child" == "$parent/"* ]]
}

maybe_mount_target_path()
{
    local virtual_path="$1" mode="$2"
    case "$virtual_path" in
        /boot/efi|/boot/efi/*)
            mount_target_boot_entry "/boot" "$mode"
            mount_target_boot_entry "/boot/efi" "$mode"
            ;;
        /boot|/boot/*)
            mount_target_boot_entry "/boot" "$mode"
            ;;
    esac
}

nearest_existing_directory()
{
    local path="$1"
    while [[ ! -d "$path" ]]; do
        [[ "$path" != "/" ]] || break
        path="$(dirname -- "$path")"
    done
    [[ -d "$path" ]] || return 1
    printf '%s\n' "$path"
}

target_source_path()
{
    local virtual_path="$1" candidate parent_real root_real
    validate_virtual_path "$virtual_path"
    candidate="$TARGET_ROOT$virtual_path"
    [[ -e "$candidate" || -L "$candidate" ]] || fail "Repair-system source does not exist: $virtual_path"

    root_real="$(realpath_existing "$TARGET_ROOT")" || fail "Unable to resolve mounted target root."
    parent_real="$(realpath_existing "$(dirname -- "$candidate")")" || fail "Unable to resolve repair-system source parent: $virtual_path"
    path_within "$parent_real" "$root_real" || fail "Repair-system source escapes the selected target through a symlink: $virtual_path"
    printf '%s\n' "$candidate"
}

target_destination_path()
{
    local virtual_path="$1" create="$2" candidate root_real probe probe_real owner_uid owner_gid missing_path
    local -a missing_paths=()
    validate_virtual_path "$virtual_path"
    candidate="$TARGET_ROOT$virtual_path"
    root_real="$(realpath_existing "$TARGET_ROOT")" || fail "Unable to resolve mounted target root."

    if [[ -L "$candidate" ]]; then
        fail "Repair-system destination must not be a symbolic link: $virtual_path"
    fi
    if [[ -e "$candidate" && ! -d "$candidate" ]]; then
        fail "Repair-system destination exists but is not a directory: $virtual_path"
    fi

    probe="$(nearest_existing_directory "$candidate")" || fail "No existing parent directory was found for target destination: $virtual_path"
    probe_real="$(realpath_existing "$probe")" || fail "Unable to resolve target destination parent: $virtual_path"
    path_within "$probe_real" "$root_real" || fail "Repair-system destination escapes the selected target through a symlink: $virtual_path"
    read -r owner_uid owner_gid < <(stat -c '%u %g' -- "$probe_real")

    if [[ "$create" == "yes" && ! -d "$candidate" ]]; then
        missing_path="$candidate"
        while [[ ! -e "$missing_path" && "$missing_path" != "$probe" ]]; do
            missing_paths+=("$missing_path")
            missing_path="$(dirname -- "$missing_path")"
        done
        mkdir -p -- "$candidate"
        for missing_path in "${missing_paths[@]}"; do
            chown "$owner_uid:$owner_gid" -- "$missing_path"
        done
        log "Created target destination directory $virtual_path with inherited owner $owner_uid:$owner_gid" | tee -a "$SESSION_LOG" >&2
    fi

    if [[ -d "$candidate" ]]; then
        probe_real="$(realpath_existing "$candidate")" || fail "Unable to resolve target destination: $virtual_path"
        path_within "$probe_real" "$root_real" || fail "Repair-system destination escapes the selected target through a symlink: $virtual_path"
        [[ ! -L "$candidate" ]] || fail "Repair-system destination must not be a symbolic link: $virtual_path"
    fi
    printf '%s\n' "$candidate"
}

validate_host_source()
{
    local source="$1" parent_real
    [[ "$source" == /* ]] || fail "Host source paths must be absolute: $source"
    [[ "$source" != *$'\n'* && "$source" != *$'\r'* ]] || fail "Paths containing line breaks are not supported."
    [[ "$source" != "/" ]] || fail "The running host root directory cannot be copied as one File Copy item. Choose a more specific path."
    [[ -e "$source" || -L "$source" ]] || fail "Host source does not exist: $source"
    parent_real="$(realpath_existing "$(dirname -- "$source")")" || fail "Unable to resolve host source parent: $source"
    case "$parent_real" in
        /proc|/proc/*|/sys|/sys/*|/dev|/dev/*|"$STATE_ROOT"|"$STATE_ROOT"/*)
            fail "Pseudo-filesystem/session sources are not accepted: $source"
            ;;
    esac
}

validate_host_destination()
{
    local destination="$1" real mount_target
    [[ "$destination" == /* ]] || fail "Host destination must be an absolute path: $destination"
    [[ "$destination" != *$'\n'* && "$destination" != *$'\r'* ]] || fail "Paths containing line breaks are not supported."
    [[ -d "$destination" && ! -L "$destination" ]] || fail "Host destination must be an existing non-symlink directory: $destination"
    real="$(realpath_existing "$destination")" || fail "Unable to resolve host destination: $destination"

    case "$real" in
        /|/boot|/boot/*|/etc|/etc/*|/usr|/usr/*|/var|/var/*|/bin|/bin/*|/sbin|/sbin/*|/lib|/lib/*|/lib64|/lib64/*|/dev|/dev/*|/proc|/proc/*|/sys|/sys/*|/run|/run/*)
            case "$real" in
                /run/media|/run/media/*) ;;
                *) fail "Repair-to-Host recovery refuses system-critical host destinations: $real" ;;
            esac
            ;;
    esac

    case "$real" in
        /home|/home/*|/mnt|/mnt/*|/media|/media/*|/run/media|/run/media/*|/tmp|/tmp/*)
            printf '%s\n' "$real"
            return 0
            ;;
    esac

    mount_target="$(findmnt -rn -o TARGET --target "$real" 2>/dev/null | head -n1 || true)"
    if [[ -n "$mount_target" && "$mount_target" != "/" && "$mount_target" != "/boot" && "$mount_target" != "/boot/efi" ]]; then
        printf '%s\n' "$real"
        return 0
    fi

    fail "Host destination must be under /home, /mnt, /media, /run/media, /tmp, or a separate non-system mount: $real"
}

target_destination_sensitive()
{
    local path="$1"
    case "$path" in
        /boot|/boot/*|/etc|/etc/*|/usr|/usr/*|/root|/root/*|/var|/var/*|/bin|/bin/*|/sbin|/sbin/*|/lib|/lib/*|/lib64|/lib64/*|/opt|/opt/*)
            return 0
            ;;
    esac
    return 1
}

host_user_name()
{
    getent passwd "$1" 2>/dev/null | awk -F: 'NR==1 {print $1}'
}

host_group_name()
{
    getent group "$1" 2>/dev/null | awk -F: 'NR==1 {print $1}'
}

target_user_name()
{
    local uid="$1"
    [[ -f "$TARGET_ROOT/etc/passwd" ]] || return 0
    awk -F: -v id="$uid" '$3 == id {print $1; exit}' "$TARGET_ROOT/etc/passwd"
}

target_group_name()
{
    local gid="$1"
    [[ -f "$TARGET_ROOT/etc/group" ]] || return 0
    awk -F: -v id="$gid" '$3 == id {print $1; exit}' "$TARGET_ROOT/etc/group"
}

smart_chown_for_item()
{
    local direction="$1" source="$2" destination="$3"
    local source_uid source_gid destination_uid destination_gid source_user source_group peer_user peer_group
    read -r source_uid source_gid < <(stat -c '%u %g' -- "$source")
    read -r destination_uid destination_gid < <(stat -c '%u %g' -- "$destination")

    if [[ "$direction" == "host-to-repair" ]]; then
        source_user="$(host_user_name "$source_uid")"
        source_group="$(host_group_name "$source_gid")"
        peer_user="$(target_user_name "$source_uid")"
        peer_group="$(target_group_name "$source_gid")"
    else
        source_user="$(target_user_name "$source_uid")"
        source_group="$(target_group_name "$source_gid")"
        peer_user="$(host_user_name "$source_uid")"
        peer_group="$(host_group_name "$source_gid")"
    fi

    if [[ -n "$source_user" && -n "$source_group" && "$source_user" == "$peer_user" && "$source_group" == "$peer_group" ]]; then
        log "Ownership validation: $source_uid:$source_gid maps to $source_user:$source_group on both sides; preserving numeric ownership" | tee -a "$SESSION_LOG" >&2
        return 0
    fi

    log "Ownership validation: source $source_uid:$source_gid (${source_user:-unknown}:${source_group:-unknown}) does not map identically on the destination side; using destination owner $destination_uid:$destination_gid" | tee -a "$SESSION_LOG" >&2
    printf '%s:%s\n' "$destination_uid" "$destination_gid"
}

verify_sha256_item()
{
    local source="$1" destination_dir="$2" source_file destination_file relative base source_hash destination_hash
    local verified=0
    base="$(basename -- "$source")"

    if [[ -f "$source" && ! -L "$source" ]]; then
        destination_file="$destination_dir/$base"
        [[ -f "$destination_file" && ! -L "$destination_file" ]] || fail "Verification failed; destination file missing: $destination_file"
        source_hash="$(sha256sum -- "$source" | awk '{print $1}')"
        destination_hash="$(sha256sum -- "$destination_file" | awk '{print $1}')"
        [[ "$source_hash" == "$destination_hash" ]] || fail "SHA-256 verification failed: $source"
        printf '1\n'
        return 0
    fi

    if [[ -d "$source" && ! -L "$source" ]]; then
        while IFS= read -r -d '' source_file; do
            relative="${source_file#"$source"/}"
            destination_file="$destination_dir/$base/$relative"
            [[ -f "$destination_file" && ! -L "$destination_file" ]] || fail "Verification failed; destination file missing: $destination_file"
            source_hash="$(sha256sum -- "$source_file" | awk '{print $1}')"
            destination_hash="$(sha256sum -- "$destination_file" | awk '{print $1}')"
            [[ "$source_hash" == "$destination_hash" ]] || fail "SHA-256 verification failed: $source_file"
            verified=$((verified + 1))
        done < <(find "$source" -type f -print0)
    fi

    printf '%s\n' "$verified"
}

run_rsync_item()
{
    local mode="$1" source="$2" destination="$3" chown_value="$4"
    local -a options=(-aHAX --numeric-ids --human-readable --itemize-changes)
    [[ "$mode" == "preview" ]] && options+=(--dry-run)
    [[ "$mode" == "copy" ]] && options+=(--stats)
    [[ -n "$chown_value" ]] && options+=("--chown=$chown_value")

    rsync "${options[@]}" -- "$source" "$destination/"
}

verify_rsync_item()
{
    local source="$1" destination="$2" chown_value="$3" output
    local -a options=(-aHAX --numeric-ids --checksum --dry-run --itemize-changes)
    [[ -n "$chown_value" ]] && options+=("--chown=$chown_value")
    output="$(rsync "${options[@]}" -- "$source" "$destination/" 2>&1)" || fail "Post-copy rsync verification failed for $source: $output"
    [[ -z "$output" ]] || fail "Post-copy metadata/content verification still reports differences for $source: $output"
}

# Guarded File Copy entry point for both copy-preview and copy.  Arguments:
# action, direction (host-to-repair|repair-to-host), ownership (smart|preserve),
# approval (normal|sensitive-ok), destination, then one or more sources.  The
# target is always mounted read-only first; real copies are promoted to rw only
# after containment checks and are verified with rsync --checksum and SHA-256.
run_file_copy()
{
    local action="$1"; shift
    (($# >= 5)) || fail "File Copy requires direction, ownership, approval, destination and at least one source."

    local direction="$1" ownership="$2" approval="$3" destination_virtual="$4"; shift 4
    [[ "$destination_virtual" == "/" ]] || destination_virtual="${destination_virtual%/}"
    local -a requested_sources=("$@")
    ((${#requested_sources[@]} > 0)) || fail "No File Copy sources were provided."
    [[ "$direction" == "host-to-repair" || "$direction" == "repair-to-host" ]] || fail "Unknown File Copy direction: $direction"
    [[ "$ownership" == "smart" || "$ownership" == "preserve" ]] || fail "Unknown ownership policy: $ownership"
    [[ "$approval" == "normal" || "$approval" == "sensitive-ok" ]] || fail "Unknown File Copy approval token."

    need rsync
    need realpath
    need stat
    need sha256sum
    need find
    need getent

    local mode="preview"
    [[ "$action" == "copy" ]] && mode="copy"
    [[ "$action" == "copy-preview" || "$action" == "copy" ]] || fail "Internal File Copy action error: $action"

    # Always identify and mount the target read-only first. Host-to-Repair is
    # promoted to rw only after all target identity/safety checks have passed.
    prepare_target ro

    local destination ownership_destination source virtual_path chown_value sha_count=0 item_count=0
    local -a sources=()

    if [[ "$direction" == "host-to-repair" ]]; then
        validate_virtual_path "$destination_virtual"
        if [[ "$mode" == "copy" ]] && target_destination_sensitive "$destination_virtual" && [[ "$approval" != "sensitive-ok" ]]; then
            fail "Sensitive repair-system destination requires explicit confirmation: $destination_virtual"
        fi

        # Resolve any separate /boot or /boot/efi filesystem read-only and
        # validate the destination containment before the root can become rw.
        # A real copy later remounts only these already-confirmed target mounts.
        maybe_mount_target_path "$destination_virtual" ro
        destination="$(target_destination_path "$destination_virtual" no)"
        ownership_destination="$(nearest_existing_directory "$destination")" || fail "No existing target destination parent was found."

        for source in "${requested_sources[@]}"; do
            [[ "$source" == "/" ]] || source="${source%/}"
            validate_host_source "$source"
            sources+=("$source")
        done

        if [[ "$mode" == "copy" ]]; then
            promote_target_data_rw
            maybe_mount_target_path "$destination_virtual" rw
            destination="$(target_destination_path "$destination_virtual" yes)"
            ownership_destination="$destination"
        fi
    else
        destination="$(validate_host_destination "$destination_virtual")"
        ownership_destination="$destination"
        for virtual_path in "${requested_sources[@]}"; do
            [[ "$virtual_path" == "/" ]] || virtual_path="${virtual_path%/}"
            validate_virtual_path "$virtual_path"
            maybe_mount_target_path "$virtual_path" ro
            source="$(target_source_path "$virtual_path")"
            sources+=("$source")
        done
    fi

    local source_name
    local -A seen_destination_names=()
    for source in "${sources[@]}"; do
        source_name="$(basename -- "$source")"
        [[ -n "$source_name" && "$source_name" != "." && "$source_name" != ".." ]] || fail "Invalid top-level source name: $source"
        if [[ -n "${seen_destination_names[$source_name]:-}" ]]; then
            fail "Multiple selected sources would map to the same destination name '$source_name'. Copy them separately or rename one first."
        fi
        seen_destination_names[$source_name]=1
    done

    if [[ "$mode" == "preview" && "$direction" == "host-to-repair" && ! -d "$destination" ]]; then
        log "PREVIEW: target destination directory would be created: $destination_virtual" | tee -a "$SESSION_LOG"
    elif [[ ! -d "$destination" ]]; then
        fail "Destination directory is unavailable: $destination_virtual"
    fi

    log "File Copy ${mode^^}: ${direction}" | tee -a "$SESSION_LOG"
    log "Destination: $destination_virtual" | tee -a "$SESSION_LOG"
    log "Ownership policy: $ownership" | tee -a "$SESSION_LOG"
    log "Sources: ${#sources[@]}" | tee -a "$SESSION_LOG"

    for source in "${sources[@]}"; do
        chown_value=""
        if [[ "$ownership" == "smart" ]]; then
            chown_value="$(smart_chown_for_item "$direction" "$source" "$ownership_destination")"
        fi
        log "${mode^^}: $(basename -- "$source")" | tee -a "$SESSION_LOG"
        run_rsync_item "$mode" "$source" "$destination" "$chown_value" 2>&1 | tee -a "$SESSION_LOG"
        item_count=$((item_count + 1))

        if [[ "$mode" == "copy" ]]; then
            verify_rsync_item "$source" "$destination" "$chown_value"
            sha_count=$((sha_count + $(verify_sha256_item "$source" "$destination")))
        fi
    done

    if [[ "$mode" == "copy" ]]; then
        sync
        log "COPY COMPLETE" | tee -a "$SESSION_LOG"
        log "  Source items: $item_count" | tee -a "$SESSION_LOG"
        log "  SHA-256 regular files verified: $sha_count" | tee -a "$SESSION_LOG"
        log "  rsync metadata/content re-check: PASS" | tee -a "$SESSION_LOG"
        log "  Unrelated destination files: retained (no --delete used)" | tee -a "$SESSION_LOG"
    else
        log "PREVIEW COMPLETE — no files were changed." | tee -a "$SESSION_LOG"
    fi
}

# Fail unless every target crypttab entry either uses a keyless/none source or
# resolves to a block device on the selected target disk.  Read-only gate run
# before package, initramfs and bootloader repairs that depend on mapper names.
validate_mapper_crypttab()
{
    local crypttab="$TARGET_ROOT/etc/crypttab"
    local name source key options resolved failures=0

    [[ -s "$crypttab" ]] || {
        log "Mapper/crypttab gate: no target crypttab entries; PASS" | tee -a "$SESSION_LOG"
        return 0
    }

    while read -r name source key options _rest; do
        [[ -n "$name" && "${name:0:1}" != "#" ]] || continue
        [[ -n "$source" ]] || continue

        case "$source" in
            UUID=*|PARTUUID=*|LABEL=*|PARTLABEL=*|/dev/*)
                resolved="$(resolve_fstab_source "$source")"
                if [[ -z "$resolved" || ! -b "$resolved" ]]; then
                    log "Mapper/crypttab gate: unresolved entry '$name' -> '$source'" | tee -a "$SESSION_LOG"
                    failures=$((failures + 1))
                elif ! same_single_top_disk "$TARGET_DISK" "$resolved"; then
                    log "Mapper/crypttab gate: entry '$name' resolves outside the selected disk: $resolved" | tee -a "$SESSION_LOG"
                    failures=$((failures + 1))
                else
                    log "Mapper/crypttab gate: $name -> $resolved" | tee -a "$SESSION_LOG"
                fi
                ;;
            none|-)
                ;;
            *)
                log "Mapper/crypttab gate: unsupported source syntax for '$name': $source" | tee -a "$SESSION_LOG"
                failures=$((failures + 1))
                ;;
        esac
    done < "$crypttab"

    ((failures == 0)) || fail "Mapper/crypttab consistency check failed for $failures entry/entries."
    log "Mapper/crypttab consistency gate: PASS" | tee -a "$SESSION_LOG"
}

# ---------------------------------------------------------------------------
# Guarded command execution (target chroot and native running host)
# ---------------------------------------------------------------------------
# Install the request-scoped efibootmgr shim and prove that the private mount
# namespace can remount efivarfs read-only.  From here on every selected chroot
# command and native host command runs with firmware writes intercepted.
prepare_host_command_guard()
{
    local real_efi
    need unshare
    need mount
    HOST_COMMAND_GUARD_DIR="$SESSION_DIR/host-command-guard"
    mkdir -m 0700 "$HOST_COMMAND_GUARD_DIR"
    real_efi="$(command -v efibootmgr || true)"
    if [[ -n "$real_efi" ]]; then
        cat > "$HOST_COMMAND_GUARD_DIR/efibootmgr" <<EOF
#!/bin/sh
set -eu
for arg in "\$@"; do
    case "\$arg" in
        -v|--verbose|-h|--help|-V|--version) ;;
        *)
            echo "Boot Bitch EFI guard: hook firmware mutation deferred to the selected-system reconciler." >&2
            exit 0
            ;;
    esac
done
exec "$real_efi" "\$@"
EOF
        chmod 0700 "$HOST_COMMAND_GUARD_DIR/efibootmgr"
    fi
    HOST_COMMAND_GUARD=1
    run_selected_chroot /bin/true \
        || fail "Unable to isolate firmware writes for native host maintenance; no repair stage was started."
    log "Host command guard: firmware variables are read-only to package/kernel/vendor hooks; the helper owns explicit firmware registration." | tee -a "$SESSION_LOG"
}

host_package_manager_gate()
{
    local proc
    # Native package actions must not race apt, dpkg, pacman, rpm/dnf5,
    # unattended-upgrades, or another package frontend already using the live
    # host. Stale lock files alone are not considered active; inspect processes
    # and lock holders. PackageKit's daemon normally stays resident on KDE-based
    # systems even when idle, so it is only treated as a conflict while it
    # actually holds package-manager locks or has spawned apt/dpkg.
    #
    # The Alpine apk process and database-lock checks live in
    # alpine_apk_preflight(), which runs exactly for the modifying apk stages
    # (fix-broken/upgrade).  The rpm/dnf5 process and fcntl-lock checks also
    # live in rpm_preflight(); this gate additionally catches an active rpmdb
    # or dnf5 transaction before any host package stage starts.  Read-only
    # diagnostics, validate and capability probes never call this gate.
    for proc in apt apt-get dpkg pacman makepkg yay paru unattended-upgrade \
        dnf dnf5 dnf4 dnf-3 rpm rpmkeys dnf-automatic; do
        if pgrep -x "$proc" >/dev/null 2>&1; then
            fail "Package manager process '$proc' is already running; refusing a concurrent host package repair."
        fi
    done
    if pgrep -x packagekitd >/dev/null 2>&1; then
        log "PackageKit daemon is running; it is only a conflict while it holds package-manager locks or spawns apt/dpkg." | tee -a "$SESSION_LOG"
    fi
    if command -v fuser >/dev/null 2>&1; then
        for proc in /var/lib/dpkg/lock /var/lib/dpkg/lock-frontend /var/lib/apt/lists/lock; do
            if [[ -e "$proc" ]] && fuser -s "$proc" 2>/dev/null; then
                fail "Package manager lock '$proc' is active; refusing a concurrent host package repair."
            fi
        done
    fi
    if command -v fuser >/dev/null 2>&1 \
       && [[ -e /var/lib/pacman/db.lck ]] \
       && fuser -s /var/lib/pacman/db.lck 2>/dev/null; then
        fail "Package manager lock '/var/lib/pacman/db.lck' is active; refusing a concurrent host package repair."
    fi
    if rpm_lock_held; then
        fail "Package manager lock on the rpm/dnf5 database is active; refusing a concurrent host package repair."
    fi
    log "Host package-manager concurrency gate: PASS" | tee -a "$SESSION_LOG"
}

# Run one command inside TARGET_ROOT.  When the host command guard is active,
# execute inside the private mount namespace that makes efivarfs read-only and
# prepend the efibootmgr shim directory to every explicit PATH assignment.
run_selected_chroot()
{
    local i
    local -a args=("$@")
    if (( HOST_COMMAND_GUARD == 1 )); then
        # Keep the standard command environment, adding the hook shim to each
        # explicit PATH assignment. Absolute efibootmgr calls still encounter
        # a read-only efivarfs in the private mount namespace below.
        for i in "${!args[@]}"; do
            if [[ "${args[$i]}" == PATH=* ]]; then
                args[$i]="PATH=$HOST_COMMAND_GUARD_DIR:${args[$i]#PATH=}"
            fi
        done
        unshare --mount --propagation private /bin/sh -eu -c '
            if mountpoint -q /sys/firmware/efi/efivars; then
                mount --bind /sys/firmware/efi/efivars /sys/firmware/efi/efivars
                mount -o remount,bind,ro /sys/firmware/efi/efivars
                findmnt -rn -o OPTIONS --target /sys/firmware/efi/efivars | tr "," "\n" | grep -Fxq ro
            elif [ -d /sys/firmware/efi/efivars ]; then
                echo "Unable to prove firmware variable mount isolation." >&2
                exit 1
            fi
            exec chroot "$@"
        ' boot-repair-host-command "$TARGET_ROOT" "${args[@]}"
    else
        chroot "$TARGET_ROOT" "${args[@]}"
    fi
}

# Native host maintenance commands run against the live running system, so
# there is no filesystem root to enter.  They still execute inside the private
# mount namespace that makes efivarfs read-only and the hook shim PATH-first,
# exactly like the guarded branch of run_selected_chroot.
run_host_command_isolated()
{
    local i
    local -a args=("$@")
    (( HOST_COMMAND_GUARD == 1 )) \
        || fail "Host command guard is not prepared; refusing to run a native host command."
    # Keep the standard command environment, adding the hook shim to each
    # explicit PATH assignment. Absolute efibootmgr calls still encounter
    # a read-only efivarfs in the private mount namespace below.
    for i in "${!args[@]}"; do
        if [[ "${args[$i]}" == PATH=* ]]; then
            args[$i]="PATH=$HOST_COMMAND_GUARD_DIR:${args[$i]#PATH=}"
        fi
    done
    unshare --mount --propagation private /bin/sh -eu -c '
        if mountpoint -q /sys/firmware/efi/efivars; then
            mount --bind /sys/firmware/efi/efivars /sys/firmware/efi/efivars
            mount -o remount,bind,ro /sys/firmware/efi/efivars
            findmnt -rn -o OPTIONS --target /sys/firmware/efi/efivars | tr "," "\n" | grep -Fxq ro
        elif [ -d /sys/firmware/efi/efivars ]; then
            echo "Unable to prove firmware variable mount isolation." >&2
            exit 1
        fi
        exec "$@"
    ' boot-repair-host-command "${args[@]}"
}

# Run a labelled command in the target chroot with a clean noninteractive
# environment, tee the transcript into the session log and fail with the
# command's exit code while preserving the stage context.
run_chroot()
{
    local label="$1"; shift
    log "BEGIN: $label" | tee -a "$SESSION_LOG"
    # With pipefail+errexit enabled, a failed chroot in this pipeline would
    # terminate the helper before PIPESTATUS is inspected. That produced only
    # a bare session exit code (and hid the stage context) in the GUI. Capture
    # the pipeline status explicitly, then route it through fail() so the
    # caller receives the command's output and the owning repair stage.
    set +e
    run_selected_chroot /usr/bin/env \
        HOME=/root \
        PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
        DEBIAN_FRONTEND=noninteractive \
        APT_LISTCHANGES_FRONTEND=none \
        "$@" 2>&1 | tee -a "$SESSION_LOG"
    local rc=${PIPESTATUS[0]}
    set -e
    ((rc == 0)) || fail "$label failed with exit code $rc"
    log "PASS: $label" | tee -a "$SESSION_LOG"
}

# ---------------------------------------------------------------------------
# Package-manager feedback evidence
# ---------------------------------------------------------------------------
# Package managers report held-back, skipped, ignored, masked or pinned
# packages without failing the transaction.  Such a package is not a failure:
# every guarded backend keeps its transaction and surfaces the package names as
# stable evidence lines plus one summary line, and the change-status reason
# carries the same summary so the GUI result summary can name the packages.
# The summary lives in PACKAGE_FEEDBACK_SUMMARY until the stage publishes it.
PACKAGE_FEEDBACK_SUMMARY=""
PACKAGE_FEEDBACK_BACKEND=""

package_feedback_reset()
{
    PACKAGE_FEEDBACK_SUMMARY=""
    PACKAGE_FEEDBACK_BACKEND=""
}

# Emit one stable evidence line on stdout and into the session log, exactly
# like repair_change_status, so the repair log keeps the package feedback even
# when the raw transaction transcript scrolls past.
package_feedback_evidence()
{
    printf '%s\n' "$*"
    if [[ -n "$SESSION_LOG" ]]; then
        printf '%s\n' "$*" >> "$SESSION_LOG" 2>/dev/null || true
    fi
}

package_feedback_join()
{
    local joined="" name
    for name in "$@"; do
        joined+="${joined:+, }$name"
    done
    printf '%s' "$joined"
}

# Deduplicate names in first-seen order.
package_feedback_unique()
{
    awk '!seen[$0]++'
}

# Record one summary fragment for the current backend.  A backend may report
# more than one category (for example masked and pinned apk entries); the
# fragments are joined in the published summary.
package_feedback_add()
{
    local backend="$1" fragment="$2"
    [[ -n "$fragment" ]] || return 0
    PACKAGE_FEEDBACK_SUMMARY="${PACKAGE_FEEDBACK_SUMMARY:+$PACKAGE_FEEDBACK_SUMMARY; }$fragment"
    PACKAGE_FEEDBACK_BACKEND="$backend"
}

# Emit the accumulated feedback summary as one stable evidence line.  No-op
# when the backend reported no held-back/skipped packages.
package_feedback_publish()
{
    [[ -n "$PACKAGE_FEEDBACK_SUMMARY" ]] || return 0
    package_feedback_evidence "Package manager feedback: ${PACKAGE_FEEDBACK_BACKEND:-package manager}: $PACKAGE_FEEDBACK_SUMMARY"
}

# Emit `Package <label>: <name>` for every name and record the summary fragment
# "<count> package(s) <label>: <names>" (without names when only a count is
# known).  <count> is derived from the names when 0.
package_feedback_report()
{
    local backend="$1" label="$2" count="$3"
    shift 3
    local -a names=("$@")
    local name fragment
    (( count > 0 )) || count=${#names[@]}
    (( count > 0 )) || return 0
    for name in "${names[@]}"; do
        package_feedback_evidence "Package $label: $name"
    done
    if ((${#names[@]} > 0)); then
        if (( count == 1 )); then
            fragment="1 package $label: $(package_feedback_join "${names[@]}")"
        else
            fragment="$count packages $label: $(package_feedback_join "${names[@]}")"
        fi
    elif (( count == 1 )); then
        fragment="1 package $label"
    else
        fragment="$count packages $label"
    fi
    package_feedback_add "$backend" "$fragment"
}

# APT: the "The following packages have been kept back:" block and the
# "N not upgraded." summary line.  Both the simulation and the apply transcript
# carry them; callers merge both outputs.
apt_package_feedback_names()
{
    local output="$1" line in_block=0
    while IFS= read -r line; do
        if [[ "$line" == 'The following packages have been kept back:'* ]]; then
            in_block=1
            continue
        fi
        if (( in_block == 1 )); then
            if [[ "$line" =~ ^[[:space:]]+([^[:space:]]+) ]]; then
                printf '%s\n' "${BASH_REMATCH[1]}"
                continue
            fi
            in_block=0
        fi
    done <<<"$output"
}

apt_package_feedback_not_upgraded_count()
{
    local output="$1" line count=0
    while IFS= read -r line; do
        if [[ "$line" =~ (^|[[:space:]])([0-9]+)[[:space:]]+not[[:space:]]+upgraded\.?[[:space:]]*$ ]]; then
            count="${BASH_REMATCH[2]}"
        fi
    done <<<"$output"
    printf '%s\n' "$count"
}

apt_package_feedback_report()
{
    local output name count=0 parsed
    local -a names=()
    while IFS= read -r name; do
        [[ -n "$name" ]] && names+=("$name")
    done < <(
        for output in "$@"; do
            apt_package_feedback_names "$output"
        done | package_feedback_unique
    )
    for output in "$@"; do
        parsed="$(apt_package_feedback_not_upgraded_count "$output")"
        [[ "$parsed" =~ ^[0-9]+$ ]] || parsed=0
        if (( parsed > count )); then
            count=$parsed
        fi
    done
    if (( ${#names[@]} > count )); then
        count=${#names[@]}
    fi
    if ((${#names[@]} == 0)); then
        package_feedback_report apt/dpkg "not upgraded" "$count"
    else
        package_feedback_report apt/dpkg "kept back" "$count" "${names[@]}"
    fi
}

# dnf5: "Skipping packages with conflicts:" / "... broken dependencies:"
# sections list the skipped packages; the transaction summary prints
# "Skipping: N packages".  Skipped packages stay non-fatal evidence.
rpm_package_feedback_names()
{
    local output="$1" line in_block=0
    while IFS= read -r line; do
        if [[ "$line" =~ ^[[:space:]]*Skipping[[:space:]]+packages[[:space:]]+with[[:space:]]+(conflicts|broken[[:space:]]+dependencies):[[:space:]]*$ ]]; then
            in_block=1
            continue
        fi
        if (( in_block == 1 )); then
            if [[ "$line" =~ ^[[:space:]]+([^[:space:]]+) ]]; then
                printf '%s\n' "${BASH_REMATCH[1]}"
                continue
            fi
            in_block=0
        fi
    done <<<"$output"
}

rpm_package_feedback_skip_count()
{
    local output="$1" line count=0
    while IFS= read -r line; do
        if [[ "$line" =~ ^[[:space:]]*Skipping:[[:space:]]+([0-9]+)[[:space:]]+packages?[[:space:]]*$ ]]; then
            count="${BASH_REMATCH[1]}"
        fi
    done <<<"$output"
    printf '%s\n' "$count"
}

rpm_package_feedback_report()
{
    local output name count=0 parsed
    local -a names=()
    while IFS= read -r name; do
        [[ -n "$name" ]] && names+=("$name")
    done < <(
        for output in "$@"; do
            rpm_package_feedback_names "$output"
        done | package_feedback_unique
    )
    for output in "$@"; do
        parsed="$(rpm_package_feedback_skip_count "$output")"
        [[ "$parsed" =~ ^[0-9]+$ ]] || parsed=0
        if (( parsed > count )); then
            count=$parsed
        fi
    done
    if (( ${#names[@]} > count )); then
        count=${#names[@]}
    fi
    package_feedback_report rpm "skipped" "$count" "${names[@]}"
}

# pacman: "warning: <pkg>: ignoring package upgrade (<old> => <new>)".
pacman_package_feedback_names()
{
    local output="$1"
    sed -nE 's/^warning: ([^:]+): (ignoring package upgrade|ignoring package).*$/\1/p' <<<"$output" \
        | package_feedback_unique
}

pacman_package_feedback_report()
{
    local output name
    local -a names=()
    while IFS= read -r name; do
        [[ -n "$name" ]] && names+=("$name")
    done < <(
        for output in "$@"; do
            pacman_package_feedback_names "$output"
        done | package_feedback_unique
    )
    package_feedback_report pacman "ignored" "${#names[@]}" "${names[@]}"
}

# apk: masked/held packages are reported as "WARNING: <pkg>: ... masked ..." /
# "... ignoring package upgrade ..."; world version pins are read from
# /etc/apk/world (=, <, >, ~ constraints).  All three stay non-fatal evidence.
apk_package_feedback_masked_names()
{
    local output="$1"
    sed -nE 's/^WARNING: ([^:]+): .*masked.*$/\1/p' <<<"$output" | package_feedback_unique
}

apk_package_feedback_held_names()
{
    local output="$1"
    sed -nE 's/^WARNING: ([^:]+): .*(ignoring package upgrade|held).*$/\1/p' <<<"$output" | package_feedback_unique
}

apk_package_feedback_world_pins()
{
    local world="$TARGET_ROOT/etc/apk/world" line name
    [[ -r "$world" ]] || return 0
    while IFS= read -r line; do
        line="${line%%#*}"
        [[ "$line" =~ ^[[:space:]]*([^[:space:]]+) ]] || continue
        name="${BASH_REMATCH[1]}"
        case "$name" in
            *[=~]*|*'<'*|*'>'*) ;;
            *) continue ;;
        esac
        name="${name%%[=<>~]*}"
        [[ -n "$name" ]] && printf '%s\n' "$name"
    done < "$world" | package_feedback_unique
}

apk_package_feedback_report()
{
    local output name
    local -a masked=() held=() pinned=()
    while IFS= read -r name; do
        [[ -n "$name" ]] && masked+=("$name")
    done < <(
        for output in "$@"; do
            apk_package_feedback_masked_names "$output"
        done | package_feedback_unique
    )
    while IFS= read -r name; do
        [[ -n "$name" ]] && held+=("$name")
    done < <(
        for output in "$@"; do
            apk_package_feedback_held_names "$output"
        done | package_feedback_unique
    )
    while IFS= read -r name; do
        [[ -n "$name" ]] && pinned+=("$name")
    done < <(apk_package_feedback_world_pins)
    package_feedback_report apk "masked" "${#masked[@]}" "${masked[@]}"
    package_feedback_report apk "held" "${#held[@]}" "${held[@]}"
    package_feedback_report apk "pinned" "${#pinned[@]}" "${pinned[@]}"
}

# ---------------------------------------------------------------------------
# APT/dpkg repair backend (Debian/Ubuntu family)
# ---------------------------------------------------------------------------
# APT aborts non-interactively when a repository's release metadata changes
# its Origin/Label/Suite/Codename/Version.  Vendor repositories (for example
# TUXEDO's txos.tuxedocomputers.com) make such changes legitimately.  The
# classifier below accepts only that exact error class; every other apt error,
# download failure or integrity warning vetoes the retry.
apt_update_release_info_change_only()
{
    local output="$1" line found=0
    local release_info_re="^E: Repository '[^']*' changed its '(Origin|Label|Suite|Codename|Version)' value"
    while IFS= read -r line; do
        [[ -n "$line" ]] || continue
        if [[ "$line" =~ $release_info_re ]]; then
            found=1
            continue
        fi
        case "$line" in
            E:*|Err:*|e:*) return 1 ;;
            W:*|w:*)
                # Benign deprecation notices (for example the legacy
                # trusted.gpg keyring warning) must not block the retry, but
                # any warning that reports a real error does.  The retry still
                # performs the complete signature and key verification.
                grep -Eiq 'error|failed|unable|could not' <<<"$line" && return 1
                ;;
        esac
        if grep -Eiq 'failed to fetch|some index files failed|temporary failure resolving|could not resolve|err:[[:space:]]' <<<"$line"; then
            return 1
        fi
    done <<<"$output"
    ((found == 1))
}

apt_update_release_info_change_repos()
{
    local output="$1"
    sed -n "s/^E: Repository '\([^']*\)' changed its '[^']*' value from '[^']*' to '[^']*'.*$/\1/p" <<<"$output" \
        | LC_ALL=C sort -u \
        | awk 'BEGIN { sep = "" } { printf "%s%s", sep, $0; sep = ", " } END { if (NR > 0) printf "\n" }'
}

apt_update_release_info_change_details()
{
    local output="$1"
    sed -n "s/^E: Repository '\([^']*\)' changed its '\([^']*\)' value from '\([^']*\)' to '\([^']*\)'.*$/  \1: \2 changed from '\3' to '\4'/p" <<<"$output"
}

# Byte fingerprint of the APT package-list cache the metadata refresh may
# rewrite.  A refresh that only answers "Hit" leaves the list files untouched;
# an identical fingerprint plus no fetched index proves the no-op.
apt_lists_fingerprint()
{
    local dir="$TARGET_ROOT/var/lib/apt/lists" file
    command -v sha256sum >/dev/null 2>&1 || { printf 'no-sha256sum\n'; return 0; }
    [[ -d "$dir" ]] || { printf 'lists missing\n'; return 0; }
    while IFS= read -r file; do
        [[ -n "$file" ]] || continue
        printf '%s %s\n' "${file#"$TARGET_ROOT"}" "$(repair_file_fingerprint "$file")"
    done < <(find "$dir" -maxdepth 1 -type f ! -name 'lock' 2>/dev/null | LC_ALL=C sort)
}

# Refresh package metadata.  When the only failure is a repository release
# metadata change, name the affected repositories, retry once with
# Acquire::AllowReleaseInfoChange=true and continue.  This option is confined
# to this metadata refresh; package install/upgrade transactions never receive
# it, and no signature, keyring or integrity check is relaxed.
apt_update_allow_release_info_retry()
{
    local output rc first_rc first_error repos details lists_before lists_after

    lists_before="$(apt_lists_fingerprint)"
    log "BEGIN: Refresh package metadata" | tee -a "$SESSION_LOG"
    set +e
    output="$(run_selected_chroot /usr/bin/env \
        HOME=/root \
        PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
        DEBIAN_FRONTEND=noninteractive \
        APT_LISTCHANGES_FRONTEND=none \
        apt-get update 2>&1)"
    rc=$?
    set -e
    printf '%s\n' "$output" | tee -a "$SESSION_LOG"

    if ((rc != 0)); then
        apt_update_release_info_change_only "$output" \
            || fail "Refresh package metadata failed with exit code $rc."
        first_rc=$rc
        first_error="$(grep -E -m 1 '^(E:|Err:)' <<<"$output" || true)"
        repos="$(apt_update_release_info_change_repos "$output")"
        details="$(apt_update_release_info_change_details "$output")"
        log "WARNING: apt-get update refused a repository release metadata change for: ${repos:-unknown repository}" | tee -a "$SESSION_LOG"
        [[ -z "$details" ]] || printf '%s\n' "$details" | tee -a "$SESSION_LOG"
        log "WARNING: retrying metadata refresh with -o Acquire::AllowReleaseInfoChange=true (release-info changes only; signatures, keys and package verification remain enforced)." | tee -a "$SESSION_LOG"
        set +e
        output="$(run_selected_chroot /usr/bin/env \
            HOME=/root \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            DEBIAN_FRONTEND=noninteractive \
            APT_LISTCHANGES_FRONTEND=none \
            apt-get update -o Acquire::AllowReleaseInfoChange=true 2>&1)"
        rc=$?
        set -e
        printf '%s\n' "$output" | tee -a "$SESSION_LOG"
        if ((rc != 0)); then
            fail "Refresh package metadata failed with exit code $rc after accepting repository release metadata changes. Original error (exit code $first_rc): ${first_error:-see transcript above}"
        fi
        if grep -Eiq 'failed to fetch|some index files failed|temporary failure resolving|could not resolve|err:[[:space:]]' <<<"$output"; then
            fail "Refresh package metadata did not complete after accepting repository release metadata changes; repository indexes could not be refreshed. Check target networking or repository configuration."
        fi
        log "WARN: accepted release metadata change for ${repos:-unknown repository}; metadata refreshed." | tee -a "$SESSION_LOG"
    else
        # apt-get can return success while retaining stale indexes when every
        # repository fetch fails. Treat that as a failed refresh so a subsequent
        # package repair never proceeds on misleading metadata.
        if grep -Eiq 'failed to fetch|some index files failed|temporary failure resolving|could not resolve|err:[[:space:]]' <<<"$output"; then
            fail "Refresh package metadata did not complete; repository indexes could not be refreshed. Check target networking or repository configuration."
        fi
    fi
    lists_after="$(apt_lists_fingerprint)"
    if [[ "$lists_before" != "lists missing" && "$lists_before" != "no-sha256sum" \
          && "$lists_before" == "$lists_after" ]] \
       && ! grep -Eiq '^Get:' <<<"$output"; then
        log "PASS: Refresh package metadata (package lists byte-identical; no repository index was fetched)" | tee -a "$SESSION_LOG"
        repair_change_status aptupdate "unchanged|APT package lists are byte-identical and no repository index was fetched"
        return 0
    fi
    log "PASS: Refresh package metadata" | tee -a "$SESSION_LOG"
    log "APT package lists changed or a repository index was fetched; reporting the metadata refresh as changed." | tee -a "$SESSION_LOG"
    repair_change_status aptupdate changed
}

run_apt_update()
{
    apt_update_allow_release_info_retry
}

# Complete interrupted dpkg configuration.  dpkg-query proves whether any
# package is still unpacked/half-configured before the apply step runs; only a
# proven-empty pending set may report unchanged.
dpkg_configure_stage()
{
    local pending=false
    if dpkg_configuration_pending; then
        pending=true
    fi
    run_chroot "Complete interrupted package configuration" dpkg --configure -a
    if [[ "$pending" == true ]]; then
        repair_change_status dpkg changed
    else
        repair_change_status dpkg "unchanged|dpkg reported no packages pending configuration"
    fi
}

APT_SIM_OUTPUT=""
APT_SIM_RC=0

simulate_apt_upgrade_mode()
{
    local mode="$1" output rc

    log "SIMULATE: apt-get $mode (no packages will be changed)" | tee -a "$SESSION_LOG"

    set +e
    output="$(
        run_selected_chroot /usr/bin/env \
            HOME=/root \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            DEBIAN_FRONTEND=noninteractive \
            APT_LISTCHANGES_FRONTEND=none \
            apt-get -s -o Debug::NoLocking=1 "$mode" 2>&1
    )"
    rc=$?
    set -e

    APT_SIM_OUTPUT="$output"
    APT_SIM_RC=$rc

    printf '%s\n' "$output" | tee -a "$SESSION_LOG"
    log "Simulation exit code for '$mode': $rc" | tee -a "$SESSION_LOG"
}

apt_simulation_has_pending_packages()
{
    local output="$1"
    grep -Eiq \
        'packages? (have|has) been kept back|[[:space:]][1-9][0-9]* not upgraded\.?$' \
        <<<"$output"
}

apt_simulation_requests_full_upgrade()
{
    local output="$1"
    grep -Eiq \
        "upgrade.*(disabled|not supported)|use .*full-upgrade|use .*dist-upgrade|full-upgrade.*dist-upgrade|dist-upgrade.*full-upgrade" \
        <<<"$output"
}

apt_simulation_is_safe()
{
    local output="$1" pkg
    local -a removals=()
    local max_removals=20

    if grep -Eiq 'essential packages will be removed|WARNING:.*essential.*removed' <<<"$output"; then
        log "REFUSED: APT simulation proposes removing essential packages." | tee -a "$SESSION_LOG"
        return 1
    fi

    mapfile -t removals < <(awk '$1 == "Remv" {print $2}' <<<"$output")

    for pkg in "${removals[@]}"; do
        pkg="${pkg%%:*}"
        case "$pkg" in
            tuxedoos-desktop|tuxedo-base-files|tuxedo-btrfs|linux-image-*|linux-headers-*|linux-modules-*|\
            sddm|plasma-desktop|plasma-workspace|cryptsetup|cryptsetup-initramfs|initramfs-tools|initramfs-tools-core|\
            grub*|systemd*|libc6*|apt|apt-transport-*|dpkg|base-files|base-passwd)
                log "REFUSED: APT simulation would remove protected package '$pkg'." | tee -a "$SESSION_LOG"
                return 1
                ;;
        esac
    done

    if ((${#removals[@]} > max_removals)); then
        log "REFUSED: APT simulation would remove ${#removals[@]} packages (safety limit: $max_removals)." | tee -a "$SESSION_LOG"
        return 1
    fi

    if ((${#removals[@]} > 0)); then
        log "APT simulation proposes ${#removals[@]} non-protected package removal(s): ${removals[*]}" | tee -a "$SESSION_LOG"
    fi

    return 0
}

# Upgrade installed packages with the least invasive safe transaction:
# simulate upgrade first, then full-upgrade/dist-upgrade only when the target's
# policy demands it or packages remain pending.  Every candidate is validated
# by apt_simulation_is_safe before the chosen transaction is applied.
adaptive_apt_upgrade()
{
    local chosen="" upgrade_output="" full_output="" dist_output="" chosen_output=""
    local apply_output="" state=""

    package_feedback_reset
    [[ -x "$TARGET_ROOT/usr/bin/apt-get" ]] \
        || fail "apt-get is not installed in the target system."

    # Always trial the least invasive mode first. Some distributions, notably
    # current TUXEDO OS releases, intentionally disable apt-get upgrade and tell
    # callers to use full-upgrade/dist-upgrade instead. We follow that feedback
    # only after simulating and safety-checking the suggested transaction.
    simulate_apt_upgrade_mode upgrade
    upgrade_output="$APT_SIM_OUTPUT"

    if ((APT_SIM_RC == 0)); then
        apt_simulation_is_safe "$upgrade_output" \
            || fail "The simulated apt-get upgrade transaction failed Boot Bitch safety checks."

        if apt_simulation_has_pending_packages "$upgrade_output"; then
            log "Standard upgrade leaves packages pending; evaluating full-upgrade simulation." | tee -a "$SESSION_LOG"
            simulate_apt_upgrade_mode full-upgrade
            full_output="$APT_SIM_OUTPUT"
            if ((APT_SIM_RC == 0)) && apt_simulation_is_safe "$full_output"; then
                chosen="full-upgrade"
            else
                log "Full-upgrade simulation was unavailable or unsafe; using the successful standard upgrade transaction." | tee -a "$SESSION_LOG"
                chosen="upgrade"
            fi
        else
            chosen="upgrade"
        fi
    elif apt_simulation_requests_full_upgrade "$upgrade_output"; then
        log "Target package policy rejected standard upgrade and requested full/dist upgrade; evaluating full-upgrade." | tee -a "$SESSION_LOG"
        simulate_apt_upgrade_mode full-upgrade
        full_output="$APT_SIM_OUTPUT"

        if ((APT_SIM_RC == 0)) && apt_simulation_is_safe "$full_output"; then
            chosen="full-upgrade"
        else
            log "full-upgrade simulation did not produce an acceptable transaction; evaluating dist-upgrade." | tee -a "$SESSION_LOG"
            simulate_apt_upgrade_mode dist-upgrade
            dist_output="$APT_SIM_OUTPUT"
            if ((APT_SIM_RC == 0)) && apt_simulation_is_safe "$dist_output"; then
                chosen="dist-upgrade"
            else
                fail "APT rejected standard upgrade and neither full-upgrade nor dist-upgrade produced a safe successful simulation."
            fi
        fi
    else
        fail "apt-get upgrade simulation failed for a reason that did not request a different upgrade mode. Review the simulation output above."
    fi

    case "$chosen" in
        upgrade) chosen_output="$upgrade_output" ;;
        full-upgrade) chosen_output="$full_output" ;;
        dist-upgrade) chosen_output="$dist_output" ;;
    esac

    log "APT upgrade decision: '$chosen' selected from simulation results." | tee -a "$SESSION_LOG"
    run_chroot_try "Upgrade installed packages ($chosen)" apt-get -y "$chosen"
    ((CHROOT_TRY_RC == 0)) \
        || fail "Upgrade installed packages ($chosen) failed with exit code $CHROOT_TRY_RC."
    apply_output="$CHROOT_TRY_OUTPUT"

    # dpkg --audit is non-destructive and gives an immediate post-upgrade sanity
    # check. Any output is logged for the recovery record without turning a
    # harmless informational audit into a second package operation.
    log "Post-upgrade dpkg audit:" | tee -a "$SESSION_LOG"
    run_selected_chroot /usr/bin/env \
        HOME=/root \
        PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
        dpkg --audit 2>&1 | tee -a "$SESSION_LOG" || true

    # The chosen simulation and the apply transcript both report kept-back
    # packages; merge them so the names and the "N not upgraded" count stay
    # visible without duplicating evidence.
    apt_package_feedback_report "$chosen_output" "$apply_output"
    package_feedback_publish

    # The apply step is the same transaction that was just simulated; a
    # simulation with no package, removal or configuration action proves the
    # upgrade performed no work.  Kept-back packages never turn the stage into
    # a failure; they are appended to the status reason instead.
    if apt_transaction_reported_no_changes "$chosen_output"; then
        state="unchanged|simulated upgrade transaction proposed no package changes"
    else
        state="changed"
    fi
    if [[ -n "$PACKAGE_FEEDBACK_SUMMARY" ]]; then
        if [[ "$state" == unchanged* ]]; then
            state+="; $PACKAGE_FEEDBACK_SUMMARY"
        else
            state+="|$PACKAGE_FEEDBACK_SUMMARY"
        fi
    fi
    repair_change_status upgrade "$state"
}

# Repair broken APT dependencies: simulate `apt-get -f install`, run it through
# the shared removal safety policy and only then apply the identical command.
adaptive_fix_broken()
{
    local output rc state=""

    package_feedback_reset
    [[ -x "$TARGET_ROOT/usr/bin/apt-get" ]] \
        || fail "apt-get is not installed in the target system."

    # APT's --fix-broken transaction can remove or replace packages.  Keep the
    # same simulation and removal policy used by apt-upgrade before allowing
    # the modifying transaction to run.
    log "SIMULATE: apt-get --fix-broken install (no packages will be changed)" | tee -a "$SESSION_LOG"
    set +e
    output="$(
        run_selected_chroot /usr/bin/env \
            HOME=/root \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            DEBIAN_FRONTEND=noninteractive \
            APT_LISTCHANGES_FRONTEND=none \
            apt-get -s -o Debug::NoLocking=1 -f install 2>&1
    )"
    rc=$?
    set -e
    printf '%s\n' "$output" | tee -a "$SESSION_LOG"
    log "Simulation exit code for '--fix-broken install': $rc" | tee -a "$SESSION_LOG"
    ((rc == 0)) || fail "APT --fix-broken simulation failed; no packages were changed."
    apt_simulation_is_safe "$output" \
        || fail "The simulated apt-get --fix-broken transaction failed Boot Bitch safety checks."

    run_chroot_try "Repair broken package dependencies" apt-get -y -f install
    ((CHROOT_TRY_RC == 0)) \
        || fail "Repair broken package dependencies failed with exit code $CHROOT_TRY_RC."

    # The simulation and the apply transcript both report kept-back packages;
    # merge them into the same evidence lines used by the upgrade stage.
    apt_package_feedback_report "$output" "$CHROOT_TRY_OUTPUT"
    package_feedback_publish

    # The simulation describes the exact apply transaction; no proposed
    # package/removal/configuration action proves nothing was written.
    # Kept-back packages stay non-fatal and are appended to the status reason.
    if apt_transaction_reported_no_changes "$output"; then
        state="unchanged|simulated fix-broken transaction proposed no package changes"
    else
        state="changed"
    fi
    if [[ -n "$PACKAGE_FEEDBACK_SUMMARY" ]]; then
        if [[ "$state" == unchanged* ]]; then
            state+="; $PACKAGE_FEEDBACK_SUMMARY"
        else
            state+="|$PACKAGE_FEEDBACK_SUMMARY"
        fi
    fi
    repair_change_status fixbroken "$state"
}

ARCH_PACMAN_TRY_OUTPUT=""
ARCH_PACMAN_TRY_RC=0

# ---------------------------------------------------------------------------
# Arch/pacman repair backend (one sandboxed full transaction)
# ---------------------------------------------------------------------------
# Copy the target pacman database into a request-scoped /tmp sandbox (without
# sync databases) so `pacman -Syyuw` can resolve a full transaction without
# touching the target package database.  The sandbox is removed by cleanup().
arch_pacman_prepare_sandbox()
{
    local tag
    target_pacman_detected \
        || fail "Arch pacman backend is not selected for this target: no pacman database or executable was detected."
    [[ -x "$TARGET_ROOT/usr/bin/pacman" || -x "$TARGET_ROOT/usr/bin/pacman-static" ]] \
        || fail "pacman is not installed in the target system."
    [[ -f "$TARGET_ROOT/etc/pacman.conf" ]] \
        || fail "The target has no /etc/pacman.conf; refusing a package transaction."
    [[ -d "$TARGET_ROOT/var/lib/pacman" ]] \
        || fail "The target pacman database directory is missing."
    [[ ! -e "$TARGET_ROOT/var/lib/pacman/db.lck" ]] \
        || fail "The target pacman database is locked; refusing a concurrent package transaction."

    tag="$(basename -- "$SESSION_DIR")"
    [[ "$tag" =~ ^session\.[[:alnum:]]+$ ]] || tag="session"
    ARCH_PACMAN_SANDBOX="/tmp/boot-repair-pacman-$tag"
    ARCH_PACMAN_DB_REL="$ARCH_PACMAN_SANDBOX/db"
    ARCH_PACMAN_CACHE_REL="$ARCH_PACMAN_SANDBOX/cache"
    ARCH_PACMAN_LOG_REL="$ARCH_PACMAN_SANDBOX/pacman.log"
    TEMP_TARGET_PATHS+=("$ARCH_PACMAN_SANDBOX")
    rm -rf -- "$TARGET_ROOT$ARCH_PACMAN_SANDBOX"
    mkdir -p -- "$TARGET_ROOT$ARCH_PACMAN_DB_REL" "$TARGET_ROOT$ARCH_PACMAN_CACHE_REL"
    cp -a -- "$TARGET_ROOT/var/lib/pacman/." "$TARGET_ROOT$ARCH_PACMAN_DB_REL/"
    rm -rf -- "$TARGET_ROOT$ARCH_PACMAN_DB_REL/sync"
    mkdir -p -- "$TARGET_ROOT$ARCH_PACMAN_DB_REL/sync"
    log "Arch pacman transaction sandbox prepared under $ARCH_PACMAN_SANDBOX; the target package database will not be used for preflight." | tee -a "$SESSION_LOG"
}

arch_pacman_transaction_try()
{
    local label="$1"; shift
    local output rc
    log "TRY: $label" | tee -a "$SESSION_LOG"
    set +e
    output="$(
        run_selected_chroot /usr/bin/env \
            HOME=/root \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            pacman --config /etc/pacman.conf \
                --dbpath "$ARCH_PACMAN_DB_REL" \
                --cachedir "$ARCH_PACMAN_CACHE_REL" \
                --logfile "$ARCH_PACMAN_LOG_REL" \
                "$@" 2>&1
    )"
    rc=$?
    set -e
    ARCH_PACMAN_TRY_OUTPUT="$output"
    ARCH_PACMAN_TRY_RC=$rc
    printf '%s\n' "$output" | tee -a "$SESSION_LOG"
    log "TRY exit code: $rc ($label)" | tee -a "$SESSION_LOG"
    return 0
}

arch_pacman_preflight_result()
{
    local rc="$1" output="$2" fatal_errors
    ((rc == 0)) || return 1
    fatal_errors="$(grep -E '^error:' <<<"$output" 2>/dev/null | grep -Ev "^error: failed retrieving file '.+' from .+ : " || true)"
    [[ -z "$fatal_errors" ]]
}

arch_pacman_transaction_is_safe()
{
    local output="$1" rc="${2:-0}" package_count mirror_failures mirrors
    if ! arch_pacman_preflight_result "$rc" "$output"; then
        log "REFUSED: pacman preflight reported repository, download or transaction integrity errors." | tee -a "$SESSION_LOG"
        return 1
    fi
    mirror_failures="$(grep -Ec "^error: failed retrieving file '.+' from .+ : " <<<"$output" 2>/dev/null || true)"
    if ((mirror_failures > 0)); then
        log "WARN: pacman encountered ${mirror_failures} recoverable mirror retrieval failure(s)." | tee -a "$SESSION_LOG"
        mirrors="$(sed -nE "s/^error: failed retrieving file '.+' from ([^ ]+) : .*/\1/p" <<<"$output" | sort -u)"
        while IFS= read -r mirror; do
            [[ -n "$mirror" ]] && log "WARN: failing mirror: $mirror" | tee -a "$SESSION_LOG"
        done <<<"$mirrors"
        log "Arch pacman transaction completed successfully despite mirror fallback." | tee -a "$SESSION_LOG"
    fi
    if grep -Eiq '(^|[[:space:]])(removing|remove)[[:space:]]|packages[[:space:]]+to[[:space:]]+remove|cannot[[:space:]]+resolve[[:space:]]+dependencies' <<<"$output"; then
        log "REFUSED: pacman preflight proposes removals or unresolved dependencies." | tee -a "$SESSION_LOG"
        return 1
    fi
    package_count="$(sed -nE 's/^[[:space:]]*Packages \(([0-9]+)\):.*/\1/p; s/^[[:space:]]*Packages \(([0-9]+)\).*/\1/p' <<<"$output" | head -1)"
    if [[ "$package_count" =~ ^[0-9]+$ ]] && ((package_count > 1000)); then
        log "REFUSED: pacman preflight proposes $package_count packages (safety limit: 1000)." | tee -a "$SESSION_LOG"
        return 1
    fi
    return 0
}

preflight_arch_pacman_transaction()
{
    arch_pacman_prepare_sandbox
    log "SIMULATE/PREFLIGHT: full Arch pacman transaction (sandboxed database/cache; no target packages will be changed)" | tee -a "$SESSION_LOG"
    arch_pacman_transaction_try "Arch pacman full transaction preflight" -Syyuw --noconfirm
    ((ARCH_PACMAN_TRY_RC == 0)) \
        || fail "Arch pacman transaction preflight failed; no target packages were changed."
    arch_pacman_transaction_is_safe "$ARCH_PACMAN_TRY_OUTPUT" "$ARCH_PACMAN_TRY_RC" \
        || fail "Arch pacman transaction was rejected by Boot Bitch safety policy."
    log "PASS: Arch pacman transaction preflight resolved without removals." | tee -a "$SESSION_LOG"
}

# Arch pacman apply output proves "no work" only when the transaction reports
# nothing to do and printed no package header, action or removal line.  Any
# package change, removal or unexpected transaction output stays "changed".
arch_pacman_transaction_reported_no_changes()
{
    local output="$1"
    grep -Eiq '^[[:space:]]*Packages \([0-9]+\)' <<<"$output" && return 1
    grep -Eiq '^\([0-9]+/[0-9]+\)[[:space:]]+(installing|upgrading|downgrading|reinstalling|removing)' <<<"$output" && return 1
    grep -Eiq '^[[:space:]]*(installing|upgrading|downgrading|reinstalling|removing)[[:space:]]' <<<"$output" && return 1
    grep -Eiq 'there is nothing to do' <<<"$output"
}

adaptive_arch_pacman_repair()
{
    local label="${1:-Upgrade installed packages}" tool_key="${2:-upgrade}" state=""
    package_feedback_reset
    preflight_arch_pacman_transaction
    run_chroot_try "$label (pacman -Syu)" pacman --noconfirm -Syu
    ((CHROOT_TRY_RC == 0)) || fail "$label failed with exit code $CHROOT_TRY_RC."
    log "PASS: $label completed through one full pacman transaction." | tee -a "$SESSION_LOG"

    # Ignored package upgrades ("warning: <pkg>: ignoring package upgrade") are
    # non-fatal feedback from both the preflight and the apply transaction.
    pacman_package_feedback_report "$ARCH_PACMAN_TRY_OUTPUT" "$CHROOT_TRY_OUTPUT"
    package_feedback_publish

    if arch_pacman_transaction_reported_no_changes "$CHROOT_TRY_OUTPUT"; then
        state="unchanged|pacman transaction reported no packages to install, upgrade or remove"
    else
        state="changed"
    fi
    if [[ -n "$PACKAGE_FEEDBACK_SUMMARY" ]]; then
        if [[ "$state" == unchanged* ]]; then
            state+="; $PACKAGE_FEEDBACK_SUMMARY"
        else
            state+="|$PACKAGE_FEEDBACK_SUMMARY"
        fi
    fi
    repair_change_status "$tool_key" "$state"
}

CHROOT_TRY_OUTPUT=""
CHROOT_TRY_RC=0

# Non-fatal chroot runner used by simulation/trial preflights: capture output
# and status in CHROOT_TRY_OUTPUT/CHROOT_TRY_RC instead of failing, so callers
# can inspect the result and apply known corrections.
run_chroot_try()
{
    local label="$1"; shift
    local output rc

    log "TRY: $label" | tee -a "$SESSION_LOG"
    set +e
    output="$(
        run_selected_chroot /usr/bin/env \
            HOME=/root \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            DEBIAN_FRONTEND=noninteractive \
            APT_LISTCHANGES_FRONTEND=none \
            "$@" 2>&1
    )"
    rc=$?
    set -e

    CHROOT_TRY_OUTPUT="$output"
    CHROOT_TRY_RC=$rc
    printf '%s\n' "$output" | tee -a "$SESSION_LOG"
    log "TRY exit code: $rc ($label)" | tee -a "$SESSION_LOG"
    return 0
}

# ---------------------------------------------------------------------------
# Alpine/apk repair backend (simulation-first guarded package transactions)
# ---------------------------------------------------------------------------
# Safety caps for the Alpine backend.  APK_MAX_TRANSACTION_CHANGES bounds a
# simulated package transaction (the same 1000-package limit as the pacman and
# APT backends); APK_MAX_MISSING_FILES/APK_MAX_MISSING_PACKAGES bound the
# read-only missing-file detection so a corrupt installed database cannot
# produce an unbounded repair.
APK_MAX_TRANSACTION_CHANGES=1000
APK_MAX_MISSING_FILES=1000
APK_MAX_MISSING_PACKAGES=1000

# Read-only audit of the target's missing package files.  apk-tools reports
# "X <path>" for every file recorded in the installed database that no longer
# exists.  run_chroot_try tees the raw transcript into the session log, so
# stdout is suppressed here and only the missing paths are printed in stable
# order for command-substitution callers.  Fails closed when the audit cannot
# run or reports an error.
alpine_apk_audit_missing_paths()
{
    local output
    run_chroot_try "Alpine apk audit --system (missing-file detection)" \
        apk audit --system >/dev/null
    (( CHROOT_TRY_RC == 0 )) \
        || fail "apk audit --system failed with exit code $CHROOT_TRY_RC; refusing a package repair without reliable missing-file detection."
    output="$CHROOT_TRY_OUTPUT"
    if grep -Eq '^ERROR(:|[[:space:]])|^error:' <<<"$output"; then
        fail "apk audit --system reported an error; refusing a package repair without reliable missing-file detection."
    fi
    printf '%s\n' "$output" | sed -n 's/^X //p' | LC_ALL=C sort
}

# Map missing files to their owning installed packages with one read-only
# `apk info --who-owns` query inside the target chroot.  Prints the
# deduplicated package names and fails closed when the mapping is incomplete,
# an owner string cannot be parsed, or a safety cap is exceeded.
alpine_apk_missing_file_packages()
{
    local audit_output owner name
    local -a missing_paths=() absolute_paths=() owners=() packages=()

    if ! audit_output="$(alpine_apk_audit_missing_paths)"; then
        fail "Alpine missing-file detection failed; refusing the package repair."
    fi
    if [[ -n "$audit_output" ]]; then
        mapfile -t missing_paths <<<"$audit_output"
    fi
    ((${#missing_paths[@]} > 0)) || return 0
    if ((${#missing_paths[@]} > APK_MAX_MISSING_FILES)); then
        fail "apk audit --system reports ${#missing_paths[@]} missing files (safety limit: $APK_MAX_MISSING_FILES); refusing an unbounded repair."
    fi

    for owner in "${missing_paths[@]}"; do
        absolute_paths+=("/$owner")
    done
    run_chroot_try "Alpine apk info --who-owns (${#missing_paths[@]} missing files)" \
        apk info --who-owns "${absolute_paths[@]}" >/dev/null
    (( CHROOT_TRY_RC == 0 )) \
        || fail "apk info --who-owns could not map every missing file to an installed package; refusing the repair."
    mapfile -t owners < <(sed -n 's/^.* is owned by //p' <<<"$CHROOT_TRY_OUTPUT")
    ((${#owners[@]} == ${#missing_paths[@]})) \
        || fail "apk info --who-owns returned ${#owners[@]} owner line(s) for ${#missing_paths[@]} missing file(s); refusing the repair."

    for owner in "${owners[@]}"; do
        # Owner strings are "name-version-release"; neither a package name nor
        # a version may contain a dash, so the shortest suffix match strips the
        # version without guessing where the name ends.
        name="${owner%-*-r*}"
        [[ -n "$name" && "$name" != "$owner" ]] \
            || fail "Unrecognized apk owner string '$owner'; refusing the repair."
        [[ "$name" =~ ^[A-Za-z0-9][A-Za-z0-9._+-]*$ ]] \
            || fail "Unrecognized apk package name '$name'; refusing the repair."
        packages+=("$name")
    done
    mapfile -t packages < <(printf '%s\n' "${packages[@]}" | LC_ALL=C sort -u)
    if ((${#packages[@]} > APK_MAX_MISSING_PACKAGES)); then
        fail "The missing files map to ${#packages[@]} packages (safety limit: $APK_MAX_MISSING_PACKAGES); refusing an unbounded repair."
    fi
    printf '%s\n' "${packages[@]}"
}

# Digest of the missing-file situation used by alpine_apk_fingerprint(): a
# transaction that restores deleted package files while leaving world and
# installed byte-identical must still be reported as changed.
alpine_apk_missing_files_fingerprint()
{
    local paths
    command -v sha256sum >/dev/null 2>&1 || { printf 'no-sha256sum\n'; return 0; }
    paths="$(alpine_apk_audit_missing_paths)" || return 1
    printf '%s\n' "$paths" | sha256sum | awk '{print $1}'
}

# Byte fingerprint of the apk world/installed databases and the missing-file
# situation.  Used to prove that a simulation did not write and that a no-op
# apply left the package state byte-identical; /var/log/apk.log is deliberately
# excluded because apk may append a transaction banner even when no package
# changed.
alpine_apk_fingerprint()
{
    local world="$TARGET_ROOT/etc/apk/world" installed="$TARGET_ROOT/lib/apk/db/installed"
    command -v sha256sum >/dev/null 2>&1 || { printf 'no-sha256sum\n'; return 0; }
    printf 'world '
    repair_file_fingerprint "$world"
    printf 'installed '
    repair_file_fingerprint "$installed"
    printf 'missing '
    alpine_apk_missing_files_fingerprint
}

# Read-only evidence probe: report whether the target apk database lock is
# currently held.  Diagnostics and capability evidence must never be refused
# because of a held lock; this only records the state.  A lock file the current
# user cannot read (for example the root-owned host lock probed by an
# unprivileged test harness) is reported as unknown, not held.
alpine_apk_lock_held()
{
    local lock="${TARGET_ROOT:-/}/lib/apk/db/lock"
    [[ -e "$lock" ]] || return 1
    command -v flock >/dev/null 2>&1 || return 1
    [[ -r "$lock" ]] || return 1
    ! flock -n "$lock" true 2>/dev/null
}

# Mandatory read-only apk preflight: executable, repositories, installed
# database, world file, database lock, /boot free space and the recorded apk
# version.  Fails closed before any simulation or apply.  The host apk process
# check belongs to this modifying-stage preflight only, never to the generic
# host gate or the read-only capability path.
alpine_apk_preflight()
{
    local free_kb largest=0 size image required_kb
    local lock="$TARGET_ROOT/lib/apk/db/lock"

    target_apk_detected \
        || fail "Alpine apk backend is not selected for this target: no apk database or executable was detected."
    if (( RUNNING_HOST_MODE == 1 )) && pgrep -x apk >/dev/null 2>&1; then
        fail "Package manager process 'apk' is already running; refusing a concurrent host package repair."
    fi
    target_has_executable /sbin/apk /usr/sbin/apk /usr/bin/apk \
        || fail "apk is not installed in the target system."
    if [[ ! -s "$TARGET_ROOT/etc/apk/repositories" ]] \
        || ! grep -Eq '^[[:space:]]*[^#[:space:]]' "$TARGET_ROOT/etc/apk/repositories" 2>/dev/null; then
        fail "The target has no configured apk repositories."
    fi
    [[ -f "$TARGET_ROOT/lib/apk/db/installed" ]] \
        || fail "The target apk installed database is missing."
    [[ -f "$TARGET_ROOT/etc/apk/world" ]] \
        || fail "The target apk world file is missing."
    if command -v flock >/dev/null 2>&1 && [[ -e "$lock" ]]; then
        flock -n "$lock" true 2>/dev/null \
            || fail "The target apk database is locked; refusing a concurrent transaction."
    fi
    [[ -d "$TARGET_ROOT/boot" ]] \
        || fail "The target has no /boot directory; refusing a package transaction that can rebuild kernels."
    target_path_is_mounted_rw "$TARGET_ROOT/boot" \
        || fail "The target /boot is not mounted read-write; refusing a package transaction that can rebuild kernels."
    while IFS= read -r image; do
        [[ -n "$image" ]] || continue
        size="$(stat -c '%s' "$TARGET_ROOT/boot/$image" 2>/dev/null || printf '0')"
        [[ "$size" =~ ^[0-9]+$ ]] || size=0
        if (( size > largest )); then
            largest="$size"
        fi
    done < <(alpine_initramfs_images)
    # Two copies of the largest initramfs plus a 64 MiB working margin.
    required_kb=$(( (largest * 2 + 1023) / 1024 + 65536 ))
    free_kb="$(df -Pk "$TARGET_ROOT/boot" 2>/dev/null | awk 'NR==2 {print $4}' | head -n1 || true)"
    if [[ "$free_kb" =~ ^[0-9]+$ ]] && (( free_kb < required_kb )); then
        fail "The target /boot has ${free_kb} KiB free; at least ${required_kb} KiB is required for a safe package transaction."
    fi
    run_chroot_try "Record Alpine apk version" apk --version
    log "Alpine apk preflight: $(sed -n '1p' <<<"$CHROOT_TRY_OUTPUT")" | tee -a "$SESSION_LOG"
}

APK_SIM_OUTPUT=""
APK_SIM_RC=0

alpine_apk_transaction_try()
{
    local label="$1"; shift
    local output rc
    log "TRY: $label" | tee -a "$SESSION_LOG"
    set +e
    output="$(
        run_selected_chroot /usr/bin/env \
            HOME=/root \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            apk "$@" 2>&1
    )"
    rc=$?
    set -e
    APK_SIM_OUTPUT="$output"
    APK_SIM_RC=$rc
    printf '%s\n' "$output" | tee -a "$SESSION_LOG"
    log "TRY exit code: $rc ($label)" | tee -a "$SESSION_LOG"
    return 0
}

# Fail-closed output policy for an apk simulation: no removals (except a
# same-package replacement), no downgrades, no untrusted signatures, no locked
# database, no unresolved packages, no incomplete metadata, no unrecognized
# errors and a bounded package count.
alpine_apk_simulation_is_safe()
{
    local output="$1" rc="${2:-0}" removal change allowed count=0
    local -a changes=() removals=()

    (( rc == 0 )) || { log "REFUSED: apk simulation failed with exit code $rc." | tee -a "$SESSION_LOG"; return 1; }
    if grep -Eiq 'UNTRUSTED signature|BAD signature|verification failed|WARNING:.*UNTRUSTED' <<<"$output"; then
        log "REFUSED: apk simulation reported an untrusted package signature." | tee -a "$SESSION_LOG"
        return 1
    fi
    if grep -Eiq 'database locked|another apk instance|unable to lock|lock file' <<<"$output"; then
        log "REFUSED: apk simulation reported a locked package database." | tee -a "$SESSION_LOG"
        return 1
    fi
    if grep -Eiq 'unable to select packages|unsatisfiable constraints|has no installation candidate|conflicting' <<<"$output"; then
        log "REFUSED: apk simulation reported unresolved or conflicting packages." | tee -a "$SESSION_LOG"
        return 1
    fi
    if grep -Eiq 'temporary error|failed to fetch|APKINDEX' <<<"$output"; then
        log "REFUSED: apk simulation reported incomplete repository metadata." | tee -a "$SESSION_LOG"
        return 1
    fi
    if grep -Eiq '(^|[[:space:]])Downgrading[[:space:]]' <<<"$output"; then
        log "REFUSED: apk simulation proposes a package downgrade." | tee -a "$SESSION_LOG"
        return 1
    fi
    if grep -Eq '^[[:space:]]*ERROR|(^|[[:space:]])error:' <<<"$output"; then
        log "REFUSED: apk simulation reported an error Boot Bitch does not recognize as safe." | tee -a "$SESSION_LOG"
        return 1
    fi

    mapfile -t changes < <(sed -nE 's/^\([0-9]+\/[0-9]+\)[[:space:]]+(Installing|Upgrading|Replacing|Reinstalling)[[:space:]]+([^[:space:]]+).*/\2/p' <<<"$output")
    mapfile -t removals < <(sed -nE 's/^\([0-9]+\/[0-9]+\)[[:space:]]+(Purging|Removing)[[:space:]]+([^[:space:]]+).*/\2/p' <<<"$output")
    count=$(( ${#changes[@]} + ${#removals[@]} ))
    if (( count > APK_MAX_TRANSACTION_CHANGES )); then
        log "REFUSED: apk simulation proposes $count package changes (safety limit: $APK_MAX_TRANSACTION_CHANGES)." | tee -a "$SESSION_LOG"
        return 1
    fi
    for removal in "${removals[@]}"; do
        allowed=false
        for change in "${changes[@]}"; do
            if [[ "$change" == "$removal" ]]; then
                allowed=true
                break
            fi
        done
        if [[ "$allowed" != true ]]; then
            log "REFUSED: apk simulation would remove '$removal' without replacing it in the same transaction." | tee -a "$SESSION_LOG"
            return 1
        fi
        log "apk simulation replaces package '$removal' in the same transaction; removal accepted." | tee -a "$SESSION_LOG"
    done
    return 0
}

# apk has no "nothing to do" summary: a simulation that printed no package
# action line proposed no changes.  Callers additionally require identical
# world/installed fingerprints before reporting unchanged.
alpine_apk_transaction_reported_no_changes()
{
    local output="$1"
    grep -Eq '^\([0-9]+/[0-9]+\)[[:space:]]+(Installing|Upgrading|Replacing|Reinstalling|Downgrading|Purging|Removing)[[:space:]]' <<<"$output" && return 1
    grep -Eq '^[[:space:]]*(Installing|Upgrading|Replacing|Reinstalling|Downgrading|Purging|Removing)[[:space:]]' <<<"$output" && return 1
    return 0
}

# Shared simulate/apply engine for the Alpine package stages.  The caller has
# already run alpine_apk_preflight() so the fix-broken stage can inject the
# packages owning missing files into the exact simulated command: fingerprint
# the package state, simulate, fail closed, apply the exact simulated command
# once and derive changed/unchanged from the simulation plus the
# world/installed/missing-file fingerprints.
adaptive_alpine_apk_apply()
{
    local label="$1" tool_key="$2" fingerprint_before="" fingerprint_after=""
    local no_changes=false apply_output="" state=""
    shift 2
    local -a apk_command=("$@")

    package_feedback_reset
    fingerprint_before="$(alpine_apk_fingerprint)"
    log "SIMULATE: apk ${apk_command[*]} --simulate (no packages will be changed)" | tee -a "$SESSION_LOG"
    alpine_apk_transaction_try "Alpine apk ${apk_command[*]} simulation" "${apk_command[@]}" --simulate
    (( APK_SIM_RC == 0 )) || fail "apk ${apk_command[*]} simulation failed; no packages were changed."
    alpine_apk_simulation_is_safe "$APK_SIM_OUTPUT" "$APK_SIM_RC" \
        || fail "The simulated apk ${apk_command[*]} transaction failed Boot Bitch safety checks."
    fingerprint_after="$(alpine_apk_fingerprint)"
    [[ "$fingerprint_before" == "$fingerprint_after" ]] \
        || fail "The apk ${apk_command[*]} simulation modified the package database; refusing to continue."
    if alpine_apk_transaction_reported_no_changes "$APK_SIM_OUTPUT"; then
        no_changes=true
    fi

    run_chroot_try "$label" apk "${apk_command[@]}"
    (( CHROOT_TRY_RC == 0 )) || fail "$label failed with exit code $CHROOT_TRY_RC."
    log "PASS: $label" | tee -a "$SESSION_LOG"
    # Capture the apply transcript before the post-apply fingerprint, which
    # itself runs read-only chroot probes through CHROOT_TRY_OUTPUT.
    apply_output="$CHROOT_TRY_OUTPUT"
    fingerprint_after="$(alpine_apk_fingerprint)"

    # Masked/held warnings and /etc/apk/world version pins are non-fatal
    # feedback from the simulation and the apply transaction.
    apk_package_feedback_report "$APK_SIM_OUTPUT" "$apply_output"
    package_feedback_publish

    if [[ "$no_changes" == true && "$fingerprint_before" == "$fingerprint_after" ]]; then
        state="unchanged|apk simulated no package changes and the package state is byte-identical"
    else
        state="changed"
    fi
    if [[ -n "$PACKAGE_FEEDBACK_SUMMARY" ]]; then
        if [[ "$state" == unchanged* ]]; then
            state+="; $PACKAGE_FEEDBACK_SUMMARY"
        else
            state+="|$PACKAGE_FEEDBACK_SUMMARY"
        fi
    fi
    repair_change_status "$tool_key" "$state"
}

adaptive_alpine_apk_stage()
{
    alpine_apk_preflight
    adaptive_alpine_apk_apply "$@"
}

# fix-broken repairs dependency/world breakage with `apk fix --depends` exactly
# as before.  In addition, missing package files are detected read-only with
# `apk audit --system` and mapped to their owning packages, which are passed to
# the same command: `apk fix --depends <pkgs>` reinstalls those packages (and
# their dependency closure) while still fixing the world in the one simulated
# transaction.  An empty list keeps the previous behavior unchanged.
adaptive_alpine_apk_fix_broken()
{
    local detected=""
    local -a missing_packages=()

    alpine_apk_preflight
    if ! detected="$(alpine_apk_missing_file_packages)"; then
        fail "Alpine missing-file detection failed; refusing the package repair."
    fi
    if [[ -n "$detected" ]]; then
        mapfile -t missing_packages <<<"$detected"
    fi
    if ((${#missing_packages[@]} == 0)); then
        log "Alpine missing-file detection: no missing package files; running the dependency-only apk fix transaction." | tee -a "$SESSION_LOG"
        adaptive_alpine_apk_apply "Repair Alpine package dependencies (apk fix --depends)" fixbroken fix --depends
        return 0
    fi
    log "Alpine missing-file repair: reinstalling ${#missing_packages[@]} package(s) with missing files: ${missing_packages[*]}" | tee -a "$SESSION_LOG"
    adaptive_alpine_apk_apply "Repair Alpine package dependencies and missing files (apk fix --depends ${missing_packages[*]})" fixbroken fix --depends "${missing_packages[@]}"
}

adaptive_alpine_apk_upgrade()
{
    adaptive_alpine_apk_stage "Upgrade installed Alpine packages (apk upgrade)" upgrade upgrade
}

# ---------------------------------------------------------------------------
# Fedora/rpm-dnf5 repair backend (simulation-first guarded package transactions)
# ---------------------------------------------------------------------------
# Safety caps mirror the Alpine/pacman/APT backends: one simulated transaction
# may touch at most 1000 packages, and the read-only missing-file detection is
# bounded so a corrupt rpmdb cannot produce an unbounded repair.
RPM_MAX_TRANSACTION_CHANGES=1000
RPM_MAX_MISSING_FILES=1000
RPM_MAX_MISSING_PACKAGES=1000

# Lock files that can block a dnf5/rpm transaction.  rpm uses POSIX record
# locks (fcntl), which flock(1) cannot see, so the probe below uses lslocks
# with a fuser fallback instead of flock -n.
RPM_LOCK_PATHS=(
    /usr/lib/sysimage/rpm/.rpm.lock
    /var/lib/rpm/.rpm.lock
    /usr/lib/sysimage/libdnf5/system-repo.lock
    /run/dnf/rpmtransaction.lock
)

RPM_DNF_TOOL="dnf5"

rpm_lock_probe_available()
{
    command -v lslocks >/dev/null 2>&1 || command -v fuser >/dev/null 2>&1
}

# Read-only evidence probe: report whether a WRITE lock is currently held on
# one of the rpm/dnf5 lock files.  A READ lock (dnf5 queries and --assumeno
# simulations) does not block the guarded transaction and is not treated as
# held.  Diagnostics and capability evidence must never be refused because of
# a held lock; this only records the state.
rpm_lock_held()
{
    local path mode path_field line lock
    local -a locks=()
    for path in "${RPM_LOCK_PATHS[@]}"; do
        [[ -e "$TARGET_ROOT$path" ]] && locks+=("$TARGET_ROOT$path")
    done
    ((${#locks[@]} > 0)) || return 1

    if command -v lslocks >/dev/null 2>&1; then
        # lslocks -n -o MODE,PATH prints one lock per line; the last
        # whitespace-separated field is the locked file.
        while IFS= read -r line; do
            [[ -n "$line" ]] || continue
            mode="${line% *}"
            path_field="${line##* }"
            [[ "$mode" == *WRITE* ]] || continue
            for lock in "${locks[@]}"; do
                [[ "$path_field" == "$lock" ]] && return 0
            done
        done < <(lslocks -n -o MODE,PATH 2>/dev/null)
        return 1
    fi
    if command -v fuser >/dev/null 2>&1; then
        for lock in "${locks[@]}"; do
            fuser -s "$lock" 2>/dev/null && return 0
        done
    fi
    return 1
}

# Byte fingerprint of the rpm state a guarded dnf5 transaction may touch: the
# sqlite rpmdb, the transaction history and the imported signing keys.  The
# sqlite -wal/-shm sidecars are deliberately excluded: a cold target has none
# and the first rpmdb access creates or truncates them without changing any
# package data, which would otherwise fail the first simulation closed as a
# false "simulation modified the rpm database".  A simulation must leave this
# byte-identical; a no-op apply may report unchanged only when the fingerprint
# did not move.
rpm_database_fingerprint()
{
    local root="" candidate key
    command -v sha256sum >/dev/null 2>&1 || { printf 'no-sha256sum\n'; return 0; }
    for candidate in /usr/lib/sysimage/rpm /var/lib/rpm; do
        if [[ -e "$TARGET_ROOT$candidate/rpmdb.sqlite" ]]; then
            root="$candidate"
            break
        fi
    done
    if [[ -z "$root" ]]; then
        printf 'rpmdb missing\n'
        return 0
    fi
    printf 'rpmdb '
    repair_file_fingerprint "$TARGET_ROOT$root/rpmdb.sqlite"
    printf 'history '
    repair_file_fingerprint "$TARGET_ROOT$root/history.sqlite"
    printf 'keyring'
    if [[ -d "$TARGET_ROOT$root/pubkeys" ]]; then
        while IFS= read -r key; do
            [[ -n "$key" ]] || continue
            printf ' %s %s' "${key##*/}" "$(repair_file_fingerprint "$key")"
        done < <(find "$TARGET_ROOT$root/pubkeys" -maxdepth 1 -type f 2>/dev/null | LC_ALL=C sort)
    fi
    printf '\n'
}

# Mandatory read-only rpm/dnf5 preflight: executables, sqlite rpmdb, enabled
# repositories, fcntl lock state, /boot read-write + free space, dnf5/rpm
# versions and SELinux state (evidence only).  Fails closed before any
# simulation or apply.
rpm_preflight()
{
    local free_kb largest=0 size image required_kb version_line proc
    target_rpm_detected \
        || fail "Fedora rpm/dnf backend is not selected for this target: no rpm executable or database was detected."
    if (( RUNNING_HOST_MODE == 1 )); then
        for proc in rpm rpmkeys dnf dnf5 dnf4 dnf-3 dnf-automatic; do
            if pgrep -x "$proc" >/dev/null 2>&1; then
                fail "Package manager process '$proc' is already running; refusing a concurrent host package repair."
            fi
        done
    fi
    target_has_executable /usr/bin/rpm /bin/rpm /usr/sbin/rpm \
        || fail "rpm is not installed in the target system."
    if ! RPM_DNF_TOOL="$(rpm_dnf_tool)"; then
        if rpm_dnf4_present; then
            fail "dnf4 is not supported by the guarded rpm backend."
        fi
        fail "dnf5 is not installed in the target system."
    fi
    rpm_database_present \
        || fail "The target RPM database is missing; a sqlite rpmdb is required."
    rpm_repositories_present \
        || fail "The target has no enabled dnf repositories."
    rpm_lock_probe_available \
        || fail "The RPM database lock state cannot be probed (lslocks or fuser is required in the recovery host); refusing a concurrent transaction."
    rpm_lock_held \
        && fail "The RPM database is locked; refusing a concurrent transaction."
    [[ -d "$TARGET_ROOT/boot" ]] \
        || fail "The target has no /boot directory; refusing a package transaction that can rebuild kernels."
    target_path_is_mounted_rw "$TARGET_ROOT/boot" \
        || fail "The target /boot is not mounted read-write; refusing a package transaction that can rebuild kernels."
    while IFS= read -r image; do
        [[ -n "$image" ]] || continue
        size="$(stat -c '%s' "$TARGET_ROOT/boot/$image" 2>/dev/null || printf '0')"
        [[ "$size" =~ ^[0-9]+$ ]] || size=0
        if (( size > largest )); then
            largest="$size"
        fi
    done < <(find "$TARGET_ROOT/boot" -maxdepth 1 -type f -name 'initramfs-*.img' -printf '%f\n' 2>/dev/null | LC_ALL=C sort)
    # Two copies of the largest initramfs plus a 128 MiB working margin: a
    # kernel upgrade writes a new kernel + initramfs pair while the previous
    # pair is retained (installonly_limit keeps three kernels).
    required_kb=$(( (largest * 2 + 1023) / 1024 + 131072 ))
    free_kb="$(df -Pk "$TARGET_ROOT/boot" 2>/dev/null | awk 'NR==2 {print $4}' | head -n1 || true)"
    if [[ "$free_kb" =~ ^[0-9]+$ ]] && (( free_kb < required_kb )); then
        fail "The target /boot has ${free_kb} KiB free; at least ${required_kb} KiB is required for a safe package transaction."
    fi
    run_chroot_try "Record dnf5 version" "$RPM_DNF_TOOL" --version
    (( CHROOT_TRY_RC == 0 )) \
        || fail "$RPM_DNF_TOOL --version failed with exit code $CHROOT_TRY_RC; refusing the package repair."
    version_line="$(sed -n '1p' <<<"$CHROOT_TRY_OUTPUT")"
    [[ "$version_line" =~ dnf5[[:space:]]+version[[:space:]]+5\. ]] \
        || fail "The target dnf5 version '${version_line:-unknown}' is not a supported dnf5 5.x release."
    log "dnf5 preflight version: $version_line" | tee -a "$SESSION_LOG"
    run_chroot_try "Record rpm version" rpm --version
    (( CHROOT_TRY_RC == 0 )) \
        || fail "rpm --version failed with exit code $CHROOT_TRY_RC; refusing the package repair."
    log "rpm preflight version: $(sed -n '1p' <<<"$CHROOT_TRY_OUTPUT")" | tee -a "$SESSION_LOG"
    # SELinux state is recorded as evidence only; Boot Bitch never changes it.
    if target_has_executable /usr/sbin/sestatus /usr/bin/sestatus /sbin/sestatus; then
        run_chroot_try "Record SELinux status" sestatus
        log "SELinux status: $(sed -n '1p' <<<"$CHROOT_TRY_OUTPUT")" | tee -a "$SESSION_LOG"
    elif target_has_executable /usr/sbin/getenforce /usr/bin/getenforce /sbin/getenforce; then
        run_chroot_try "Record SELinux enforcement mode" getenforce
        log "SELinux enforcement: $(sed -n '1p' <<<"$CHROOT_TRY_OUTPUT")" | tee -a "$SESSION_LOG"
    else
        log "SELinux status: sestatus/getenforce are not installed in the target; recorded as unavailable." | tee -a "$SESSION_LOG"
    fi
}

RPM_SIM_OUTPUT=""
RPM_SIM_RC=0

rpm_transaction_try()
{
    local label="$1"; shift
    local output rc
    log "TRY: $label" | tee -a "$SESSION_LOG"
    set +e
    output="$(
        run_selected_chroot /usr/bin/env \
            HOME=/root \
            LC_ALL=C \
            DNF5_FORCE_INTERACTIVE=0 \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            "${RPM_DNF_TOOL:-dnf5}" "$@" 2>&1
    )"
    rc=$?
    set -e
    RPM_SIM_OUTPUT="$output"
    RPM_SIM_RC=$rc
    printf '%s\n' "$output" | tee -a "$SESSION_LOG"
    log "TRY exit code: $rc ($label)" | tee -a "$SESSION_LOG"
    return 0
}

# Fail-closed output policy for a dnf5 simulation/apply transcript.  The
# --assumeno simulation exits 1 by design after "Operation aborted by the
# user."; every other nonzero status and every removal, downgrade, untrusted
# signature, held lock, unresolved dependency, incomplete metadata, unknown
# error or >1000-package transaction is refused.  Skipped packages stay
# non-fatal feedback: the resolver may leave conflicted or dependency-broken
# packages out of an otherwise safe transaction, and the backend surfaces them
# as package-manager evidence instead of failing the stage.
# `replacing` sub-lines are classified by name: a same-name replacement is a
# normal version upgrade/reinstall (even for critical system packages), while
# a different-name replacement is an obsoletes removal and may only refuse a
# critical package.  fix-broken additionally requires every replacement to
# name one of the packages being reinstalled.
rpm_simulation_is_safe()
{
    local output="$1" rc="${2:-0}"
    shift 2 || true
    local -a reinstall_names=("$@")
    local name allowed count line current_name replaced_name
    local removal_count=0 downgrade_count=0 replacing_count=0 total=0

    if (( rc != 0 )); then
        if (( rc != 1 )) || ! grep -Fq 'Operation aborted by the user.' <<<"$output"; then
            log "REFUSED: dnf5 simulation failed with exit code $rc." | tee -a "$SESSION_LOG"
            return 1
        fi
    fi
    if grep -Eiq 'Failed to resolve the transaction|conflicting requests|nothing provides|unresolvable|^[[:space:]]*Problem:' <<<"$output"; then
        log "REFUSED: dnf5 simulation reported an unresolved or conflicting transaction." | tee -a "$SESSION_LOG"
        return 1
    fi
    removal_count="$(sed -nE 's/^[[:space:]]*Removing:[[:space:]]+([0-9]+)[[:space:]]+packages?$/\1/p' <<<"$output" | head -n1)"
    downgrade_count="$(sed -nE 's/^[[:space:]]*Downgrading:[[:space:]]+([0-9]+)[[:space:]]+packages?$/\1/p' <<<"$output" | head -n1)"
    [[ "$removal_count" =~ ^[0-9]+$ ]] || removal_count=0
    [[ "$downgrade_count" =~ ^[0-9]+$ ]] || downgrade_count=0
    if grep -Eq '^[[:space:]]*Removing:[[:space:]]*$|^[[:space:]]*Removing (dependent packages|unused dependencies):' <<<"$output" \
        || (( removal_count > 0 )); then
        log "REFUSED: dnf5 simulation proposes package removals." | tee -a "$SESSION_LOG"
        return 1
    fi
    if grep -Eq '^[[:space:]]*Downgrading:[[:space:]]*$' <<<"$output" || (( downgrade_count > 0 )); then
        log "REFUSED: dnf5 simulation proposes a package downgrade." | tee -a "$SESSION_LOG"
        return 1
    fi
    if grep -Eiq 'Public key is not installed\.|signature is valid, but the key is not trusted\.|Public key import failed\.|Failed to import OpenPGP keys|NOKEY|not signed' <<<"$output"; then
        log "REFUSED: dnf5 simulation reported an untrusted or missing package signature key." | tee -a "$SESSION_LOG"
        return 1
    fi
    if grep -Eiq 'Waiting for a lock on the system repository|Failed to obtain lock|Failed to obtain rpm transaction lock|installroot locked by transaction' <<<"$output"; then
        log "REFUSED: dnf5 simulation reported a locked package database." | tee -a "$SESSION_LOG"
        return 1
    fi
    if grep -Eiq 'Failed to download metadata|Cannot download|repomd\.xml.*failed|Temporary failure' <<<"$output"; then
        log "REFUSED: dnf5 simulation reported incomplete repository metadata." | tee -a "$SESSION_LOG"
        return 1
    fi
    if grep -Eq '^[[:space:]]*(Error|error|Transaction failed):' <<<"$output"; then
        log "REFUSED: dnf5 simulation reported an error Boot Bitch does not recognize as safe." | tee -a "$SESSION_LOG"
        return 1
    fi

    # dnf5 5.4.x prints each package row as ` <name>` (one leading space)
    # followed by one indented `   replacing <name>` (three leading spaces)
    # sub-line per replaced package.  A version upgrade (or reinstall) of a
    # package prints a same-name `replacing` line and is a normal update even
    # for critical packages; an obsoletes replacement prints a different name
    # and removes that package, so only that form can refuse a critical
    # package.  The two-space minimum keeps a package literally named
    # `replacing` from being mistaken for a sub-line.
    while IFS= read -r line; do
        [[ -n "$line" ]] || continue
        if [[ "$line" =~ ^[[:space:]][[:space:]]+replacing[[:space:]]+([^[:space:]]+) ]]; then
            replaced_name="${BASH_REMATCH[1]}"
            if ((${#reinstall_names[@]} > 0)); then
                allowed=false
                for name in "${reinstall_names[@]}"; do
                    if [[ "$replaced_name" == "$name" || "$replaced_name" == "$name"-* ]]; then
                        allowed=true
                        break
                    fi
                done
                if [[ "$allowed" != true ]]; then
                    log "REFUSED: dnf5 reinstall would replace '$replaced_name', which is not one of the reinstalled packages." | tee -a "$SESSION_LOG"
                    return 1
                fi
            elif [[ "$replaced_name" != "$current_name" ]]; then
                case "$replaced_name" in
                    kernel*|grub2*|shim*|systemd*|dracut*|glibc*|rpm|rpm-*|dnf5*|libdnf5*|selinux-policy*|btrfs-progs*)
                        log "REFUSED: dnf5 upgrade would obsolete critical package '$replaced_name'." | tee -a "$SESSION_LOG"
                        return 1
                        ;;
                esac
            fi
        elif [[ "$line" =~ ^[[:space:]][^[:space:]] ]]; then
            # Package row: a single leading space, then the name.  Summary
            # lines such as ` Installing: ...` match too, which is harmless
            # because no replacing sub-line follows them.
            current_name="${line#"${line%%[![:space:]]*}"}"
            current_name="${current_name%%[[:space:]]*}"
        else
            current_name=""
        fi
    done <<<"$output"

    # `Replacing:` counts the old packages superseded by an Installing or
    # Upgrading row; every upgrade prints one same-name replacing line, so
    # adding it to the transaction total double-counts the same change and
    # would refuse a normal ~800-package Fedora update set.  It is bounded
    # separately so an obsoletes avalanche cannot bypass the cap either.
    while IFS= read -r count; do
        [[ "$count" =~ ^[0-9]+$ ]] || continue
        total=$(( total + count ))
    done < <(sed -nE 's/^[[:space:]]*(Installing|Reinstalling|Upgrading|Removing|Downgrading):[[:space:]]+([0-9]+)[[:space:]]+packages?$/\2/p' <<<"$output")
    while IFS= read -r count; do
        [[ "$count" =~ ^[0-9]+$ ]] || continue
        replacing_count=$(( replacing_count + count ))
    done < <(sed -nE 's/^[[:space:]]*Replacing:[[:space:]]+([0-9]+)[[:space:]]+packages?$/\1/p' <<<"$output")
    if (( total > RPM_MAX_TRANSACTION_CHANGES )); then
        log "REFUSED: dnf5 simulation proposes $total package changes (safety limit: $RPM_MAX_TRANSACTION_CHANGES)." | tee -a "$SESSION_LOG"
        return 1
    fi
    if (( replacing_count > RPM_MAX_TRANSACTION_CHANGES )); then
        log "REFUSED: dnf5 simulation would replace $replacing_count packages (safety limit: $RPM_MAX_TRANSACTION_CHANGES)." | tee -a "$SESSION_LOG"
        return 1
    fi
    return 0
}

# A dnf5 transaction proves "no work" only when it printed `Nothing to do.`
# and no package action section, replacement or summary count was emitted.
rpm_transaction_reported_no_changes()
{
    local output="$1"
    grep -Eq '^[[:space:]]*(Installing|Upgrading|Reinstalling|Downgrading|Removing|Replacing):' <<<"$output" && return 1
    grep -Eq '^[[:space:]]+replacing[[:space:]]' <<<"$output" && return 1
    grep -Fq 'Nothing to do.' <<<"$output"
}

# Read-only missing-file detection: `rpm -Va --nofiledigest` prints
# `missing   <attr> <path>` for every file recorded in the rpmdb that no
# longer exists.  rpm exits 1 when it finds problems, so a 0/1 status is
# expected and anything else fails closed.  Runtime state directories under
# /run, /var/run and /var/lock are excluded: the repair chroot mounts a fresh
# tmpfs on /run (dracut hostonly needs it), so the target's runtime dirs are
# always absent there and reinstalling their owning packages could never make
# the detection idempotent.
rpm_verify_missing_paths()
{
    local paths
    run_chroot_try "RPM package file verification (rpm -Va --nofiledigest, read-only)" \
        /usr/bin/env LC_ALL=C rpm -Va --nofiledigest >/dev/null
    if (( CHROOT_TRY_RC != 0 && CHROOT_TRY_RC != 1 )); then
        fail "rpm -Va --nofiledigest failed with exit code $CHROOT_TRY_RC; refusing a package repair without reliable missing-file detection."
    fi
    if grep -Eq '^[[:space:]]*(error|Error):' <<<"$CHROOT_TRY_OUTPUT"; then
        fail "rpm -Va --nofiledigest reported an error; refusing a package repair without reliable missing-file detection."
    fi
    paths="$(printf '%s\n' "$CHROOT_TRY_OUTPUT" \
        | sed -nE 's/^missing {3}. (.*)$/\1/p' \
        | grep -Ev '^(/run|/var/run|/var/lock)(/|$)' || true)"
    printf '%s\n' "$paths" | LC_ALL=C sort -u
}

# Map missing files to their owning installed packages with one read-only
# `rpm -qf` query inside the target chroot.  Prints the deduplicated package
# names and fails closed when the mapping is incomplete, an unowned file is
# reported or a safety cap is exceeded.  rpmdb-owned symlinks are excluded:
# rpm's reinstall erase phase removes %config(noreplace) symlinks again after
# the install phase, so reinstalling their owner can never converge and would
# make the stage non-idempotent; regular files (including config files) stay.
rpm_missing_file_packages()
{
    local paths_output record name mode
    local -a missing_paths=() owner_records=() packages=()

    paths_output="$(rpm_verify_missing_paths)" \
        || fail "RPM missing-file detection failed; refusing the package repair."
    if [[ -n "$paths_output" ]]; then
        mapfile -t missing_paths <<<"$paths_output"
    fi
    ((${#missing_paths[@]} > 0)) || return 0
    if ((${#missing_paths[@]} > RPM_MAX_MISSING_FILES)); then
        fail "rpm -Va --nofiledigest reports ${#missing_paths[@]} missing files (safety limit: $RPM_MAX_MISSING_FILES); refusing an unbounded repair."
    fi
    run_chroot_try "RPM file ownership mapping (rpm -qf, ${#missing_paths[@]} missing files)" \
        /usr/bin/env LC_ALL=C rpm -qf --qf '%{NAME}\t%{FILEMODES}\n' "${missing_paths[@]}" >/dev/null
    if (( CHROOT_TRY_RC != 0 )) \
        || grep -Eq 'is not owned by any package|no package owns' <<<"$CHROOT_TRY_OUTPUT"; then
        fail "rpm -qf could not map every missing file to an installed package; refusing the repair."
    fi
    mapfile -t owner_records < <(grep -E '^[A-Za-z0-9][A-Za-z0-9._+-]*[[:space:]][0-9]+$' <<<"$CHROOT_TRY_OUTPUT")
    ((${#owner_records[@]} == ${#missing_paths[@]})) \
        || fail "rpm -qf returned ${#owner_records[@]} owner line(s) for ${#missing_paths[@]} missing file(s); refusing the repair."
    for record in "${owner_records[@]}"; do
        name="${record%%[[:space:]]*}"
        mode="${record##*[[:space:]]}"
        # S_IFLNK (0120000) entries are skipped; every other mode is kept.
        if (( (10#$mode & 0120000) == 0120000 )); then
            continue
        fi
        packages+=("$name")
    done
    mapfile -t packages < <(printf '%s\n' "${packages[@]}" | LC_ALL=C sort -u)
    if ((${#packages[@]} > RPM_MAX_MISSING_PACKAGES)); then
        fail "The missing files map to ${#packages[@]} packages (safety limit: $RPM_MAX_MISSING_PACKAGES); refusing an unbounded repair."
    fi
    printf '%s\n' "${packages[@]}"
}

# Shared simulate/apply engine for the rpm package stages.  The caller has
# already run rpm_preflight() so the fix-broken stage can inject the packages
# owning missing files into the exact simulated command: fingerprint the rpm
# state, simulate write-free with --assumeno, fail closed, apply the exact
# simulated command once with -y and derive changed/unchanged from the
# simulation plus the rpmdb/keyring/history fingerprints.
adaptive_rpm_apply()
{
    local label="$1" tool_key="$2" fingerprint_before="" fingerprint_after=""
    local no_changes=false apply_output="" state=""
    shift 2
    local -a rpm_command=("$@")
    local -a reinstall_names=()

    package_feedback_reset
    if [[ "${rpm_command[0]:-}" == reinstall ]]; then
        reinstall_names=("${rpm_command[@]:1}")
    fi

    fingerprint_before="$(rpm_database_fingerprint)"
    log "SIMULATE: dnf5 ${rpm_command[*]} --assumeno (no packages will be changed)" | tee -a "$SESSION_LOG"
    rpm_transaction_try "dnf5 ${rpm_command[*]} simulation" "${rpm_command[@]}" --assumeno
    rpm_simulation_is_safe "$RPM_SIM_OUTPUT" "$RPM_SIM_RC" "${reinstall_names[@]}" \
        || fail "The simulated dnf5 ${rpm_command[*]} transaction failed Boot Bitch safety checks."
    fingerprint_after="$(rpm_database_fingerprint)"
    [[ "$fingerprint_before" == "$fingerprint_after" ]] \
        || fail "The dnf5 ${rpm_command[*]} simulation modified the rpm database; refusing to continue."
    if rpm_transaction_reported_no_changes "$RPM_SIM_OUTPUT"; then
        no_changes=true
    fi

    run_chroot_try "$label" "${RPM_DNF_TOOL:-dnf5}" "${rpm_command[@]}" -y
    (( CHROOT_TRY_RC == 0 )) || fail "$label failed with exit code $CHROOT_TRY_RC."
    apply_output="$CHROOT_TRY_OUTPUT"
    # Post-hoc drift guard: the applied transaction table must satisfy the same
    # fail-closed policy as the simulation it was derived from.
    rpm_simulation_is_safe "$apply_output" "$CHROOT_TRY_RC" "${reinstall_names[@]}" \
        || fail "The applied dnf5 ${rpm_command[*]} transaction failed Boot Bitch safety checks after it ran."
    log "PASS: $label" | tee -a "$SESSION_LOG"
    fingerprint_after="$(rpm_database_fingerprint)"

    # Skipped packages from the simulation and the apply transcript are
    # non-fatal feedback; the summary is appended to the change status.
    rpm_package_feedback_report "$RPM_SIM_OUTPUT" "$apply_output"
    package_feedback_publish

    if [[ "$no_changes" == true && "$fingerprint_before" == "$fingerprint_after" ]]; then
        state="unchanged|dnf5 simulated no package changes and the rpm database is byte-identical"
    else
        state="changed"
    fi
    if [[ -n "$PACKAGE_FEEDBACK_SUMMARY" ]]; then
        if [[ "$state" == unchanged* ]]; then
            state+="; $PACKAGE_FEEDBACK_SUMMARY"
        else
            state+="|$PACKAGE_FEEDBACK_SUMMARY"
        fi
    fi
    repair_change_status "$tool_key" "$state"
}

adaptive_rpm_stage()
{
    rpm_preflight
    adaptive_rpm_apply "$@"
}

# fix-broken restores missing package files with one guarded `dnf5 reinstall`
# transaction built from the read-only `rpm -Va` -> `rpm -qf` mapping.  When no
# missing files are detected the stage records a read-only `dnf5 check
# --dependencies` transcript as evidence and changes nothing.
adaptive_rpm_fix_broken()
{
    local detected=""
    local -a missing_packages=()

    rpm_preflight
    if ! detected="$(rpm_missing_file_packages)"; then
        fail "RPM missing-file detection failed; refusing the package repair."
    fi
    if [[ -n "$detected" ]]; then
        mapfile -t missing_packages <<<"$detected"
    fi
    if ((${#missing_packages[@]} == 0)); then
        log "RPM missing-file detection: no missing package files; recording read-only dnf5 dependency-check evidence." | tee -a "$SESSION_LOG"
        run_chroot_try "dnf5 dependency check (read-only evidence)" "${RPM_DNF_TOOL:-dnf5}" check --dependencies
        # dnf5 check exits 1 when problems are found; that evidence is reported
        # but never auto-fixed by this stage.
        if (( CHROOT_TRY_RC != 0 && CHROOT_TRY_RC != 1 )); then
            fail "dnf5 check --dependencies failed with exit code $CHROOT_TRY_RC."
        fi
        repair_change_status fixbroken "unchanged|no missing package files were detected; dnf5 check is reported as evidence"
        return 0
    fi
    log "RPM missing-file repair: reinstalling ${#missing_packages[@]} package(s) with missing files: ${missing_packages[*]}" | tee -a "$SESSION_LOG"
    adaptive_rpm_apply "Repair RPM package files (dnf5 reinstall ${missing_packages[*]})" fixbroken reinstall "${missing_packages[@]}"
}

# Byte fingerprint of the dnf metadata cache a makecache refresh may rewrite:
# the downloaded repository metadata (repodata) and the compiled solv files,
# excluding the downloaded package payloads.  dnf5 prints "Metadata cache
# created." even when the cache content is unchanged, so the byte fingerprint
# is the only accurate change evidence.
rpm_metadata_cache_fingerprint()
{
    local root file found=0
    command -v sha256sum >/dev/null 2>&1 || { printf 'no-sha256sum\n'; return 0; }
    for root in /var/cache/libdnf5 /var/cache/dnf; do
        [[ -d "$TARGET_ROOT$root" ]] || continue
        found=1
        while IFS= read -r file; do
            [[ -n "$file" ]] || continue
            printf '%s %s\n' "${file#"$TARGET_ROOT"}" "$(repair_file_fingerprint "$file")"
        done < <(find "$TARGET_ROOT$root" -type f \( -path '*/repodata/*' -o -path '*/solv/*' \) 2>/dev/null | LC_ALL=C sort)
    done
    (( found == 1 )) || printf 'cache missing\n'
}

# Refresh dnf5 metadata (the Fedora equivalent of apt-get update).  A refresh
# that leaves the repository metadata cache byte-identical is a proven no-op;
# any cache write reports changed with explicit evidence.
adaptive_rpm_metadata_refresh()
{
    local cache_before cache_after
    rpm_preflight
    cache_before="$(rpm_metadata_cache_fingerprint)"
    run_chroot_try "Refresh dnf5 package metadata (dnf5 makecache)" "${RPM_DNF_TOOL:-dnf5}" makecache
    (( CHROOT_TRY_RC == 0 )) || fail "dnf5 makecache failed with exit code $CHROOT_TRY_RC."
    cache_after="$(rpm_metadata_cache_fingerprint)"
    if [[ "$cache_before" != "cache missing" && "$cache_before" != "no-sha256sum" \
          && "$cache_before" == "$cache_after" ]]; then
        log "PASS: dnf5 metadata cache refreshed (repository metadata cache is byte-identical)." | tee -a "$SESSION_LOG"
        repair_change_status aptupdate "unchanged|dnf5 repository metadata cache is byte-identical"
        return 0
    fi
    log "PASS: dnf5 metadata cache refreshed." | tee -a "$SESSION_LOG"
    log "dnf5 repository metadata cache rewritten (cache fingerprint changed); reporting the metadata refresh as changed." | tee -a "$SESSION_LOG"
    repair_change_status aptupdate changed
}

adaptive_rpm_upgrade()
{
    adaptive_rpm_stage "Upgrade installed RPM packages (dnf5 upgrade)" upgrade upgrade
}

installed_kernel_versions()
{
    find "$TARGET_ROOT/boot" -maxdepth 1 -type f -name 'vmlinuz-*' -printf '%f\n' 2>/dev/null \
        | sed 's/^vmlinuz-//' \
        | sort -V -u
}

apt_package_available()
{
    local package="$1"
    run_selected_chroot /usr/bin/env \
        HOME=/root PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
        apt-cache show "$package" >/dev/null 2>&1
}

# Simulate and safety-check a known package correction, then apply it.  label
# is used in log lines; reinstall=yes adds --reinstall; remaining arguments are
# the packages.  Interrupted dpkg configuration and unmet dependencies are
# corrected once, re-simulated and only then applied.
safe_apt_install_packages()
{
    local label="$1" reinstall="$2" output rc fix_output fix_rc
    shift 2
    local -a packages=("$@") sim_args=(-s -o Debug::NoLocking=1) real_args=(-y)

    ((${#packages[@]} > 0)) || return 0
    [[ -x "$TARGET_ROOT/usr/bin/apt-get" ]] || fail "apt-get is unavailable; cannot perform known package correction: $label"
    if [[ "$reinstall" == "yes" ]]; then
        sim_args+=(--reinstall)
        real_args+=(--reinstall)
    fi
    sim_args+=(install "${packages[@]}")
    real_args+=(install "${packages[@]}")

    simulate_install_once()
    {
        set +e
        output="$(
            run_selected_chroot /usr/bin/env \
                HOME=/root PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
                DEBIAN_FRONTEND=noninteractive APT_LISTCHANGES_FRONTEND=none \
                apt-get "${sim_args[@]}" 2>&1
        )"
        rc=$?
        set -e
        printf '%s\n' "$output" | tee -a "$SESSION_LOG"
        log "Correction simulation exit code: $rc ($label)" | tee -a "$SESSION_LOG"
    }

    log "SIMULATE CORRECTION: $label" | tee -a "$SESSION_LOG"
    simulate_install_once

    if ((rc != 0)) && grep -Eiq 'dpkg was interrupted|dpkg --configure -a' <<<"$output"; then
        log "KNOWN ISSUE: APT correction is blocked by interrupted dpkg configuration; completing dpkg once and re-simulating." | tee -a "$SESSION_LOG"
        run_chroot "Complete interrupted package configuration before correction" dpkg --configure -a
        simulate_install_once
    fi

    if ((rc != 0)) && grep -Eiq 'unmet dependencies|you might want to run.*apt.*--fix-broken|apt --fix-broken install' <<<"$output"; then
        log "KNOWN ISSUE: APT correction is blocked by broken dependencies; simulating --fix-broken before retry." | tee -a "$SESSION_LOG"
        set +e
        fix_output="$(
            run_selected_chroot /usr/bin/env \
                HOME=/root PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
                DEBIAN_FRONTEND=noninteractive APT_LISTCHANGES_FRONTEND=none \
                apt-get -s -o Debug::NoLocking=1 -f install 2>&1
        )"
        fix_rc=$?
        set -e
        printf '%s\n' "$fix_output" | tee -a "$SESSION_LOG"
        if ((fix_rc == 0)) && apt_simulation_is_safe "$fix_output"; then
            run_chroot "Repair broken dependencies before known package correction" apt-get -y -f install
            simulate_install_once
        fi
    fi

    ((rc == 0)) || fail "Known package correction simulation failed: $label"
    apt_simulation_is_safe "$output" || fail "Known package correction was rejected by Boot Bitch safety policy: $label"

    log "AUTO-CORRECT: $label" | tee -a "$SESSION_LOG"
    run_chroot "$label" apt-get "${real_args[@]}"
}

repair_stale_mapper_mount_alias()
{
    local source alias_name resolved
    source="$(findmnt -rn -o SOURCE --target "$TARGET_ROOT" 2>/dev/null | head -n1 || true)"
    source="${source%%[*}"
    [[ "$source" == /dev/mapper/* ]] || return 1
    [[ ! -e "$source" ]] || return 1
    alias_name="$(basename -- "$source")"
    [[ "$alias_name" =~ ^[[:alnum:]_.+-]+$ ]] || return 1

    resolved="$ROOT_CANONICAL"
    [[ -b "$resolved" ]] || return 1
    same_single_top_disk "$TARGET_DISK" "$resolved" || return 1

    ln -s "$resolved" "$source"
    TEMP_MAPPER_ALIASES+=("$source")
    log "AUTO-CORRECT: restored temporary stale mapper alias $source -> $resolved for this repair request." | tee -a "$SESSION_LOG"
    return 0
}

output_suggests_mapper_path_failure()
{
    local output="$1"
    grep -Eiq \
        'couldn.t resolve device /dev/mapper/|couldn.t determine root device|failed to get canonical path of .?/dev/mapper/|cannot find a device for /|grub-probe: error' \
        <<<"$output"
}

# Verify DKMS prerequisites for every installed kernel.  The detected
# initramfs/package backend selects the kernel inventory: mkinitfs systems pair
# flavor kernels, mkinitcpio/pacman systems use module directories, and the
# Debian layout additionally installs missing repository headers through the
# guarded APT correction path.
preflight_dkms()
{
    local kver header_pkg alpine_dkms_pair alpine_dkms_kver rpm_dkms_pair rpm_dkms_kver
    local -a missing_headers=() available_headers=() alpine_dkms_pairs=() rpm_dkms_pairs=()

    if [[ "$TARGET_INITRAMFS_BACKEND" == mkinitfs ]]; then
        [[ -x "$TARGET_ROOT/usr/bin/dkms" || -x "$TARGET_ROOT/usr/sbin/dkms" ]] \
            || fail "DKMS is not installed in the target system."
        log "SIMULATE/PREFLIGHT: mkinitfs DKMS rebuild (headers must already be installed; no package guessing is performed)" | tee -a "$SESSION_LOG"
        mapfile -t alpine_dkms_pairs < <(alpine_kernel_pairs)
        ((${#alpine_dkms_pairs[@]} > 0)) \
            || fail "No installed kernel module directories were found for DKMS."
        for alpine_dkms_pair in "${alpine_dkms_pairs[@]}"; do
            alpine_dkms_kver="${alpine_dkms_pair%% *}"
            [[ -n "$alpine_dkms_kver" ]] || continue
            run_selected_chroot /usr/bin/env PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
                test -e "/lib/modules/$alpine_dkms_kver/build" \
                || fail "DKMS preflight found no /lib/modules/$alpine_dkms_kver/build tree; install the matching headers and retry."
            log "DKMS preflight: headers/build tree present for $alpine_dkms_kver" | tee -a "$SESSION_LOG"
        done
        run_chroot_try "Inspect DKMS state" dkms status
        return 0
    fi

    if [[ "$TARGET_INITRAMFS_BACKEND" == dracut ]]; then
        # Fedora pairs dracut kernels through the rpm module inventory, never
        # through vmlinuz-* alone (which would include the -0-rescue kernel).
        # Headers must already be installed (kernel-devel); the guarded rpm
        # backend does not guess package names for this stage.
        [[ -x "$TARGET_ROOT/usr/bin/dkms" || -x "$TARGET_ROOT/usr/sbin/dkms" ]] \
            || fail "DKMS is not installed in the target system."
        log "SIMULATE/PREFLIGHT: dracut DKMS rebuild (headers must already be installed; no package guessing is performed)" | tee -a "$SESSION_LOG"
        mapfile -t rpm_dkms_pairs < <(rpm_kernel_pairs)
        ((${#rpm_dkms_pairs[@]} > 0)) \
            || fail "No installed kernel module directories were found for DKMS."
        for rpm_dkms_pair in "${rpm_dkms_pairs[@]}"; do
            rpm_dkms_kver="${rpm_dkms_pair%% *}"
            [[ -n "$rpm_dkms_kver" ]] || continue
            run_selected_chroot /usr/bin/env PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
                test -e "/lib/modules/$rpm_dkms_kver/build" \
                || fail "DKMS preflight found no /lib/modules/$rpm_dkms_kver/build tree; install the matching kernel-devel packages and retry."
            log "DKMS preflight: headers/build tree present for $rpm_dkms_kver" | tee -a "$SESSION_LOG"
        done
        run_chroot_try "Inspect DKMS state" dkms status
        return 0
    fi

    if [[ "$TARGET_INITRAMFS_BACKEND" == mkinitcpio ]] || target_pacman_detected; then
        [[ -x "$TARGET_ROOT/usr/bin/dkms" || -x "$TARGET_ROOT/usr/sbin/dkms" ]] \
            || fail "DKMS is not installed in the target system."
        log "SIMULATE/PREFLIGHT: mkinitcpio DKMS rebuild (headers must already be installed; no package guessing is performed)" | tee -a "$SESSION_LOG"
        while IFS= read -r kver; do
            [[ -n "$kver" ]] || continue
            run_selected_chroot /usr/bin/env PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
                test -e "/lib/modules/$kver/build" \
                || fail "DKMS preflight found no /lib/modules/$kver/build tree; install the matching headers and retry."
            log "DKMS preflight: headers/build tree present for $kver" | tee -a "$SESSION_LOG"
        done < <(arch_kernel_versions)
        run_chroot_try "Inspect DKMS state" dkms status
        return 0
    fi

    [[ -x "$TARGET_ROOT/usr/sbin/dkms" || -x "$TARGET_ROOT/usr/bin/dkms" ]] \
        || fail "DKMS is not installed in the target system."

    log "SIMULATE/PREFLIGHT: DKMS module rebuild" | tee -a "$SESSION_LOG"
    run_chroot_try "Inspect DKMS state" dkms status
    # dkms status may return nonzero when no modules are registered; that is not
    # itself a repair failure. The kernel/header inventory below decides whether
    # autoinstall has a viable build environment.

    while IFS= read -r kver; do
        [[ -n "$kver" ]] || continue
        if run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
            test -e "/lib/modules/$kver/build"; then
            log "DKMS preflight: headers/build tree present for $kver" | tee -a "$SESSION_LOG"
            continue
        fi
        missing_headers+=("$kver")
        header_pkg="linux-headers-$kver"
        if apt_package_available "$header_pkg"; then
            available_headers+=("$header_pkg")
            log "DKMS preflight: missing headers for $kver; repository package $header_pkg is available." | tee -a "$SESSION_LOG"
        else
            log "WARNING: DKMS preflight found no build tree for $kver and $header_pkg is unavailable from configured repositories." | tee -a "$SESSION_LOG"
        fi
    done < <(installed_kernel_versions)

    if ((${#available_headers[@]} > 0)); then
        safe_apt_install_packages \
            "Install missing kernel headers required by DKMS" no \
            "${available_headers[@]}"
    fi

    # Re-check any missing build trees after the known correction.  Do not fail
    # solely because an old installed kernel has no repository headers; DKMS may
    # have no module work for it.  The real autoinstall result remains decisive.
    for kver in "${missing_headers[@]}"; do
        if run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
            test -e "/lib/modules/$kver/build"; then
            log "DKMS preflight correction: build tree now present for $kver" | tee -a "$SESSION_LOG"
        fi
    done
}

adaptive_dkms_repair()
{
    preflight_dkms
    run_chroot_try "Rebuild DKMS modules" dkms autoinstall
    if ((CHROOT_TRY_RC != 0)); then
        if grep -Eiq 'kernel headers.*(not found|missing)|your kernel headers.*cannot be found|/lib/modules/.*/build.*(missing|no such)' \
            <<<"$CHROOT_TRY_OUTPUT"; then
            log "KNOWN ISSUE: DKMS reported missing kernel headers; re-running header preflight/correction once." | tee -a "$SESSION_LOG"
            preflight_dkms
            run_chroot_try "Retry DKMS autoinstall after header correction" dkms autoinstall
        fi
    fi
    ((CHROOT_TRY_RC == 0)) || fail "DKMS autoinstall failed after preflight/known correction."
    log "PASS: DKMS autoinstall completed." | tee -a "$SESSION_LOG"
    run_chroot_try "Post-repair DKMS status" dkms status
    # DKMS rebuilds cannot be proven byte-identical without hashing every
    # module tree and its side effects; stay changed (fail safe).
    repair_change_status dkms changed
}

# ---------------------------------------------------------------------------
# Display manager detection and offline graphical-login restoration
# ---------------------------------------------------------------------------
display_manager_package_for_service()
{
    # The package name for a display manager depends on the detected package
    # manager, not on the distribution family: Debian-family trees use gdm3,
    # pacman/apk trees use gdm.  dpkg evidence is the tie-breaker.
    if target_dpkg_detected; then
        case "$1" in
            sddm.service) printf '%s\n' 'sddm' ;;
            gdm.service|gdm3.service) printf '%s\n' 'gdm3' ;;
            lightdm.service) printf '%s\n' 'lightdm' ;;
            greetd.service) printf '%s\n' 'greetd' ;;
            ly.service) printf '%s\n' 'ly' ;;
            *) printf '%s\n' '' ;;
        esac
        return 0
    fi
    case "$1" in
        gdm.service|gdm3.service) printf '%s\n' 'gdm' ;;
        *) printf '%s\n' "${1%.service}" ;;
    esac
}

display_manager_label_for_service()
{
    case "$1" in
        sddm.service|sddm) printf '%s\n' 'SDDM' ;;
        gdm.service|gdm) printf '%s\n' 'GDM' ;;
        gdm3.service|gdm3) printf '%s\n' 'GDM3' ;;
        lightdm.service|lightdm) printf '%s\n' 'LightDM' ;;
        greetd.service|greetd) printf '%s\n' 'greetd' ;;
        ly.service|ly) printf '%s\n' 'ly' ;;
        slim) printf '%s\n' 'SLiM' ;;
        lxdm) printf '%s\n' 'LXDM' ;;
        *) printf '%s\n' "${1%.service}" ;;
    esac
}

display_manager_unit_rel()
{
    local service="$1" candidate
    for candidate in "/usr/lib/systemd/system/$service" "/lib/systemd/system/$service"; do
        if [[ -f "$TARGET_ROOT$candidate" ]]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done
    return 1
}

trial_display_manager_headless()
{
    local verify_output="" exec_path="" unit_file="${TARGET_ROOT}${DISPLAY_MANAGER_UNIT_REL}"

    # Starting a display manager in the repair chroot would attach it to the
    # recovery host's display/session and can leave processes behind.  Use
    # systemd's offline verifier instead, which checks unit syntax and
    # dependencies without switching the target or launching a GUI.
    if command -v systemd-analyze >/dev/null 2>&1; then
        if verify_output="$(systemd-analyze --root="$TARGET_ROOT" verify "$DISPLAY_MANAGER_SERVICE" graphical.target 2>&1)"; then
            log "PASS: headless systemd verification for $DISPLAY_MANAGER_SERVICE" | tee -a "$SESSION_LOG"
        else
            log "WARNING: headless systemd verification reported issues for $DISPLAY_MANAGER_SERVICE (the manager will not be started during repair)." | tee -a "$SESSION_LOG"
            [[ -z "$verify_output" ]] || sed -n '1,40p' <<<"$verify_output" | tee -a "$SESSION_LOG"
        fi
    else
        log "WARNING: systemd-analyze is unavailable; skipped headless display-manager verification." | tee -a "$SESSION_LOG"
    fi

    # Confirm the unit's primary executable exists inside the target.  Unit
    # files may contain wrappers or specifiers, so this is advisory only.
    exec_path="$(sed -n 's/^ExecStart=\([^[:space:];]*\).*/\1/p' "$unit_file" 2>/dev/null | head -n1 || true)"
    if [[ -n "$exec_path" && "$exec_path" == /* && ! -x "$TARGET_ROOT$exec_path" ]]; then
        log "WARNING: $DISPLAY_MANAGER_SERVICE ExecStart is not executable in the target: $exec_path" | tee -a "$SESSION_LOG"
    elif [[ -n "$exec_path" ]]; then
        log "PASS: $DISPLAY_MANAGER_SERVICE ExecStart is present for headless preflight: $exec_path" | tee -a "$SESSION_LOG"
    fi
}

target_package_installed()
{
    local package="$1"
    [[ -n "$package" ]] || return 1
    if target_has_executable /usr/bin/dpkg-query /usr/sbin/dpkg-query /bin/dpkg-query; then
        run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
            dpkg-query -W -f='${db:Status-Status}' "$package" 2>/dev/null \
            | grep -Fxq installed
        return $?
    fi
    if target_has_executable /usr/bin/pacman /usr/bin/pacman-static; then
        run_selected_chroot /usr/bin/env PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            pacman --root / -Q "$package" >/dev/null 2>&1
        return $?
    fi
    if target_has_executable /sbin/apk /usr/sbin/apk /usr/bin/apk; then
        target_apk_package_installed "$package"
        return $?
    fi
    if [[ -f "$TARGET_ROOT/lib/apk/db/installed" ]]; then
        target_apk_package_installed "$package"
        return $?
    fi
    if target_has_executable /usr/bin/rpm /bin/rpm /usr/sbin/rpm; then
        target_rpm_package_installed "$package"
        return $?
    fi
    return 1
}

# True when at least one detected package manager can answer installed-state
# queries for the target.
package_query_available()
{
    target_has_executable /usr/bin/dpkg-query /usr/sbin/dpkg-query /bin/dpkg-query \
        /usr/bin/pacman /usr/bin/pacman-static \
        /sbin/apk /usr/sbin/apk /usr/bin/apk \
        /usr/bin/rpm /bin/rpm /usr/sbin/rpm
}

last_boot_display_manager_candidates()
{
    local candidate
    local -a known_services=(sddm.service gdm.service gdm3.service lightdm.service greetd.service ly.service)
    command -v journalctl >/dev/null 2>&1 || return 0
    target_journal_evidence_present || return 0
    for candidate in "${known_services[@]}"; do
        # A unit-scoped query is used only for selection.  The complete
        # graphics journal remains visible in diagnostic_display below.
        if journalctl --root="$TARGET_ROOT" -b 0 -u "$candidate" --no-pager -n 5 2>/dev/null | grep -q .; then
            printf '%s\n' "$candidate"
        fi
    done
}

# Discover the target's display manager from the existing
# display-manager.service link, installed units and last-boot journal evidence.
# Sets DISPLAY_MANAGER_SERVICE/PACKAGE/LABEL/UNIT_REL; fails instead of
# guessing when the configuration is ambiguous or points at a missing unit.
detect_display_manager()
{
    local configured_service="" link="" service="" package="" unit=""
    local candidate installed_count=0 installed_service="" journal_count=0 journal_service=""
    local -a known_services=(sddm.service gdm.service gdm3.service lightdm.service greetd.service ly.service)

    DISPLAY_MANAGER_SERVICE=""
    DISPLAY_MANAGER_PACKAGE=""
    DISPLAY_MANAGER_LABEL=""
    DISPLAY_MANAGER_UNIT_REL=""

    if [[ -L "$TARGET_ROOT/etc/systemd/system/display-manager.service" ]]; then
        link="$(readlink "$TARGET_ROOT/etc/systemd/system/display-manager.service" 2>/dev/null || true)"
        configured_service="$(basename -- "$link" 2>/dev/null || true)"
        # Only accept a conventional unit basename.  The unit must still be
        # present under the target's systemd unit directories below.
        if [[ "$configured_service" =~ ^[[:alnum:]_.@:+-]+\.service$ ]]; then
            unit="$(display_manager_unit_rel "$configured_service" || true)"
            if [[ -n "$unit" ]]; then
                service="$configured_service"
            else
                fail "display-manager.service points to missing target unit $configured_service; refusing to switch to another installed manager."
            fi
        else
            fail "display-manager.service has an invalid target; refusing to switch to another installed manager."
        fi
    fi

    if [[ -z "$service" ]]; then
        # Without a valid display-manager alias, infer only when exactly one
        # known manager is installed.  Multiple candidates are ambiguous and
        # must not be replaced by an arbitrary manager.
        for candidate in "${known_services[@]}"; do
            package="$(display_manager_package_for_service "$candidate")"
            if [[ -n "$package" ]] && target_package_installed "$package" && display_manager_unit_rel "$candidate" >/dev/null; then
                installed_count=$((installed_count + 1))
                installed_service="$candidate"
            fi
        done
        if ((installed_count == 1)); then
            service="$installed_service"
            unit="$(display_manager_unit_rel "$service")"
        elif ((installed_count > 1)); then
            while IFS= read -r candidate; do
                [[ -n "$candidate" ]] || continue
                journal_count=$((journal_count + 1))
                journal_service="$candidate"
            done < <(last_boot_display_manager_candidates)
            if ((journal_count == 1)); then
                service="$journal_service"
                unit="$(display_manager_unit_rel "$service")"
                log "Selected $service from last-boot display-manager journal evidence because multiple managers are installed." | tee -a "$SESSION_LOG"
            else
                fail "Multiple display managers are installed but display-manager.service is not configured; refusing to guess ($installed_count candidates)."
            fi
        elif ((installed_count == 0)); then
            while IFS= read -r candidate; do
                [[ -n "$candidate" ]] || continue
                [[ -n "$(display_manager_unit_rel "$candidate" || true)" ]] || continue
                journal_count=$((journal_count + 1))
                journal_service="$candidate"
            done < <(last_boot_display_manager_candidates)
            if ((journal_count == 1)); then
                service="$journal_service"
                unit="$(display_manager_unit_rel "$service")"
                log "Selected $service from last-boot display-manager journal evidence." | tee -a "$SESSION_LOG"
            fi
        fi
    fi

    [[ -n "$service" ]] || fail "No configured graphical login manager was found. Install and configure a supported display manager (SDDM, GDM, LightDM, greetd or ly) first."
    DISPLAY_MANAGER_SERVICE="$service"
    DISPLAY_MANAGER_UNIT_REL="${unit:-$(display_manager_unit_rel "$service" || true)}"
    DISPLAY_MANAGER_PACKAGE="$(display_manager_package_for_service "$service")"
    DISPLAY_MANAGER_LABEL="$(display_manager_label_for_service "$service")"
    [[ -n "$DISPLAY_MANAGER_UNIT_REL" ]] || fail "Display manager $DISPLAY_MANAGER_SERVICE has no unit file in the target."
}

# Validate that the detected display manager is installed with its unit and
# graphical.target present, correcting known missing packages through the
# detected APT backend before the offline systemd link repair is allowed to
# run.  Package-state queries use whichever package manager was detected.
preflight_display_manager()
{
    local desktop_status=""
    need systemctl

    log "SIMULATE/PREFLIGHT: graphical login / display manager" | tee -a "$SESSION_LOG"
    detect_display_manager
    log "Detected display manager: $DISPLAY_MANAGER_LABEL ($DISPLAY_MANAGER_SERVICE)" | tee -a "$SESSION_LOG"

    [[ -f "$TARGET_ROOT/usr/lib/systemd/system/graphical.target" || -f "$TARGET_ROOT/lib/systemd/system/graphical.target" ]] \
        || fail "graphical.target is missing from the target system; repair systemd packages before restoring graphical login."
    [[ -f "$TARGET_ROOT$DISPLAY_MANAGER_UNIT_REL" ]] \
        || fail "$DISPLAY_MANAGER_LABEL service unit is missing from the target."

    if ! target_package_installed "$DISPLAY_MANAGER_PACKAGE"; then
        if target_apt_ready; then
            DISPLAY_MANAGER_PACKAGE_WORK=true
            safe_apt_install_packages "Install missing $DISPLAY_MANAGER_LABEL display manager" no "$DISPLAY_MANAGER_PACKAGE"
        else
            fail "$DISPLAY_MANAGER_LABEL is not installed according to the detected package manager; refusing to enable an unverified display manager."
        fi
    fi

    if [[ "$TARGET_OS_ID" == "tuxedo" ]] && target_apt_ready; then
        desktop_status="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
            dpkg-query -W -f='${db:Status-Status}' tuxedoos-desktop 2>/dev/null || true)"
        if [[ "$desktop_status" != "installed" ]] && apt_package_available tuxedoos-desktop; then
            DISPLAY_MANAGER_PACKAGE_WORK=true
            safe_apt_install_packages "Restore the TUXEDO desktop meta-package required for graphical login" no tuxedoos-desktop
        fi
    fi

    [[ -f "$TARGET_ROOT$DISPLAY_MANAGER_UNIT_REL" ]] \
        || fail "$DISPLAY_MANAGER_LABEL service unit is still missing after known package correction."

    trial_display_manager_headless

    log "Display-manager preflight plan: set graphical.target default, enable $DISPLAY_MANAGER_SERVICE, and repair display-manager.service offline." | tee -a "$SESSION_LOG"
}

# True when the offline graphical-login state already matches the repair
# target: default.target resolves to graphical.target, display-manager.service
# resolves to the detected manager, and the unit is enabled when installable.
display_manager_already_correct()
{
    local display_link default_link
    display_link="$(readlink "$TARGET_ROOT/etc/systemd/system/display-manager.service" 2>/dev/null || true)"
    default_link="$(readlink "$TARGET_ROOT/etc/systemd/system/default.target" 2>/dev/null || true)"
    [[ "$display_link" == *"$DISPLAY_MANAGER_SERVICE" ]] || return 1
    [[ "$default_link" == *graphical.target ]] || return 1
    if grep -Eq '^\[Install\][[:space:]]*$' "$TARGET_ROOT$DISPLAY_MANAGER_UNIT_REL" 2>/dev/null; then
        systemctl --root="$TARGET_ROOT" is-enabled "$DISPLAY_MANAGER_SERVICE" 2>/dev/null \
            | grep -Fxq enabled || return 1
    fi
    return 0
}

# ---------------------------------------------------------------------------
# Alpine/OpenRC display-manager repair (offline runlevel link only)
# ---------------------------------------------------------------------------
# Detect the OpenRC service that provides display-manager for offline repair.
# Sets DISPLAY_MANAGER_SERVICE/PACKAGE/LABEL/UNIT_REL; fails on ambiguity
# instead of guessing which graphical login to restore.
detect_alpine_display_manager()
{
    local service
    local -a services=()
    while IFS= read -r service; do
        [[ -n "$service" ]] || continue
        services+=("$service")
    done < <(alpine_display_manager_services)
    ((${#services[@]} > 0)) \
        || fail "No OpenRC service provides display-manager in the target; install and configure a display manager first."
    ((${#services[@]} == 1)) \
        || fail "Multiple OpenRC services provide display-manager (${services[*]}); refusing to guess which graphical login to restore."
    DISPLAY_MANAGER_SERVICE="${services[0]}"
    DISPLAY_MANAGER_PACKAGE="$(display_manager_package_for_service "$DISPLAY_MANAGER_SERVICE")"
    DISPLAY_MANAGER_LABEL="$(display_manager_label_for_service "$DISPLAY_MANAGER_SERVICE")"
    DISPLAY_MANAGER_UNIT_REL="/etc/init.d/$DISPLAY_MANAGER_SERVICE"
}

# Parse the primary command= path from /etc/conf.d/<service> or the init
# script so the preflight can prove the executable exists in the target.
alpine_display_manager_command_path()
{
    local service="$1" exec_path=""
    if [[ -r "$TARGET_ROOT/etc/conf.d/$service" ]]; then
        exec_path="$(sed -nE 's/^[[:space:]]*command=[[:space:]]*"?([^"[:space:]]+).*/\1/p' "$TARGET_ROOT/etc/conf.d/$service" | head -n1)"
    fi
    if [[ -z "$exec_path" && -r "$TARGET_ROOT/etc/init.d/$service" ]]; then
        exec_path="$(sed -nE 's/^[[:space:]]*command=[[:space:]]*"?([^"[:space:]]+).*/\1/p' "$TARGET_ROOT/etc/init.d/$service" | head -n1)"
    fi
    printf '%s\n' "$exec_path"
}

# Offline OpenRC display-manager preflight: the service must provide
# display-manager, its apk package must be installed, the init script must
# pass sh -n and rc-service -e, and the primary command must exist in the
# target.  The service is never started.
preflight_alpine_display_manager()
{
    local command_path=""
    log "SIMULATE/PREFLIGHT: OpenRC graphical login / display manager (offline only)" | tee -a "$SESSION_LOG"
    alpine_openrc_present || fail "OpenRC is not present in the Alpine target."
    detect_alpine_display_manager
    log "Detected Alpine OpenRC display manager: $DISPLAY_MANAGER_LABEL ($DISPLAY_MANAGER_SERVICE)" | tee -a "$SESSION_LOG"
    [[ -x "$TARGET_ROOT/etc/init.d/$DISPLAY_MANAGER_SERVICE" ]] \
        || fail "$DISPLAY_MANAGER_LABEL init script is missing from the target: /etc/init.d/$DISPLAY_MANAGER_SERVICE"
    target_package_installed "$DISPLAY_MANAGER_PACKAGE" \
        || fail "$DISPLAY_MANAGER_LABEL is not installed according to the detected package manager; refusing to enable an unverified display manager."
    run_chroot_try "Syntax-check $DISPLAY_MANAGER_LABEL OpenRC init script" sh -n "/etc/init.d/$DISPLAY_MANAGER_SERVICE"
    (( CHROOT_TRY_RC == 0 )) \
        || fail "$DISPLAY_MANAGER_LABEL init script failed sh -n; refusing to enable it."
    run_chroot_try "Verify $DISPLAY_MANAGER_LABEL service exists (rc-service -e)" rc-service -e "$DISPLAY_MANAGER_SERVICE"
    (( CHROOT_TRY_RC == 0 )) \
        || fail "rc-service does not recognize $DISPLAY_MANAGER_SERVICE in the target."
    command_path="$(alpine_display_manager_command_path "$DISPLAY_MANAGER_SERVICE")"
    if [[ -n "$command_path" ]]; then
        [[ "$command_path" == /* ]] \
            || fail "$DISPLAY_MANAGER_LABEL command is not an absolute target path: $command_path"
        [[ -x "$TARGET_ROOT$command_path" ]] \
            || fail "$DISPLAY_MANAGER_LABEL command is missing from the target: $command_path"
        log "PASS: $DISPLAY_MANAGER_LABEL command present for headless preflight: $command_path" | tee -a "$SESSION_LOG"
    else
        log "WARNING: no explicit command= was found for $DISPLAY_MANAGER_SERVICE; sh -n and rc-service -e still passed." | tee -a "$SESSION_LOG"
    fi
    log "$DISPLAY_MANAGER_LABEL will be configured offline only; Boot Bitch will not start a graphical session." | tee -a "$SESSION_LOG"
}

# Offline OpenRC runlevel repair: restore the default-runlevel symlink for the
# service that provides display-manager.  Idempotent, never starts the GUI and
# reports changed/unchanged from the link state.
adaptive_alpine_display_manager_repair()
{
    local runlevel_dir="$TARGET_ROOT/etc/runlevels/default"
    local link_before="" link_after=""
    DISPLAY_MANAGER_PACKAGE_WORK=false
    preflight_alpine_display_manager
    link_before="$(readlink "$runlevel_dir/$DISPLAY_MANAGER_SERVICE" 2>/dev/null || true)"
    mkdir -p "$runlevel_dir"
    log "Restoring the OpenRC default-runlevel link for $DISPLAY_MANAGER_LABEL (offline only; the service is NOT started)." | tee -a "$SESSION_LOG"
    ln -sfn "/etc/init.d/$DISPLAY_MANAGER_SERVICE" "$runlevel_dir/$DISPLAY_MANAGER_SERVICE"
    link_after="$(readlink "$runlevel_dir/$DISPLAY_MANAGER_SERVICE" 2>/dev/null || true)"
    [[ "$link_after" == "/etc/init.d/$DISPLAY_MANAGER_SERVICE" ]] \
        || fail "$DISPLAY_MANAGER_SERVICE runlevel link does not resolve to /etc/init.d/$DISPLAY_MANAGER_SERVICE after repair."
    log "PASS: /etc/runlevels/default/$DISPLAY_MANAGER_SERVICE -> $link_after" | tee -a "$SESSION_LOG"
    log "$DISPLAY_MANAGER_LABEL was configured offline only; Boot Bitch intentionally did not start a graphical session." | tee -a "$SESSION_LOG"
    if [[ "$link_before" == "/etc/init.d/$DISPLAY_MANAGER_SERVICE" ]]; then
        repair_change_status display "unchanged|OpenRC default runlevel already enabled $DISPLAY_MANAGER_SERVICE"
    else
        repair_change_status display changed
    fi
}

adaptive_display_manager_repair()
{
    local links_before=false links_after=false display_backend
    display_backend="$(target_display_manager_backend)"
    if [[ "$display_backend" == OpenRC ]]; then
        # OpenRC has no systemd units: restore the runlevel symlink for the
        # service that provides display-manager and never run systemctl or
        # start the graphical session.
        adaptive_alpine_display_manager_repair
        return 0
    fi
    if [[ "$display_backend" != systemd ]]; then
        fail "No supported display-manager backend was detected (service manager: ${TARGET_SERVICE_MANAGER:-unknown}; display backend: ${display_backend})."
    fi
    DISPLAY_MANAGER_PACKAGE_WORK=false
    preflight_display_manager
    if display_manager_already_correct; then
        links_before=true
    fi
    restore_display_manager
    if display_manager_already_correct; then
        links_after=true
    fi
    # Only the offline links are proven unchanged; a preflight package install
    # is a system change even when the links happened to be correct already.
    if [[ "$links_before" == true && "$links_after" == true && "$DISPLAY_MANAGER_PACKAGE_WORK" != true ]]; then
        repair_change_status display "unchanged|default.target and display-manager.service were already correct"
    else
        repair_change_status display changed
    fi
}

# Trial-build an initramfs image for every installed kernel into a temporary
# /tmp path inside the target.  The target images are not touched; a stale
# mapper-path failure is retried once after compatibility alias correction.
preflight_initramfs()
{
    local kver sim_path
    local -a kernels=()

    [[ -x "$TARGET_ROOT/usr/sbin/update-initramfs" ]] \
        || fail "update-initramfs is not installed in the target system."
    [[ -x "$TARGET_ROOT/usr/sbin/mkinitramfs" ]] \
        || fail "mkinitramfs is not installed in the target system."

    validate_mapper_crypttab
    mapfile -t kernels < <(installed_kernel_versions)
    ((${#kernels[@]} > 0)) || fail "No installed kernels were found under target /boot."

    log "SIMULATE/PREFLIGHT: initramfs generation for ${#kernels[@]} installed kernel(s)" | tee -a "$SESSION_LOG"
    for kver in "${kernels[@]}"; do
        [[ -d "$TARGET_ROOT/lib/modules/$kver" || -d "$TARGET_ROOT/usr/lib/modules/$kver" ]] \
            || fail "Installed kernel $kver has no matching modules directory."
        # mkinitramfs runs after chroot, so its absolute output path must be
        # visible inside the target rather than in the host session directory.
        sim_path="/tmp/boot-repair-initramfs-preflight-${kver}.img"
        rm -f -- "$TARGET_ROOT$sim_path"
        run_chroot_try "Trial initramfs build for $kver (temporary output only)" \
            mkinitramfs -o "$sim_path" "$kver"
        if ((CHROOT_TRY_RC != 0)) && output_suggests_mapper_path_failure "$CHROOT_TRY_OUTPUT"; then
            if repair_stale_mapper_mount_alias; then
                run_chroot_try "Retry trial initramfs build for $kver after mapper alias correction" \
                    mkinitramfs -o "$sim_path" "$kver"
            fi
        fi
        ((CHROOT_TRY_RC == 0)) || fail "Trial initramfs build failed for $kver; target initramfs files were not changed."
        [[ -s "$TARGET_ROOT$sim_path" ]] || fail "Trial initramfs build for $kver produced no image."
        rm -f -- "$TARGET_ROOT$sim_path"
        log "PASS: trial initramfs build for $kver" | tee -a "$SESSION_LOG"
    done
}

arch_kernel_versions()
{
    local root candidate
    for root in /usr/lib/modules /lib/modules; do
        [[ -d "$TARGET_ROOT$root" ]] || continue
        while IFS= read -r candidate; do
            [[ "$candidate" =~ ^[[:alnum:]][[:alnum:].+_-]*$ ]] || continue
            printf '%s\n' "$candidate"
        done < <(find "$TARGET_ROOT$root" -mindepth 1 -maxdepth 1 -type d -printf '%f\n' 2>/dev/null)
    done | sort -V -u
}

# Shared kernel/module inventory for the Fedora/rpm-dracut backend.  Fedora
# kernel names are version-only: every module directory pairs with
# /boot/vmlinuz-<kver> and /boot/initramfs-<kver>.img.  The rescue pair
# (-0-rescue-<machine-id>) has no module directory and is excluded by design;
# it is a --no-hostonly rescue image and is never rebuilt by the repair.
# strict additionally fails closed when a module directory has no matching
# vmlinuz; the read-only capability/evidence probes skip such orphans.
_rpm_kernel_pairs_impl()
{
    local strict="$1" root candidate kver
    for root in /lib/modules /usr/lib/modules; do
        [[ -d "$TARGET_ROOT$root" ]] || continue
        for candidate in "$TARGET_ROOT$root"/*; do
            [[ -d "$candidate" ]] || continue
            kver="$(basename -- "$candidate")"
            [[ "$kver" =~ ^[[:alnum:]][[:alnum:].+_-]*$ ]] || continue
            [[ "$kver" == 0-rescue-* ]] && continue
            if [[ ! -f "$TARGET_ROOT/boot/vmlinuz-$kver" ]]; then
                if [[ "$strict" == strict ]]; then
                    fail "Installed kernel module directory $kver has no /boot/vmlinuz-$kver."
                fi
                continue
            fi
            printf '%s /boot/vmlinuz-%s /boot/initramfs-%s.img\n' "$kver" "$kver" "$kver"
        done
    done | sort -V -u
}

rpm_kernel_pairs()
{
    _rpm_kernel_pairs_impl strict
}

rpm_kernel_pairs_readonly()
{
    _rpm_kernel_pairs_impl loose
}

preflight_arch_initramfs()
{
    local kver sim_path
    local -a kernels=()

    [[ "$TARGET_INITRAMFS_BACKEND" == mkinitcpio ]] \
        || fail "mkinitcpio is not the selected initramfs backend for this target."
    target_has_executable /usr/bin/mkinitcpio /usr/sbin/mkinitcpio \
        || fail "mkinitcpio is not installed in the target system."
    validate_mapper_crypttab
    mapfile -t kernels < <(arch_kernel_versions)
    ((${#kernels[@]} > 0)) || fail "No installed kernel module directories were found for mkinitcpio."

    log "SIMULATE/PREFLIGHT: mkinitcpio trial builds for ${#kernels[@]} installed kernel(s)" | tee -a "$SESSION_LOG"
    for kver in "${kernels[@]}"; do
        sim_path="/tmp/boot-repair-initramfs-preflight-${kver}.img"
        rm -f -- "$TARGET_ROOT$sim_path"
        run_chroot_try "Trial mkinitcpio build for $kver (temporary output only)" \
            mkinitcpio -k "$kver" -g "$sim_path"
        ((CHROOT_TRY_RC == 0)) \
            || fail "Trial mkinitcpio build failed for $kver; target initramfs files were not changed."
        [[ -s "$TARGET_ROOT$sim_path" ]] \
            || fail "Trial mkinitcpio build for $kver produced no image."
        rm -f -- "$TARGET_ROOT$sim_path"
        log "PASS: trial mkinitcpio build for $kver" | tee -a "$SESSION_LOG"
    done
}

adaptive_arch_initramfs_repair()
{
    local image verify_rc
    local -a images=()
    preflight_arch_initramfs
    run_chroot_try "Rebuild Arch initramfs images with mkinitcpio -P" mkinitcpio -P
    ((CHROOT_TRY_RC == 0)) || fail "mkinitcpio -P failed after transaction-specific preflight."
    mapfile -t images < <(find "$TARGET_ROOT/boot" -maxdepth 1 -type f -name 'initramfs-*.img' -size +0c -print 2>/dev/null | sort -V)
    ((${#images[@]} > 0)) || fail "mkinitcpio completed but no non-empty /boot/initramfs-*.img image was found."
    if target_has_executable /usr/bin/lsinitcpio /usr/sbin/lsinitcpio; then
        for image in "${images[@]}"; do
            set +e
            run_selected_chroot /usr/bin/env HOME=/root PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
                lsinitcpio "${image#"$TARGET_ROOT"}" >/dev/null 2>&1
            verify_rc=$?
            set -e
            ((verify_rc == 0)) || fail "Generated Arch initramfs could not be read back by lsinitcpio: ${image#"$TARGET_ROOT"}"
        done
        log "PASS: lsinitcpio verified ${#images[@]} Arch initramfs image(s)" | tee -a "$SESSION_LOG"
    fi
    log "PASS: Arch initramfs images rebuilt and verified." | tee -a "$SESSION_LOG"
}

# Verify one Alpine initramfs image without lsinitcpio: list the archive with
# the target's zcat/cpio, and fall back to the mkinitfs -l build-input listing
# only when cpio cannot read it.  Never writes.
alpine_initramfs_verify()
{
    local image="$1" kver="$2" verify_rc
    set +e
    run_selected_chroot /usr/bin/env \
        HOME=/root PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
        sh -c "zcat '$image' | cpio -t >/dev/null 2>&1"
    verify_rc=$?
    set -e
    if (( verify_rc == 0 )); then
        log "PASS: initramfs $image verified by zcat/cpio listing" | tee -a "$SESSION_LOG"
        return 0
    fi
    run_chroot_try "Verify mkinitfs build inputs for $kver (mkinitfs -l)" mkinitfs -l "$kver"
    (( CHROOT_TRY_RC == 0 )) \
        || fail "Generated Alpine initramfs could not be verified with zcat/cpio or mkinitfs -l: $image"
    log "PASS: initramfs $image verified by mkinitfs -l build-input listing" | tee -a "$SESSION_LOG"
}

# Trial-build an Alpine initramfs image for every installed kernel into a
# temporary /tmp path inside the target.  The target images are not touched;
# the trial output is verified with the same reader used after apply.
preflight_alpine_initramfs()
{
    local pair kver suffix _boot_image _initramfs_image sim_path
    local -a pairs=()

    [[ "$TARGET_INITRAMFS_BACKEND" == mkinitfs ]] \
        || fail "mkinitfs is not the selected initramfs backend for this target."
    target_has_executable /sbin/mkinitfs /usr/sbin/mkinitfs /usr/bin/mkinitfs \
        || fail "mkinitfs is not installed in the target system."
    target_has_path /etc/mkinitfs/mkinitfs.conf /etc/mkinitfs \
        || fail "The target has no mkinitfs configuration."
    validate_mapper_crypttab
    mapfile -t pairs < <(alpine_kernel_pairs)
    ((${#pairs[@]} > 0)) || fail "No installed Alpine kernels were found under target /boot."

    log "SIMULATE/PREFLIGHT: mkinitfs trial builds for ${#pairs[@]} installed kernel(s)" | tee -a "$SESSION_LOG"
    for pair in "${pairs[@]}"; do
        read -r kver suffix _boot_image _initramfs_image <<<"$pair"
        [[ -n "$kver" ]] || continue
        sim_path="/tmp/boot-repair-initramfs-preflight-${kver}"
        rm -f -- "$TARGET_ROOT$sim_path"
        run_chroot_try "Trial mkinitfs build for $kver (temporary output only)" \
            mkinitfs -o "$sim_path" "$kver"
        (( CHROOT_TRY_RC == 0 )) \
            || fail "Trial mkinitfs build failed for $kver; target initramfs files were not changed."
        [[ -s "$TARGET_ROOT$sim_path" ]] \
            || fail "Trial mkinitfs build for $kver produced no image."
        alpine_initramfs_verify "$sim_path" "$kver"
        rm -f -- "$TARGET_ROOT$sim_path"
        log "PASS: trial mkinitfs build for $kver" | tee -a "$SESSION_LOG"
    done
}

# Apply mkinitfs per installed kernel (there is no mkinitcpio -P equivalent),
# then verify every rebuilt image with the same reader as the trial.
adaptive_alpine_initramfs_repair()
{
    local pair kver suffix _boot_image initramfs_image
    local -a pairs=()

    preflight_alpine_initramfs
    mapfile -t pairs < <(alpine_kernel_pairs)
    for pair in "${pairs[@]}"; do
        read -r kver suffix _boot_image initramfs_image <<<"$pair"
        [[ -n "$kver" ]] || continue
        run_chroot_try "Rebuild Alpine initramfs for $kver with mkinitfs" mkinitfs "$kver"
        (( CHROOT_TRY_RC == 0 )) \
            || fail "mkinitfs failed for $kver after transaction-specific preflight."
        [[ -s "$TARGET_ROOT$initramfs_image" ]] \
            || fail "mkinitfs completed but $initramfs_image is missing or empty."
        alpine_initramfs_verify "$initramfs_image" "$kver"
        log "PASS: Alpine initramfs rebuilt and verified for $kver" | tee -a "$SESSION_LOG"
    done
}

# ---------------------------------------------------------------------------
# Fedora/dracut initramfs backend (trial-verified per-kernel rebuilds)
# ---------------------------------------------------------------------------
# Read-only lsinitrd verification.  The non-fatal variant lets the apply path
# restore a backup before failing; the public wrapper fails closed.
dracut_initramfs_verify_rc()
{
    local image="$1" verify_rc
    set +e
    run_selected_chroot /usr/bin/env \
        HOME=/root PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
        lsinitrd "$image" >/dev/null 2>&1
    verify_rc=$?
    set -e
    return "$verify_rc"
}

dracut_initramfs_verify()
{
    local image="$1"
    dracut_initramfs_verify_rc "$image" \
        || fail "Generated dracut initramfs could not be read back by lsinitrd: $image"
    log "PASS: lsinitrd verified $image" | tee -a "$SESSION_LOG"
}

# Trial-build a dracut initramfs for every installed rpm kernel into a
# temporary /tmp path inside the target and prove the trial never touched
# /boot.  The target images are not modified; the trial output is verified
# with the same lsinitrd reader used after apply.
preflight_dracut_initramfs()
{
    local pair kver vmlinuz image sim_path free_kb largest=0 size required_kb
    local fingerprint_before="" fingerprint_after=""
    local -a pairs=()

    [[ "$TARGET_INITRAMFS_BACKEND" == dracut ]] \
        || fail "dracut is not the selected initramfs backend for this target."
    target_has_executable /usr/bin/dracut /usr/sbin/dracut \
        || fail "dracut is not installed in the target system."
    target_has_path /usr/lib/dracut \
        || fail "the dracut generator directory is missing from the target."
    target_has_executable /usr/bin/lsinitrd /usr/sbin/lsinitrd \
        || fail "lsinitrd is not installed in the target system; dracut image verification is unavailable."
    validate_mapper_crypttab
    mapfile -t pairs < <(rpm_kernel_pairs)
    ((${#pairs[@]} > 0)) || fail "No installed dracut kernels were found under target /boot."
    for pair in "${pairs[@]}"; do
        read -r kver vmlinuz image <<<"$pair"
        [[ -n "$kver" ]] || continue
        [[ -d "$TARGET_ROOT/lib/modules/$kver" || -d "$TARGET_ROOT/usr/lib/modules/$kver" ]] \
            || fail "Installed kernel $kver has no matching modules directory."
        [[ -f "$TARGET_ROOT$vmlinuz" ]] \
            || fail "Installed kernel $kver has no $vmlinuz."
    done
    [[ -d "$TARGET_ROOT/boot" ]] \
        || fail "The target has no /boot directory; refusing a dracut rebuild."
    target_path_is_mounted_rw "$TARGET_ROOT/boot" \
        || fail "The target /boot is not mounted read-write; refusing a dracut rebuild."
    while IFS= read -r image; do
        [[ -n "$image" ]] || continue
        size="$(stat -c '%s' "$TARGET_ROOT/boot/$image" 2>/dev/null || printf '0')"
        [[ "$size" =~ ^[0-9]+$ ]] || size=0
        if (( size > largest )); then
            largest="$size"
        fi
    done < <(find "$TARGET_ROOT/boot" -maxdepth 1 -type f -name 'initramfs-*.img' -printf '%f\n' 2>/dev/null | LC_ALL=C sort)
    # Two copies of the largest image or a 64 MiB working margin, whichever is
    # larger (the existing mkinitfs formula).
    required_kb=$(( (largest * 2 + 1023) / 1024 ))
    (( required_kb < 65536 )) && required_kb=65536
    free_kb="$(df -Pk "$TARGET_ROOT/boot" 2>/dev/null | awk 'NR==2 {print $4}' | head -n1 || true)"
    if [[ "$free_kb" =~ ^[0-9]+$ ]] && (( free_kb < required_kb )); then
        fail "The target /boot has ${free_kb} KiB free; at least ${required_kb} KiB is required for a safe dracut rebuild."
    fi

    log "SIMULATE/PREFLIGHT: dracut trial builds for ${#pairs[@]} installed kernel(s)" | tee -a "$SESSION_LOG"
    fingerprint_before="$(initramfs_image_fingerprint)"
    for pair in "${pairs[@]}"; do
        read -r kver vmlinuz image <<<"$pair"
        [[ -n "$kver" ]] || continue
        sim_path="/tmp/boot-repair-initramfs-preflight-${kver}.img"
        rm -f -- "$TARGET_ROOT$sim_path"
        run_chroot_try "Trial dracut build for $kver (temporary output only)" \
            dracut -f --kver "$kver" "$sim_path"
        (( CHROOT_TRY_RC == 0 )) \
            || fail "Trial dracut build failed for $kver; target initramfs files were not changed."
        [[ -s "$TARGET_ROOT$sim_path" ]] \
            || fail "Trial dracut build for $kver produced no image."
        dracut_initramfs_verify "$sim_path"
        rm -f -- "$TARGET_ROOT$sim_path"
        log "PASS: trial dracut build for $kver" | tee -a "$SESSION_LOG"
    done
    # A trial must never touch /boot: prove every image fingerprint is
    # unchanged before the apply is allowed to run.
    fingerprint_after="$(initramfs_image_fingerprint)"
    [[ "$fingerprint_before" == "$fingerprint_after" ]] \
        || fail "The dracut trial build modified a /boot initramfs image; refusing the repair."
    run_chroot_try "Record dracut version" dracut --version
    (( CHROOT_TRY_RC == 0 )) \
        || fail "dracut --version failed with exit code $CHROOT_TRY_RC; refusing the dracut rebuild."
    log "dracut preflight version: $(sed -n '1p' <<<"$CHROOT_TRY_OUTPUT")" | tee -a "$SESSION_LOG"
}

# Apply dracut per installed rpm kernel after the guarded trial preflight:
# back up the existing image, rebuild it, verify it with lsinitrd and restore
# the backup (with a proven fingerprint) when the applied image cannot be read
# back.  The rescue image, BLS entries, grubenv and grub.cfg are never touched.
adaptive_dracut_initramfs_repair()
{
    local pair kver vmlinuz image backup restore_fingerprint
    local -a pairs=()

    preflight_dracut_initramfs
    mapfile -t pairs < <(rpm_kernel_pairs)
    for pair in "${pairs[@]}"; do
        read -r kver vmlinuz image <<<"$pair"
        [[ -n "$kver" ]] || continue
        backup="$SESSION_DIR/initramfs-before-$kver.img"
        if [[ -s "$TARGET_ROOT$image" ]]; then
            install -m 0600 "$TARGET_ROOT$image" "$backup"
        else
            log "KNOWN ISSUE: $image is missing; creating it instead of attempting an update." | tee -a "$SESSION_LOG"
            rm -f -- "$backup"
        fi
        run_chroot_try "Rebuild Fedora initramfs for $kver with dracut" \
            dracut -f --kver "$kver" "$image"
        if (( CHROOT_TRY_RC != 0 )); then
            # dracut writes <outfile>.tmp and only mv -f's it on success, so
            # the previous image is retained by dracut itself.
            fail "dracut failed for $kver after transaction-specific preflight; the previous initramfs is retained by dracut."
        fi
        [[ -s "$TARGET_ROOT$image" ]] \
            || fail "dracut completed but $image is missing or empty."
        if ! dracut_initramfs_verify_rc "$image"; then
            if [[ -s "$backup" ]]; then
                install -m 0600 "$backup" "$TARGET_ROOT$image"
                restore_fingerprint="$(repair_file_fingerprint "$TARGET_ROOT$image")"
                [[ "$restore_fingerprint" == "$(repair_file_fingerprint "$backup")" ]] \
                    || fail "dracut verification failed for $kver and the previous initramfs restore could not be proven."
                fail "dracut verification failed for $kver; the previous initramfs was restored."
            fi
            fail "dracut verification failed for $kver and no previous initramfs backup was available."
        fi
        log "PASS: Fedora initramfs rebuilt and verified for $kver" | tee -a "$SESSION_LOG"
    done
}

# Rebuild every installed kernel's initramfs (mkinitcpio on Arch, mkinitfs on
# Alpine, update-initramfs on Debian), verify each image is readable and
# compare the before/after image fingerprints for the change-status line.
adaptive_initramfs_repair()
{
    local fingerprint_before="" fingerprint_after=""
    if command -v sha256sum >/dev/null 2>&1; then
        fingerprint_before="$(initramfs_image_fingerprint)"
    fi
    if [[ "$TARGET_INITRAMFS_BACKEND" == mkinitcpio ]]; then
        adaptive_arch_initramfs_repair
    elif [[ "$TARGET_INITRAMFS_BACKEND" == mkinitfs ]]; then
        adaptive_alpine_initramfs_repair
    elif [[ "$TARGET_INITRAMFS_BACKEND" == dracut ]]; then
        adaptive_dracut_initramfs_repair
    elif [[ "$TARGET_INITRAMFS_BACKEND" == initramfs-tools ]]; then
        local kver mode
        local -a kernels=()

        preflight_initramfs
        mapfile -t kernels < <(installed_kernel_versions)
        for kver in "${kernels[@]}"; do
            if [[ -s "$TARGET_ROOT/boot/initrd.img-$kver" ]]; then
                mode="-u"
            else
                mode="-c"
                log "KNOWN ISSUE: initrd.img-$kver is missing; creating it instead of attempting an update." | tee -a "$SESSION_LOG"
            fi
            if [[ "$mode" == "-u" ]]; then
                run_chroot_try "Update initramfs for $kver" update-initramfs -u -k "$kver"
            else
                run_chroot_try "Create missing initramfs for $kver" update-initramfs -c -k "$kver"
            fi
            if ((CHROOT_TRY_RC != 0)) && output_suggests_mapper_path_failure "$CHROOT_TRY_OUTPUT"; then
                if repair_stale_mapper_mount_alias; then
                    run_chroot_try "Retry initramfs for $kver after mapper alias correction" \
                        update-initramfs "$mode" -k "$kver"
                fi
            fi
            ((CHROOT_TRY_RC == 0)) || fail "initramfs generation failed for $kver after known corrections."
            [[ -s "$TARGET_ROOT/boot/initrd.img-$kver" ]] \
                || fail "initramfs command succeeded but /boot/initrd.img-$kver is missing or empty."
            if [[ -x "$TARGET_ROOT/usr/bin/lsinitramfs" || -x "$TARGET_ROOT/usr/sbin/lsinitramfs" ]]; then
                local verify_err="$SESSION_DIR/lsinitramfs-$kver.err" verify_rc
                set +e
                run_selected_chroot /usr/bin/env \
                    HOME=/root PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
                    lsinitramfs "/boot/initrd.img-$kver" >/dev/null 2>"$verify_err"
                verify_rc=$?
                set -e
                if ((verify_rc != 0)); then
                    cat "$verify_err" 2>/dev/null | tee -a "$SESSION_LOG" || true
                    fail "Generated initramfs for $kver could not be read back by lsinitramfs."
                fi
                log "PASS: lsinitramfs verified /boot/initrd.img-$kver" | tee -a "$SESSION_LOG"
            fi
            log "PASS: initramfs verified for $kver" | tee -a "$SESSION_LOG"
        done
    else
        fail "The detected initramfs backend '${TARGET_INITRAMFS_BACKEND:-unknown}' has no guarded repair implementation (mkinitfs, mkinitcpio, dracut and initramfs-tools are supported)."
    fi
    # Compare the rebuilt images themselves: an update-initramfs/mkinitcpio run
    # that produced byte-identical images did not change the system evidence.
    if [[ -z "$fingerprint_before" ]]; then
        repair_change_status initramfs changed
        return 0
    fi
    fingerprint_after="$(initramfs_image_fingerprint)"
    if [[ "$fingerprint_before" == "$fingerprint_after" ]]; then
        repair_change_status initramfs "unchanged|rebuilt initramfs images are byte-identical"
    else
        repair_change_status initramfs changed
    fi
}

# ---------------------------------------------------------------------------
# Backend-generic GRUB tool/path resolution
# ---------------------------------------------------------------------------
# Fedora/RHEL ship grub2-* tooling and keep the configuration under
# /boot/grub2; Debian/Arch/Alpine ship grub-*/update-grub and use /boot/grub.
# The layout is resolved from target evidence (existing tree and installed
# tools), never from the distribution family.

grub2_layout_detected()
{
    [[ "$(grub_config_path)" == /boot/grub2/grub.cfg ]]
}

# Resolve the configuration file the detected GRUB layout uses.
grub_config_path()
{
    if target_has_path /boot/grub2 \
        || target_has_executable /usr/sbin/grub2-mkconfig /usr/bin/grub2-mkconfig \
        || target_has_executable /usr/sbin/grub2-install /usr/bin/grub2-install \
        || target_has_executable /usr/bin/grub2-editenv /usr/sbin/grub2-editenv; then
        printf '%s\n' '/boot/grub2/grub.cfg'
    else
        printf '%s\n' '/boot/grub/grub.cfg'
    fi
}

# Resolve the configuration generator.  The layout-native generator is
# preferred; a mixed tree falls back to the other naming.  Returns 1 when no
# generator is installed.
grub_generator_tool()
{
    local candidate
    if grub2_layout_detected; then
        for candidate in /usr/sbin/grub2-mkconfig /usr/bin/grub2-mkconfig; do
            if [[ -x "$TARGET_ROOT$candidate" ]]; then
                printf '%s\n' "$candidate"
                return 0
            fi
        done
    fi
    for candidate in /usr/sbin/grub-mkconfig /usr/bin/grub-mkconfig \
        /usr/sbin/update-grub /usr/bin/update-grub; do
        if [[ -x "$TARGET_ROOT$candidate" ]]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done
    for candidate in /usr/sbin/grub2-mkconfig /usr/bin/grub2-mkconfig; do
        if [[ -x "$TARGET_ROOT$candidate" ]]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done
    return 1
}

# Resolve the configuration syntax checker (Fedora ships grub2-script-check,
# Debian/Arch ship grub-script-check).  Returns 1 when none is installed.
grub_script_check_tool()
{
    local candidate
    if grub2_layout_detected; then
        for candidate in /usr/bin/grub2-script-check /usr/sbin/grub2-script-check \
            /usr/bin/grub-script-check /usr/sbin/grub-script-check; do
            if [[ -x "$TARGET_ROOT$candidate" ]]; then
                printf '%s\n' "$candidate"
                return 0
            fi
        done
        return 1
    fi
    for candidate in /usr/bin/grub-script-check /usr/sbin/grub-script-check \
        /usr/bin/grub2-script-check /usr/sbin/grub2-script-check; do
        if [[ -x "$TARGET_ROOT$candidate" ]]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done
    return 1
}

# Resolve the BIOS boot-code installer for the detected layout.
grub_install_tool()
{
    local candidate
    if grub2_layout_detected; then
        for candidate in /usr/sbin/grub2-install /usr/bin/grub2-install; do
            if [[ -x "$TARGET_ROOT$candidate" ]]; then
                printf '%s\n' "$candidate"
                return 0
            fi
        done
    fi
    for candidate in /usr/sbin/grub-install /usr/bin/grub-install; do
        if [[ -x "$TARGET_ROOT$candidate" ]]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done
    for candidate in /usr/sbin/grub2-install /usr/bin/grub2-install; do
        if [[ -x "$TARGET_ROOT$candidate" ]]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done
    return 1
}

# Resolve grubenv's editor/reader (Fedora: grub2-editenv, Debian: grub-editenv).
grub_editenv_tool()
{
    local candidate
    for candidate in /usr/bin/grub2-editenv /usr/sbin/grub2-editenv \
        /usr/bin/grub-editenv /usr/sbin/grub-editenv; do
        if [[ -x "$TARGET_ROOT$candidate" ]]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done
    return 1
}

# The grubenv that belongs to the resolved GRUB configuration.
grub_env_path()
{
    printf '%s\n' "$(dirname -- "$(grub_config_path)")/grubenv"
}

# A valid GRUB environment block is exactly 1024 bytes and starts with the
# documented header.  A missing, short or foreign file fails closed: the
# regeneration path must not overwrite an unprovable grubenv.
grub_env_block_valid()
{
    local grubenv size header
    grubenv="$(grub_env_path)"
    [[ -f "$TARGET_ROOT$grubenv" ]] || return 1
    size="$(stat -c '%s' "$TARGET_ROOT$grubenv" 2>/dev/null || true)"
    [[ "$size" == "1024" ]] || return 1
    header="$(head -c 24 "$TARGET_ROOT$grubenv" 2>/dev/null || true)"
    [[ "$header" == '# GRUB Environment Block' ]]
}

# True when the recovery host booted in legacy BIOS mode.  Kept as a separate
# probe so contract fixtures can pin the firmware mode without touching sysfs.
bios_firmware_mode()
{
    [[ ! -d /sys/firmware/efi ]]
}

# Generate a GRUB candidate configuration to an isolated path, syntax-check it,
# require that every existing menu entry is preserved and only then allow the
# real grub.cfg to be replaced.  A stale mapper-path failure is retried once.
preflight_grub()
{
    local grub_mkconfig="" sim_path target_sim_path err_path output err_output rc

    if [[ -x "$TARGET_ROOT/usr/sbin/grub-mkconfig" ]]; then
        grub_mkconfig="/usr/sbin/grub-mkconfig"
    elif [[ -x "$TARGET_ROOT/usr/bin/grub-mkconfig" ]]; then
        grub_mkconfig="/usr/bin/grub-mkconfig"
    else
        fail "grub-mkconfig is not installed in the target system."
    fi
    # update-grub is only a wrapper that writes /boot/grub/grub.cfg and sends
    # progress to stdout.  It cannot produce an isolated candidate for a
    # guarded preflight, so prefer grub-mkconfig whenever it is installed.
    # This also makes the Linux-entry check inspect the actual generated file
    # on Debian/TUXEDO instead of the wrapper's progress messages.
    if [[ -x "$TARGET_ROOT/usr/sbin/grub-mkconfig" || -x "$TARGET_ROOT/usr/bin/grub-mkconfig" ]]; then
        log "Using grub-mkconfig with an isolated output path for preflight." | tee -a "$SESSION_LOG"
    else
        fail "Neither grub-mkconfig nor update-grub is installed in the target system."
    fi

    sim_path="$SESSION_DIR/grub-preflight.cfg"
    # The generated file is captured outside the target for GUI evidence, but
    # grub-script-check runs inside the target chroot and therefore needs a
    # path that exists beneath TARGET_ROOT as well.
    target_sim_path="/run/boot-repair-grub-preflight.cfg"
    err_path="$SESSION_DIR/grub-preflight.err"
    log "SIMULATE/PREFLIGHT: generate GRUB configuration to temporary session output" | tee -a "$SESSION_LOG"

    grub_trial_once()
    {
        set +e
        output="$(
            run_selected_chroot /usr/bin/env \
                HOME=/root PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
                "$grub_mkconfig" -o "$target_sim_path" 2>"$err_path"
        )"
        rc=$?
        set -e
        err_output="$(cat "$err_path" 2>/dev/null || true)"
        printf '%s\n' "$err_output" | tee -a "$SESSION_LOG"
        log "Trial GRUB generation exit code: $rc" | tee -a "$SESSION_LOG"
    }

    grub_trial_once
    if ((rc != 0)) && output_suggests_mapper_path_failure "$err_output"; then
        if repair_stale_mapper_mount_alias; then
            log "KNOWN ISSUE: GRUB trial failed on a stale mapper path; retrying once after compatibility alias correction." | tee -a "$SESSION_LOG"
            grub_trial_once
        fi
    fi
    ((rc == 0)) || fail "GRUB trial generation failed; /boot/grub/grub.cfg was not changed."

    [[ -s "$TARGET_ROOT$target_sim_path" ]] || fail "grub-mkconfig completed but produced no temporary configuration."
    cp -- "$TARGET_ROOT$target_sim_path" "$sim_path"
    [[ -s "$sim_path" ]] || fail "GRUB trial generation produced an empty configuration."
    install -D -m 0644 "$sim_path" "$TARGET_ROOT$target_sim_path"
    if compgen -G "$TARGET_ROOT/boot/vmlinuz-*" >/dev/null \
       && ! grep -Eq '(^|[[:space:]])(menuentry|linux|linuxefi)[[:space:]]' "$sim_path"; then
        rm -f "$TARGET_ROOT$target_sim_path"
        fail "GRUB trial configuration contains no Linux boot entry despite installed kernels."
    fi
    if [[ -x "$TARGET_ROOT/usr/bin/grub-script-check" || -x "$TARGET_ROOT/usr/sbin/grub-script-check" ]]; then
        run_chroot_try "Syntax-check trial GRUB configuration" grub-script-check "$target_sim_path"
        rm -f "$TARGET_ROOT$target_sim_path"
        ((CHROOT_TRY_RC == 0)) || fail "Trial GRUB configuration failed grub-script-check."
    fi
    guard_grub_candidate_preserves_entries "$TARGET_ROOT/boot/grub/grub.cfg" "$sim_path" \
        || fail "Refusing to replace GRUB configuration because one or more existing boot entries are absent from the generated candidate. Enable/configure os-prober or inspect the candidate before retrying."
    rm -f "$TARGET_ROOT$target_sim_path"
    log "PASS: GRUB trial configuration generated successfully." | tee -a "$SESSION_LOG"
}

grub_entry_keys()
{
    local config="$1"
    [[ -s "$config" ]] || return 0
    # Menuentry/submenu declarations are stable identifiers for the existing
    # boot menu.  Ignore indentation differences so a harmless formatter change
    # does not block a repair, while retaining the complete declaration (title,
    # class and menuentry id) so entries from another ESP cannot disappear
    # unnoticed.
    sed -nE '/^[[:space:]]*(menuentry|submenu)[[:space:]]/p' "$config" \
        | sed -E 's/[[:space:]]+/ /g; s/^ //; s/ $//' \
        | sort -u
}

# os-prober declares foreign-OS entries with an "(on /dev/...)" title suffix
# and an "osprober-..." menuentry id; both forms identify an entry that belongs
# to another installation rather than this target's own boot menu.
grub_entry_is_foreign()
{
    grep -Eq '\(on /dev/|osprober-' <<<"$1"
}

# Compare the existing GRUB menu with a generated candidate.  Added and
# removed entries are always reported as evidence: os-prober legitimately adds
# or drops foreign-OS entries between runs, so that must never happen
# silently.  The guard fails closed only when an existing native entry would
# be lost; a foreign entry that disappears is reported but does not block the
# repair, because the regenerated menu still boots this system.
guard_grub_candidate_preserves_entries()
{
    local existing="$1" candidate="$2" old_keys new_keys removed added native_removed
    local entry

    [[ -s "$existing" && -s "$candidate" ]] || return 0
    old_keys="$SESSION_DIR/grub-existing-entries"
    new_keys="$SESSION_DIR/grub-candidate-entries"
    removed="$SESSION_DIR/grub-removed-entries"
    added="$SESSION_DIR/grub-added-entries"
    native_removed="$SESSION_DIR/grub-native-removed-entries"
    grub_entry_keys "$existing" > "$old_keys"
    grub_entry_keys "$candidate" > "$new_keys"
    comm -23 "$old_keys" "$new_keys" > "$removed" || true
    comm -13 "$old_keys" "$new_keys" > "$added" || true

    if [[ -s "$added" ]]; then
        log "GRUB menu entries added by the candidate: $(wc -l < "$added" | tr -d '[:space:]')" | tee -a "$SESSION_LOG"
        while IFS= read -r entry; do
            [[ -n "$entry" ]] || continue
            if grub_entry_is_foreign "$entry"; then
                log "  foreign-entry-added: $entry" | tee -a "$SESSION_LOG"
            else
                log "  menu-entry-added: $entry" | tee -a "$SESSION_LOG"
            fi
        done < "$added"
    fi
    if [[ -s "$removed" ]]; then
        grep -Ev '\(on /dev/|osprober-' "$removed" > "$native_removed" || true
        while IFS= read -r entry; do
            [[ -n "$entry" ]] || continue
            if grub_entry_is_foreign "$entry"; then
                log "  foreign-entry-removed: $entry" | tee -a "$SESSION_LOG"
            fi
        done < "$removed"
        if [[ -s "$native_removed" ]]; then
            log "ERROR: GRUB preflight candidate would remove existing native menu entries; the target configuration was left unchanged." | tee -a "$SESSION_LOG"
            sed 's/^/  preserved-entry-required: /' "$native_removed" | tee -a "$SESSION_LOG"
            return 1
        fi
    fi
}

# Regenerate /boot/grub/grub.cfg with the target's update-grub or grub-mkconfig
# after the guarded preflight, roll back when a previous menu entry would be
# lost and report changed/unchanged from the grub.cfg fingerprint.
adaptive_grub_repair()
{
    local old_cfg="$SESSION_DIR/grub-before-update.cfg" generator="update-grub"
    local grub_before="" grub_after=""

    if [[ ! -x "$TARGET_ROOT/usr/sbin/update-grub" && ! -x "$TARGET_ROOT/usr/bin/update-grub" ]]; then
        generator="grub-mkconfig"
    fi

    if command -v sha256sum >/dev/null 2>&1; then
        grub_before="$(repair_file_fingerprint "$TARGET_ROOT/boot/grub/grub.cfg")"
    fi

    preflight_grub
    if [[ -s "$TARGET_ROOT/boot/grub/grub.cfg" ]]; then
        install -m 0644 "$TARGET_ROOT/boot/grub/grub.cfg" "$old_cfg"
    else
        rm -f "$old_cfg"
    fi
    if [[ "$generator" == update-grub ]]; then
        run_chroot_try "Regenerate GRUB configuration" update-grub
    else
        run_chroot_try "Regenerate GRUB configuration with grub-mkconfig" grub-mkconfig -o /boot/grub/grub.cfg
    fi
    if ((CHROOT_TRY_RC != 0)) && output_suggests_mapper_path_failure "$CHROOT_TRY_OUTPUT"; then
        if repair_stale_mapper_mount_alias; then
            [[ -s "$old_cfg" ]] && install -m 0644 "$old_cfg" "$TARGET_ROOT/boot/grub/grub.cfg"
            preflight_grub
            if [[ "$generator" == update-grub ]]; then
                run_chroot_try "Retry GRUB configuration after mapper alias correction" update-grub
            else
                run_chroot_try "Retry GRUB configuration after mapper alias correction" grub-mkconfig -o /boot/grub/grub.cfg
            fi
        fi
    fi
    ((CHROOT_TRY_RC == 0)) || fail "$generator failed after preflight/known correction."
    [[ -s "$TARGET_ROOT/boot/grub/grub.cfg" ]] \
        || fail "$generator completed but /boot/grub/grub.cfg is missing or empty."
    if [[ -s "$old_cfg" ]]; then
        if ! guard_grub_candidate_preserves_entries "$old_cfg" "$TARGET_ROOT/boot/grub/grub.cfg"; then
            # The candidate has already been generated, but restoring the
            # known-good file keeps a repair failure from silently removing
            # another disk's Linux menu entries.
            install -m 0644 "$old_cfg" "$TARGET_ROOT/boot/grub/grub.cfg"
            fail "GRUB regeneration was rolled back because it removed an existing boot entry."
        fi
    fi
    if [[ -x "$TARGET_ROOT/usr/bin/grub-script-check" || -x "$TARGET_ROOT/usr/sbin/grub-script-check" ]]; then
        run_chroot_try "Verify installed GRUB configuration syntax" grub-script-check /boot/grub/grub.cfg
        ((CHROOT_TRY_RC == 0)) || fail "Installed GRUB configuration failed grub-script-check after regeneration."
    fi
    log "PASS: GRUB configuration regenerated and verified." | tee -a "$SESSION_LOG"
    if [[ -z "$grub_before" ]]; then
        repair_change_status grub changed
        return 0
    fi
    grub_after="$(repair_file_fingerprint "$TARGET_ROOT/boot/grub/grub.cfg")"
    if [[ "$grub_before" == "$grub_after" ]]; then
        repair_change_status grub "unchanged|grub.cfg is byte-identical"
    else
        repair_change_status grub changed
    fi
}

# GRUB stage dispatcher: a Fedora/RHEL grub2 layout is repaired through the
# guarded --no-grubenv-update path (with the evidence-triggered BIOS boot-code
# reinstall); Debian/Arch/Alpine keep the existing generator path.  The
# optional `config-only` mode skips the boot-code reinstall substage for
# callers that must stay non-destructive (boot-stack reconciliation).
adaptive_grub_stage()
{
    local mode="${1:-full}"
    if grub2_layout_detected; then
        adaptive_fedora_grub_repair "$mode"
    else
        adaptive_grub_repair
    fi
}

# ---------------------------------------------------------------------------
# Fedora/RHEL GRUB2 configuration regeneration (grub2-mkconfig, BLS-aware)
# ---------------------------------------------------------------------------
# Fedora's grub2-mkconfig wrapper mutates /boot/loader/entries/*.conf,
# /etc/kernel/cmdline and grubenv unless --no-grubenv-update is passed.  An
# offline repair must not propagate /etc/default/grub into BLS entries or fake
# a good boot (grubby owns that), so the guarded path always passes the flag
# and fails closed when grubenv, the menu entry set or the BLS entry set moves.

# Fingerprint the complete Fedora GRUB2 artifact set the stage may touch:
# grub.cfg, grubenv and every BLS entry.  A missing file is represented
# explicitly so a repair that only creates it is still detected as changed.
grub_artifact_fingerprint()
{
    local config grubenv entry
    config="$(grub_config_path)"
    grubenv="$(grub_env_path)"
    command -v sha256sum >/dev/null 2>&1 || { printf 'no-sha256sum\n'; return 0; }
    printf '%s %s\n' "$config" "$(repair_file_fingerprint "$TARGET_ROOT$config")"
    printf '%s %s\n' "$grubenv" "$(repair_file_fingerprint "$TARGET_ROOT$grubenv")"
    while IFS= read -r entry; do
        [[ -n "$entry" ]] || continue
        printf '%s %s\n' "${entry#"$TARGET_ROOT"}" "$(repair_file_fingerprint "$entry")"
    done < <(find "$TARGET_ROOT/boot/loader/entries" -maxdepth 1 -type f -name '*.conf' 2>/dev/null | LC_ALL=C sort)
}

# Normalized BLS entry identity: basename plus the version/linux/initrd keys.
# The entry set must survive both the trial and the apply; --no-grubenv-update
# must not add, remove or retarget an entry.
fedora_bls_entry_keys()
{
    local file base version linux initrd
    [[ -d "$TARGET_ROOT/boot/loader/entries" ]] || return 0
    while IFS= read -r file; do
        [[ -n "$file" ]] || continue
        base="$(basename -- "$file")"
        version="$(sed -n 's/^version[[:space:]]*//p' "$file" 2>/dev/null | head -n1)"
        linux="$(sed -n 's/^linux[[:space:]]*//p' "$file" 2>/dev/null | head -n1)"
        initrd="$(sed -n 's/^initrd[[:space:]]*//p' "$file" 2>/dev/null | paste -sd, -)"
        printf '%s\t%s\t%s\t%s\n' "$base" "$version" "$linux" "$initrd"
    done < <(find "$TARGET_ROOT/boot/loader/entries" -maxdepth 1 -type f -name '*.conf' 2>/dev/null | LC_ALL=C sort)
}

guard_fedora_bls_entries_preserved()
{
    local before="$1" after="$2" missing
    [[ -s "$before" ]] || return 0
    missing="$SESSION_DIR/fedora-bls-missing-entries"
    comm -23 "$before" "$after" > "$missing" || true
    if [[ -s "$missing" ]]; then
        log "ERROR: the Fedora GRUB2 preflight would remove existing BLS entries; the target configuration was left unchanged." | tee -a "$SESSION_LOG"
        sed 's/^/  preserved-BLS-entry-required: /' "$missing" | tee -a "$SESSION_LOG"
        return 1
    fi
}

# Session-scoped backup of grub.cfg, grubenv, the BLS entry directory,
# /etc/default/grub and /etc/kernel/cmdline, with a SHA256SUMS manifest used to
# prove a rollback byte-identical.
FEDORA_GRUB_BACKUP_DIR=""

fedora_grub_backup_state()
{
    local backup="$SESSION_DIR/fedora-grub-backup" config grubenv entry
    config="$(grub_config_path)"
    grubenv="$(grub_env_path)"
    rm -rf -- "$backup"
    mkdir -p -- "$backup$(dirname -- "$config")" "$backup$(dirname -- "$grubenv")" \
        "$backup/boot/loader/entries" "$backup/etc/default" "$backup/etc/kernel" || return 1
    [[ -f "$TARGET_ROOT$config" ]] && cp -a -- "$TARGET_ROOT$config" "$backup$config" || true
    [[ -f "$TARGET_ROOT$grubenv" ]] && cp -a -- "$TARGET_ROOT$grubenv" "$backup$grubenv" || true
    [[ -f "$TARGET_ROOT/etc/default/grub" ]] && cp -a -- "$TARGET_ROOT/etc/default/grub" "$backup/etc/default/grub" || true
    [[ -f "$TARGET_ROOT/etc/kernel/cmdline" ]] && cp -a -- "$TARGET_ROOT/etc/kernel/cmdline" "$backup/etc/kernel/cmdline" || true
    while IFS= read -r entry; do
        [[ -n "$entry" ]] || continue
        cp -a -- "$entry" "$backup/boot/loader/entries/${entry##*/}" || return 1
    done < <(find "$TARGET_ROOT/boot/loader/entries" -maxdepth 1 -type f -name '*.conf' 2>/dev/null | LC_ALL=C sort)
    (cd "$backup" && find . -type f ! -name SHA256SUMS -exec sha256sum {} + > SHA256SUMS) || return 1
    FEDORA_GRUB_BACKUP_DIR="$backup"
    log "Backed up Fedora GRUB2 configuration artifacts to $backup." | tee -a "$SESSION_LOG"
}

# Prove every backed-up artifact is present in the target with its original
# digest.  Used after a rollback; a rollback that cannot be proven fails loud.
fedora_grub_verify_backup_restore()
{
    local backup="$1" line digest path
    [[ -s "$backup/SHA256SUMS" ]] || return 1
    while IFS= read -r line; do
        [[ -n "$line" ]] || continue
        read -r digest path <<<"$line" || return 1
        path="${path#./}"
        [[ -n "$digest" && -n "$path" ]] || return 1
        [[ "$path" == SHA256SUMS ]] && continue
        [[ "$(repair_file_fingerprint "$TARGET_ROOT/$path")" == "$digest" ]] || return 1
    done < "$backup/SHA256SUMS"
    return 0
}

fedora_grub_restore_backup()
{
    local backup="${FEDORA_GRUB_BACKUP_DIR:-$SESSION_DIR/fedora-grub-backup}" config grubenv entry
    [[ -d "$backup" ]] || return 1
    config="$(grub_config_path)"
    grubenv="$(grub_env_path)"
    if [[ -f "$backup$config" ]]; then
        cp -a -- "$backup$config" "$TARGET_ROOT$config" || return 1
    fi
    if [[ -f "$backup$grubenv" ]]; then
        cp -a -- "$backup$grubenv" "$TARGET_ROOT$grubenv" || return 1
    fi
    if [[ -f "$backup/etc/default/grub" ]]; then
        cp -a -- "$backup/etc/default/grub" "$TARGET_ROOT/etc/default/grub" || return 1
    fi
    if [[ -f "$backup/etc/kernel/cmdline" ]]; then
        cp -a -- "$backup/etc/kernel/cmdline" "$TARGET_ROOT/etc/kernel/cmdline" || return 1
    fi
    if [[ -d "$backup/boot/loader/entries" ]]; then
        mkdir -p -- "$TARGET_ROOT/boot/loader/entries" || return 1
        find "$TARGET_ROOT/boot/loader/entries" -maxdepth 1 -type f -name '*.conf' -delete 2>/dev/null || true
        while IFS= read -r entry; do
            [[ -n "$entry" ]] || continue
            cp -a -- "$entry" "$TARGET_ROOT/boot/loader/entries/${entry##*/}" || return 1
        done < <(find "$backup/boot/loader/entries" -maxdepth 1 -type f -name '*.conf' 2>/dev/null | LC_ALL=C sort)
    fi
    log "Restored the Fedora GRUB2 configuration artifacts from $backup." | tee -a "$SESSION_LOG"
}

# Mandatory read-only Fedora GRUB2 preflight: tools, configuration, grubenv
# validity, /boot read-write + free space, mapper/crypttab, the BLS path rule
# for btrfs/xfs, a trial generation with --no-grubenv-update, the menu-entry
# preservation guard and the BLS entry-set guard.  The backup is taken before
# the trial so an unexpected trial mutation can also be rolled back.
preflight_fedora_grub()
{
    local generator config grubenv editenv_tool boot_fstype free_kb required_kb=1024
    local target_sim_path="/run/boot-repair-grub2-preflight.cfg"
    local sim_path="$SESSION_DIR/fedora-grub2-preflight.cfg"
    local err_path="$SESSION_DIR/fedora-grub2-preflight.err"
    local bls_keys_before="$SESSION_DIR/fedora-bls-keys-before"
    local bls_keys_after="$SESSION_DIR/fedora-bls-keys-after"
    local script_tool output err_output rc

    grub2_layout_detected || fail "The detected GRUB layout is not a Fedora/RHEL grub2 layout."
    generator="$(grub_generator_tool)" || fail "grub2-mkconfig is not installed in the target system."
    [[ "$(basename -- "$generator")" == "grub2-mkconfig" ]] \
        || fail "grub2-mkconfig is not installed in the target system."
    editenv_tool="$(grub_editenv_tool)" \
        || fail "grub2-editenv is not installed in the target system."
    config="$(grub_config_path)"
    [[ -f "$TARGET_ROOT$config" ]] || fail "$config is missing."
    grubenv="$(grub_env_path)"
    grub_env_block_valid \
        || fail "grubenv is missing or not a valid GRUB environment block."
    # Read the block with the target's own editor as additional evidence: a
    # file that passes the byte checks but cannot be parsed fails closed.
    run_chroot_try "Read Fedora grubenv (grub2-editenv list)" "$editenv_tool" "$grubenv" list
    (( CHROOT_TRY_RC == 0 )) \
        || fail "grub2-editenv could not read $grubenv; refusing a GRUB2 regeneration."

    target_path_is_mounted_rw "$TARGET_ROOT/boot" \
        || fail "The target /boot is not mounted read-write; refusing a GRUB2 regeneration."
    free_kb="$(df -Pk "$TARGET_ROOT/boot" 2>/dev/null | awk 'NR==2 {print $4}' | head -n1 || true)"
    if [[ "$free_kb" =~ ^[0-9]+$ ]] && (( free_kb < required_kb )); then
        fail "The target /boot has ${free_kb} KiB free; at least ${required_kb} KiB is required for a safe GRUB2 regeneration."
    fi
    validate_mapper_crypttab

    # With --no-grubenv-update GRUB cannot derive the BLS directory on a
    # filesystem without a stable GRUB path; Fedora's grubenv carries blsdir
    # for those layouts.  Refuse rather than generate a menu without entries.
    boot_fstype="$(findmnt -rn -o FSTYPE --target "$TARGET_ROOT/boot" 2>/dev/null | head -n1 || true)"
    if [[ "$boot_fstype" == btrfs || "$boot_fstype" == xfs ]]; then
        grep -Eq '^blsdir=/boot/loader/entries([[:space:]]|$)' "$TARGET_ROOT$grubenv" \
            || fail "grubenv does not declare blsdir=/boot/loader/entries; --no-grubenv-update cannot derive the BLS path on $boot_fstype."
    fi

    fedora_grub_backup_state \
        || fail "Unable to back up the Fedora GRUB2 configuration before regeneration."
    fedora_bls_entry_keys > "$bls_keys_before"

    log "SIMULATE/PREFLIGHT: generate Fedora GRUB2 configuration with --no-grubenv-update" | tee -a "$SESSION_LOG"
    set +e
    output="$(
        run_selected_chroot /usr/bin/env \
            HOME=/root PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            "$generator" --no-grubenv-update -o "$target_sim_path" 2>"$err_path"
    )"
    rc=$?
    set -e
    err_output="$(cat "$err_path" 2>/dev/null || true)"
    printf '%s\n' "$output" "$err_output" | tee -a "$SESSION_LOG"
    (( rc == 0 )) || fail "Fedora GRUB2 trial generation failed; $config was not changed."

    [[ -s "$TARGET_ROOT$target_sim_path" ]] \
        || fail "grub2-mkconfig completed but produced no temporary configuration."
    install -D -m 0600 "$TARGET_ROOT$target_sim_path" "$sim_path"
    [[ -s "$sim_path" ]] || fail "Fedora GRUB2 trial generation produced an empty configuration."
    if ! grep -Eq '(^|[[:space:]])(blscfg|menuentry)([[:space:]]|$)' "$sim_path"; then
        rm -f -- "$TARGET_ROOT$target_sim_path"
        fail "Fedora GRUB2 trial configuration contains neither a blscfg directive nor a menuentry."
    fi
    if script_tool="$(grub_script_check_tool)"; then
        run_chroot_try "Syntax-check trial Fedora GRUB2 configuration" "$script_tool" "$target_sim_path"
        rm -f -- "$TARGET_ROOT$target_sim_path"
        (( CHROOT_TRY_RC == 0 )) || fail "Trial Fedora GRUB2 configuration failed grub2-script-check."
    else
        rm -f -- "$TARGET_ROOT$target_sim_path"
    fi
    guard_grub_candidate_preserves_entries "$TARGET_ROOT$config" "$sim_path" \
        || fail "Refusing to replace the Fedora GRUB2 configuration because one or more existing menu entries are absent from the generated candidate."
    fedora_bls_entry_keys > "$bls_keys_after"
    guard_fedora_bls_entries_preserved "$bls_keys_before" "$bls_keys_after" \
        || fail "Refusing to replace the Fedora GRUB2 configuration because the BLS entry set changed during the trial."
    log "PASS: Fedora GRUB2 trial configuration generated successfully." | tee -a "$SESSION_LOG"
}

# Apply the guarded Fedora GRUB2 regeneration: grub2-mkconfig with
# --no-grubenv-update, then verify the installed file, the grubenv fingerprint,
# the preserved menu entries, the BLS entry set and the script syntax.  Any
# failure restores the session backup and fails closed.
fedora_grub_regenerate_config()
{
    local config generator script_tool old_cfg="$SESSION_DIR/fedora-grub-before-update.cfg"
    local grubenv_before="" grubenv_after=""
    local bls_keys_after="$SESSION_DIR/fedora-bls-keys-applied"

    config="$(grub_config_path)"
    preflight_fedora_grub
    generator="$(grub_generator_tool)"
    script_tool="$(grub_script_check_tool 2>/dev/null || true)"
    if command -v sha256sum >/dev/null 2>&1; then
        grubenv_before="$(repair_file_fingerprint "$TARGET_ROOT$(grub_env_path)")"
    fi
    install -m 0600 "$TARGET_ROOT$config" "$old_cfg"

    run_chroot_try "Regenerate Fedora GRUB2 configuration (grub2-mkconfig --no-grubenv-update)" \
        "$generator" --no-grubenv-update -o "$config"
    if (( CHROOT_TRY_RC != 0 )); then
        if fedora_grub_restore_backup && fedora_grub_verify_backup_restore "$FEDORA_GRUB_BACKUP_DIR"; then
            fail "grub2-mkconfig failed after preflight; the previous GRUB2 configuration and grubenv were restored."
        fi
        repair_change_status grub changed
        fail "grub2-mkconfig failed and the previous GRUB2 configuration restore could not be proven."
    fi
    if [[ ! -s "$TARGET_ROOT$config" ]]; then
        if fedora_grub_restore_backup && fedora_grub_verify_backup_restore "$FEDORA_GRUB_BACKUP_DIR"; then
            fail "grub2-mkconfig completed but $config is missing or empty; the previous configuration was restored."
        fi
        repair_change_status grub changed
        fail "grub2-mkconfig completed but $config is missing or empty and the restore could not be proven."
    fi
    grubenv_after="$(repair_file_fingerprint "$TARGET_ROOT$(grub_env_path)")"
    if [[ -n "$grubenv_before" && "$grubenv_before" != "$grubenv_after" ]]; then
        if fedora_grub_restore_backup && fedora_grub_verify_backup_restore "$FEDORA_GRUB_BACKUP_DIR"; then
            fail "GRUB2 regeneration modified grubenv despite --no-grubenv-update; the previous configuration was restored."
        fi
        repair_change_status grub changed
        fail "GRUB2 regeneration modified grubenv and the restore could not be proven."
    fi
    if ! guard_grub_candidate_preserves_entries "$old_cfg" "$TARGET_ROOT$config"; then
        if fedora_grub_restore_backup && fedora_grub_verify_backup_restore "$FEDORA_GRUB_BACKUP_DIR"; then
            fail "Fedora GRUB2 regeneration was rolled back because it removed an existing boot entry."
        fi
        repair_change_status grub changed
        fail "Fedora GRUB2 regeneration removed an existing boot entry and the restore could not be proven."
    fi
    fedora_bls_entry_keys > "$bls_keys_after"
    if ! guard_fedora_bls_entries_preserved "$SESSION_DIR/fedora-bls-keys-before" "$bls_keys_after"; then
        if fedora_grub_restore_backup && fedora_grub_verify_backup_restore "$FEDORA_GRUB_BACKUP_DIR"; then
            fail "Fedora GRUB2 regeneration was rolled back because the BLS entry set changed."
        fi
        repair_change_status grub changed
        fail "Fedora GRUB2 regeneration changed the BLS entry set and the restore could not be proven."
    fi
    if [[ -n "$script_tool" ]]; then
        run_chroot_try "Verify installed Fedora GRUB2 configuration syntax" "$script_tool" "$config"
        if (( CHROOT_TRY_RC != 0 )); then
            if fedora_grub_restore_backup && fedora_grub_verify_backup_restore "$FEDORA_GRUB_BACKUP_DIR"; then
                fail "Installed Fedora GRUB2 configuration failed grub2-script-check; the previous configuration was restored."
            fi
            repair_change_status grub changed
            fail "Installed Fedora GRUB2 configuration failed grub2-script-check and the restore could not be proven."
        fi
    fi
    log "PASS: Fedora GRUB2 configuration regenerated and verified." | tee -a "$SESSION_LOG"
}

# ---------------------------------------------------------------------------
# Fedora/RHEL GRUB2 BIOS boot-code reinstall (evidence-triggered, no new key)
# ---------------------------------------------------------------------------
# The reinstall substage runs only when the read-only probe finds the MBR boot
# area or the GPT bios_grub partition without a GRUB signature.  A healthy
# system stays config-only, so the destructive path never runs on evidence of
# a working boot.

# Print the exactly-one GPT BIOS boot partition device of the selected disk.
fedora_bios_grub_partition()
{
    local disk_name count=0 found="" line
    [[ -n "${TARGET_DISK:-}" && -e "$TARGET_DISK" ]] || return 1
    disk_name="$(basename -- "$TARGET_DISK")"
    while IFS= read -r line; do
        [[ -n "$line" ]] || continue
        count=$((count + 1))
        found="${line%% *}"
    done < <(lsblk -rno NAME,PARTTYPE "$TARGET_DISK" 2>/dev/null \
        | awk -v disk="$disk_name" '$1 != disk && $2 == "21686148-6449-6e6f-744e-656564454649" {print $1}')
    (( count == 1 )) || return 1
    printf '/dev/%s\n' "$found"
}

# Read-only boot-code probe: true when MBR bytes 0-445 carry no GRUB boot.img
# signature or the bios_grub partition carries no core.img signature.
fedora_grub_boot_code_broken()
{
    local part
    if ! dd if="$TARGET_DISK" bs=512 count=1 2>/dev/null | head -c 446 \
        | grep -aqE 'GRUB|Geom|Hard Disk|Read Error'; then
        return 0
    fi
    part="$(fedora_bios_grub_partition 2>/dev/null || true)"
    [[ -n "$part" ]] || return 0
    if ! dd if="$part" bs=512 count=1 2>/dev/null | grep -aqE 'GRUB|Geom|loading'; then
        return 0
    fi
    return 1
}

# Partition-table fingerprint: MBR bytes 446-511, the GPT header/entry array
# (LBA 1-33) and the lsblk PARTUUID/PARTTYPE/SIZE inventory.  The reinstall
# must never change the table; a mismatch fails closed before any restore.
partition_table_fingerprint()
{
    local inventory
    command -v sha256sum >/dev/null 2>&1 || { printf 'no-sha256sum\n'; return 0; }
    printf 'mbr-tail '
    dd if="$TARGET_DISK" bs=1 skip=446 count=66 2>/dev/null | sha256sum | awk '{print $1}'
    printf 'gpt-head '
    dd if="$TARGET_DISK" bs=512 skip=1 count=33 2>/dev/null | sha256sum | awk '{print $1}'
    inventory="$(lsblk -rno NAME,PARTUUID,PARTTYPE,SIZE "$TARGET_DISK" 2>/dev/null | LC_ALL=C sort)"
    printf 'inventory %s\n' "$(printf '%s' "$inventory" | sha256sum | awk '{print $1}')"
}

# Fingerprint the GRUB2 BIOS boot code the reinstall may rewrite: MBR bytes
# 0-445, the bios_grub partition (core.img + blocklists) and the i386-pc
# module directory.  grub2-install output is not reproducible across runs
# (blocklist patching), so a successful reinstall legitimately reports changed.
fedora_grub_boot_fingerprint()
{
    local part module
    command -v sha256sum >/dev/null 2>&1 || { printf 'no-sha256sum\n'; return 0; }
    part="$(fedora_bios_grub_partition 2>/dev/null || true)"
    printf 'mbr-boot '
    dd if="$TARGET_DISK" bs=1 count=446 2>/dev/null | sha256sum | awk '{print $1}'
    printf 'bios-grub '
    if [[ -n "$part" ]]; then
        dd if="$part" bs=1M count=1 2>/dev/null | sha256sum | awk '{print $1}'
    else
        printf 'missing\n'
    fi
    if [[ -d "$TARGET_ROOT/boot/grub2/i386-pc" ]]; then
        while IFS= read -r module; do
            [[ -n "$module" ]] || continue
            printf 'i386-pc/%s %s\n' "${module##*/}" "$(repair_file_fingerprint "$module")"
        done < <(find "$TARGET_ROOT/boot/grub2/i386-pc" -maxdepth 1 -type f 2>/dev/null | LC_ALL=C sort)
    else
        printf 'i386-pc missing\n'
    fi
}

FEDORA_GRUB_BOOT_BACKUP_DIR=""

fedora_grub_backup_boot_state()
{
    local backup="$SESSION_DIR/fedora-grub-boot-backup" part
    part="$(fedora_bios_grub_partition)" || return 1
    rm -rf -- "$backup"
    mkdir -p -- "$backup" || return 1
    dd if="$TARGET_DISK" of="$backup/mbr.bin" bs=512 count=1 2>/dev/null || return 1
    dd if="$part" of="$backup/bios-grub.bin" bs=1M count=1 2>/dev/null || return 1
    if [[ -f "$TARGET_ROOT/boot/grub2/device.map" ]]; then
        cp -a -- "$TARGET_ROOT/boot/grub2/device.map" "$backup/device.map" || return 1
    fi
    if [[ -d "$TARGET_ROOT/boot/grub2/i386-pc" ]]; then
        cp -a -- "$TARGET_ROOT/boot/grub2/i386-pc" "$backup/i386-pc" || return 1
    fi
    (cd "$backup" && find . -type f ! -name SHA256SUMS -exec sha256sum {} + > SHA256SUMS) || return 1
    FEDORA_GRUB_BOOT_BACKUP_DIR="$backup"
    log "Backed up GRUB2 BIOS boot code (MBR, BIOS boot partition, i386-pc modules) to $backup." | tee -a "$SESSION_LOG"
}

# Restore MBR (full 512 B), the bios_grub partition (1 MiB) and the i386-pc
# module directory from the session backup.  Never touches the GPT table.
fedora_grub_restore_boot_backup()
{
    local backup="${FEDORA_GRUB_BOOT_BACKUP_DIR:-$SESSION_DIR/fedora-grub-boot-backup}" part
    [[ -d "$backup" ]] || return 1
    part="$(fedora_bios_grub_partition 2>/dev/null || true)"
    [[ -n "$part" ]] || return 1
    [[ -s "$backup/mbr.bin" ]] || return 1
    [[ -s "$backup/bios-grub.bin" ]] || return 1
    dd if="$backup/mbr.bin" of="$TARGET_DISK" bs=512 count=1 conv=notrunc 2>/dev/null || return 1
    dd if="$backup/bios-grub.bin" of="$part" bs=1M count=1 conv=notrunc 2>/dev/null || return 1
    if [[ -f "$backup/device.map" ]]; then
        cp -a -- "$backup/device.map" "$TARGET_ROOT/boot/grub2/device.map" || return 1
    fi
    if [[ -d "$backup/i386-pc" ]]; then
        rm -rf -- "$TARGET_ROOT/boot/grub2/i386-pc"
        cp -a -- "$backup/i386-pc" "$TARGET_ROOT/boot/grub2/i386-pc" || return 1
    fi
    log "Restored the GRUB2 BIOS boot code from $backup." | tee -a "$SESSION_LOG"
}

# Prove the restore is byte-identical to the session backup.
fedora_grub_boot_backup_restored()
{
    local backup="$1" part
    [[ -d "$backup" ]] || return 1
    part="$(fedora_bios_grub_partition 2>/dev/null || true)"
    [[ -n "$part" ]] || return 1
    cmp -s -- "$backup/mbr.bin" <(dd if="$TARGET_DISK" bs=512 count=1 2>/dev/null) || return 1
    cmp -s -- "$backup/bios-grub.bin" <(dd if="$part" bs=1M count=1 2>/dev/null) || return 1
    if [[ -d "$backup/i386-pc" ]]; then
        [[ -d "$TARGET_ROOT/boot/grub2/i386-pc" ]] || return 1
        diff -rq -- "$backup/i386-pc" "$TARGET_ROOT/boot/grub2/i386-pc" >/dev/null 2>&1 || return 1
    fi
    return 0
}

# Mandatory read-only preflight for the boot-code reinstall: BIOS only, a
# grub2 layout with grub2-install, a GPT bios_grub partition on the selected
# disk (exactly one, plain partition, unmounted, >= 64 KiB), /boot on the same
# disk and no unresolved LUKS mapping.
preflight_fedora_grub_reinstall()
{
    local part pttable ptype mounted size boot_source install_tool
    (( RUNNING_HOST_MODE == 0 )) \
        || fail "GRUB2 BIOS boot-code reinstall is only implemented for a mounted repair target."
    bios_firmware_mode \
        || fail "UEFI firmware detected; the GRUB2 BIOS boot-code reinstall is BIOS-only."
    grub2_layout_detected || fail "The detected GRUB layout is not a Fedora/RHEL grub2 layout."
    install_tool="$(grub_install_tool)" || fail "grub2-install is not installed in the target system."
    grub_editenv_tool >/dev/null || fail "grub2-editenv is not installed in the target system."
    [[ -f "$TARGET_ROOT$(grub_config_path)" ]] || fail "$(grub_config_path) is missing."
    validate_mapper_crypttab

    pttable="$(lsblk -ndo PTTYPE "$TARGET_DISK" 2>/dev/null | tr -d '[:space:]' | head -n1 || true)"
    [[ "$pttable" == "gpt" ]] \
        || fail "the selected disk uses a ${pttable:-unknown} partition table; the guarded GRUB2 reinstall requires a GPT bios_grub partition."
    part="$(fedora_bios_grub_partition)" \
        || fail "the selected disk has no BIOS boot partition for GRUB core.img."
    ptype="$(lsblk -ndo TYPE "$part" 2>/dev/null | head -n1 || true)"
    [[ "$ptype" == "part" ]] \
        || fail "the BIOS boot partition $part is not a plain partition (type: ${ptype:-unknown}); LVM/md/RAID is not supported."
    # findmnt --target on a device node resolves the filesystem containing the
    # node (devtmpfs at /dev), so the partition's mount state is read from
    # lsblk's device-scoped MOUNTPOINTS column instead.
    mounted="$(lsblk -nrpo MOUNTPOINTS "$part" 2>/dev/null | tr -d '[:space:]')"
    [[ -z "$mounted" ]] || fail "the BIOS boot partition $part is mounted at $mounted; refusing to reinstall GRUB2 boot code."
    size="$(lsblk -bdno SIZE "$part" 2>/dev/null | head -n1 || true)"
    if [[ ! "$size" =~ ^[0-9]+$ ]] || (( size < 65536 )); then
        fail "the BIOS boot partition $part is smaller than 64 KiB; refusing to reinstall GRUB2 boot code."
    fi
    boot_source="$(findmnt -rn -o SOURCE --target "$TARGET_ROOT/boot" 2>/dev/null | head -n1 || true)"
    boot_source="${boot_source%%[*}"
    if [[ -n "$boot_source" && -b "$boot_source" ]]; then
        same_single_top_disk "$TARGET_DISK" "$boot_source" \
            || fail "the target /boot is not on the selected disk $TARGET_DISK; refusing to reinstall GRUB2 boot code."
    fi
    run_chroot_try "Record grub2-install version" "$install_tool" --version
    (( CHROOT_TRY_RC == 0 )) \
        || fail "grub2-install is present but could not execute during the preflight."
}

# Apply the guarded reinstall: back up MBR/bios_grub/i386-pc, run
# grub2-install --target=i386-pc --boot-directory=/boot --recheck on the
# selected disk, verify the partition table is byte-identical, then verify the
# GRUB signatures, module files and configuration syntax.  Any failure restores
# the session backup and re-proves the restore; an unprovable restore reports
# changed and fails loudly.
fedora_grub_reinstall_boot_code()
{
    local install_tool table_before="" table_after=""
    local script_tool config

    preflight_fedora_grub_reinstall
    install_tool="$(grub_install_tool)"
    config="$(grub_config_path)"
    table_before="$(partition_table_fingerprint)"
    fedora_grub_backup_boot_state \
        || fail "Unable to back up the GRUB2 BIOS boot code before reinstall."

    log "REPAIR: reinstall GRUB2 BIOS boot code ($(basename -- "$install_tool") --target=i386-pc --boot-directory=/boot --recheck $TARGET_DISK)" | tee -a "$SESSION_LOG"
    run_chroot_try "Reinstall GRUB2 BIOS boot code" \
        "$install_tool" --target=i386-pc --boot-directory=/boot --recheck "$TARGET_DISK"

    table_after="$(partition_table_fingerprint)"
    if [[ "$table_before" != "$table_after" ]]; then
        fail "The partition table changed during the GRUB2 boot-code reinstall; refusing to restore over a changed partition table."
    fi

    if (( CHROOT_TRY_RC != 0 )) \
        || fedora_grub_boot_code_broken \
        || [[ ! -f "$TARGET_ROOT/boot/grub2/i386-pc/boot.img" ]] \
        || [[ ! -f "$TARGET_ROOT/boot/grub2/i386-pc/core.img" ]]; then
        if fedora_grub_restore_boot_backup && fedora_grub_boot_backup_restored "$FEDORA_GRUB_BOOT_BACKUP_DIR"; then
            fail "grub2-install failed verification; the MBR, BIOS boot partition and i386-pc modules were restored byte-identical."
        fi
        repair_change_status grub changed
        fail "grub2-install failed verification and the GRUB2 boot-code restore could not be proven; the target boot code must be inspected."
    fi
    script_tool="$(grub_script_check_tool 2>/dev/null || true)"
    if [[ -n "$script_tool" ]]; then
        run_chroot_try "Verify Fedora GRUB2 configuration syntax after boot-code reinstall" "$script_tool" "$config"
        if (( CHROOT_TRY_RC != 0 )); then
            if fedora_grub_restore_boot_backup && fedora_grub_boot_backup_restored "$FEDORA_GRUB_BOOT_BACKUP_DIR"; then
                fail "Installed Fedora GRUB2 configuration failed grub2-script-check; the MBR, BIOS boot partition and i386-pc modules were restored byte-identical."
            fi
            repair_change_status grub changed
            fail "Installed Fedora GRUB2 configuration failed grub2-script-check and the GRUB2 boot-code restore could not be proven."
        fi
    fi
    log "PASS: GRUB2 BIOS boot code reinstalled and verified (partition table byte-identical)." | tee -a "$SESSION_LOG"
}

# Fedora BIOS boot-stack fail-closed pairing check: every installed kernel
# module directory has a vmlinuz (strict rpm_kernel_pairs), a non-empty
# initramfs image and a BLS entry.  Runs before GRUB reconciliation so a
# partial package/kernel update cannot be masked by a config regeneration.
fedora_bootstack_pairing_check()
{
    local pair kver machine_id entry found
    machine_id="$(tr -d '[:space:]' < "$TARGET_ROOT/etc/machine-id" 2>/dev/null || true)"
    while IFS= read -r pair; do
        kver="${pair%% *}"
        [[ -n "$kver" ]] || continue
        [[ -s "$TARGET_ROOT/boot/initramfs-$kver.img" ]] \
            || fail "Fedora boot-stack reconciliation requires /boot/initramfs-$kver.img for kernel $kver; run the initramfs stage first."
        if [[ -n "$machine_id" ]]; then
            [[ -f "$TARGET_ROOT/boot/loader/entries/$machine_id-$kver.conf" ]] \
                || fail "Fedora boot-stack reconciliation requires /boot/loader/entries/$machine_id-$kver.conf for kernel $kver; run the package stage first."
        else
            found=false
            while IFS= read -r entry; do
                [[ -n "$entry" ]] || continue
                if grep -Eq "^version[[:space:]]+$kver$" "$entry" 2>/dev/null; then
                    found=true
                    break
                fi
            done < <(find "$TARGET_ROOT/boot/loader/entries" -maxdepth 1 -type f -name '*.conf' 2>/dev/null | LC_ALL=C sort)
            [[ "$found" == true ]] \
                || fail "Fedora boot-stack reconciliation requires a BLS entry for kernel $kver under /boot/loader/entries."
        fi
    done < <(rpm_kernel_pairs)
}

# Fedora GRUB2 stage entry point: evidence-triggered boot-code reinstall (only
# when the MBR/bios_grub probe finds broken boot code and this is a mounted
# target), followed by the guarded configuration regeneration, with one
# aggregated change status.
adaptive_fedora_grub_repair()
{
    local mode="${1:-full}"
    local grub_before="" grub_after="" config_changed=false
    local boot_before="" boot_after="" boot_attempted=false boot_changed=false

    if command -v sha256sum >/dev/null 2>&1; then
        grub_before="$(grub_artifact_fingerprint)"
    fi
    if [[ "$mode" == "config-only" ]]; then
        log "GRUB2 config-only reconciliation requested; the BIOS boot-code reinstall substage is skipped." | tee -a "$SESSION_LOG"
    elif ! bios_firmware_mode; then
        log "UEFI firmware detected; the GRUB2 BIOS boot-code reinstall substage is not applicable." | tee -a "$SESSION_LOG"
    elif fedora_grub_boot_code_broken; then
        if (( RUNNING_HOST_MODE == 1 )); then
            log "NOTE: GRUB2 BIOS boot-code reinstall is only implemented for a mounted repair target; the running host stays config-only." | tee -a "$SESSION_LOG"
        else
            boot_attempted=true
            boot_before="$(fedora_grub_boot_fingerprint)"
            fedora_grub_reinstall_boot_code
        fi
    else
        log "GRUB2 boot-code probe: MBR and BIOS boot partition contain a GRUB signature; config-only regeneration." | tee -a "$SESSION_LOG"
    fi

    fedora_grub_regenerate_config
    if command -v sha256sum >/dev/null 2>&1; then
        grub_after="$(grub_artifact_fingerprint)"
        [[ "$grub_before" == "$grub_after" ]] || config_changed=true
    else
        config_changed=true
    fi
    if [[ "$boot_attempted" == true ]]; then
        boot_after="$(fedora_grub_boot_fingerprint)"
        [[ "$boot_before" == "$boot_after" ]] || boot_changed=true
        if [[ "$config_changed" == true || "$boot_changed" == true ]]; then
            repair_change_status grub changed
        else
            repair_change_status grub "unchanged|MBR, BIOS boot partition and i386-pc modules are byte-identical"
        fi
        return 0
    fi
    if [[ "$config_changed" == true ]]; then
        repair_change_status grub changed
    else
        repair_change_status grub "unchanged|grub.cfg and grubenv are byte-identical"
    fi
}

# ---------------------------------------------------------------------------
# Alpine extlinux configuration regeneration (no boot-sector writes)
# ---------------------------------------------------------------------------
# Normalized LABEL/LINUX/INITRD/KERNEL keys from an extlinux configuration.
extlinux_entry_keys()
{
    local config="$1"
    [[ -s "$config" ]] || return 0
    awk '
        /^[[:space:]]*LABEL[[:space:]]/ { label = $2; printf "LABEL\t%s\n", label; next }
        /^[[:space:]]*LINUX[[:space:]]/ { if (label != "") printf "LINUX\t%s\t%s\n", label, $2; next }
        /^[[:space:]]*INITRD[[:space:]]/ { if (label != "") printf "INITRD\t%s\t%s\n", label, $2; next }
        /^[[:space:]]*KERNEL[[:space:]]/ { if (label != "") printf "KERNEL\t%s\t%s\n", label, $2; next }
    ' "$config" | sort -u
}

# Every existing LABEL plus its LINUX/INITRD/KERNEL paths must survive in the
# generated candidate, mirroring the GRUB entry-preservation guard.
guard_extlinux_candidate_preserves_entries()
{
    local existing="$1" candidate="$2" old_keys new_keys missing
    [[ -s "$existing" && -s "$candidate" ]] || return 0
    old_keys="$SESSION_DIR/extlinux-existing-entries"
    new_keys="$SESSION_DIR/extlinux-candidate-entries"
    missing="$SESSION_DIR/extlinux-missing-entries"
    extlinux_entry_keys "$existing" > "$old_keys"
    extlinux_entry_keys "$candidate" > "$new_keys"
    comm -23 "$old_keys" "$new_keys" > "$missing" || true
    if [[ -s "$missing" ]]; then
        log "ERROR: extlinux preflight candidate would remove existing boot entries; the target configuration was left unchanged." | tee -a "$SESSION_LOG"
        sed 's/^/  preserved-entry-required: /' "$missing" | tee -a "$SESSION_LOG"
        return 1
    fi
}

# Require every kernel/initramfs path referenced by the candidate to exist in
# the target /boot so a truncated or partial generation is never installed.
# INITRD may be a comma-separated microcode+initramfs list.
extlinux_verify_candidate()
{
    local candidate="$1" line path count=0
    local -a paths=()
    if [[ ! -s "$candidate" ]]; then
        log "ERROR: update-extlinux produced an empty extlinux configuration candidate." | tee -a "$SESSION_LOG"
        return 1
    fi
    while IFS= read -r line; do
        [[ -n "$line" ]] || continue
        IFS=',' read -ra paths <<<"$line"
        for path in "${paths[@]}"; do
            [[ -n "$path" ]] || continue
            if [[ "$path" == /* ]]; then
                log "ERROR: extlinux candidate references an absolute boot path: $path" | tee -a "$SESSION_LOG"
                return 1
            fi
            if [[ ! -e "$TARGET_ROOT/boot/$path" ]]; then
                log "ERROR: extlinux candidate references a missing boot file: $path" | tee -a "$SESSION_LOG"
                return 1
            fi
            count=$((count + 1))
        done
    done < <(awk '/^[[:space:]]*(LINUX|INITRD|KERNEL)[[:space:]]/ { print $2 }' "$candidate" | sort -u)
    if compgen -G "$TARGET_ROOT/boot/vmlinuz*" >/dev/null && (( count == 0 )); then
        log "ERROR: extlinux candidate contains no kernel or initramfs entry despite installed kernels." | tee -a "$SESSION_LOG"
        return 1
    fi
    log "PASS: extlinux candidate references $count boot artifact(s) present in the target" | tee -a "$SESSION_LOG"
}

extlinux_backup_state()
{
    local dir="$SESSION_DIR/extlinux-backup"
    rm -rf -- "$dir"
    mkdir -p -- "$dir"
    if [[ -f "$TARGET_ROOT/boot/extlinux.conf" ]]; then
        cp -a -- "$TARGET_ROOT/boot/extlinux.conf" "$dir/extlinux.conf"
    fi
    if [[ -f "$TARGET_ROOT/etc/update-extlinux.conf" ]]; then
        cp -a -- "$TARGET_ROOT/etc/update-extlinux.conf" "$dir/update-extlinux.conf"
    fi
    log "Backed up the extlinux configuration under $dir" | tee -a "$SESSION_LOG"
}

extlinux_restore_backup()
{
    local dir="$SESSION_DIR/extlinux-backup"
    if [[ -f "$dir/extlinux.conf" ]]; then
        cp -a -- "$dir/extlinux.conf" "$TARGET_ROOT/boot/extlinux.conf"
    else
        rm -f -- "$TARGET_ROOT/boot/extlinux.conf"
    fi
    if [[ -f "$dir/update-extlinux.conf" ]]; then
        cp -a -- "$dir/update-extlinux.conf" "$TARGET_ROOT/etc/update-extlinux.conf"
    fi
    rm -f -- "$TARGET_ROOT/boot/extlinux.conf.new"
    log "Restored the pre-repair extlinux configuration" | tee -a "$SESSION_LOG"
}

# Read-only extlinux preflight: update-extlinux, /etc/update-extlinux.conf,
# an existing extlinux/syslinux configuration, the syslinux package, the
# mapper/crypttab gate and the /boot backing disk.  The boot sector is never
# part of this repair.
preflight_extlinux()
{
    local updater="" candidate boot_source boot_canonical
    for candidate in /sbin/update-extlinux /usr/sbin/update-extlinux /usr/bin/update-extlinux; do
        if [[ -x "$TARGET_ROOT$candidate" ]]; then
            updater="$candidate"
            break
        fi
    done
    [[ -n "$updater" ]] || fail "update-extlinux is not installed in the target system."
    [[ -f "$TARGET_ROOT/etc/update-extlinux.conf" ]] \
        || fail "The target has no /etc/update-extlinux.conf; refusing to regenerate extlinux.conf."
    target_has_path /boot/extlinux.conf /boot/syslinux/syslinux.cfg /boot/syslinux/ldlinux.sys \
        || fail "No extlinux/syslinux configuration was detected under target /boot."
    if ! package_query_available; then
        fail "No detected package manager can verify the syslinux package; refusing to regenerate extlinux.conf."
    fi
    target_package_installed syslinux \
        || fail "The syslinux package is not installed according to the detected package manager; refusing to regenerate extlinux.conf."
    validate_mapper_crypttab
    boot_source="$(findmnt -rn -o SOURCE --target "$TARGET_ROOT/boot" 2>/dev/null | head -n1 || true)"
    if [[ -n "$boot_source" ]]; then
        boot_source="${boot_source%%[*}"
        boot_canonical="$(canonical_block "$boot_source" 2>/dev/null || true)"
        if [[ -n "$boot_canonical" ]]; then
            same_single_top_disk "$TARGET_DISK" "$boot_canonical" \
                || fail "The target /boot filesystem does not belong to the selected target disk; refusing to regenerate extlinux.conf."
        fi
    fi
    log "SIMULATE/PREFLIGHT: extlinux configuration regeneration via $updater (overwrite=0 trial; no boot sector write)" | tee -a "$SESSION_LOG"
}

# Regenerate /boot/extlinux.conf with the target's update-extlinux using the
# overwrite=0 trial pattern: update-extlinux writes only
# /boot/extlinux.conf.new and exits before it copies modules or updates the
# boot sector.  The candidate is entry-guarded, verified and only then
# installed; any failure restores the captured files.
adaptive_extlinux_repair()
{
    local cfg_before="" cfg_after="" candidate="$TARGET_ROOT/boot/extlinux.conf.new"
    local original_conf="$SESSION_DIR/extlinux-backup/update-extlinux.conf"
    local config="$TARGET_ROOT/etc/update-extlinux.conf"

    preflight_extlinux
    cfg_before="$(repair_file_fingerprint "$TARGET_ROOT/boot/extlinux.conf")"
    extlinux_backup_state

    # Force the documented overwrite=0 behavior for the trial.  The original
    # configuration is restored before the candidate is judged, so the target
    # never keeps the temporary setting.
    sed -i -E 's/^[[:space:]]*overwrite=.*/overwrite=0/' "$config"
    if ! grep -Eq '^[[:space:]]*overwrite=' "$config"; then
        printf '\noverwrite=0\n' >> "$config"
    fi
    run_chroot_try "Regenerate extlinux configuration (overwrite=0 trial; no boot sector write)" update-extlinux
    cp -a -- "$original_conf" "$config"

    if (( CHROOT_TRY_RC != 0 )); then
        rm -f -- "$candidate"
        fail "update-extlinux trial failed; /boot/extlinux.conf was not changed."
    fi
    if [[ ! -s "$candidate" ]]; then
        # update-extlinux removes the candidate when it is byte-identical.
        rm -f -- "$candidate"
        cfg_after="$(repair_file_fingerprint "$TARGET_ROOT/boot/extlinux.conf")"
        if [[ "$cfg_before" == "$cfg_after" ]]; then
            repair_change_status extlinux "unchanged|update-extlinux generated a byte-identical configuration"
        else
            repair_change_status extlinux changed
        fi
        return 0
    fi

    if ! guard_extlinux_candidate_preserves_entries "$TARGET_ROOT/boot/extlinux.conf" "$candidate"; then
        rm -f -- "$candidate"
        extlinux_restore_backup
        fail "extlinux regeneration was rolled back because it removed an existing boot entry."
    fi
    if ! extlinux_verify_candidate "$candidate"; then
        rm -f -- "$candidate"
        extlinux_restore_backup
        fail "extlinux regeneration candidate failed verification; the previous configuration was restored."
    fi
    install -m 0644 "$candidate" "$TARGET_ROOT/boot/extlinux.conf"
    rm -f -- "$candidate"
    if [[ ! -s "$TARGET_ROOT/boot/extlinux.conf" ]]; then
        extlinux_restore_backup
        fail "extlinux regeneration produced no usable /boot/extlinux.conf; the previous configuration was restored."
    fi
    log "PASS: /boot/extlinux.conf regenerated and verified; the boot sector was not written." | tee -a "$SESSION_LOG"
    cfg_after="$(repair_file_fingerprint "$TARGET_ROOT/boot/extlinux.conf")"
    if [[ "$cfg_before" == "$cfg_after" ]]; then
        repair_change_status extlinux "unchanged|extlinux.conf is byte-identical"
    else
        repair_change_status extlinux changed
    fi
}

# Preflight a TUXEDO UKI rebuild: validate the vendor layout, ensure the
# newest kernel has an initramfs (rebuilding it first when missing), require
# enough ESP free space and report whether the embedded UKI kernel is stale.
preflight_tuxedo_uki()
{
    local kver initrd esp_free_kb uki_kb required_kb current=""

    validate_tuxedo_uki_target
    kver="$(newest_tuxedo_kernel)"
    initrd="$TARGET_ROOT/boot/initrd.img-$kver"
    if [[ ! -s "$initrd" ]]; then
        log "KNOWN ISSUE: newest TUXEDO kernel $kver has no matching initramfs; rebuilding initramfs before UKI." | tee -a "$SESSION_LOG"
        adaptive_initramfs_repair
    fi
    [[ -s "$initrd" ]] || fail "Newest TUXEDO kernel $kver still has no matching initramfs after correction."

    esp_free_kb="$(df -Pk "$TARGET_ROOT/boot/efi" 2>/dev/null | awk 'NR==2 {print $4}' | head -1)"
    uki_kb="$(du -Pk "$TARGET_ROOT/boot/efi/EFI/BOOT/TUX.EFI" 2>/dev/null | awk '{print $1}' | head -1 || true)"
    [[ "$uki_kb" =~ ^[0-9]+$ ]] || uki_kb=196608
    required_kb=$((uki_kb + 65536))
    if [[ "$esp_free_kb" =~ ^[0-9]+$ ]] && ((esp_free_kb < required_kb)); then
        fail "EFI System Partition has ${esp_free_kb} KiB free; at least ${required_kb} KiB is required for a safe TUXEDO UKI rebuild."
    fi

    if [[ -s "$TARGET_ROOT/boot/efi/EFI/BOOT/TUX.EFI" ]] && command -v objcopy >/dev/null 2>&1; then
        local tmp="$SESSION_DIR/uki-preflight-uname"
        if objcopy --dump-section ".uname=$tmp" "$TARGET_ROOT/boot/efi/EFI/BOOT/TUX.EFI" >/dev/null 2>&1; then
            current="$(tr '\0' '\n' < "$tmp" | head -1)"
            log "UKI preflight: current embedded kernel=${current:-unknown}; target newest kernel=$kver" | tee -a "$SESSION_LOG"
            if [[ -n "$current" && "$current" != "$kver" ]]; then
                log "KNOWN ISSUE: UKI kernel is stale; vendor UKI rebuild will correct it." | tee -a "$SESSION_LOG"
            fi
        fi
    fi
    log "PASS: TUXEDO UKI preflight prerequisites are satisfied." | tee -a "$SESSION_LOG"
}

# Read-only conventional GRUB EFI preflight: validate the selected ESP and
# grub-install, choose the NVRAM or --no-nvram mode and verify grub-install
# executes inside the target before any file is written.
preflight_generic_efi()
{
    local nvram_mode
    validate_efi_bootloader_target
    if uefi_nvram_writable; then
        nvram_mode="firmware NVRAM registration"
    else
        nvram_mode="--no-nvram file-only reinstall"
    fi
    log "SIMULATE/PREFLIGHT: GRUB EFI install target=$EFI_ESP_SOURCE fs=$EFI_ESP_FSTYPE id=$EFI_BOOTLOADER_ID mode=$nvram_mode" | tee -a "$SESSION_LOG"
    run_chroot_try "Check grub-install availability/version" "$EFI_GRUB_INSTALL_PATH" --version
    ((CHROOT_TRY_RC == 0)) || fail "grub-install is present but could not execute during EFI preflight."
}

# Open the selected LUKS component with the installed-system `luks-<UUID>`
# mapper name.  The passphrase is read from stdin only; an existing mapping of
# the same device is reused and the opened mapping is kept for the session.
unlock_target()
{
    local fstype uuid mapper_name mapper_path existing_mapper

    need lsblk
    need findmnt
    need cryptsetup
    need readlink

    TARGET_DISK="$(canonical_block "$TARGET_DISK")" || fail "Target disk is not a block device."
    ROOT_DEVICE="$(canonical_block "$ROOT_DEVICE")" || fail "LUKS component is not a block device."

    assert_target_not_host "$TARGET_DISK"
    same_single_top_disk "$TARGET_DISK" "$ROOT_DEVICE" \
        || fail "Selected encrypted component does not belong exclusively to the target disk."

    fstype="$(lsblk -ndo FSTYPE "$ROOT_DEVICE" 2>/dev/null | head -n1)"
    if [[ "$fstype" != "crypto_LUKS" ]] && ! cryptsetup isLuks "$ROOT_DEVICE" >/dev/null 2>&1; then
        fail "Selected component is not a LUKS container: $ROOT_DEVICE"
    fi

    uuid="$(cryptsetup luksUUID "$ROOT_DEVICE" 2>/dev/null || true)"
    [[ -n "$uuid" ]] || fail "Unable to determine the LUKS UUID."

    # Debian/TUXEDO crypttab convention uses luks-<UUID>.  Opening with the
    # installed-system style name from the beginning prevents a repair chroot
    # from later seeing a stale boot-repair-* mount source that grub-probe or
    # cryptsetup-initramfs cannot resolve.
    mapper_name="luks-$uuid"
    mapper_path="/dev/mapper/$mapper_name"

    # Reuse any existing dm-crypt mapping of this exact LUKS device.  This is
    # important when another recovery utility or an earlier shell already
    # opened it; Boot Bitch must not create a second mapping or close one it
    # does not own.
    existing_mapper="$(find_crypt_mapper_for_device "$ROOT_DEVICE" 2>/dev/null || true)"
    if [[ -n "$existing_mapper" ]]; then
        log "LUKS target is already unlocked by existing mapper: $existing_mapper"
        printf 'UNLOCKED=%s\n' "$existing_mapper"
        return 0
    fi

    if [[ -e "$mapper_path" || -L "$mapper_path" ]]; then
        fail "Mapper name collision at $mapper_path; refusing to replace an existing mapping."
    fi

    log "Protected host check: PASS"
    log "Unlocking LUKS target $ROOT_DEVICE"
    log "Mapper name: $mapper_name"

    # --key-file - consumes the exact bytes from stdin. The GUI closes stdin
    # immediately after writing the passphrase, so no trailing newline is added.
    # cryptsetup exit code 2 is its documented "no permission" result, which
    # includes an incorrect LUKS passphrase. Emit a machine-readable marker so
    # the GUI can offer a passphrase retry without re-running Polkit auth.
    set +e
    cryptsetup open --type luks --key-file - "$ROOT_DEVICE" "$mapper_name"
    crypt_rc=$?
    set -e
    if (( crypt_rc != 0 )); then
        if (( crypt_rc == 2 )); then
            printf 'UNLOCK_AUTH_FAILED=1\n' >&2
            fail "LUKS passphrase was not accepted."
        fi
        fail "cryptsetup could not unlock the selected LUKS target (exit code $crypt_rc)."
    fi

    [[ -b "$mapper_path" ]] || {
        cryptsetup close "$mapper_name" >/dev/null 2>&1 || true
        fail "cryptsetup reported success but the mapper device did not appear."
    }

    log "LUKS unlock complete. The mapper remains open for this recovery session."
    printf 'UNLOCKED=%s\n' "$mapper_path"
}

# ---------------------------------------------------------------------------
# Read-only diagnostics and repair capability evidence
# ---------------------------------------------------------------------------
diagnostic_title()
{
    case "$1" in
        environment) printf '%s\n' "Environment validation" ;;
        backend)     printf '%s\n' "Distribution and boot backend profile" ;;
        boot)        printf '%s\n' "Boot diagnostics" ;;
        boot-evidence) printf '%s\n' "Boot evidence and selection history" ;;
        kernel)      printf '%s\n' "Kernel / initramfs" ;;
        grub)        printf '%s\n' "GRUB configuration" ;;
        uki)         printf '%s\n' "EFI / UKI boot state" ;;
        display)     printf '%s\n' "Graphical login / display manager" ;;
        errors)      printf '%s\n' "Boot errors" ;;
        usage)       printf '%s\n' "Disk usage" ;;
        fstab)       printf '%s\n' "fstab" ;;
        btrfs)       printf '%s\n' "Btrfs status" ;;
        mapper)      printf '%s\n' "Mapper status" ;;
        luks)        printf '%s\n' "LUKS / crypttab" ;;
        *)           printf '%s\n' "$1" ;;
    esac
}

diagnostic_environment()
{
    local fstype scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    fstype="$(lsblk -ndo FSTYPE "$ROOT_DEVICE" 2>/dev/null | head -n1 || true)"
    printf 'System: %s\n' "$TARGET_PRETTY"
    printf 'Physical drive: %s\n' "$TARGET_DISK"
    printf 'Detected component: %s\n' "$ROOT_DEVICE"
    printf 'Root subvolume: %s\n' "${TARGET_SUBVOL:-none}"
    printf 'Filesystem: %s\n' "${fstype:-unknown}"
    printf '%s root mount source: %s\n' "${scope_label^}" "$(findmnt -rn -o SOURCE --target "$TARGET_ROOT" 2>/dev/null | head -n1 || echo unknown)"
    printf '%s root mount options: %s\n' "${scope_label^}" "$(findmnt -rn -o OPTIONS --target "$TARGET_ROOT" 2>/dev/null | head -n1 || echo unknown)"
    printf 'Inspection mode: read-only\n'
    printf '/etc/os-release: %s\n' "$([[ -f "$TARGET_ROOT/etc/os-release" ]] && echo present || echo missing)"
    printf '/etc/fstab: %s\n' "$([[ -s "$TARGET_ROOT/etc/fstab" ]] && echo present || echo missing/empty)"
    printf '/etc/crypttab: %s\n' "$([[ -s "$TARGET_ROOT/etc/crypttab" ]] && echo present || echo missing/empty)"
    printf '/boot: %s\n' "$([[ -d "$TARGET_ROOT/boot" ]] && echo present || echo missing)"
    printf '/boot/efi: %s\n' "$([[ -d "$TARGET_ROOT/boot/efi" ]] && echo present || echo missing)"
    printf '/efi: %s\n' "$([[ -d "$TARGET_ROOT/efi" ]] && echo present || echo missing)"
}

diagnostic_backend_profile()
{
    local scope_label="target" native_manager="" backend guarded_note="" unsupported_note=""
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    profile_target_backends

    echo "${scope_label^} backend profile (read-only):"
    echo "Distribution ID: ${TARGET_OS_ID:-unknown}"
    echo "Distribution family: ${TARGET_DISTRO_FAMILY:-unknown}"
    echo "Distribution name: ${TARGET_PRETTY:-unknown}"
    echo "Package manager backend: ${TARGET_PACKAGE_MANAGER:-unknown}"
    if ((${#TARGET_PACKAGE_MANAGERS[@]} > 0)); then
        echo "Package manager backends: $(join_comma "${TARGET_PACKAGE_MANAGERS[@]}")"
    else
        echo "Package manager backends: none"
    fi
    echo "Service manager: ${TARGET_SERVICE_MANAGER:-unknown}"
    echo "Display manager backend: ${TARGET_DISPLAY_BACKEND:-none}"
    echo "Initramfs backend: ${TARGET_INITRAMFS_BACKEND:-unknown}"
    if ((${#TARGET_INITRAMFS_BACKENDS[@]} > 0)); then
        echo "Initramfs backends: $(join_comma "${TARGET_INITRAMFS_BACKENDS[@]}")"
    else
        echo "Initramfs backends: none"
    fi
    echo "Bootloader backend: ${TARGET_BOOTLOADER_BACKEND:-unknown}"
    if [[ "${TARGET_BOOTLOADER_BACKEND:-}" == "grub" ]]; then
        echo "GRUB tools: $(repair_capability_evidence grub)"
    fi
    echo "ESP mount candidate: ${TARGET_ESP_MOUNT:-unresolved}"
    echo "Kernel layout: ${TARGET_KERNEL_LAYOUT:-unknown}"
    echo "Logging backend: ${TARGET_LOGGING_BACKEND:-none}"
    echo "Repair capability: ${TARGET_REPAIR_BACKEND:-unknown}"

    # The policy notes below are wording hints.  Availability is decided by the
    # capability probes, never by these lines.
    case "$TARGET_DISTRO_FAMILY" in
        arch)
            echo "Arch policy: package changes require an explicit full pacman transaction; partial metadata refresh is not treated as a repair."
            ;;
        debian)
            echo "Debian policy: existing guarded APT/dpkg repair backend selected when its stage-specific preflight passes."
            ;;
        fedora)
            echo "Fedora policy: guarded rpm/dnf5 package transactions and dracut initramfs rebuilds are selected when their stage-specific preflights pass."
            ;;
    esac

    if [[ "$TARGET_DISPLAY_BACKEND" == OpenRC ]]; then
        echo "OpenRC display manager: $(alpine_display_manager_present 2>/dev/null || echo 'none detected')"
    elif [[ "$TARGET_DISPLAY_BACKEND" == systemd ]]; then
        echo "Systemd display manager: $(systemd_display_manager_present 2>/dev/null || echo 'none detected')"
    fi
    case "$TARGET_LOGGING_BACKEND" in
        journald) echo "Logging source: persistent systemd journal" ;;
        syslog-ng) echo "Logging source: syslog-ng" ;;
        busybox-syslog) echo "Logging source: BusyBox syslog" ;;
        messages) echo "Logging source: /var/log/messages or /var/log/syslog" ;;
        dmesg) echo "Logging source: kernel ring buffer or saved dmesg" ;;
        *) echo "Logging source: none detected" ;;
    esac

    # A non-native package-manager backend is named explicitly so a mixed
    # system (for example Debian with apk installed) is never a surprise.
    # Managers without a guarded transaction implementation are reported
    # separately instead of claiming they participate in repairs.
    native_manager="$(native_package_manager_for_family)"
    local guarded_note="" unsupported_note=""
    if ((${#TARGET_PACKAGE_MANAGERS[@]} > 0)); then
        for backend in "${TARGET_PACKAGE_MANAGERS[@]}"; do
            [[ "$backend" == "$native_manager" ]] && continue
            case "$backend" in
                apt/dpkg|apk|pacman|rpm) guarded_note+="${guarded_note:+; }$backend" ;;
                *) unsupported_note+="${unsupported_note:+; }$backend" ;;
            esac
        done
    fi
    if [[ -n "$guarded_note" ]]; then
        echo "Backend note: ${TARGET_DISTRO_FAMILY^}-family system with $guarded_note detected — package stages run every detected backend (${native_manager} first, then $guarded_note)."
    fi
    if [[ -n "$unsupported_note" ]]; then
        echo "Backend note: $unsupported_note detected; no guarded package transaction backend is implemented for it, so package stages use the implemented backends only."
    fi

    if [[ "$TARGET_DISTRO_FAMILY" == alpine ]]; then
        echo "Alpine status: guarded repairs are selected per detected backend; EFI/GRUB availability follows the detected bootloader backend."
    fi
}

# Read-only systemd display-manager probe: print the configured (or single
# installed supported) display-manager unit name, or return 1 when none is
# present.  Used by capability evidence, not by the repair path.  The probe is
# service-manager evidence, so it is shared by every systemd target instead of
# being tied to one distribution family.
systemd_display_manager_present()
{
    local link service dir candidate
    local -a units=(sddm.service gdm.service gdm3.service lightdm.service greetd.service ly.service)
    local -a dirs=(usr/lib/systemd/system lib/systemd/system etc/systemd/system)

    if [[ -L "$TARGET_ROOT/etc/systemd/system/display-manager.service" ]]; then
        link="$(readlink "$TARGET_ROOT/etc/systemd/system/display-manager.service" 2>/dev/null || true)"
        service="$(basename -- "$link" 2>/dev/null || true)"
        [[ "$service" =~ ^[[:alnum:]_.@:+-]+\.service$ ]] || return 1
        for dir in "${dirs[@]}"; do
            if [[ -f "$TARGET_ROOT/$dir/$service" ]]; then
                printf '%s\n' "$service"
                return 0
            fi
        done
        return 1
    fi

    for candidate in "${units[@]}"; do
        for dir in "${dirs[@]}"; do
            if [[ -f "$TARGET_ROOT/$dir/$candidate" ]]; then
                printf '%s\n' "$candidate"
                return 0
            fi
        done
    done
    return 1
}

# Compatibility alias for callers that grew around the Arch-specific name.
arch_display_manager_present()
{
    systemd_display_manager_present
}

# ---------------------------------------------------------------------------
# Capability readiness probes
#
# Each function below mirrors the mandatory preflight of the guarded stage it
# describes.  It returns 0 (printing nothing) when the stage is runnable and
# prints the specific unavailable reason otherwise.  The distribution family
# is only consulted for wording; availability comes from target evidence.
# ---------------------------------------------------------------------------

# Print the detected package-manager backends that can run the given stage, in
# native-first deterministic order.
package_stage_backends()
{
    local stage="$1" backend
    if ((${#TARGET_PACKAGE_MANAGERS[@]} == 0)); then
        profile_target_backends
    fi
    for backend in "${TARGET_PACKAGE_MANAGERS[@]}"; do
        case "$stage:$backend" in
            dpkg-configure:apt/dpkg) printf '%s\n' "$backend" ;;
            fix-broken:apt/dpkg|fix-broken:apk|fix-broken:pacman|fix-broken:rpm) printf '%s\n' "$backend" ;;
            apt-update:apt/dpkg|apt-update:rpm) printf '%s\n' "$backend" ;;
            apt-upgrade:apt/dpkg|apt-upgrade:apk|apt-upgrade:pacman|apt-upgrade:rpm) printf '%s\n' "$backend" ;;
        esac
    done
    return 0
}

# Read-only per-backend prerequisite reason.  Prints nothing and returns 0 when
# the backend can run the stage; prints the reason and returns 1 otherwise.
package_backend_unavailable_reason()
{
    local stage="$1" backend="$2"
    case "$backend" in
        apt/dpkg)
            if ! target_has_executable /usr/bin/dpkg /usr/sbin/dpkg /bin/dpkg; then
                printf 'dpkg is not installed in the target'
                return 1
            fi
            if ! target_has_executable /usr/bin/apt-get /usr/sbin/apt-get /bin/apt-get; then
                printf 'apt-get is not installed in the target'
                return 1
            fi
            return 0
            ;;
        apk)
            if ! target_has_executable /sbin/apk /usr/sbin/apk /usr/bin/apk; then
                printf 'apk is not installed in the target'
            elif [[ ! -s "$TARGET_ROOT/etc/apk/repositories" ]] \
                || ! grep -Eq '^[[:space:]]*[^#[:space:]]' "$TARGET_ROOT/etc/apk/repositories" 2>/dev/null; then
                printf 'The target has no configured apk repositories'
            elif [[ ! -f "$TARGET_ROOT/lib/apk/db/installed" ]]; then
                printf 'The target apk installed database is missing'
            elif [[ ! -f "$TARGET_ROOT/etc/apk/world" ]]; then
                printf 'The target apk world file is missing'
            else
                return 0
            fi
            return 1
            ;;
        pacman)
            if [[ ! -x "$TARGET_ROOT/usr/bin/pacman" && ! -x "$TARGET_ROOT/usr/bin/pacman-static" ]]; then
                printf 'pacman is not installed in the target'
            elif [[ ! -f "$TARGET_ROOT/etc/pacman.conf" ]]; then
                printf 'The target has no /etc/pacman.conf; refusing a package transaction'
            elif [[ ! -d "$TARGET_ROOT/var/lib/pacman" ]]; then
                printf 'The target pacman database directory is missing'
            else
                return 0
            fi
            return 1
            ;;
        rpm)
            if ! target_has_executable /usr/bin/rpm /bin/rpm /usr/sbin/rpm; then
                printf 'rpm is not installed in the target system'
            elif ! rpm_dnf_tool >/dev/null; then
                if rpm_dnf4_present; then
                    printf 'dnf4 is not supported by the guarded rpm backend'
                else
                    printf 'dnf5 is not installed in the target system'
                fi
            elif ! rpm_database_present; then
                printf 'the target RPM database is missing'
            elif ! rpm_repositories_present; then
                printf 'The target has no enabled dnf repositories'
            else
                return 0
            fi
            return 1
            ;;
    esac
    printf 'backend %s has no guarded implementation for stage %s' "$backend" "$stage"
    return 1
}

# Combined reason for a package stage: runnable when at least one detected
# backend is ready; otherwise every applicable backend's reason is listed so
# the user sees exactly which detected manager blocks the stage.
package_stage_unavailable_reason()
{
    local stage="$1" backend reason reasons_list=""
    local -a applicable=()
    mapfile -t applicable < <(package_stage_backends "$stage")
    if ((${#applicable[@]} == 0)); then
        printf 'No guarded package-manager backend was detected (detected: %s)' "${TARGET_PACKAGE_MANAGERS[*]:-none}"
        return 1
    fi
    if ((${#applicable[@]} == 1)); then
        # Single-backend targets keep the exact backend-specific reason.
        package_backend_unavailable_reason "$stage" "${applicable[0]}"
        return $?
    fi
    for backend in "${applicable[@]}"; do
        if reason="$(package_backend_unavailable_reason "$stage" "$backend")"; then
            return 0
        fi
        reasons_list+="${reasons_list:+; }$backend: $reason"
    done
    printf '%s' "$reasons_list"
    return 1
}

dpkg_unavailable_reason()
{
    if target_dpkg_ready; then
        return 0
    fi
    if [[ "$TARGET_DISTRO_FAMILY" == arch ]]; then
        printf 'dpkg configuration is not available on Arch; use the Arch package transaction stages instead'
    elif [[ "$TARGET_DISTRO_FAMILY" == alpine ]]; then
        printf 'Alpine uses apk; dpkg configuration is not available on Alpine'
    elif [[ "$TARGET_DISTRO_FAMILY" == fedora ]]; then
        printf 'Fedora uses rpm/dnf; dpkg configuration is not available'
    elif [[ ! -f "$TARGET_ROOT/var/lib/dpkg/status" ]]; then
        printf 'dpkg is not installed in the target'
    else
        printf 'The target dpkg database or executable is incomplete'
    fi
    return 1
}

aptupdate_unavailable_reason()
{
    if target_has_executable /usr/bin/apt-get /usr/sbin/apt-get /bin/apt-get \
        && target_apt_sources_present; then
        return 0
    fi
    # Fedora's metadata refresh equivalent is `dnf5 makecache`; it needs dnf5
    # and at least one enabled repository, not the sqlite rpmdb.
    if rpm_dnf_tool >/dev/null && rpm_repositories_present; then
        return 0
    fi
    if ! target_has_executable /usr/bin/apt-get /usr/sbin/apt-get /bin/apt-get; then
        case "$TARGET_DISTRO_FAMILY" in
            arch) printf 'Standalone APT metadata refresh is not available on Arch; use Upgrade installed packages for one full pacman transaction' ;;
            alpine) printf 'Standalone APK metadata refresh is not available on Alpine; use Upgrade installed packages for one guarded apk transaction' ;;
            fedora)
                if rpm_dnf_tool >/dev/null; then
                    printf 'The target has no enabled dnf repositories to refresh'
                else
                    printf 'dnf5 is not installed in the target'
                fi
                ;;
            *) printf 'apt-get is not installed in the target' ;;
        esac
    else
        printf 'The target has no configured APT sources to refresh'
    fi
    return 1
}

display_unavailable_reason()
{
    local backend dm_unit
    backend="$(target_display_manager_backend)"
    case "$backend" in
        systemd)
            if [[ ! -f "$TARGET_ROOT/usr/lib/systemd/system/graphical.target" \
                && ! -f "$TARGET_ROOT/lib/systemd/system/graphical.target" ]]; then
                printf 'graphical.target is missing from the target'
                return 1
            fi
            dm_unit="$(systemd_display_manager_present 2>/dev/null || true)"
            if [[ -z "$dm_unit" && ! -L "$TARGET_ROOT/etc/systemd/system/display-manager.service" ]]; then
                printf 'No supported display manager unit is installed in the target'
                return 1
            fi
            return 0
            ;;
        OpenRC)
            if [[ -z "$(alpine_display_manager_services)" \
                && -z "$(alpine_display_manager_present 2>/dev/null || true)" ]]; then
                printf 'No supported OpenRC display manager service is installed in the target'
                return 1
            fi
            return 0
            ;;
        sysvinit)
            printf 'A sysvinit display-manager script was detected; offline repair is not implemented for sysvinit'
            return 1
            ;;
        *)
            if [[ "$TARGET_SERVICE_MANAGER" == unknown ]]; then
                printf 'No supported service manager (systemd or OpenRC) was detected in the target'
            else
                printf 'No supported display manager was detected in the target'
            fi
            return 1
            ;;
    esac
}

initramfs_unavailable_reason()
{
    local backend="${TARGET_INITRAMFS_BACKEND:-unknown}"
    case "$backend" in
        mkinitfs)
            if ! target_has_executable /sbin/mkinitfs /usr/sbin/mkinitfs /usr/bin/mkinitfs; then
                printf 'mkinitfs is not installed in the target system'
            elif ! target_has_path /etc/mkinitfs/mkinitfs.conf /etc/mkinitfs; then
                printf 'The target has no mkinitfs configuration'
            elif [[ -z "$(alpine_kernel_pairs)" ]]; then
                printf 'No installed Alpine kernels were found under target /boot'
            else
                return 0
            fi
            ;;
        mkinitcpio)
            if ! target_has_executable /usr/bin/mkinitcpio /usr/sbin/mkinitcpio; then
                printf 'mkinitcpio is not installed in the target system'
            elif [[ -z "$(arch_kernel_versions)" ]]; then
                printf 'No installed kernel module directories were found for mkinitcpio'
            else
                return 0
            fi
            ;;
        initramfs-tools)
            if target_has_executable /usr/sbin/update-initramfs /usr/bin/update-initramfs \
                && target_has_executable /usr/sbin/mkinitramfs /usr/bin/mkinitramfs; then
                return 0
            fi
            printf 'update-initramfs/mkinitramfs are not installed in the target'
            ;;
        dracut)
            if ! target_has_executable /usr/bin/dracut /usr/sbin/dracut; then
                printf 'dracut is not installed in the target system'
            elif ! target_has_path /usr/lib/dracut; then
                printf 'the dracut generator directory is missing from the target'
            elif ! target_has_executable /usr/bin/lsinitrd /usr/sbin/lsinitrd; then
                printf 'lsinitrd is not installed in the target system; dracut image verification is unavailable'
            elif [[ -z "$(rpm_kernel_pairs_readonly)" ]]; then
                printf 'No installed dracut kernels were found under target /boot'
            else
                return 0
            fi
            ;;
        booster)
            printf 'initramfs backend is booster, which the current repair implementation does not handle'
            ;;
        *)
            printf 'No supported initramfs backend (mkinitfs, mkinitcpio, dracut or initramfs-tools) was detected'
            ;;
    esac
    return 1
}

# EFI availability follows the detected bootloader/backend.  On Alpine the
# detected EFI backend (GRUB EFI, EFI-stub, syslinux-EFI) decides which
# guarded path applies; every missing prerequisite fails closed with its exact
# probe reason.
efi_unavailable_reason()
{
    local esp_root="" backend
    # Self-contained probe: callers may invoke this directly (runtime stage
    # gate, boot-stack prerequisites), so refresh the detected backends here.
    profile_target_backends
    if is_alpine_family; then
        backend="$(alpine_efi_backend)"
        case "$backend" in
            grub)
                if ! alpine_efi_firmware_available; then
                    printf 'GRUB detected on legacy BIOS; no EFI boot path is available'
                    return 1
                fi
                if ! alpine_grub_install_present; then
                    printf 'grub-install is not installed in the Alpine target'
                    return 1
                fi
                if ! target_apk_package_installed grub; then
                    printf 'the grub package is not installed in the Alpine target'
                    return 1
                fi
                if ! target_apk_package_installed grub-efi; then
                    printf 'grub-efi is not installed in the Alpine target'
                    return 1
                fi
                if ! alpine_grub_module_dir_present; then
                    printf 'the x86_64-efi GRUB module directory is missing from the Alpine target'
                    return 1
                fi
                if [[ "${TARGET_ESP_MOUNT:-unresolved}" == unresolved ]]; then
                    printf 'no EFI System Partition candidate on the selected disk'
                    return 1
                fi
                return 0
                ;;
            efi-stub)
                if ! alpine_efi_firmware_available; then
                    printf 'EFI-stub boot requires UEFI firmware; the recovery host booted in legacy BIOS mode'
                    return 1
                fi
                if ! command -v efibootmgr >/dev/null 2>&1; then
                    printf 'EFI-stub entry repair requires efibootmgr in the recovery host'
                    return 1
                fi
                if [[ "${TARGET_ESP_MOUNT:-unresolved}" == unresolved ]]; then
                    printf 'no EFI System Partition candidate on the selected disk'
                    return 1
                fi
                if [[ -z "$(alpine_efi_esp_kernel_images)" ]]; then
                    printf 'no EFI-stub kernel image (vmlinuz-*) is present at the EFI System Partition root'
                    return 1
                fi
                if [[ -z "$(alpine_efi_esp_initramfs_images)" ]]; then
                    printf 'no EFI-stub initramfs image (initramfs-*) is present at the EFI System Partition root'
                    return 1
                fi
                return 0
                ;;
            syslinux-efi)
                printf 'Alpine syslinux-EFI boot detected (EFI/syslinux/syslinux.efi); guarded repair is not implemented'
                return 1
                ;;
            *)
                if [[ "$TARGET_BOOTLOADER_BACKEND" == syslinux/extlinux ]]; then
                    printf 'syslinux/extlinux (BIOS) boot detected; no EFI boot path is available'
                else
                    printf 'no EFI boot path was detected for the Alpine target (bootloader backend %s)' "${TARGET_BOOTLOADER_BACKEND:-unknown}"
                fi
                return 1
                ;;
        esac
    fi
    # A Fedora/RHEL grub2 layout on legacy BIOS has no EFI boot path at all:
    # the chain is MBR/bios_grub -> GRUB2 -> BLS.  This is layout + firmware
    # evidence, not a distribution-family gate.
    if grub2_layout_detected && bios_firmware_mode; then
        printf 'legacy BIOS target; no EFI boot path is available'
        return 1
    fi
    if [[ "$TARGET_OS_ID" == tuxedo ]] && ! tuxedo_uki_builder_present; then
        printf 'TUXEDO UKI builder create_boot_uki_base.sh is not installed in the target'
        return 1
    fi
    if ! grub_generator_tool >/dev/null; then
        if grub2_layout_detected; then
            printf 'grub2-mkconfig is not installed in the target'
        else
            printf 'requires GRUB configuration tooling'
        fi
        return 1
    fi
    if ! grub_install_tool >/dev/null; then
        if grub2_layout_detected; then
            printf 'grub2-install is not installed in the target'
        else
            printf 'grub-install missing'
        fi
        return 1
    fi
    if [[ "$TARGET_DISTRO_FAMILY" == arch ]]; then
        esp_root="$(profile_esp_root 2>/dev/null || true)"
        if [[ -z "$esp_root" ]]; then
            printf 'No EFI System Partition was identified for the selected Arch target'
            return 1
        fi
    fi
    return 0
}

grub_unavailable_reason()
{
    local config generator
    profile_target_backends
    if is_alpine_family && [[ "$TARGET_BOOTLOADER_BACKEND" != grub ]]; then
        printf 'The detected bootloader is %s; GRUB is not the selected bootloader' "${TARGET_BOOTLOADER_BACKEND:-unknown}"
        return 1
    fi
    config="$(grub_config_path)"
    if [[ "$config" == /boot/grub2/grub.cfg ]]; then
        if ! generator="$(grub_generator_tool)" \
            || [[ "$(basename -- "$generator")" != "grub2-mkconfig" ]]; then
            printf 'grub2-mkconfig is not installed in the target'
            return 1
        fi
        [[ -f "$TARGET_ROOT$config" ]] || { printf '%s is missing' "$config"; return 1; }
        grub_env_block_valid \
            || { printf 'grubenv is missing or not a valid GRUB environment block'; return 1; }
        return 0
    fi
    if ! grub_generator_tool >/dev/null; then
        printf 'Neither grub-mkconfig nor update-grub is installed in the target system'
        return 1
    fi
    return 0
}

extlinux_unavailable_reason()
{
    if ! target_has_executable /sbin/update-extlinux /usr/sbin/update-extlinux /usr/bin/update-extlinux; then
        printf 'update-extlinux is not installed in the target'
        return 1
    fi
    if ! target_has_path /boot/extlinux.conf /boot/syslinux/syslinux.cfg /boot/syslinux/ldlinux.sys /etc/update-extlinux.conf; then
        printf 'No extlinux/syslinux configuration was detected in the target'
        return 1
    fi
    if ! package_query_available; then
        printf 'No detected package manager can verify the syslinux package'
        return 1
    fi
    if ! target_package_installed syslinux; then
        printf 'The syslinux package is not installed according to the detected package manager'
        return 1
    fi
    return 0
}

dkms_unavailable_reason()
{
    local kver pair
    if [[ ! -x "$TARGET_ROOT/usr/bin/dkms" && ! -x "$TARGET_ROOT/usr/sbin/dkms" ]]; then
        case "$TARGET_DISTRO_FAMILY" in
            arch) printf 'DKMS is not installed in the Arch target system' ;;
            alpine) printf 'DKMS is not installed in the Alpine target system' ;;
            *) printf 'DKMS is not installed in the target' ;;
        esac
        return 1
    fi
    if [[ "$TARGET_INITRAMFS_BACKEND" == dracut ]]; then
        # Fedora DKMS pairs through the rpm module inventory (the rescue pair
        # is excluded) and needs kernel-devel build trees.  The header
        # correction path (kernel-devel/akmods) is design-only in this
        # milestone, so the capability stays unavailable by policy.
        while IFS= read -r pair; do
            [[ -n "$pair" ]] || continue
            kver="${pair%% *}"
            if ! target_has_path "/lib/modules/$kver/build" "/usr/lib/modules/$kver/build"; then
                printf 'DKMS preflight found no build tree for installed kernel %s; install the matching kernel-devel packages and retry' "$kver"
                return 1
            fi
        done < <(rpm_kernel_pairs_readonly)
        printf 'the rpm DKMS header correction (kernel-devel/akmods) is not implemented'
        return 1
    fi
    if [[ "$TARGET_INITRAMFS_BACKEND" == mkinitfs ]]; then
        while IFS= read -r pair; do
            [[ -n "$pair" ]] || continue
            kver="${pair%% *}"
            if ! target_has_path "/lib/modules/$kver/build" "/usr/lib/modules/$kver/build"; then
                printf 'Alpine DKMS preflight found no build tree for installed kernel %s; install the matching headers and retry' "$kver"
                return 1
            fi
        done < <(alpine_kernel_pairs)
        return 0
    fi
    if [[ "$TARGET_INITRAMFS_BACKEND" == mkinitcpio ]] || target_pacman_detected; then
        while IFS= read -r kver; do
            [[ -n "$kver" ]] || continue
            if ! target_has_path "/lib/modules/$kver/build" "/usr/lib/modules/$kver/build"; then
                printf 'Arch DKMS preflight found no build tree for installed kernel %s; install the matching headers and retry' "$kver"
                return 1
            fi
        done < <(arch_kernel_versions)
        return 0
    fi
    return 0
}

bootstack_unavailable_reason()
{
    local reason
    if is_alpine_family; then
        if [[ "$TARGET_BOOTLOADER_BACKEND" == syslinux/extlinux ]]; then
            printf 'Alpine uses OpenRC, mkinitfs and syslinux/extlinux; boot-stack reconciliation is not enabled (run the initramfs and extlinux stages separately)'
        else
            printf 'Alpine boot-stack reconciliation is not enabled (run the initramfs and GRUB stages separately)'
        fi
        return 1
    fi
    if ! reason="$(initramfs_unavailable_reason)"; then
        printf 'Requires available initramfs and GRUB repair prerequisites'
        return 1
    fi
    if ! reason="$(grub_unavailable_reason)"; then
        printf 'Requires available initramfs and GRUB repair prerequisites'
        return 1
    fi
    if [[ "$TARGET_DISTRO_FAMILY" == arch ]]; then
        if ! reason="$(efi_unavailable_reason)"; then
            printf 'Arch boot-stack reconciliation requires available initramfs, GRUB and EFI repair prerequisites'
            return 1
        fi
    fi
    return 0
}

# Read-only evidence for one detected package-manager backend.
package_backend_evidence()
{
    local backend="$1"
    case "$backend" in
        apt/dpkg)
            if target_has_executable /usr/bin/dpkg /usr/sbin/dpkg /bin/dpkg \
                && target_has_executable /usr/bin/apt-get /usr/sbin/apt-get /bin/apt-get; then
                printf '/usr/bin/dpkg and /usr/bin/apt-get present'
            elif target_has_executable /usr/bin/dpkg /usr/sbin/dpkg /bin/dpkg; then
                printf '/usr/bin/apt-get missing'
            else
                printf '/usr/bin/dpkg missing'
            fi
            ;;
        apk)
            if ! target_has_executable /sbin/apk /usr/sbin/apk /usr/bin/apk; then
                printf 'apk executable missing'
            elif [[ ! -s "$TARGET_ROOT/etc/apk/repositories" ]] \
                || ! grep -Eq '^[[:space:]]*[^#[:space:]]' "$TARGET_ROOT/etc/apk/repositories" 2>/dev/null; then
                printf 'apk executable present; /etc/apk/repositories missing or empty'
            elif [[ ! -f "$TARGET_ROOT/lib/apk/db/installed" ]]; then
                printf 'apk executable present; /etc/apk/repositories present; /lib/apk/db/installed missing'
            elif [[ ! -f "$TARGET_ROOT/etc/apk/world" ]]; then
                printf 'apk executable present; /etc/apk/repositories present; /lib/apk/db/installed present; /etc/apk/world missing'
            else
                printf 'apk executable present; /etc/apk/repositories present; /lib/apk/db/installed present; /etc/apk/world present'
                if alpine_apk_lock_held; then
                    printf '; apk database lock is held by another process'
                fi
            fi
            ;;
        pacman)
            if [[ ! -x "$TARGET_ROOT/usr/bin/pacman" && ! -x "$TARGET_ROOT/usr/bin/pacman-static" ]]; then
                printf 'pacman executable missing'
            elif [[ ! -f "$TARGET_ROOT/etc/pacman.conf" ]]; then
                printf 'pacman executable present; /etc/pacman.conf missing'
            elif [[ ! -d "$TARGET_ROOT/var/lib/pacman" ]]; then
                printf 'pacman executable present; /etc/pacman.conf present; /var/lib/pacman missing'
            else
                printf 'pacman executable present; /etc/pacman.conf present; /var/lib/pacman present'
            fi
            ;;
        rpm)
            if ! target_has_executable /usr/bin/rpm /bin/rpm /usr/sbin/rpm; then
                printf 'rpm executable missing'
            elif ! rpm_dnf_tool >/dev/null; then
                if rpm_dnf4_present; then
                    printf 'rpm executable present; dnf5 missing (dnf4 is not supported)'
                else
                    printf 'rpm executable present; dnf5 missing'
                fi
            elif ! rpm_database_present; then
                printf 'rpm and dnf5 present; sqlite RPM database missing'
            elif ! rpm_repositories_present; then
                printf 'rpm and dnf5 present; sqlite RPM database present; no enabled dnf repositories'
            else
                printf 'rpm and dnf5 present; sqlite RPM database present; enabled dnf repositories: %s' "$(rpm_enabled_repo_count)"
                if rpm_lock_held; then
                    printf '; RPM database lock is held by another process'
                fi
            fi
            ;;
        *)
            printf 'backend %s evidence unavailable' "$backend"
            ;;
    esac
}

# Emit the stable one-line read-only evidence string for one capability key
# (validate, filesystem, dpkg, fixbroken, aptupdate, upgrade, dkms, display,
# initramfs, efi, grub, extlinux, bootstack).  Consumed by the GUI and tests.
repair_capability_evidence()
{
    local key="$1" dm_unit kernel_list image_list esp_root detail kver backend
    local esp_source efi_id loader_path
    local -a applicable=()

    case "$key" in
        validate)
            printf 'read-only preflight always available'
            ;;
        dpkg)
            if target_dpkg_ready; then
                printf '/usr/bin/dpkg and /var/lib/dpkg/status present'
            elif [[ ! -f "$TARGET_ROOT/var/lib/dpkg/status" ]]; then
                printf '/usr/bin/dpkg missing'
            else
                printf '/usr/bin/dpkg present; /var/lib/dpkg/status missing'
            fi
            ;;
        aptupdate)
            if target_has_executable /usr/bin/apt-get /usr/sbin/apt-get /bin/apt-get; then
                if target_apt_sources_present; then
                    printf '/usr/bin/apt-get present; APT sources present'
                else
                    printf '/usr/bin/apt-get present; no APT sources configured'
                fi
            elif rpm_dnf_tool >/dev/null; then
                if rpm_repositories_present; then
                    printf 'dnf5 present; enabled dnf repositories: %s' "$(rpm_enabled_repo_count)"
                else
                    printf 'dnf5 present; no enabled dnf repositories'
                fi
            else
                printf '/usr/bin/apt-get missing'
            fi
            ;;
        fixbroken|upgrade)
            mapfile -t applicable < <(package_stage_backends "$([[ "$key" == fixbroken ]] && printf 'fix-broken' || printf 'apt-upgrade')")
            if ((${#applicable[@]} == 0)); then
                printf 'no guarded package-manager backend detected'
            elif ((${#applicable[@]} == 1)); then
                package_backend_evidence "${applicable[0]}"
            else
                detail=""
                for backend in "${applicable[@]}"; do
                    detail+="${detail:+; }$backend: $(package_backend_evidence "$backend")"
                done
                printf '%s' "$detail"
            fi
            ;;
        dkms)
            if [[ ! -x "$TARGET_ROOT/usr/bin/dkms" && ! -x "$TARGET_ROOT/usr/sbin/dkms" ]]; then
                printf 'dkms executable missing'
            else
                detail="dkms executable present"
                if [[ "$TARGET_INITRAMFS_BACKEND" == mkinitfs ]]; then
                    while IFS= read -r kver; do
                        [[ -n "$kver" ]] || continue
                        if target_has_path "/lib/modules/$kver/build" "/usr/lib/modules/$kver/build"; then
                            detail+="; kernel $kver build tree present"
                        else
                            detail+="; kernel $kver build tree missing"
                        fi
                    done < <(alpine_kernel_pairs | awk '{print $1}')
                else
                    while IFS= read -r kver; do
                        [[ -n "$kver" ]] || continue
                        if target_has_path "/lib/modules/$kver/build" "/usr/lib/modules/$kver/build"; then
                            detail+="; kernel $kver build tree present"
                        else
                            detail+="; kernel $kver build tree missing"
                        fi
                    done < <(arch_kernel_versions)
                fi
                printf '%s' "$detail"
            fi
            ;;
        display)
            case "$(target_display_manager_backend)" in
                systemd)
                    if [[ ! -f "$TARGET_ROOT/usr/lib/systemd/system/graphical.target" \
                        && ! -f "$TARGET_ROOT/lib/systemd/system/graphical.target" ]]; then
                        printf 'graphical.target missing'
                    else
                        dm_unit="$(systemd_display_manager_present 2>/dev/null || true)"
                        if [[ -n "$dm_unit" ]]; then
                            printf 'graphical.target present; display manager unit %s' "$dm_unit"
                        else
                            printf 'graphical.target present; no supported display manager unit'
                        fi
                    fi
                    ;;
                OpenRC)
                    dm_unit="$(alpine_display_manager_present 2>/dev/null || true)"
                    if [[ -n "$dm_unit" ]]; then
                        printf 'OpenRC display-manager service %s present' "$dm_unit"
                    elif openrc_present; then
                        printf 'OpenRC present; no supported display manager service'
                    else
                        printf 'OpenRC not detected; no supported display manager service'
                    fi
                    ;;
                sysvinit)
                    printf 'sysvinit display-manager script detected; offline repair is not implemented'
                    ;;
                *)
                    printf 'no supported display-manager backend detected'
                    ;;
            esac
            ;;
        initramfs)
            case "${TARGET_INITRAMFS_BACKEND:-unknown}" in
                mkinitfs)
                    kernel_list="$(alpine_kernel_images | paste -sd' ' -)"
                    image_list="$(alpine_initramfs_images | paste -sd' ' -)"
                    if ! target_has_executable /sbin/mkinitfs /usr/sbin/mkinitfs /usr/bin/mkinitfs \
                        && ! target_has_path /etc/mkinitfs; then
                        printf 'Alpine mkinitfs backend; mkinitfs executable and /etc/mkinitfs missing'
                    else
                        printf 'Alpine mkinitfs backend; kernels: %s; initramfs images: %s' \
                            "${kernel_list:-none}" "${image_list:-none}"
                    fi
                    ;;
                mkinitcpio)
                    kernel_list="$(arch_kernel_versions | paste -sd' ' -)"
                    if ! target_has_executable /usr/bin/mkinitcpio /usr/sbin/mkinitcpio; then
                        printf 'mkinitcpio executable missing'
                    elif [[ -z "$kernel_list" ]]; then
                        printf 'mkinitcpio executable present; no kernel module directories'
                    else
                        printf 'mkinitcpio executable present; kernel module directories: %s' "$kernel_list"
                    fi
                    ;;
                dracut)
                    if ! target_has_executable /usr/bin/dracut /usr/sbin/dracut; then
                        printf 'initramfs backend: dracut; dracut executable missing'
                    elif ! target_has_path /usr/lib/dracut; then
                        printf 'initramfs backend: dracut; dracut executable present; /usr/lib/dracut missing'
                    elif ! target_has_executable /usr/bin/lsinitrd /usr/sbin/lsinitrd; then
                        printf 'initramfs backend: dracut; dracut executable and /usr/lib/dracut present; lsinitrd missing'
                    else
                        kernel_list="$(rpm_kernel_pairs_readonly | awk '{print $1}' | paste -sd' ' -)"
                        image_list="$(rpm_kernel_pairs_readonly | awk '{print $3}' | sed 's#^/boot/##' | paste -sd' ' -)"
                        printf 'initramfs backend: dracut; dracut executable and /usr/lib/dracut present; kernels: %s; images: %s' \
                            "${kernel_list:-none}" "${image_list:-none}"
                    fi
                    ;;
                initramfs-tools)
                    if target_has_executable /usr/sbin/update-initramfs /usr/bin/update-initramfs \
                        && target_has_executable /usr/sbin/mkinitramfs /usr/bin/mkinitramfs; then
                        printf 'initramfs backend: initramfs-tools; update-initramfs and mkinitramfs present'
                    else
                        printf 'initramfs backend: initramfs-tools; update-initramfs or mkinitramfs missing'
                    fi
                    ;;
                *)
                    printf 'initramfs backend: %s; no guarded repair implementation' "${TARGET_INITRAMFS_BACKEND:-unknown}"
                    ;;
            esac
            ;;
        grub)
            if [[ "$TARGET_BOOTLOADER_BACKEND" == syslinux/extlinux ]]; then
                printf '%s is the selected bootloader; GRUB is not detected' "$TARGET_BOOTLOADER_BACKEND"
            elif grub2_layout_detected; then
                detail=""
                if [[ -x "$TARGET_ROOT/usr/sbin/grub2-mkconfig" || -x "$TARGET_ROOT/usr/bin/grub2-mkconfig" ]]; then
                    detail="grub2-mkconfig present"
                else
                    detail="grub2-mkconfig missing"
                fi
                if [[ -f "$TARGET_ROOT/boot/grub2/grub.cfg" ]]; then
                    detail+="; /boot/grub2/grub.cfg present"
                else
                    detail+="; /boot/grub2/grub.cfg missing"
                fi
                if grub_env_block_valid; then
                    detail+="; grubenv present"
                else
                    detail+="; grubenv missing or invalid"
                fi
                if [[ -n "$(fedora_bios_grub_partition 2>/dev/null || true)" ]]; then
                    detail+="; BIOS boot partition present"
                else
                    detail+="; BIOS boot partition missing"
                fi
                printf '%s' "$detail"
            elif is_alpine_family && [[ "$TARGET_BOOTLOADER_BACKEND" == grub ]]; then
                detail=""
                if [[ -x "$TARGET_ROOT/usr/sbin/grub-mkconfig" || -x "$TARGET_ROOT/usr/bin/grub-mkconfig" ]]; then
                    detail="grub-mkconfig present"
                elif [[ -x "$TARGET_ROOT/usr/sbin/update-grub" || -x "$TARGET_ROOT/usr/bin/update-grub" ]]; then
                    detail="update-grub present"
                else
                    detail="grub-mkconfig and update-grub missing"
                fi
                if [[ -f "$TARGET_ROOT/boot/grub/grub.cfg" ]]; then
                    detail+="; /boot/grub/grub.cfg present"
                else
                    detail+="; /boot/grub/grub.cfg missing"
                fi
                printf '%s' "$detail"
            elif [[ -x "$TARGET_ROOT/usr/sbin/grub-mkconfig" || -x "$TARGET_ROOT/usr/bin/grub-mkconfig" ]]; then
                printf 'grub-mkconfig present'
            elif [[ -x "$TARGET_ROOT/usr/sbin/update-grub" || -x "$TARGET_ROOT/usr/bin/update-grub" ]]; then
                printf 'update-grub present'
            else
                printf 'grub-mkconfig and update-grub missing'
            fi
            ;;
        extlinux)
            if ! target_has_executable /sbin/update-extlinux /usr/sbin/update-extlinux /usr/bin/update-extlinux; then
                printf 'update-extlinux is not installed in the target'
            else
                detail="update-extlinux present"
                if [[ -f "$TARGET_ROOT/boot/extlinux.conf" ]]; then
                    detail+="; /boot/extlinux.conf present"
                elif [[ -f "$TARGET_ROOT/boot/syslinux/syslinux.cfg" ]]; then
                    detail+="; /boot/syslinux/syslinux.cfg present"
                elif [[ -f "$TARGET_ROOT/boot/syslinux/ldlinux.sys" ]]; then
                    detail+="; /boot/syslinux/ldlinux.sys present"
                elif [[ -f "$TARGET_ROOT/etc/update-extlinux.conf" ]]; then
                    detail+="; /etc/update-extlinux.conf present"
                else
                    detail+="; no extlinux/syslinux configuration"
                fi
                printf '%s' "$detail"
            fi
            ;;
        efi)
            if is_alpine_family; then
                case "$(alpine_efi_backend)" in
                    grub)
                        if ! alpine_efi_firmware_available; then
                            printf 'Alpine GRUB backend; firmware booted in legacy BIOS mode; no EFI boot path'
                        else
                            detail="Alpine GRUB EFI backend"
                            if alpine_grub_install_present; then
                                detail+="; grub-install present"
                            else
                                detail+="; grub-install missing"
                            fi
                            if target_apk_package_installed grub && target_apk_package_installed grub-efi; then
                                detail+="; grub and grub-efi packages installed"
                            else
                                detail+="; grub or grub-efi package missing"
                            fi
                            if alpine_grub_module_dir_present; then
                                detail+="; x86_64-efi module directory present"
                            else
                                detail+="; x86_64-efi module directory missing"
                            fi
                            if [[ "${TARGET_ESP_MOUNT:-unresolved}" != unresolved ]]; then
                                detail+="; ESP candidate: $TARGET_ESP_MOUNT"
                                esp_source="$(findmnt -rn -o SOURCE --target "$(target_path "$TARGET_ESP_MOUNT")" 2>/dev/null \
                                    | awk '$1 ~ /^\/dev\// {print $1; exit}')"
                                [[ -n "$esp_source" ]] && detail+="; ESP device: $esp_source"
                                efi_id="$(detect_efi_bootloader_id 2>/dev/null || true)"
                                if [[ -n "$efi_id" ]]; then
                                    loader_path="$(find "$(target_path "$TARGET_ESP_MOUNT")/EFI/$efi_id" -maxdepth 1 \
                                        -type f -iname 'grub*.efi' -print -quit 2>/dev/null || true)"
                                    # ${TARGET_ROOT%/} keeps the leading slash in running-host mode (TARGET_ROOT=/).
                                    [[ -n "$loader_path" ]] && detail+="; loader: ${loader_path#"${TARGET_ROOT%/}"}"
                                fi
                            else
                                detail+="; no EFI System Partition candidate"
                            fi
                            printf '%s' "$detail"
                        fi
                        ;;
                    efi-stub)
                        detail="Alpine EFI-stub backend"
                        detail+="; ESP candidate: ${TARGET_ESP_MOUNT:-unresolved}"
                        detail+="; kernel images: $(alpine_efi_esp_kernel_images | paste -sd' ' -)"
                        detail+="; initramfs images: $(alpine_efi_esp_initramfs_images | paste -sd' ' -)"
                        if command -v efibootmgr >/dev/null 2>&1; then
                            detail+="; efibootmgr present"
                        else
                            detail+="; efibootmgr missing"
                        fi
                        printf '%s' "$detail"
                        ;;
                    syslinux-efi)
                        printf 'Alpine syslinux-EFI backend; ESP candidate: %s; detected EFI/syslinux/syslinux.efi; guarded repair is not implemented' "${TARGET_ESP_MOUNT:-unresolved}"
                        ;;
                    *)
                        if [[ "$TARGET_BOOTLOADER_BACKEND" == syslinux/extlinux ]]; then
                            printf 'Alpine syslinux/extlinux (BIOS) backend; no EFI boot path'
                        else
                            printf 'Alpine bootloader backend %s; no EFI boot path detected' "${TARGET_BOOTLOADER_BACKEND:-unknown}"
                        fi
                        ;;
                esac
            elif grub2_layout_detected && bios_firmware_mode; then
                printf 'legacy BIOS target; no EFI boot path is available'
            elif [[ "$TARGET_OS_ID" == tuxedo ]] && ! tuxedo_uki_builder_present; then
                printf 'TUXEDO UKI builder create_boot_uki_base.sh missing'
            elif ! grub_generator_tool >/dev/null; then
                if grub2_layout_detected; then
                    printf 'grub2-mkconfig is not installed in the target'
                else
                    printf 'requires GRUB configuration tooling'
                fi
            elif ! grub_install_tool >/dev/null; then
                if grub2_layout_detected; then
                    printf 'grub2-install is not installed in the target'
                else
                    printf 'grub-install missing'
                fi
            else
                esp_root="$(profile_esp_root 2>/dev/null || true)"
                if [[ "$TARGET_DISTRO_FAMILY" == arch && -z "$esp_root" ]]; then
                    printf 'grub-install present; no ESP mount candidate'
                elif [[ "$TARGET_OS_ID" == tuxedo && -n "$esp_root" ]]; then
                    printf 'grub-install present; TUXEDO UKI builder create_boot_uki_base.sh present; ESP mount candidate: %s' "${esp_root#"$TARGET_ROOT"}"
                elif [[ "$TARGET_OS_ID" == tuxedo ]]; then
                    printf 'grub-install present; TUXEDO UKI builder create_boot_uki_base.sh present; no ESP mount candidate'
                else
                    printf 'grub-install present; ESP mount candidate: %s' "${TARGET_ESP_MOUNT:-unresolved}"
                fi
            fi
            ;;
        bootstack)
            if is_alpine_family; then
                if [[ "$TARGET_BOOTLOADER_BACKEND" == syslinux/extlinux ]]; then
                    printf 'Alpine uses OpenRC, mkinitfs and syslinux/extlinux; boot-stack reconciliation is not enabled (run the initramfs and extlinux stages separately)'
                else
                    printf 'Alpine uses OpenRC and mkinitfs; boot-stack reconciliation is not enabled (run the initramfs stage separately)'
                fi
            elif ! initramfs_unavailable_reason >/dev/null \
                || ! grub_unavailable_reason >/dev/null; then
                printf 'initramfs backend: %s; initramfs or GRUB prerequisites unavailable' "${TARGET_INITRAMFS_BACKEND:-unknown}"
            elif [[ "$TARGET_DISTRO_FAMILY" == arch ]] && ! efi_unavailable_reason >/dev/null; then
                printf 'initramfs, GRUB or EFI prerequisites unavailable'
            elif [[ "$TARGET_DISTRO_FAMILY" == arch ]]; then
                printf 'initramfs, GRUB and EFI prerequisites available'
            elif grub2_layout_detected && bios_firmware_mode; then
                printf 'initramfs backend: %s; GRUB2 BIOS prerequisites available; no EFI/UKI artifacts' "${TARGET_INITRAMFS_BACKEND:-unknown}"
            else
                printf 'initramfs backend: %s; initramfs and GRUB prerequisites available' "${TARGET_INITRAMFS_BACKEND:-unknown}"
            fi
            ;;
        filesystem)
            filesystem_capability_evidence
            ;;
        *)
            printf 'no evidence recorded'
            ;;
    esac
}

# ---------------------------------------------------------------------------
# File system repair engine
#
# The engine is deliberately conservative:
#   * fs-inspect resolves the scope's root, /boot, ESP and /home filesystems
#     and runs each filesystem's read-only check tool.  It never writes.
#   * fs-repair re-resolves the same scope, refuses any device that is not part
#     of it, refuses offline tools on mounted filesystems (including the whole
#     running host), and requires the filesystem-specific tool to exist.
#   * Unsupported filesystems are reported, never guessed about.
# ---------------------------------------------------------------------------
FS_SCOPE_DEVICES=()
FS_SCOPE_MOUNTS=()
FS_SCOPE_FSTYPES=()
FS_SCOPE_UUIDS=()
FS_SCOPE_TOOLS=()
declare -A FS_SCOPE_DEVICE_INDEX=()

filesystem_normalize_fstype()
{
    case "${1,,}" in
        vfat|fat16|fat32|msdos) printf 'fat\n' ;;
        ntfs3) printf 'ntfs\n' ;;
        zfs_member) printf 'zfs\n' ;;
        *) printf '%s\n' "${1,,}" ;;
    esac
}

# Authoritative mode matrix for the file system repair engine.  Every mode the
# engine is willing to execute is declared here; `filesystem_mode_matrix`
# exposes the same matrix to the contract test, which cross-checks it against
# MainWindow::filesystemRepairModes() so the helper and GUI lists cannot drift.
# f2fs is repair-only: fsck.f2fs has no read-only check mode (`-n` is a usage
# error) and its exit status is not trustworthy, so it is never inspected.
filesystem_mode_field()
{
    local fstype mode field tool offline online
    fstype="$(filesystem_normalize_fstype "$1")"
    mode="$2"
    field="$3"
    case "$fstype:$mode" in
        ext2:check|ext3:check|ext4:check|ext2:repair|ext3:repair|ext4:repair)
            tool=e2fsck; offline=yes; online=no ;;
        xfs:check|xfs:repair)
            tool=xfs_repair; offline=yes; online=no ;;
        btrfs:check|btrfs:repair|btrfs:rescue)
            tool=btrfs; offline=yes; online=no ;;
        btrfs:scrub)
            tool=btrfs; offline=no; online=yes ;;
        fat:check|fat:repair)
            tool=fsck.fat; offline=yes; online=no ;;
        exfat:check|exfat:repair)
            tool=fsck.exfat; offline=yes; online=no ;;
        ntfs:check|ntfs:repair)
            tool=ntfsfix; offline=yes; online=no ;;
        f2fs:repair)
            tool=fsck.f2fs; offline=yes; online=no ;;
        jfs:check|jfs:repair)
            tool=jfs_fsck; offline=yes; online=no ;;
        reiserfs:check|reiserfs:repair)
            tool=reiserfsck; offline=yes; online=no ;;
        zfs:check|zfs:scrub)
            tool=zpool; offline=no; online=yes ;;
        *) return 1 ;;
    esac
    case "$field" in
        tool) printf '%s' "$tool" ;;
        offline) printf '%s' "$offline" ;;
        online) printf '%s' "$online" ;;
        *) return 1 ;;
    esac
    return 0
}

# Stable text rendering of the mode matrix: one "<fstype> <mode> <offline>
# <online>" line per supported pair.  Consumed by the contract test, never by
# the repair path itself.
filesystem_mode_matrix()
{
    local pair fstype mode offline online
    for pair in \
        ext2:repair ext2:check \
        ext3:repair ext3:check \
        ext4:repair ext4:check \
        xfs:repair xfs:check \
        btrfs:repair btrfs:rescue btrfs:scrub btrfs:check \
        fat:repair fat:check \
        exfat:repair exfat:check \
        ntfs:repair ntfs:check \
        f2fs:repair \
        jfs:repair jfs:check \
        reiserfs:repair reiserfs:check \
        zfs:scrub zfs:check; do
        fstype="${pair%%:*}"
        mode="${pair##*:}"
        offline="$(filesystem_mode_field "$fstype" "$mode" offline 2>/dev/null || true)"
        online="$(filesystem_mode_field "$fstype" "$mode" online 2>/dev/null || true)"
        printf '%s %s %s %s\n' "$fstype" "$mode" "$offline" "$online"
    done
}

filesystem_repair_tool()
{
    local fstype tool="" path
    fstype="$(filesystem_normalize_fstype "$1")"
    case "$fstype" in
        ext2|ext3|ext4) tool="e2fsck" ;;
        xfs) tool="xfs_repair" ;;
        btrfs) tool="btrfs" ;;
        fat) tool="fsck.fat" ;;
        exfat) tool="fsck.exfat" ;;
        ntfs) tool="ntfsfix" ;;
        f2fs) tool="fsck.f2fs" ;;
        jfs) tool="jfs_fsck" ;;
        reiserfs) tool="reiserfsck" ;;
        zfs) tool="zpool" ;;
        *) return 1 ;;
    esac
    for path in "/usr/sbin/$tool" "/usr/bin/$tool" "/sbin/$tool" "/bin/$tool"; do
        if [[ -x "$path" ]]; then
            printf '%s\n' "$path"
            return 0
        fi
    done
    if command -v "$tool" >/dev/null 2>&1; then
        command -v "$tool"
        return 0
    fi
    return 1
}

# Build FS_INSPECT_ARGUMENTS for one supported fstype:mode pair.  Read-only
# modes use the tool's non-destructive flags; repair modes use the least
# invasive form (for example e2fsck -p) with any forced retry handled by the
# caller.  Returns 1 for an unsupported pair.
filesystem_inspect_arguments()
{
    local fstype mode device mountpoint pool tool
    fstype="$(filesystem_normalize_fstype "$1")"
    mode="$2"
    device="$3"
    mountpoint="${4:-}"
    pool="${5:-}"
    FS_INSPECT_ARGUMENTS=()
    tool="$(filesystem_mode_field "$fstype" "$mode" tool 2>/dev/null || true)"
    [[ -n "$tool" ]] || return 1
    case "$fstype:$mode" in
        ext2:check|ext3:check|ext4:check) FS_INSPECT_ARGUMENTS=("$tool" -f -n "$device") ;;
        # Safe preen first; fs_repair retries with -f -y only after preen
        # reports uncorrected errors and the GUI confirmation is in place.
        ext2:repair|ext3:repair|ext4:repair) FS_INSPECT_ARGUMENTS=("$tool" -f -p "$device") ;;
        xfs:check) FS_INSPECT_ARGUMENTS=("$tool" -n "$device") ;;
        # -e extends the exit status: 4 means repairs were applied.  -L is
        # never used because it can destroy an unreplayed log.
        xfs:repair) FS_INSPECT_ARGUMENTS=("$tool" -e "$device") ;;
        btrfs:check) FS_INSPECT_ARGUMENTS=("$tool" check --readonly "$device") ;;
        btrfs:repair) FS_INSPECT_ARGUMENTS=("$tool" check --repair "$device") ;;
        btrfs:rescue) FS_INSPECT_ARGUMENTS=("$tool" rescue super-recover -y "$device") ;;
        btrfs:scrub) FS_INSPECT_ARGUMENTS=("$tool" scrub start -B -d "$mountpoint") ;;
        fat:check) FS_INSPECT_ARGUMENTS=("$tool" -n -v "$device") ;;
        fat:repair) FS_INSPECT_ARGUMENTS=("$tool" -a -v "$device") ;;
        exfat:check) FS_INSPECT_ARGUMENTS=("$tool" -n "$device") ;;
        exfat:repair) FS_INSPECT_ARGUMENTS=("$tool" -p "$device") ;;
        ntfs:check) FS_INSPECT_ARGUMENTS=("$tool" -n "$device") ;;
        ntfs:repair) FS_INSPECT_ARGUMENTS=("$tool" "$device") ;;
        f2fs:repair) FS_INSPECT_ARGUMENTS=("$tool" -f "$device") ;;
        jfs:check) FS_INSPECT_ARGUMENTS=("$tool" -n "$device") ;;
        jfs:repair) FS_INSPECT_ARGUMENTS=("$tool" -f "$device") ;;
        reiserfs:check) FS_INSPECT_ARGUMENTS=("$tool" --check "$device") ;;
        reiserfs:repair) FS_INSPECT_ARGUMENTS=("$tool" -y --fix-fixable "$device") ;;
        zfs:check) FS_INSPECT_ARGUMENTS=("$tool" status -v "$pool") ;;
        zfs:scrub) FS_INSPECT_ARGUMENTS=("$tool" scrub -w "$pool") ;;
        *) return 1 ;;
    esac
    return 0
}

filesystem_zfs_status_result()
{
    local rc="$1" output="$2"
    if (( rc != 0 )); then
        printf 'issues\n'
        return 0
    fi
    # "errors: No known data errors" is the healthy form; only the explicit
    # permanent/uncorrectable forms count as issues.
    if grep -Eiq 'state:[[:space:]]*(FAULTED|DEGRADED|UNAVAIL|REMOVED)|errors:[[:space:]]*(PERMANENT|UNCORRECT|LIST OF ERRORS UNAVAILABLE)|status:.*(FAULTED|UNAVAIL|REMOVED|corrupt|unrecoverable)' <<<"$output"; then
        printf 'issues\n'
    else
        printf 'clean\n'
    fi
}

filesystem_btrfs_scrub_result()
{
    local rc="$1" output="$2"
    if (( rc != 0 )); then
        # Exit code 3 is documented as uncorrectable errors.
        printf 'issues\n'
        return 0
    fi
    if grep -Eqi 'uncorrectable|superblock.*(error|corrupt)' <<<"$output"; then
        printf 'issues\n'
        return 0
    fi
    if grep -Eq 'Error summary:.*[a-z_]+=[1-9]' <<<"$output"; then
        printf 'repaired\n'
        return 0
    fi
    printf 'clean\n'
}

# Classify a filesystem tool result from the per-filesystem exit map.  The
# upstream exit codes are authoritative; output parsing is used only where
# upstream documents the codes as unreliable (fsck.f2fs) or where scrub
# statistics carry the real answer (btrfs, zfs).  Returned values:
#   clean     the tool reported no errors (a repair made no changes)
#   repaired  the tool corrected errors or the repair modified the filesystem
#   issues    errors remain uncorrected or the tool failed
filesystem_result_classify()
{
    local fstype mode rc output
    fstype="$(filesystem_normalize_fstype "$1")"
    mode="$2"
    rc="$3"
    output="$4"
    if [[ "$mode" == "check" ]]; then
        if [[ "$fstype" == "zfs" ]]; then
            filesystem_zfs_status_result "$rc" "$output"
        elif (( rc == 0 )); then
            printf 'clean\n'
        else
            printf 'issues\n'
        fi
        return 0
    fi
    case "$fstype" in
        ext2|ext3|ext4|jfs|reiserfs|exfat)
            # 0 clean; 1 corrected; 2 corrected with a reboot recommended;
            # 4/6 (and anything else) uncorrected or a hard tool failure.
            case "$rc" in
                0) printf 'clean\n' ;;
                1|2) printf 'repaired\n' ;;
                *) printf 'issues\n' ;;
            esac ;;
        fat)
            # 0 clean; 1 corrected; 2 is a usage error and the filesystem was
            # not touched, so it must never be treated as success.
            case "$rc" in
                0) printf 'clean\n' ;;
                1) printf 'repaired\n' ;;
                *) printf 'issues\n' ;;
            esac ;;
        xfs)
            # 0 no changes; 4 repairs applied (-e); 1 uncorrected errors;
            # 2 dirty log with nothing repaired.
            case "$rc" in
                0) printf 'clean\n' ;;
                4) printf 'repaired\n' ;;
                *) printf 'issues\n' ;;
            esac ;;
        ntfs)
            # 0 success; 1 failure; 255 errors remain.  ntfsfix never performs
            # a full NTFS repair, so the caller must report chkdsk.
            case "$rc" in
                0) printf 'clean\n' ;;
                *) printf 'issues\n' ;;
            esac ;;
        f2fs)
            # fsck.f2fs exits 0 even when corruption remains; classify from
            # its [FSCK] result lines instead of the exit status.
            if grep -Eqi '\[Fail\]|corrupted|unrecoverable' <<<"$output"; then
                printf 'issues\n'
            else
                printf 'repaired\n'
            fi ;;
        btrfs)
            if [[ "$mode" == "scrub" ]]; then
                filesystem_btrfs_scrub_result "$rc" "$output"
            elif (( rc == 0 )); then
                printf 'clean\n'
            else
                printf 'issues\n'
            fi ;;
        zfs)
            filesystem_zfs_status_result "$rc" "$output" ;;
        *)
            (( rc == 0 )) && printf 'clean\n' || printf 'issues\n' ;;
    esac
}

filesystem_detail_line()
{
    local device="$1" tool="$2" output="$3" line detail=""
    while IFS= read -r line; do
        [[ -n "$line" ]] || continue
        line="${line//$'\t'/ }"
        line="${line//$'\r'/}"
        detail+="${detail:+; }$line"
        ((${#detail} >= 240)) && break
    done < <(printf '%s\n' "$output" | head -n 4)
    detail="${detail//$'\n'/; }"
    ((${#detail} > 320)) && detail="${detail:0:320}"
    printf 'File system check detail %s: tool=%s output=%s\n' "$device" "${tool:-none}" "${detail:-none}"
}

filesystem_device_record()
{
    local device="$1" mount_hint="${2:-}" canonical fstype uuid existing mountpoint pool
    canonical="$(canonical_block "$device" 2>/dev/null || true)"
    if [[ -n "$canonical" ]]; then
        fstype="$(lsblk -ndo FSTYPE "$canonical" 2>/dev/null | head -n1 || true)"
        uuid="$(blkid -s UUID -o value "$canonical" 2>/dev/null | head -n1 || true)"
    else
        # ZFS dataset sources (rpool/ROOT/...) are not block devices.  Record
        # the imported pool that backs them so zpool checks stay pool-scoped.
        pool="$(filesystem_pool_for_device "$device" 2>/dev/null || true)"
        [[ -n "$pool" ]] || return 1
        canonical="$pool"
        fstype="zfs"
        uuid="$(zpool list -H -o guid "$pool" 2>/dev/null | head -n1 || true)"
    fi
    existing="${FS_SCOPE_DEVICE_INDEX[$canonical]:-}"
    if [[ -n "$existing" ]]; then
        if [[ -z "${FS_SCOPE_MOUNTS[$existing]}" && -n "$mount_hint" ]]; then
            FS_SCOPE_MOUNTS[$existing]="$mount_hint"
        fi
        return 0
    fi
    mountpoint="$(filesystem_mountpoint_for_device "$canonical" "$mount_hint")"
    FS_SCOPE_DEVICE_INDEX[$canonical]=${#FS_SCOPE_DEVICES[@]}
    FS_SCOPE_DEVICES+=("$canonical")
    FS_SCOPE_MOUNTS+=("$mountpoint")
    FS_SCOPE_FSTYPES+=("$fstype")
    FS_SCOPE_UUIDS+=("${uuid:-none}")
    FS_SCOPE_TOOLS+=("")
}

# Resolve where a device is currently mounted.  The target's own mount tree is
# checked first so a repair target that reuses a host-visible device name can
# never be confused with the running host; the live mount table is the
# fallback for running-host maintenance.
filesystem_mountpoint_for_device()
{
    local device="$1" hint="${2:-}" source mountpoint
    # Running-host scope: TARGET_ROOT is "/" and the hint is a live mountpoint
    # that is verified by the findmnt scan below.
    if (( RUNNING_HOST_MODE == 1 )); then
        hint=""
    elif [[ -n "$hint" && -n "$TARGET_ROOT" ]] && mountpoint -q "$TARGET_ROOT$hint" 2>/dev/null; then
        printf '%s\n' "$hint"
        return 0
    fi
    while IFS=' ' read -r source mountpoint; do
        [[ -n "$source" && -n "$mountpoint" ]] || continue
        source="${source%%\[*}"
        [[ "$(canonical_block "$source" 2>/dev/null || true)" == "$device" ]] || continue
        # A live mount under the target's own temporary tree is the selected
        # repair system, not the running host.
        if [[ -n "$TARGET_ROOT" && "$mountpoint" == "$TARGET_ROOT/"* ]]; then
            continue
        fi
        printf '%s\n' "$mountpoint"
        return 0
    done < <(findmnt -rn -o SOURCE,TARGET 2>/dev/null || true)
    printf '\n'
}

# Release every helper-owned mount of one device before an offline repair.
# Mounts stacked inside the target root are released as well when the root
# device itself is being repaired, otherwise the root cannot be unmounted
# cleanly.  Returns 0 when at least one mount was released, 1 when the device
# was not mounted by this helper request, and 2 when a mount could not be
# released.
filesystem_release_mounts_for_device()
{
    local device="$1" idx mountpath source root_device released=0
    [[ -n "$MOUNT_BASE" ]] || return 1
    root_device="$(canonical_block "$ROOT_DEVICE" 2>/dev/null || true)"
    for (( idx=${#MOUNTS[@]}-1; idx>=0; --idx )); do
        mountpath="${MOUNTS[$idx]:-}"
        [[ -n "$mountpath" ]] || continue
        mountpoint -q "$mountpath" 2>/dev/null || continue
        source="$(findmnt -rn -o SOURCE --target "$mountpath" 2>/dev/null | head -n1 || true)"
        source="${source%%\[*}"
        if [[ "$(canonical_block "$source" 2>/dev/null || true)" != "$device" ]]; then
            # Not the requested device: only release it when it is stacked
            # inside the target root and the root device itself is repaired.
            if [[ -z "$root_device" || "$device" != "$root_device" \
                || ( "$mountpath" != "$MOUNT_BASE" && "$mountpath" != "$MOUNT_BASE/"* ) ]]; then
                continue
            fi
        fi
        if ! umount "$mountpath" 2>/dev/null && ! umount -l "$mountpath" 2>/dev/null; then
            return 2
        fi
        released=1
        unset 'MOUNTS[idx]'
    done
    MOUNTS=("${MOUNTS[@]}")
    (( released == 1 ))
}

# Read-only inspection runs against unmounted devices so mount-sensitive
# filesystem tools (for example btrfs check) still produce valid evidence.
# Only helper-owned mounts are released.
filesystem_release_all_mounts()
{
    local idx mountpath
    for (( idx=${#MOUNTS[@]}-1; idx>=0; --idx )); do
        mountpath="${MOUNTS[$idx]:-}"
        [[ -n "$mountpath" ]] || continue
        if mountpoint -q "$mountpath" 2>/dev/null; then
            umount "$mountpath" 2>/dev/null || umount -l "$mountpath" 2>/dev/null || true
        fi
        unset 'MOUNTS[idx]'
    done
    MOUNTS=("${MOUNTS[@]}")
}

filesystem_tool_timeout()
{
    local value="${BOOT_REPAIR_FS_TOOL_TIMEOUT:-1800}"
    case "$value" in
        ''|*[!0-9]*) value=1800 ;;
    esac
    (( value > 0 )) || value=1800
    printf '%s\n' "$value"
}

# Reset the scope arrays and resolve the filesystems that make up the selected
# scope (target root, /boot, ESP, /home or the live running-host equivalents).
filesystem_scope_resolve()
{
    FS_SCOPE_DEVICES=()
    FS_SCOPE_MOUNTS=()
    FS_SCOPE_FSTYPES=()
    FS_SCOPE_UUIDS=()
    FS_SCOPE_TOOLS=()
    FS_SCOPE_DEVICE_INDEX=()

    if (( RUNNING_HOST_MODE == 1 )); then
        filesystem_scope_resolve_host
    else
        filesystem_scope_resolve_target
    fi
}

filesystem_scope_resolve_target()
{
    local root_device root_source spec fstype mp resolved pool
    [[ -n "$TARGET_ROOT" && -d "$TARGET_ROOT" ]] || return 1

    root_device="$(canonical_block "$ROOT_DEVICE" 2>/dev/null || true)"
    if [[ -z "$root_device" ]]; then
        # Fall back to the live mount backing the selected target root when the
        # caller's root component no longer resolves to a block device.
        root_source="$(findmnt -rn -o SOURCE --target "$TARGET_ROOT" 2>/dev/null | head -n1 || true)"
        root_source="${root_source%%\[*}"
        [[ -n "$root_source" ]] && root_device="$(canonical_block "$root_source" 2>/dev/null || true)"
    fi
    if [[ -n "$root_device" ]]; then
        filesystem_device_record "$root_device" ""
    else
        # A ZFS root is a dataset (rpool/ROOT/...), not a block device; the
        # pool entry represents it for check/scrub purposes.
        pool="$(filesystem_pool_for_device "$ROOT_DEVICE" 2>/dev/null || true)"
        [[ -n "$pool" ]] || return 1
        filesystem_device_record "$pool" ""
    fi

    while IFS=$'\t' read -r spec fstype mp; do
        [[ -n "$spec" && -n "$mp" ]] || continue
        case "$spec" in
            /dev/*|UUID=*|PARTUUID=*|LABEL=*|PARTLABEL=*)
                resolved="$(resolve_fstab_source "$spec")"
                [[ -n "$resolved" ]] || continue
                is_block_device "$resolved" || continue
                resolved="$(canonical_block "$resolved" 2>/dev/null || true)"
                [[ -n "$resolved" ]] || continue
                filesystem_device_record "$resolved" "$mp" || true
                ;;
            *)
                # ZFS datasets (pool/dataset) are recorded through their pool.
                if [[ "$(filesystem_normalize_fstype "$fstype")" == "zfs" ]]; then
                    filesystem_device_record "$spec" "$mp" || true
                fi
                ;;
        esac
    done < <(
        awk '
            /^[[:space:]]*#/ { next }
            NF >= 3 && ($2 == "/boot" || $2 == "/boot/efi" || $2 == "/efi" || $2 == "/home") {
                print $1 "\t" $3 "\t" $2
            }
        ' "$TARGET_ROOT/etc/fstab" 2>/dev/null
    )
    return 0
}

filesystem_scope_resolve_host()
{
    local mp source fstype
    for mp in / /boot /boot/efi /efi /home; do
        # A path can expose stacked mounts: systemd's automount unit publishes a
        # synthetic autofs source (for example systemd-1) before the real
        # filesystem, and findmnt --target lists every stack member.  Resolve
        # each candidate and record the first real filesystem so the running
        # host's ESP and a separate /boot are not lost behind the automount.
        while IFS=' ' read -r source fstype; do
            [[ -n "$source" ]] || continue
            source="${source%%\[*}"
            if [[ "$mp" == "/efi" ]]; then
                case "${fstype,,}" in
                    vfat|fat|fat16|fat32|msdos) ;;
                    *) continue ;;
                esac
            fi
            if is_block_device "$source"; then
                filesystem_device_record "$source" "$mp" || true
                break
            elif [[ "$(filesystem_normalize_fstype "$fstype")" == "zfs" ]]; then
                # ZFS datasets are not block devices; the pool entry represents
                # them for zpool checks and scrubs.
                filesystem_device_record "$source" "$mp" || true
                break
            fi
        done < <(findmnt -rn -o SOURCE,FSTYPE --target "$mp" 2>/dev/null || true)
    done
    return 0
}

filesystem_scope_tools()
{
    local idx tool fstype
    FS_SCOPE_TOOLS=()
    for idx in "${!FS_SCOPE_DEVICES[@]}"; do
        fstype="${FS_SCOPE_FSTYPES[$idx]}"
        tool="$(filesystem_repair_tool "$fstype" 2>/dev/null || true)"
        FS_SCOPE_TOOLS[$idx]="$tool"
    done
}

filesystem_scope_contains_device()
{
    local device="$1" canonical idx pool
    canonical="$(canonical_block "$device" 2>/dev/null || true)"
    [[ -n "$canonical" ]] || canonical="$device"
    idx="${FS_SCOPE_DEVICE_INDEX[$canonical]:-}"
    if [[ -z "$idx" && "$canonical" != /dev/* ]]; then
        # A ZFS dataset request maps to its pool's scope entry.
        pool="$(filesystem_pool_for_device "$canonical" 2>/dev/null || true)"
        [[ -n "$pool" ]] && idx="${FS_SCOPE_DEVICE_INDEX[$pool]:-}"
    fi
    [[ -n "$idx" ]] || return 1
    printf '%s\n' "$idx"
    return 0
}

# Resolve the imported ZFS pool that backs a device, a pool name or a dataset
# source (rpool/ROOT/...).  Only imported pools are listed by `zpool list`.
filesystem_pool_for_device()
{
    local device="$1" pool candidate
    command -v zpool >/dev/null 2>&1 || return 1
    if [[ "$device" != /dev/* && "$device" == */* ]]; then
        candidate="${device%%/*}"
        if zpool list -H -o name 2>/dev/null | grep -Fqx -- "$candidate"; then
            printf '%s\n' "$candidate"
            return 0
        fi
    fi
    if [[ "$device" != /dev/* ]] && zpool list -H -o name 2>/dev/null | grep -Fqx -- "$device"; then
        printf '%s\n' "$device"
        return 0
    fi
    while IFS= read -r pool; do
        [[ -n "$pool" ]] || continue
        if zpool status -P "$pool" 2>/dev/null | awk -v dev="$device" '
            { for (i = 1; i <= NF; i++) if ($i == dev) found = 1 }
            END { exit(found ? 0 : 1) }'; then
            printf '%s\n' "$pool"
            return 0
        fi
    done < <(zpool list -H -o name 2>/dev/null || true)
    return 1
}

filesystem_run_inspect_command()
{
    local tool="$1" device="$2" rc=0 output=""
    shift 2
    if ! command -v "$tool" >/dev/null 2>&1; then
        FS_INSPECT_OUTPUT=""
        FS_INSPECT_RC=127
        return 0
    fi
    local -a inspect_command=("$@")
    if command -v timeout >/dev/null 2>&1; then
        inspect_command=(timeout --foreground "$(filesystem_tool_timeout)" "${inspect_command[@]}")
    fi
    set +e
    output="$("${inspect_command[@]}" 2>&1)"
    rc=$?
    set -e
    FS_INSPECT_OUTPUT="$output"
    FS_INSPECT_RC="$rc"
    return 0
}

filesystem_run_repair_command()
{
    local -a repair_command=("$@")
    if command -v timeout >/dev/null 2>&1; then
        repair_command=(timeout --foreground "$(filesystem_tool_timeout)" "${repair_command[@]}")
    fi
    set +e
    FS_REPAIR_OUTPUT="$("${repair_command[@]}" 2>&1)"
    FS_REPAIR_RC=$?
    set -e
    printf '%s\n' "$FS_REPAIR_OUTPUT" | tee -a "$SESSION_LOG"
    return 0
}

fs_inspect_scope()
{
    local idx device mountpoint fstype uuid tool tool_name tool_path pool
    local output result summary_clean=0 summary_issues=0 summary_unsupported=0 summary_missing=0 summary_skipped=0

    filesystem_scope_resolve || true
    if (( ${#FS_SCOPE_DEVICES[@]} == 0 )); then
        printf 'File system check summary: devices=0 clean=0 issues=0 unsupported=0 tool-missing=0 skipped=0\n'
        log "File system inspection found no resolvable scope filesystems (root, /boot, ESP and /home)." | tee -a "$SESSION_LOG"
        return 0
    fi
    filesystem_scope_tools
    if (( RUNNING_HOST_MODE == 0 )); then
        # Read-only checks run against unmounted devices so mount-sensitive
        # tools such as btrfs check report real evidence instead of refusing a
        # helper-owned read-only mount.
        filesystem_release_all_mounts
    fi

    for idx in "${!FS_SCOPE_DEVICES[@]}"; do
        device="${FS_SCOPE_DEVICES[$idx]}"
        fstype="${FS_SCOPE_FSTYPES[$idx]}"
        uuid="${FS_SCOPE_UUIDS[$idx]}"
        tool="${FS_SCOPE_TOOLS[$idx]}"
        pool=""
        # Recompute the live mount state: helper-owned mounts were released
        # above, so a mount still present here is a real (host or foreign)
        # mount that offline-only checks must not run against.
        mountpoint="$(filesystem_mountpoint_for_device "$device" "${FS_SCOPE_MOUNTS[$idx]:-}")"

        if ! filesystem_mode_field "$fstype" check tool >/dev/null 2>&1; then
            printf 'File system check %s: %s uuid=%s mount=%s tool=none result=unsupported\n' \
                "$device" "${fstype:-unknown}" "$uuid" "${mountpoint:-unmounted}"
            ((summary_unsupported += 1))
            continue
        fi
        if [[ -z "$tool" ]]; then
            printf 'File system check %s: %s uuid=%s mount=%s tool=none result=tool-missing\n' \
                "$device" "${fstype:-unknown}" "$uuid" "${mountpoint:-unmounted}"
            ((summary_missing += 1))
            continue
        fi

        tool_name="$(basename -- "$tool")"
        tool_path="$(filesystem_repair_tool "$fstype" 2>/dev/null || true)"
        if [[ -z "$tool_path" ]]; then
            printf 'File system check %s: %s uuid=%s mount=%s tool=%s result=tool-missing\n' \
                "$device" "${fstype:-unknown}" "$uuid" "${mountpoint:-unmounted}" "$tool_name"
            ((summary_missing += 1))
            continue
        fi

        # Offline-only check tools must never run against a mounted filesystem
        # (XFS and Btrfs refuse outright; other tools can report stale state).
        # The target's helper-owned mounts were released above, so a mount that
        # is still present belongs to the running host or another tool.
        if [[ -n "$mountpoint" ]] \
            && [[ "$(filesystem_mode_field "$fstype" check offline 2>/dev/null || true)" == "yes" ]]; then
            printf 'File system check %s: %s uuid=%s mount=%s tool=%s result=skipped\n' \
                "$device" "${fstype:-unknown}" "$uuid" "$mountpoint" "$tool_name"
            filesystem_detail_line "$device" "$tool_name" \
                "not run: $device is mounted at $mountpoint; this check tool is offline-only"
            ((summary_skipped += 1))
            continue
        fi

        if [[ "$fstype" == "zfs" ]]; then
            pool="$(filesystem_pool_for_device "$device" 2>/dev/null || true)"
            if [[ -z "$pool" ]]; then
                printf 'File system check %s: %s uuid=%s mount=%s tool=%s result=skipped\n' \
                    "$device" "$fstype" "$uuid" "${mountpoint:-unmounted}" "$tool_name"
                filesystem_detail_line "$device" "$tool_name" \
                    "not run: no imported ZFS pool was identified for $device"
                ((summary_skipped += 1))
                continue
            fi
        fi

        filesystem_inspect_arguments "$fstype" check "$device" "$mountpoint" "$pool" || {
            printf 'File system check %s: %s uuid=%s mount=%s tool=none result=unsupported\n' \
                "$device" "${fstype:-unknown}" "$uuid" "${mountpoint:-unmounted}"
            ((summary_unsupported += 1))
            continue
        }
        filesystem_run_inspect_command "$tool_path" "$device" "${FS_INSPECT_ARGUMENTS[@]}"
        output="$FS_INSPECT_OUTPUT"
        result="$(filesystem_result_classify "$fstype" check "$FS_INSPECT_RC" "$output")"
        printf 'File system check %s: %s uuid=%s mount=%s tool=%s result=%s\n' \
            "$device" "$fstype" "$uuid" "${mountpoint:-unmounted}" "$tool_name" "$result"
        filesystem_detail_line "$device" "$tool_name" "$output"
        if [[ "$result" == "clean" ]]; then
            ((summary_clean += 1))
        else
            ((summary_issues += 1))
        fi
    done

    summary="File system check summary: devices=${#FS_SCOPE_DEVICES[@]} clean=$summary_clean issues=$summary_issues unsupported=$summary_unsupported tool-missing=$summary_missing skipped=$summary_skipped"
    printf '%s\n' "$summary"
    log "$summary" | tee -a "$SESSION_LOG"
    return 0
}

# fs-inspect entry point: prepare the scope read-only (target or running host),
# resolve the root, /boot, ESP and /home filesystems and run each read-only
# check tool.
fs_inspect()
{
    local raw_disk="$1" raw_root="$2"
    CURRENT_STAGE="file system inspection"

    if (( RUNNING_HOST_MODE == 1 )); then
        prepare_running_host "$raw_disk" "$raw_root" no
    else
        prepare_target ro
        mount_target_boot_entry "/boot" ro
        mount_target_boot_entry "/boot/efi" ro
        mount_target_boot_entry "/efi" ro
    fi

    log "File system inspection started (read-only; no repair tool is invoked)." | tee -a "$SESSION_LOG"
    fs_inspect_scope
    return 0
}

# fs-repair entry point: re-resolve the scope, prove the requested device
# belongs to the selected disk, release helper-owned mounts for offline modes
# and run the mode matrix's repair command with the documented exit map.
fs_repair()
{
    local raw_disk="$1" raw_root="$2" requested_device="$3" requested_mode="$4"
    local device canonical idx mountpoint fstype uuid tool_path tool_name rc=0 output
    local offline online pool release_rc=1 status_rc=0 status_output

    CURRENT_STAGE="file system repair"
    [[ -n "$requested_device" ]] || fail "fs-repair requires a device to repair."
    [[ -n "$requested_mode" ]] || fail "fs-repair requires a repair mode."

    if (( RUNNING_HOST_MODE == 1 )); then
        prepare_running_host "$raw_disk" "$raw_root" no
    else
        prepare_target ro
        mount_target_boot_entry "/boot" ro
        mount_target_boot_entry "/boot/efi" ro
        mount_target_boot_entry "/efi" ro
    fi

    canonical="$(canonical_block "$requested_device" 2>/dev/null || true)"
    if [[ -z "$canonical" ]]; then
        # Imported ZFS pools and datasets (for example rpool/ROOT/arch) are
        # valid repair targets even though they are not block devices.  The
        # pool name is the scope key and the unit of a zpool scrub.
        pool="$(filesystem_pool_for_device "$requested_device" 2>/dev/null || true)"
        [[ -n "$pool" ]] || fail "The requested repair device is not a block device: $requested_device"
        canonical="$pool"
    fi

    filesystem_scope_resolve || true
    idx="$(filesystem_scope_contains_device "$canonical" 2>/dev/null || true)"
    [[ -n "$idx" ]] \
        || fail "The requested device is not part of the resolved scope (root, /boot, ESP, /home): $canonical"

    device="$canonical"
    fstype="${FS_SCOPE_FSTYPES[$idx]:-}"
    if [[ "$fstype" != "zfs" ]]; then
        same_single_top_disk "$TARGET_DISK" "$device" \
            || fail "The requested repair device is outside the selected scope's disk: $device"
    fi
    mountpoint="$(filesystem_mountpoint_for_device "$device" "${FS_SCOPE_MOUNTS[$idx]:-}")"
    uuid="$(blkid -s UUID -o value "$device" 2>/dev/null | head -n1 || true)"
    [[ -n "$uuid" ]] || uuid="none"

    if ! filesystem_mode_field "$fstype" "$requested_mode" tool >/dev/null 2>&1; then
        fail "Repair mode '$requested_mode' is not supported for filesystem '${fstype:-unknown}' on $device."
    fi
    tool_path="$(filesystem_repair_tool "$fstype" 2>/dev/null || true)"
    [[ -n "$tool_path" ]] \
        || fail "The filesystem check/repair tool for '${fstype:-unknown}' is not installed in the recovery environment."
    tool_name="$(basename -- "$tool_path")"

    offline="$(filesystem_mode_field "$fstype" "$requested_mode" offline)"
    online="$(filesystem_mode_field "$fstype" "$requested_mode" online)"

    if [[ "$offline" == "yes" ]]; then
        if (( RUNNING_HOST_MODE == 0 )); then
            filesystem_release_mounts_for_device "$device" && release_rc=0 || release_rc=$?
        fi
        if (( release_rc == 2 )); then
            fail "Offline file system repair could not release the helper's read-only mount of $device. Unmount it manually and retry."
        fi
        # Recompute the live mount state after releasing helper-owned mounts.
        mountpoint="$(filesystem_mountpoint_for_device "$device" "${FS_SCOPE_MOUNTS[$idx]:-}")"
        if [[ -n "$mountpoint" ]]; then
            fail "Offline file system repair refuses mounted filesystem $device (mounted at $mountpoint). Unmount it first or use an online mode."
        fi
        if (( release_rc == 0 )); then
            log "Released the helper's read-only mount(s) of $device before offline repair." | tee -a "$SESSION_LOG"
        fi
    fi
    # Online modes normally require a mountpoint, but a ZFS scrub only needs
    # the pool to be imported; that is verified through the pool lookup below.
    if [[ "$online" == "yes" && "$fstype" != "zfs" && -z "$mountpoint" ]]; then
        fail "Online file system repair requires $device to be mounted first; no mountpoint was found."
    fi

    pool=""
    if [[ "$fstype" == "zfs" ]]; then
        pool="$(filesystem_pool_for_device "$device" 2>/dev/null || true)"
        [[ -n "$pool" ]] || fail "The ZFS pool for $device could not be identified; refusing a scrub."
    fi

    filesystem_inspect_arguments "$fstype" "$requested_mode" "$device" "$mountpoint" "$pool" \
        || fail "No repair command is defined for '$fstype:$requested_mode'."

    if [[ "$fstype" == "btrfs" && "$requested_mode" == "repair" ]]; then
        log "WARNING: btrfs check --repair is a last-resort operation that upstream documents as dangerous and can make a damaged filesystem worse. The caller must have explicit user confirmation and a backup." | tee -a "$SESSION_LOG"
    fi
    if [[ "$fstype" == "ntfs" ]]; then
        log "NOTE: ntfsfix only clears the NTFS dirty state; Windows chkdsk /f is required for a real NTFS repair." | tee -a "$SESSION_LOG"
    fi

    log "REPAIR: $tool_name ($requested_mode) on $device (fstype=${fstype:-unknown}, uuid=$uuid, mount=${mountpoint:-unmounted})" | tee -a "$SESSION_LOG"
    filesystem_run_repair_command "${FS_INSPECT_ARGUMENTS[@]}"
    rc="$FS_REPAIR_RC"
    output="$FS_REPAIR_OUTPUT"

    # e2fsck: safe preen runs first.  The GUI already obtained explicit user
    # confirmation for this repair, so a forced -y pass is attempted only when
    # preen reports errors left uncorrected (exit code 4).
    if [[ "$fstype" == ext2 || "$fstype" == ext3 || "$fstype" == ext4 ]] \
        && [[ "$requested_mode" == "repair" ]] && (( rc == 4 )); then
        log "e2fsck preen left uncorrected errors (exit code 4); running the confirmed forced repair pass." | tee -a "$SESSION_LOG"
        filesystem_run_repair_command "$tool_name" -f -y "$device"
        rc="$FS_REPAIR_RC"
        output+=$'\n'"$FS_REPAIR_OUTPUT"
    fi

    # A zpool scrub only reports scrub completion; the authoritative error
    # state comes from the pool status parsed afterward.
    if [[ "$fstype" == "zfs" && "$requested_mode" == "scrub" ]]; then
        filesystem_run_inspect_command "$tool_path" "$device" "$tool_name" status -v "$pool"
        status_rc="$FS_INSPECT_RC"
        status_output="$FS_INSPECT_OUTPUT"
        printf '%s\n' "$status_output" | tee -a "$SESSION_LOG"
        output+=$'\n'"$status_output"
        (( rc == 0 && status_rc != 0 )) && rc="$status_rc"
    fi

    result="$(filesystem_result_classify "$fstype" "$requested_mode" "$rc" "$output")"
    printf 'File system repair %s: %s uuid=%s mount=%s tool=%s mode=%s result=%s\n' \
        "$device" "$fstype" "$uuid" "${mountpoint:-unmounted}" "$tool_name" "$requested_mode" "$result"
    if (( rc == 124 || rc == 137 )); then
        log "FAIL: file system repair ($requested_mode) timed out for $device (exit code $rc)." | tee -a "$SESSION_LOG" >&2
        return 1
    fi
    case "$result" in
        clean)
            log "PASS: file system repair ($requested_mode) completed for $device with exit code 0." | tee -a "$SESSION_LOG"
            if [[ "$requested_mode" == "check" ]]; then
                # Only a read-only check command proves that nothing was
                # written; repair modes may still update filesystem metadata
                # (superblock timestamps, scrub statistics) even when the tool
                # reports no errors.
                repair_change_status filesystem "unchanged|read-only check reported no errors"
            else
                repair_change_status filesystem changed
            fi
            return 0
            ;;
        repaired)
            if (( rc == 2 )); then
                log "PASS: file system repair ($requested_mode) corrected errors on $device; a reboot is recommended before using the filesystem (exit code 2)." | tee -a "$SESSION_LOG"
            else
                log "PASS: file system repair ($requested_mode) corrected errors on $device (exit code $rc)." | tee -a "$SESSION_LOG"
            fi
            repair_change_status filesystem changed
            return 0
            ;;
        *)
            if [[ "$fstype" == "ntfs" ]]; then
                log "FAIL: ntfsfix could not fully repair $device (exit code $rc); run Windows chkdsk /f for a real NTFS repair." | tee -a "$SESSION_LOG" >&2
            else
                log "FAIL: file system repair ($requested_mode) reported unresolved issues for $device (exit code $rc)." | tee -a "$SESSION_LOG" >&2
            fi
            return 1
            ;;
    esac
}

# One-line filesystem capability evidence for the resolved scope: which
# supported filesystems were found and which check tools are installed.
filesystem_capability_evidence()
{
    local idx tool tools="" fstype supported=0 available=0
    filesystem_scope_resolve || true
    if (( ${#FS_SCOPE_DEVICES[@]} == 0 )); then
        printf 'scope filesystems unresolved'
        return 0
    fi
    filesystem_scope_tools
    for idx in "${!FS_SCOPE_DEVICES[@]}"; do
        fstype="${FS_SCOPE_FSTYPES[$idx]}"
        if ! filesystem_mode_field "$fstype" check tool >/dev/null 2>&1; then
            continue
        fi
        supported=$((supported + 1))
        tool="${FS_SCOPE_TOOLS[$idx]:-}"
        [[ -n "$tool" ]] || continue
        tools+="${tools:+, }$(basename -- "$tool")(${fstype:-unknown})"
        available=$((available + 1))
    done
    if (( available > 0 )); then
        printf 'scope filesystems resolved; tools: %s' "$tools"
    elif (( supported == 0 )); then
        printf 'scope filesystems resolved; no supported file system type for a read-only check'
    else
        printf 'scope filesystems resolved; no supported file system check tool installed'
    fi
}

# Emit the gating contract consumed by MainWindow::repairToolAvailable(): one
# `Repair tool <key>: available|unavailable|<reason>` line per key followed by
# one `Repair capability evidence <key>: ...` line.  Read-only; fails closed
# for every unsupported backend or missing prerequisite.
diagnostic_repair_capabilities()
{
    local key reason
    profile_target_backends
    local -a keys=(validate filesystem dpkg fixbroken aptupdate upgrade dkms display initramfs efi grub extlinux bootstack)
    local -A reasons=()

    for key in "${keys[@]}"; do
        reasons[$key]=""
    done

    reasons[validate]="available"

    # Every capability below is decided by the detected backend probes, never
    # by the distribution ID/family.  A reason names the exact missing
    # prerequisite so a mixed-manager target stays attributable.
    if reason="$(dpkg_unavailable_reason)"; then reasons[dpkg]="available"; else reasons[dpkg]="$reason"; fi
    if reason="$(package_stage_unavailable_reason fix-broken)"; then reasons[fixbroken]="available"; else reasons[fixbroken]="$reason"; fi
    if reason="$(aptupdate_unavailable_reason)"; then reasons[aptupdate]="available"; else reasons[aptupdate]="$reason"; fi
    if reason="$(package_stage_unavailable_reason apt-upgrade)"; then reasons[upgrade]="available"; else reasons[upgrade]="$reason"; fi
    if reason="$(dkms_unavailable_reason)"; then reasons[dkms]="available"; else reasons[dkms]="$reason"; fi
    if reason="$(display_unavailable_reason)"; then reasons[display]="available"; else reasons[display]="$reason"; fi
    if reason="$(initramfs_unavailable_reason)"; then reasons[initramfs]="available"; else reasons[initramfs]="$reason"; fi
    if reason="$(efi_unavailable_reason)"; then reasons[efi]="available"; else reasons[efi]="$reason"; fi
    if reason="$(grub_unavailable_reason)"; then reasons[grub]="available"; else reasons[grub]="$reason"; fi
    if reason="$(extlinux_unavailable_reason)"; then reasons[extlinux]="available"; else reasons[extlinux]="$reason"; fi
    if reason="$(bootstack_unavailable_reason)"; then reasons[bootstack]="available"; else reasons[bootstack]="$reason"; fi

    # File system repair is backend-independent: the selected scope's root,
    # /boot, ESP and /home filesystems are inspected with the host's
    # filesystem-specific check tools.  The stage is offered only when the
    # scope resolves to at least one supported filesystem and a matching tool
    # is actually installed.
    if [[ "${reasons[filesystem]}" == "" ]]; then
        filesystem_scope_resolve || true
        if (( ${#FS_SCOPE_DEVICES[@]} == 0 )); then
            reasons[filesystem]="The selected scope's root, /boot, ESP and /home filesystems could not be resolved"
        else
            filesystem_scope_tools >/dev/null
            local fs_index fs_supported=0 fs_available=0
            for fs_index in "${!FS_SCOPE_DEVICES[@]}"; do
                if ! filesystem_mode_field "${FS_SCOPE_FSTYPES[$fs_index]}" check tool >/dev/null 2>&1; then
                    continue
                fi
                fs_supported=$((fs_supported + 1))
                if [[ -n "${FS_SCOPE_TOOLS[$fs_index]:-}" ]]; then
                    fs_available=$((fs_available + 1))
                fi
            done
            if (( fs_available > 0 )); then
                reasons[filesystem]="available"
            elif (( fs_supported == 0 )); then
                reasons[filesystem]="The selected scope has no supported file system type for a read-only check"
            else
                reasons[filesystem]="No supported file system check tool is installed in the recovery environment"
            fi
        fi
    fi

    echo "Repair capability probes (read-only, selected target):"
    for key in "${keys[@]}"; do
        if [[ "${reasons[$key]}" == "available" ]]; then
            printf 'Repair tool %s: available\n' "$key"
        else
            printf 'Repair tool %s: unavailable|%s\n' "$key" "${reasons[$key]}"
        fi
    done
    echo "Repair capability evidence (read-only):"
    for key in "${keys[@]}"; do
        printf 'Repair capability evidence %s: %s\n' "$key" "$(repair_capability_evidence "$key")"
    done

    # Running-host maintenance adds two scope-specific capability lines. They
    # are deliberately not `Repair tool` keys: the 13-key gating contract and
    # the Settings/Full Repair plan stay untouched, and the Snapshots tab
    # consumes these lines from the cached host capability preamble.
    if (( RUNNING_HOST_MODE == 1 )); then
        local host_reason
        if host_reason="$(host_snapshot_rollback_unavailable_reason)"; then
            printf 'Host snapshot rollback: available\n'
        else
            printf 'Host snapshot rollback: unavailable|%s\n' "$host_reason"
        fi
        printf 'Host snapshot rollback evidence: %s\n' "$(host_snapshot_rollback_evidence)"
        if host_reason="$(host_reboot_unavailable_reason)"; then
            printf 'Host reboot: available\n'
        else
            printf 'Host reboot: unavailable|%s\n' "$host_reason"
        fi
    fi
}

diagnostic_boot()
{
    local scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    echo "${scope_label^} mounts:"
    findmnt -R "$MOUNT_BASE" 2>/dev/null || true
    echo
    echo "${scope_label^} /boot:"
    ls -lah "$TARGET_ROOT/boot" 2>&1 | head -200 || true
    echo
    echo "${scope_label^} /boot/efi:"
    if [[ -d "$TARGET_ROOT/boot/efi" ]]; then
        ls -lah "$TARGET_ROOT/boot/efi" 2>&1 | head -200 || true
    else
        echo "No /boot/efi directory is visible."
    fi
    echo
    echo "${scope_label^} /efi:"
    if [[ -d "$TARGET_ROOT/efi" ]]; then
        ls -lah "$TARGET_ROOT/efi" 2>&1 | head -200 || true
    else
        echo "No /efi directory is visible."
    fi
}

# Keep intentional reboot/power-off teardown out of repair evidence. The
# newest boot is the relevant one, but its final journal lines often contain
# orderly service-stop warnings that are not boot failures.
journal_current_boot_actionable()
{
    awk '
        /systemd-logind.*(reboot|power[- ]off|powering off|shutdown).*requested/ {
            shutdown = 1
            next
        }
        shutdown { next }
        /D-Bus is shutting down/ { next }
        /Transaction for .*destructive/ { next }
        /Failed to enqueue SYSTEMD(_USER)?_WANTS job, ignoring/ { next }
        {
            # Journal retries often differ only by timestamp, helper PID, or
            # a transport sequence number. Keep the first occurrence so a
            # burst of identical failures does not drown out distinct evidence.
            key = $0
            sub(/^[A-Z][a-z][a-z] [ 0-9][0-9] [0-9:]+ [^ ]+ /, "", key)
            gsub(/\[[0-9]+\]/, "[]", key)
            gsub(/\[[[:space:]]*[0-9]+\.[0-9]+\]/, "[time]", key)
            gsub(/\(seq [0-9]+\)/, "(seq #)", key)
            gsub(/\/dev\/i2c-[0-9]+/, "/dev/i2c-#", key)
            gsub(/Message [0-9A-Fa-f]+/, "Message #", key)
            gsub(/near line [0-9]+/, "near line #", key)
            if (!seen[key]++) print
        }
    '
}

# Collapse consecutive near-identical journal lines into one line plus an
# occurrence count.  Messages that differ only by numbers, hex values, IDs or a
# trailing key/unit component share a signature; genuinely different messages
# keep their own line.  Input order is preserved and the output is bounded by
# the input, so noisy processes (for example an AppImage retrying one warning
# per layer) cannot flood the diagnostics log.
collapse_similar_journal_lines()
{
    awk '
        function signature(line,    s) {
            s = line
            sub(/^[A-Z][a-z][a-z] [ 0-9][0-9] [0-9:]+ [^ ]+ /, "", s)
            gsub(/\[[0-9]+\]/, "[]", s)
            gsub(/0[xX][0-9a-fA-F]+/, "#", s)
            gsub(/[0-9a-fA-F]{8,}/, "#", s)
            gsub(/[0-9]+/, "#", s)
            gsub(/\.[[:alnum:]_-]+$/, ".#", s)
            return s
        }
        {
            sig = signature($0)
            if (NR > 1 && sig == previous_signature) {
                run_count++
                next
            }
            if (run_count > 1) printf "%s (%d similar entries)\n", previous_line, run_count
            else if (run_count == 1) print previous_line
            previous_line = $0
            previous_signature = sig
            run_count = 1
        }
        END {
            if (run_count > 1) printf "%s (%d similar entries)\n", previous_line, run_count
            else if (run_count == 1) print previous_line
        }
    '
}

# Restrict journal evidence to entries that can describe the boot path of the
# selected system.  journalctl -o json is consumed as one object per line.
# Application and user-session noise (AppImages, desktop applications, user
# units) is excluded structurally rather than by matching a product name.
# python3 is a package dependency and does the parsing; a single-pass awk
# fallback keeps recovery hosts without python3 working.  Output is plain text
# "<timestamp> <host> <identifier>: <message>" in journal order so the
# downstream actionability and collapsing stages keep working unchanged.
journal_boot_relevant_filter()
{
    if command -v python3 >/dev/null 2>&1; then
        python3 -c '
import json, sys, time

BOOT_IDENTIFIERS = {
    "systemd", "systemd-udevd", "udev", "kernel", "dracut", "mkinitcpio",
    "cryptsetup", "systemd-cryptsetup", "lvm", "mdadm", "blkid",
    "fsck", "mount", "umount", "grub", "os-prober", "plymouth", "tuxedo",
}

def boot_relevant(entry):
    if entry.get("_SYSTEMD_USER_UNIT"):
        return False
    if entry.get("_TRANSPORT") == "kernel":
        return True
    unit = entry.get("_SYSTEMD_UNIT")
    if isinstance(unit, str) and unit:
        if unit.startswith("user@") and unit.endswith(".service"):
            return False
        if unit.endswith(".scope") and unit != "init.scope":
            return False
        return True
    identifier = entry.get("SYSLOG_IDENTIFIER") or entry.get("_COMM") or ""
    if not isinstance(identifier, str):
        return False
    return (identifier in BOOT_IDENTIFIERS
            or identifier.startswith("fsck.")
            or identifier.startswith("tuxedo"))

def text_value(value):
    if isinstance(value, str):
        return value
    if isinstance(value, list):
        try:
            return bytes(value).decode("utf-8", "replace")
        except (TypeError, ValueError):
            return ""
    return ""

MONTHS = ("Jan", "Feb", "Mar", "Apr", "May", "Jun",
          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec")

def timestamp_prefix(entry):
    stamp = entry.get("__REALTIME_TIMESTAMP")
    if not stamp:
        return ""
    try:
        local = time.localtime(int(stamp) / 1000000)
    except (TypeError, ValueError, OverflowError, OSError):
        return ""
    return "%s %2d %02d:%02d:%02d " % (
        MONTHS[local.tm_mon - 1], local.tm_mday,
        local.tm_hour, local.tm_min, local.tm_sec)

out = sys.stdout.buffer
for raw in sys.stdin.buffer:
    line = raw.rstrip(b"\n").decode("utf-8", "replace")
    if not line.strip():
        continue
    try:
        entry = json.loads(line)
    except ValueError:
        out.write(line.encode("utf-8", "replace") + b"\n")
        continue
    if not isinstance(entry, dict) or not boot_relevant(entry):
        continue
    identifier = entry.get("SYSLOG_IDENTIFIER") or entry.get("_COMM") \
        or entry.get("_SYSTEMD_UNIT") or "journal"
    if not isinstance(identifier, str) or not identifier:
        identifier = "journal"
    prefix = timestamp_prefix(entry)
    hostname = entry.get("_HOSTNAME")
    if prefix and isinstance(hostname, str) and hostname:
        prefix += hostname + " "
    message = text_value(entry.get("MESSAGE"))
    out.write(("%s%s: %s\n" % (prefix, identifier, message)).encode("utf-8", "replace"))
'
        return 0
    fi
    awk '
        function field_value(line, key,    marker, start, i, c, out, esc) {
            marker = "\"" key "\":\""
            start = index(line, marker)
            if (start == 0) return ""
            start += length(marker)
            out = ""
            esc = 0
            for (i = start; i <= length(line); i++) {
                c = substr(line, i, 1)
                if (esc) {
                    if (c == "n") out = out "\n"
                    else if (c == "t") out = out "\t"
                    else if (c == "r") out = out ""
                    else if (c == "u") { out = out "?"; i += 4 }
                    else out = out c
                    esc = 0
                } else if (c == "\\") {
                    esc = 1
                } else if (c == "\"") {
                    break
                } else {
                    out = out c
                }
            }
            return out
        }
        function field_present(line, key) {
            return index(line, "\"" key "\":") > 0
        }
        function timestamp_prefix(us,    secs, days, rest, hh, mm, ss, z, era, doe, yoe, doy, mp, d, m) {
            if (us !~ /^[0-9]+$/) return ""
            secs = int(us / 1000000)
            days = int(secs / 86400)
            rest = secs - days * 86400
            hh = int(rest / 3600)
            mm = int((rest - hh * 3600) / 60)
            ss = rest - hh * 3600 - mm * 60
            z = days + 719468
            era = int(z / 146097)
            doe = z - era * 146097
            yoe = int((doe - int(doe / 1460) + int(doe / 36524) - int(doe / 146096)) / 365)
            doy = doe - (365 * yoe + int(yoe / 4) - int(yoe / 100))
            mp = int((5 * doy + 2) / 153)
            d = doy - int((153 * mp + 2) / 5) + 1
            m = mp + (mp < 10 ? 3 : -9)
            split("Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec", month_names, " ")
            return sprintf("%s %2d %02d:%02d:%02d ", month_names[m], d, hh, mm, ss)
        }
        BEGIN {
            n = split("systemd systemd-udevd udev kernel dracut mkinitcpio cryptsetup systemd-cryptsetup lvm mdadm blkid fsck mount umount grub os-prober plymouth tuxedo", names, " ")
            for (i = 1; i <= n; i++) allowed[names[i]] = 1
        }
        {
            if ($0 !~ /^[[:space:]]*\{/) { print; next }
            if (field_present($0, "_SYSTEMD_USER_UNIT")) next
            if (index($0, "\"_TRANSPORT\":\"kernel\"") == 0) {
                if (field_present($0, "_SYSTEMD_UNIT")) {
                    unit = field_value($0, "_SYSTEMD_UNIT")
                    if (unit ~ /^user@.*\.service$/) next
                    if (unit ~ /\.scope$/ && unit != "init.scope") next
                } else {
                    ident = field_value($0, "SYSLOG_IDENTIFIER")
                    if (ident == "") ident = field_value($0, "_COMM")
                    if (!(ident in allowed) && ident !~ /^fsck\./ && ident !~ /^tuxedo/) next
                }
            }
            msg = field_value($0, "MESSAGE")
            ident = field_value($0, "SYSLOG_IDENTIFIER")
            if (ident == "") ident = field_value($0, "_COMM")
            if (ident == "") ident = field_value($0, "_SYSTEMD_UNIT")
            if (ident == "") ident = "journal"
            prefix = timestamp_prefix(field_value($0, "__REALTIME_TIMESTAMP"))
            host = field_value($0, "_HOSTNAME")
            if (prefix != "" && host != "") prefix = prefix host " "
            print prefix ident ": " msg
        }
    '
}

# Plain-text counterpart of journal_boot_relevant_filter for non-systemd
# (OpenRC) systems that log through BusyBox syslogd into /var/log/messages.
# The identifier set mirrors the journald filter so both evidence sources
# describe the same boot subsystem; kernel-facility lines are kernel evidence
# by definition and are always kept.
syslog_boot_relevant_filter()
{
    awk '
        BEGIN {
            n = split("systemd systemd-udevd udev kernel dracut mkinitcpio cryptsetup systemd-cryptsetup lvm mdadm blkid fsck mount umount grub os-prober plymouth tuxedo", names, " ")
            for (i = 1; i <= n; i++) allowed[names[i]] = 1
        }
        {
            line = $0
            # Saved kernel logs and raw ring-buffer lines carry "[    0.000000]".
            if (line ~ /^\[[[:space:]]*[0-9]+\.[0-9]+\]/) { print; next }
            rest = line
            # Strip "Mon DD HH:MM:SS host " when present.
            if (match(rest, /^[A-Z][a-z][a-z] [ 0-9][0-9] [0-9:]+ [^ ]+ /)) {
                rest = substr(rest, RSTART + RLENGTH)
            }
            # BusyBox syslogd prefixes "<facility>.<priority> ".
            if (match(rest, /^(kern|user|mail|daemon|auth|syslog|lpr|news|uucp|cron|authpriv|ftp|ntp|security|console)\.[a-z]+ /)) {
                facility = substr(rest, RSTART, RLENGTH)
                sub(/\..*$/, "", facility)
                rest = substr(rest, RSTART + RLENGTH)
                if (facility == "kern") { print; next }
            }
            # Standard syslog and BusyBox tags: "ident[pid]:" or "ident:".
            ident = ""
            if (match(rest, /^[A-Za-z0-9_.@:+-]+(\[[0-9]+\])?:/)) {
                ident = substr(rest, RSTART, RLENGTH)
                sub(/\[[0-9]+\]:$/, "", ident)
                sub(/:$/, "", ident)
            }
            if (ident == "") next
            if ((ident in allowed) || ident ~ /^fsck\./ || ident ~ /^tuxedo/) print
        }
    '
}

# Emit the kernel log for the inspected scope: the live ring buffer on the
# running host, otherwise the target's saved /var/log/dmesg snapshot.  Kernel
# lines are always boot evidence, matching the journald kernel transport rule.
non_systemd_kernel_log_lines()
{
    if (( RUNNING_HOST_MODE == 1 )) && command -v dmesg >/dev/null 2>&1; then
        dmesg 2>/dev/null || true
    elif [[ -r "$TARGET_ROOT/var/log/dmesg" ]]; then
        cat -- "$TARGET_ROOT/var/log/dmesg" 2>/dev/null || true
    fi
}

# Print every readable plain-text system log the detected logging backend may
# use.  syslog-ng/rsyslog commonly write /var/log/syslog while BusyBox syslogd
# writes /var/log/messages; reading both keeps the fallback independent of the
# distribution.
non_systemd_syslog_lines()
{
    if [[ -r "$TARGET_ROOT/var/log/messages" ]]; then
        cat -- "$TARGET_ROOT/var/log/messages" 2>/dev/null || true
    fi
    if [[ -r "$TARGET_ROOT/var/log/syslog" ]]; then
        cat -- "$TARGET_ROOT/var/log/syslog" 2>/dev/null || true
    fi
}

# Full kernel + syslog stream for diagnostics that apply their own selector
# (display manager, graphics).  No identifier filtering is applied.
non_systemd_raw_log_lines()
{
    non_systemd_kernel_log_lines
    if [[ -r "$TARGET_ROOT/var/log/messages" ]]; then
        tail -n 2000 "$TARGET_ROOT/var/log/messages" 2>/dev/null || true
    fi
    if [[ -r "$TARGET_ROOT/var/log/syslog" ]]; then
        tail -n 2000 "$TARGET_ROOT/var/log/syslog" 2>/dev/null || true
    fi
}

# Drop user-facility syslog lines (desktop applications, guest agents and
# other user-session noise) so the display/graphics fallback keeps service
# evidence only, mirroring the journald filter's user-session exclusion.
syslog_non_user_filter()
{
    awk '!/^[A-Z][a-z][a-z] [ 0-9][0-9] [0-9:]+ [^ ]+ user\.[a-z]+ /'
}

# Boot-relevant kernel + syslog stream using the same identifier set as the
# journald filter, for the boot-evidence and error fallback sections.
non_systemd_boot_log_lines()
{
    non_systemd_kernel_log_lines
    non_systemd_syslog_lines | syslog_boot_relevant_filter
}

# Firmware mode visible to the inspection host.  /sys/firmware/efi exists only
# for a UEFI-booted system; a legacy BIOS system must never be described with
# EFI firmware entries.
boot_firmware_mode()
{
    if [[ -d /sys/firmware/efi ]]; then
        printf 'UEFI\n'
    else
        printf 'BIOS/legacy\n'
    fi
}

# True when efibootmgr can read usable EFI variables.  On a legacy BIOS
# system efibootmgr prints "EFI variables are not supported"; diagnostics must
# report that instead of an empty "unknown (unresolved)" firmware inventory.
efi_variables_supported()
{
    local output
    command -v efibootmgr >/dev/null 2>&1 || return 1
    output="$(efibootmgr -v 2>&1 || true)"
    [[ -n "$output" ]] || return 1
    ! grep -qi 'EFI variables are not supported' <<<"$output"
}

# Resolve the extlinux/syslinux configuration visible for the selected scope.
extlinux_config_path()
{
    local candidate
    for candidate in /boot/extlinux.conf /boot/syslinux/syslinux.cfg /boot/syslinux.cfg; do
        if [[ -f "$TARGET_ROOT$candidate" ]]; then
            printf '%s\n' "$TARGET_ROOT$candidate"
            return 0
        fi
    done
    return 1
}

# Redact command-line secrets from an extlinux APPEND line.  The same
# identifiers as the /proc/cmdline evidence are removed before logging.
extlinux_redact_cmdline()
{
    sed -E 's/(crypt(id|root)?|rd\.luks\.(uuid|name)|luks\.uuid|password|passwd|passphrase)=[^[:space:]]+/\1=[REDACTED]/Ig'
}

# Parse LABEL/LINUX/INITRD/APPEND entries from the selected extlinux
# configuration and print them in menu order, marking the default entry
# (MENU DEFAULT or the top-level DEFAULT directive).
extlinux_menu_entries()
{
    local cfg
    cfg="$(extlinux_config_path)" || return 0
    awk '
        /^[[:space:]]*LABEL[[:space:]]/ { n++; label[n] = $2; next }
        /^[[:space:]]*MENU[[:space:]]+DEFAULT/ { menu_default[label[n]] = 1; next }
        /^[[:space:]]*DEFAULT[[:space:]]/ { default_name = $2; next }
        /^[[:space:]]*LINUX[[:space:]]/ { linux[label[n]] = $2; next }
        /^[[:space:]]*INITRD[[:space:]]/ { initrd[label[n]] = $2; next }
        /^[[:space:]]*APPEND[[:space:]]/ {
            append[label[n]] = $0
            sub(/^[[:space:]]*APPEND[[:space:]]+/, "", append[label[n]])
            next
        }
        END {
            for (i = 1; i <= n; i++) {
                chosen = (menu_default[label[i]] || (default_name != "" && label[i] == default_name))
                printf "LABEL %s%s\n", label[i], (chosen ? " [default]" : "")
                if (linux[label[i]] != "") printf "  LINUX %s\n", linux[label[i]]
                if (initrd[label[i]] != "") printf "  INITRD %s\n", initrd[label[i]]
                if (append[label[i]] != "") printf "  APPEND %s\n", append[label[i]]
            }
        }
    ' "$cfg" | extlinux_redact_cmdline
}

# Print "<label>\t<linux>\t<initrd>" for the default (or first) extlinux entry
# so the boot chain can name the kernel and initramfs that will load.
extlinux_default_entry()
{
    local cfg
    cfg="$(extlinux_config_path)" || return 0
    awk '
        /^[[:space:]]*LABEL[[:space:]]/ { n++; label[n] = $2; next }
        /^[[:space:]]*MENU[[:space:]]+DEFAULT/ { menu_default[label[n]] = 1; next }
        /^[[:space:]]*DEFAULT[[:space:]]/ { default_name = $2; next }
        /^[[:space:]]*LINUX[[:space:]]/ { linux[label[n]] = $2; next }
        /^[[:space:]]*INITRD[[:space:]]/ { initrd[label[n]] = $2; next }
        END {
            chosen = ""
            if (default_name != "") {
                for (i = 1; i <= n; i++) if (label[i] == default_name) chosen = label[i]
            }
            if (chosen == "") {
                for (i = 1; i <= n; i++) if (menu_default[label[i]]) { chosen = label[i]; break }
            }
            if (chosen == "" && n >= 1) chosen = label[1]
            if (chosen != "") printf "%s\t%s\t%s\n", chosen, linux[chosen], initrd[chosen]
        }
    ' "$cfg"
}

# Describe the detected boot chain (firmware -> loader -> initramfs -> root)
# and compare the UKI's LUKS declarations with the mounted root so the number
# of expected unlock prompts is evidence, not a guess.
diagnostic_boot_chain()
{
    local uki="" esp_root=""
    local grub_cfg
    local root_type backing luks_uuid crypttab_line crypttab_key
    local uki_cmdline="" uki_luks_uuid="" uki_luks_name="" cryptdevice_uuid=""
    local tmp default_entry extlinux_label extlinux_kernel extlinux_initrd
    local has_uki=false has_grub=false

    profile_target_backends
    grub_cfg="$TARGET_ROOT$(grub_config_path)"
    esp_root="$(profile_esp_root 2>/dev/null || true)"
    [[ -n "$esp_root" ]] && uki="$esp_root/EFI/BOOT/TUX.EFI"

    [[ -s "$uki" ]] && has_uki=true
    [[ -s "$grub_cfg" ]] && has_grub=true
    echo "Detected boot chain (read-only):"
    if [[ "$TARGET_BOOTLOADER_BACKEND" == "syslinux/extlinux" ]]; then
        # Alpine and other BIOS systems boot through extlinux/syslinux; there
        # is no firmware EFI entry in the chain.
        echo "Primary: BIOS/legacy firmware -> extlinux menu -> initramfs -> root filesystem -> graphical login."
        default_entry="$(extlinux_default_entry)"
        if [[ -n "$default_entry" ]]; then
            IFS=$'\t' read -r extlinux_label extlinux_kernel extlinux_initrd <<<"$default_entry"
            printf 'Default extlinux entry: LABEL %s -> LINUX %s + INITRD %s.\n' \
                "${extlinux_label:-unknown}" "${extlinux_kernel:-unknown}" "${extlinux_initrd:-unknown}"
        fi
    elif [[ "$TARGET_BOOTLOADER_BACKEND" == "systemd-boot + UKI" ]]; then
        echo "Primary: firmware EFI entry -> systemd-boot -> UKI or loader entry -> initramfs -> root filesystem -> graphical login."
        [[ "$has_grub" == true ]] && echo "Fallback: firmware fallback/GRUB entry -> GRUB menu -> initramfs -> root filesystem -> graphical login."
    elif [[ "$TARGET_BOOTLOADER_BACKEND" == "systemd-boot" ]]; then
        echo "Primary: firmware EFI entry -> systemd-boot loader entry -> initramfs -> root filesystem -> graphical login."
    elif [[ "$TARGET_BOOTLOADER_BACKEND" == "generic UKI" ]]; then
        echo "Primary: firmware EFI entry -> distribution UKI -> embedded initramfs -> root filesystem -> graphical login."
    elif [[ "$(alpine_efi_backend)" == "efi-stub" ]]; then
        echo "Primary: firmware EFI entry -> EFI-stub kernel on the ESP (vmlinuz-*) -> initramfs on the ESP -> root filesystem -> graphical login."
    elif [[ "$has_uki" == true && -x "$TARGET_ROOT/usr/sbin/create_boot_uki_base.sh" ]]; then
        echo "Primary: firmware EFI entry -> TUXEDO UKI (TUX.EFI) -> initramfs -> root filesystem -> graphical login."
        [[ "$has_grub" == true ]] && echo "Fallback: firmware fallback/GRUB entry -> GRUB menu -> initramfs -> root filesystem -> graphical login."
    elif [[ "$has_grub" == true && "$(boot_firmware_mode)" == "BIOS/legacy" ]]; then
        echo "Primary: BIOS/legacy firmware -> GRUB menu -> initramfs -> root filesystem -> graphical login."
    elif [[ "$has_grub" == true ]]; then
        echo "Primary: firmware EFI entry -> GRUB menu -> initramfs -> root filesystem -> graphical login."
    elif [[ "$(boot_firmware_mode)" == "BIOS/legacy" ]]; then
        echo "Primary: BIOS/legacy firmware -> distribution bootloader -> initramfs -> root filesystem -> graphical login."
    else
        echo "Primary: firmware EFI entry -> distribution EFI loader -> initramfs -> root filesystem -> graphical login."
    fi

    root_type="$(lsblk -ndo TYPE "$ROOT_DEVICE" 2>/dev/null | head -n1 || true)"
    if [[ "$root_type" == crypt ]]; then
        backing="$(crypt_backing_device "$ROOT_DEVICE" 2>/dev/null || true)"
        if [[ -n "$backing" ]] && command -v cryptsetup >/dev/null 2>&1; then
            luks_uuid="$(cryptsetup luksUUID "$backing" 2>/dev/null || true)"
        fi
        crypttab_line="$(sed -E '/^[[:space:]]*#/d; /^[[:space:]]*$/d' "$TARGET_ROOT/etc/crypttab" 2>/dev/null \
            | awk 'NF >= 2 {print; exit}' || true)"
        crypttab_key="$(awk '{print $3}' <<<"$crypttab_line")"
        if [[ -n "$crypttab_line" && "$crypttab_key" != none && "$crypttab_key" != "-" && "$crypttab_key" != "" ]]; then
            echo "Unlock handoff: crypttab supplies a key or non-interactive option for the root LUKS mapping; no extra bootloader prompt is added."
        elif [[ "$TARGET_BOOTLOADER_BACKEND" == "syslinux/extlinux" ]]; then
            echo "Unlock handoff: initramfs requests one root LUKS passphrase, then continues to the mounted root; the extlinux handoff does not add a second prompt."
        elif [[ "$(boot_firmware_mode)" == "BIOS/legacy" ]]; then
            echo "Unlock handoff: initramfs requests one root LUKS passphrase, then continues to the mounted root; the BIOS bootloader handoff does not add a second prompt."
        else
            echo "Unlock handoff: initramfs requests one root LUKS passphrase, then continues to the mounted root; the EFI/GRUB handoff does not add a second prompt."
        fi

        # Decode the UKI command line when available and compare its LUKS
        # identity with the unlocked root. `rd.luks.uuid` and `cryptdevice`
        # are compatible declarations of the same volume, not two prompts.
        if [[ "$has_uki" == true ]] && command -v objcopy >/dev/null 2>&1; then
            tmp="$SESSION_DIR/diag-boot-chain-cmdline"
            if objcopy --dump-section ".cmdline=$tmp" "$uki" >/dev/null 2>&1; then
                uki_cmdline="$(tr '\0' ' ' < "$tmp")"
                uki_luks_uuid="$(sed -nE 's/.*(^|[[:space:]])rd\.luks\.uuid=([^[:space:]]+).*/\2/p' <<<"$uki_cmdline" | head -n1)"
                uki_luks_name="$(sed -nE 's/.*(^|[[:space:]])rd\.luks\.name=([^=[:space:]]+)=.*/\2/p' <<<"$uki_cmdline" | head -n1)"
                cryptdevice_uuid="$(sed -nE 's/.*(^|[[:space:]])cryptdevice=UUID=([^:[:space:]]+):.*/\2/p' <<<"$uki_cmdline" | head -n1)"
            fi
        fi
        if [[ -n "$luks_uuid" && ( -n "$uki_luks_uuid" || -n "$cryptdevice_uuid" ) ]]; then
            if [[ "$uki_luks_uuid" == "$luks_uuid" || "$cryptdevice_uuid" == "$luks_uuid" || "$uki_luks_name" == "$luks_uuid" ]]; then
                echo "PASS: bootloader/initramfs LUKS declarations bind to the root volume UUID $luks_uuid; one unlock path is expected."
            else
                echo "FAIL: UKI LUKS declaration does not match the mounted root volume UUID $luks_uuid."
            fi
        elif [[ "$has_uki" == true ]]; then
            echo "INFO: UKI LUKS identity could not be compared with the mapped root on this inspection host."
        fi
    else
        echo "Unlock handoff: root is not an active LUKS mapping; no disk-decryption prompt is expected from this boot path."
    fi
}

diagnostic_boot_evidence()
{
    # This inspection intentionally records evidence about boot selection and
    # unlock attempts without ever exposing passphrases or changing the target.
    # Journal output is filtered to the boot stack so unrelated application
    # noise does not obscure the failure that led the user to recovery.
    local grub_cfg grubenv_path
    local cmdline_file="$TARGET_ROOT/proc/cmdline"
    local evidence_rc=0 efi_nvram_evidence extlinux_cfg="" scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    profile_target_backends
    grub_cfg="$TARGET_ROOT$(grub_config_path)"
    grubenv_path="$TARGET_ROOT$(grub_env_path)"

    echo "Boot evidence (read-only):"
    echo "${scope_label^} root: $TARGET_ROOT"
    echo "Captured: $(date --iso-8601=seconds 2>/dev/null || date)"
    echo "Mounted ${scope_label} source/options: $(findmnt -rn -o SOURCE,OPTIONS --target "$TARGET_ROOT" 2>/dev/null | head -1 || echo unknown)"
    echo "Firmware mode: $(boot_firmware_mode) (/sys/firmware/efi $([[ -d /sys/firmware/efi ]] && echo present || echo absent))"
    echo

    echo "Current recovery host kernel (context only):"
    uname -a 2>&1 || true
    echo

    echo "${scope_label^} bootloader selection settings:"
    if [[ -f "$TARGET_ROOT/etc/default/grub" ]]; then
        grep -E '^[[:space:]]*(GRUB_DEFAULT|GRUB_SAVEDEFAULT|GRUB_TIMEOUT|GRUB_CMDLINE_LINUX)' \
            "$TARGET_ROOT/etc/default/grub" 2>/dev/null || echo "No GRUB selection settings found."
    else
        echo "/etc/default/grub is not present."
    fi
    if [[ -f "$grub_cfg" ]]; then
        grep -nE 'saved_entry|next_entry|menuentry |submenu |^[[:space:]]*linux(|efi) |^[[:space:]]*initrd(|efi) ' \
            "$grub_cfg" 2>/dev/null | head -240 || true
    else
        echo "${scope_label^} grub.cfg is not visible."
    fi
    extlinux_cfg="$(extlinux_config_path || true)"
    if [[ -n "$extlinux_cfg" ]]; then
        echo "${scope_label^} extlinux configuration (${extlinux_cfg#"$TARGET_ROOT"}):"
        extlinux_menu_entries | head -200 || true
    fi
    echo
    diagnostic_boot_chain
    echo
    if [[ -f "$grubenv_path" ]]; then
        echo "GRUB environment (saved/next selection):"
        if command -v grub-editenv >/dev/null 2>&1; then
            grub-editenv "$grubenv_path" list 2>&1 || true
        elif command -v grub2-editenv >/dev/null 2>&1; then
            grub2-editenv "$grubenv_path" list 2>&1 || true
        else
            strings "$grubenv_path" 2>/dev/null \
                | grep -E '^(saved_entry|next_entry|prev_saved_entry|boot_success|menu_auto_hide|blsdir)=' \
                || echo "grub-editenv is unavailable."
        fi
    fi
    if [[ -d "$TARGET_ROOT/boot/loader" ]]; then
        if [[ "$TARGET_BOOTLOADER_BACKEND" == "grub" ]]; then
            # Fedora's /boot/loader is the GRUB2 BLS directory, not systemd-boot.
            echo "GRUB2 BLS loader entries (/boot/loader/entries):"
        else
            echo "systemd-boot loader selection:"
            [[ -f "$TARGET_ROOT/boot/loader/loader.conf" ]] \
                && sed -n '1,80p' "$TARGET_ROOT/boot/loader/loader.conf" || echo "loader.conf is not present."
        fi
        if [[ -d "$TARGET_ROOT/boot/loader/entries" ]]; then
            find "$TARGET_ROOT/boot/loader/entries" -maxdepth 1 -type f -name '*.conf' \
                -printf '%f\n' 2>/dev/null | sort | head -120
            while IFS= read -r entry; do
                echo "--- ${entry#"$TARGET_ROOT"/} ---"
                sed -n '1,80p' "$entry" 2>/dev/null || true
            done < <(find "$TARGET_ROOT/boot/loader/entries" -maxdepth 1 -type f -name '*.conf' 2>/dev/null | sort | head -40)
        fi
    fi
    if command -v efibootmgr >/dev/null 2>&1; then
        efi_nvram_evidence="$SESSION_DIR/boot-evidence-efi-nvram.txt"
        efibootmgr -v > "$efi_nvram_evidence" 2>&1 || true
        if grep -qi 'EFI variables are not supported' "$efi_nvram_evidence" 2>/dev/null; then
            echo "Firmware boot selection: EFI variables are not supported on this system; this legacy BIOS boot uses no firmware entries."
        else
            echo "Firmware boot selection and ownership (host NVRAM context):"
            efi_print_firmware_inventory "$efi_nvram_evidence"
            efibootmgr -v 2>&1 | grep -E '^(Boot(Current|Next|Order):)' | head -20 || true
        fi
    fi
    echo

    echo "${scope_label^} kernel and initramfs selection candidates:"
    find "$TARGET_ROOT/boot" -maxdepth 1 -type f \
        \( -name 'vmlinuz-*' -o -name 'initrd.img-*' -o -name 'initramfs-*' -o -name 'config-*' \) \
        -printf '%f %TY-%Tm-%Td %TH:%TM:%TS %s bytes\n' 2>/dev/null | sort -V | tail -160 || echo "No kernel artifacts found."
    if [[ -r "$cmdline_file" ]]; then
        echo
        echo "${scope_label^} /proc/cmdline (when proc is available):"
        sed -E 's/(crypt(id|root)?|rd\.luks\.(uuid|name)|luks\.uuid|password|passwd|passphrase)=[^[:space:]]+/\1=[REDACTED]/Ig' \
            "$cmdline_file" 2>/dev/null || true
    fi
    echo

    echo "Snapshot selection evidence:"
    if [[ -f "$TARGET_ROOT/etc/fstab" ]]; then
        grep -nEi 'subvol|snapshot|snapper|timeshift' "$TARGET_ROOT/etc/fstab" 2>/dev/null || echo "No snapshot references in fstab."
    fi
    if [[ -d "$TARGET_ROOT/.snapshots" ]]; then
        find "$TARGET_ROOT/.snapshots" -maxdepth 2 -type f \
            \( -name info.xml -o -name description -o -name snapshot -o -name metadata \) \
            -printf '%p\n' 2>/dev/null | sort | head -160
        # Read only Snapper metadata. Never recursively scan snapshot content:
        # snapshots can contain browser profiles, credentials, or other large
        # application files that are unrelated to boot selection.
        while IFS= read -r metadata; do
            grep -HinE 'pre-number|post-number|description|cleanup|userdata|selected|active' \
                "$metadata" 2>/dev/null || true
        done < <(find "$TARGET_ROOT/.snapshots" -maxdepth 3 -type f -name info.xml \
            -print 2>/dev/null | sort | head -160)
    else
        echo "Target /.snapshots directory is not visible."
    fi
    if command -v btrfs >/dev/null 2>&1 && [[ "$(findmnt -n -o FSTYPE "$MOUNT_BASE" 2>/dev/null || true)" == "btrfs" ]]; then
        echo
        btrfs subvolume get-default "$MOUNT_BASE" 2>&1 || true
        btrfs subvolume list -o "$MOUNT_BASE" 2>&1 | grep -Ei 'snap|timeshift|@' | head -160 || true
    fi
    echo

    echo "Unlock / master-password attempt evidence (passphrases redacted):"
    if [[ "$(lsblk -ndo FSTYPE "$ROOT_DEVICE" 2>/dev/null | head -1 || true)" == "crypto_LUKS" ]] \
       && command -v cryptsetup >/dev/null 2>&1; then
        echo "LUKS metadata/keyslot state (volume key and passphrase are never read):"
        cryptsetup luksUUID "$ROOT_DEVICE" 2>&1 || true
        cryptsetup luksDump "$ROOT_DEVICE" 2>&1 \
            | grep -E '^(LUKS header information|Version|UUID|Keyslot|[[:space:]]+[0-9]+:|[[:space:]]+Key)' \
            | head -100 || true
    fi
    if [[ -f "$TARGET_ROOT/etc/crypttab" ]]; then
        echo "${scope_label^} crypttab unlock definitions (key contents are not read):"
        sed -E 's/^[[:space:]]*#/\#/; /^[[:space:]]*$/d' "$TARGET_ROOT/etc/crypttab" \
            | sed -E 's/[[:space:]]+[^[:space:]]*key(file)?=[^[:space:]]+/ keyfile=[REDACTED]/Ig' || true
    fi
    if target_journal_evidence_present && command -v journalctl >/dev/null 2>&1; then
        # Restrict issue evidence to the most recent selected-system boot. Older boots
        # are retained in the boot-ID inventory below, but their resolved
        # failures should not drive a repair decision for the current boot.
        journalctl --root="$TARGET_ROOT" -b 0 -o json --no-pager -n 1200 2>/dev/null \
            | journal_boot_relevant_filter \
            | journal_current_boot_actionable \
            | grep -Ei 'cryptsetup|systemd-cryptsetup|luks|passphrase|password|unlock|keyslot|dracut|initramfs' \
            | sed -E 's/(password|passphrase|passwd|key)[=:][[:space:]]*[^[:space:]]+/\1=[REDACTED]/Ig' \
            | tail -260 \
            | collapse_similar_journal_lines || echo "No unlock-related ${scope_label} journal entries found."
    elif non_journald_log_fallback_ready; then
        echo "Source: $(non_journald_log_source_label) log (latest entries)."
        non_systemd_boot_log_lines \
            | grep -Ei 'cryptsetup|systemd-cryptsetup|luks|passphrase|password|unlock|keyslot|dracut|initramfs' \
            | sed -E 's/(password|passphrase|passwd|key)[=:][[:space:]]*[^[:space:]]+/\1=[REDACTED]/Ig' \
            | tail -260 \
            | collapse_similar_journal_lines || echo "No unlock-related ${scope_label} syslog entries found."
    else
        echo "No persistent ${scope_label} journal or syslog evidence source is available."
    fi
    echo

    if target_journal_evidence_present && command -v journalctl >/dev/null 2>&1; then
        echo "Boot-selection and kernel messages (${scope_label} journal):"
        echo "Journal scope: latest ${scope_label} boot (-b 0); older boot failures are omitted from inspection evidence."
        journalctl --root="$TARGET_ROOT" -b 0 -o json --no-pager -n 1600 2>/dev/null \
            | journal_boot_relevant_filter \
            | journal_current_boot_actionable \
            | grep -Ei 'kernel command line|BOOT_IMAGE|selected|default entry|menuentry|grub|systemd-boot|efiboot|efi|initramfs|mount.*(root|boot)|failed|timeout|dependency' \
            | tail -360 \
            | collapse_similar_journal_lines || echo "No boot-selection messages found."
        echo
        echo "${scope_label^} journal boot IDs (if available):"
        journalctl --root="$TARGET_ROOT" --list-boots --no-pager 2>&1 | tail -40 || true
    elif non_journald_log_fallback_ready; then
        echo "Boot-selection and kernel messages ($(non_journald_log_source_label) log, latest entries):"
        non_systemd_boot_log_lines \
            | grep -Ei 'kernel command line|BOOT_IMAGE|selected|default entry|menuentry|grub|systemd-boot|efiboot|efi|initramfs|mount.*(root|boot)|failed|timeout|dependency' \
            | tail -360 \
            | collapse_similar_journal_lines || echo "No boot-selection messages found."
    else
        echo "Boot-selection and kernel messages (${scope_label} journal):"
        echo "No persistent ${scope_label} journal or syslog evidence source is available."
    fi

    # Keep this diagnostic informational even when one optional evidence source
    # is absent. A non-zero status would make the GUI report transport failure.
    return "$evidence_rc"
}

diagnostic_kernel()
{
    local kernel kernel_name version initrd rc=0 scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    local -a kernels=()
    shopt -s nullglob
    for kernel in "$TARGET_ROOT"/boot/vmlinuz "$TARGET_ROOT"/boot/vmlinuz-*; do
        [[ -f "$kernel" ]] && kernels+=("$kernel")
    done
    profile_target_backends
    echo "${scope_label^} kernel files:"
    if ((${#kernels[@]} == 0)); then
        echo "  none found"
        rc=1
    else
        ls -lh "${kernels[@]}" 2>/dev/null || true
    fi
    echo
    echo "${scope_label^} initramfs files:"
    local -a initrds=()
    local -a initrd_patterns=("$TARGET_ROOT"/boot/initrd.img "$TARGET_ROOT"/boot/initrd.img-* \
                              "$TARGET_ROOT"/boot/initramfs-*.img)
    # mkinitfs names its initramfs images without the .img suffix
    # (initramfs-lts, initramfs-virt, initramfs-edge).
    if [[ "$TARGET_INITRAMFS_BACKEND" == mkinitfs ]]; then
        initrd_patterns+=("$TARGET_ROOT"/boot/initramfs-*)
    fi
    for initrd in "${initrd_patterns[@]}"; do
        [[ -f "$initrd" ]] && initrds+=("$initrd")
    done
    if ((${#initrds[@]} == 0)); then
        echo "  none found"
    else
        ls -lh "${initrds[@]}" 2>/dev/null || true
    fi
    echo
    echo "Kernel/initramfs pairing:"
    for kernel in "${kernels[@]}"; do
        kernel_name="${kernel##*/}"
        if [[ "$kernel_name" == vmlinuz-* ]]; then
            version="${kernel_name#vmlinuz-}"
        else
            version="$kernel_name"
        fi
        if [[ -f "$TARGET_ROOT/boot/initrd.img-$version" ]]; then
            echo "PASS: $kernel_name has matching initramfs initrd.img-$version."
        elif [[ -f "$TARGET_ROOT/boot/initramfs-$version.img" || -f "$TARGET_ROOT/boot/initramfs-$version-fallback.img" ]]; then
            echo "PASS: $kernel_name has matching initramfs initramfs-$version*.img."
        elif [[ "$TARGET_INITRAMFS_BACKEND" == mkinitfs && -f "$TARGET_ROOT/boot/initramfs-$version" ]]; then
            echo "PASS: $kernel_name has matching mkinitfs initramfs initramfs-$version."
        elif [[ "$version" == linux || "$version" == linux-lts ]] \
            && compgen -G "$TARGET_ROOT/boot/initramfs-${version}*.img" >/dev/null; then
            echo "PASS: $kernel_name has matching named-kernel initramfs."
        else
            echo "FAIL: $kernel_name has no matching initramfs."
            rc=1
        fi
    done
    shopt -u nullglob
    return "$rc"
}

diagnostic_grub()
{
    local cfg extlinux_cfg="" scope_label="target" grubenv_path
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    profile_target_backends
    cfg="$TARGET_ROOT$(grub_config_path)"
    grubenv_path="$TARGET_ROOT$(grub_env_path)"
    echo "Detected bootloader backend: ${TARGET_BOOTLOADER_BACKEND:-unknown}"
    if [[ "$TARGET_BOOTLOADER_BACKEND" == syslinux/extlinux ]]; then
        echo "GRUB is not the selected backend; this system boots through syslinux/extlinux on legacy BIOS."
    elif [[ "$TARGET_BOOTLOADER_BACKEND" != grub ]]; then
        echo "GRUB is not the selected backend; showing any visible GRUB files for comparison only."
    fi
    echo "GRUB configuration: $cfg"
    if [[ -f "$cfg" ]]; then
        grep -E '^[[:space:]]*(menuentry|submenu|blscfg)|linux[[:space:]]|linuxefi[[:space:]]|initrd[[:space:]]|initrdefi[[:space:]]|root=|subvol' "$cfg" \
            | head -300 || true
    else
        echo "GRUB configuration is not visible."
    fi
    echo
    echo "GRUB environment: $grubenv_path"
    if [[ -f "$grubenv_path" ]]; then
        if grub_env_block_valid; then
            echo "grubenv is a valid 1024-byte GRUB environment block."
        else
            echo "grubenv is missing or not a valid GRUB environment block."
        fi
        strings "$grubenv_path" 2>/dev/null \
            | grep -E '^(saved_entry|next_entry|prev_saved_entry|boot_success|boot_indeterminate|menu_auto_hide|blsdir)=' \
            || echo "No GRUB selection fields found."
    else
        echo "GRUB environment is not visible."
    fi
    extlinux_cfg="$(extlinux_config_path || true)"
    if [[ -n "$extlinux_cfg" ]]; then
        echo
        echo "extlinux/syslinux configuration (${extlinux_cfg#"$TARGET_ROOT"}):"
        extlinux_menu_entries | head -200 || true
    fi
    echo
    echo "/etc/default/grub:"
    if [[ -f "$TARGET_ROOT/etc/default/grub" ]]; then
        # Keep boot-selection settings while redacting command-line secrets
        # (crypt keys, passwords and credential paths) from the action log.
        sed -E 's/((GRUB_CMDLINE_LINUX(_DEFAULT)?|GRUB_PRELOAD_MODULES)[[:space:]]*=).*/\1<redacted>/; s/((password|passphrase|crypt(key|device)?|keyfile|credentials|token)[^=[:space:]]*[[:space:]]*=)[^[:space:]]+/\1<redacted>/Ig' \
            "$TARGET_ROOT/etc/default/grub" | head -200
    else
        echo "not present"
    fi

    # Current encrypted TUXEDO systems keep a small EFI-side GRUB handoff
    # alongside the normal /boot/grub/grub.cfg fallback.  Showing it here is
    # useful when the fallback reaches a bare grub> prompt even though the
    # normal GRUB configuration itself is valid.
    if [[ -f "$TARGET_ROOT/boot/efi/EFI/TUXEDO/grub.cfg" ]]; then
        echo
        echo "TUXEDO EFI-side GRUB handoff (/boot/efi/EFI/TUXEDO/grub.cfg):"
        sed -E 's/((password|passphrase|crypt(key|device)?|keyfile|credentials|token)[^=[:space:]]*[[:space:]]*=)[^[:space:]]+/\1<redacted>/Ig' \
            "$TARGET_ROOT/boot/efi/EFI/TUXEDO/grub.cfg" | head -400
    fi
    if [[ -f "$TARGET_ROOT/boot/efi/EFI/BOOT/BOOTX64.EFI" ]]; then
        echo
        if bios_firmware_mode; then
            echo "EFI fallback binary (present, not the active boot path for this legacy BIOS boot):"
        else
            echo "EFI fallback binary:"
        fi
        ls -lh "$TARGET_ROOT/boot/efi/EFI/BOOT/BOOTX64.EFI" 2>&1 || true
        sha256sum "$TARGET_ROOT/boot/efi/EFI/BOOT/BOOTX64.EFI" 2>&1 || true
    fi
}

diagnostic_uki()
{
    local uki="" esp_root="" efi_root="" tmp_uname tmp_cmdline partuuid embedded="" efi_nvram_diag scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    profile_target_backends
    esp_root="$(profile_esp_root 2>/dev/null || true)"
    if [[ -n "$esp_root" ]]; then
        efi_root="$esp_root"
        uki="$efi_root/EFI/BOOT/TUX.EFI"
    fi

    if bios_firmware_mode; then
        echo "Selected ${scope_label} EFI System Partition (informational only: this legacy BIOS boot has no active EFI boot path):"
    else
        echo "Selected ${scope_label} EFI System Partition:"
    fi
    if [[ -n "$TARGET_ESP_MOUNT" && "$TARGET_ESP_MOUNT" != unresolved ]] \
        && mountpoint -q "$TARGET_ROOT$TARGET_ESP_MOUNT" 2>/dev/null; then
        findmnt -rn -o SOURCE,FSTYPE,OPTIONS --target "$TARGET_ROOT$TARGET_ESP_MOUNT" 2>&1 || true
    else
        echo "No separately mounted ESP is visible."
    fi

    echo
    echo "EFI files:"
    if [[ -n "$efi_root" && -d "$efi_root/EFI" ]]; then
        find "$efi_root/EFI" -maxdepth 3 -type f -printf '%TY-%Tm-%Td %TH:%TM  %10s  %p\n' 2>/dev/null \
            | sed "s#${TARGET_ROOT}##" \
            | sort \
            | head -250
    else
        echo "No EFI directory is visible."
    fi

    if [[ -s "$uki" ]]; then
        echo
        echo "TUXEDO UKI: ${uki#"$TARGET_ROOT"}"
        if command -v objcopy >/dev/null 2>&1; then
            tmp_uname="$(mktemp "$SESSION_DIR/diag-uki-uname.XXXXXX")"
            tmp_cmdline="$(mktemp "$SESSION_DIR/diag-uki-cmdline.XXXXXX")"
            if objcopy --dump-section ".uname=$tmp_uname" "$uki" >/dev/null 2>&1; then
                embedded="$(tr '\0' '\n' < "$tmp_uname" | head -1)"
                echo "Embedded kernel: ${embedded:-unknown}"
                if [[ -n "$embedded" && -f "$TARGET_ROOT/boot/vmlinuz-$embedded" ]]; then
                    echo "PASS: matching /boot/vmlinuz-$embedded exists."
                elif [[ -n "$embedded" ]]; then
                    echo "FAIL: UKI kernel $embedded is not installed under target /boot."
                fi
            else
                echo "Unable to read the UKI .uname section."
            fi
            if objcopy --dump-section ".cmdline=$tmp_cmdline" "$uki" >/dev/null 2>&1; then
                echo "Embedded command line:"
                tr '\0' '\n' < "$tmp_cmdline" | head -20
            fi
        else
            echo "objcopy is not installed on the recovery host; UKI sections were not decoded."
        fi
    fi

    if [[ -n "$efi_root" && -d "$efi_root/EFI/Linux" ]]; then
        echo
        echo "Generic UKI images under ${efi_root#"$TARGET_ROOT"}/EFI/Linux:"
        find "$efi_root/EFI/Linux" -maxdepth 1 -type f -iname '*.efi' \
            -printf '%f\n' 2>/dev/null | sort | head -120
        if command -v objcopy >/dev/null 2>&1; then
            while IFS= read -r generic_uki; do
                [[ -s "$generic_uki" ]] || continue
                echo "Generic UKI image: ${generic_uki#"$TARGET_ROOT"}"
                tmp_uname="$SESSION_DIR/diag-generic-uki-uname"
                tmp_cmdline="$SESSION_DIR/diag-generic-uki-cmdline"
                rm -f -- "$tmp_uname" "$tmp_cmdline"
                if objcopy --dump-section ".uname=$tmp_uname" "$generic_uki" >/dev/null 2>&1; then
                    echo "Embedded kernel: $(tr '\0' '\n' < "$tmp_uname" | head -1)"
                else
                    echo "Embedded kernel: unavailable"
                fi
                if objcopy --dump-section ".cmdline=$tmp_cmdline" "$generic_uki" >/dev/null 2>&1; then
                    echo "Embedded command line: $(tr '\0' ' ' < "$tmp_cmdline" | head -1)"
                fi
            done < <(find "$efi_root/EFI/Linux" -maxdepth 1 -type f -iname '*.efi' -print 2>/dev/null | sort | head -40)
        fi
    fi
    if [[ "$TARGET_BOOTLOADER_BACKEND" == systemd-boot* ]]; then
        echo
        echo "systemd-boot loader layout:"
        for loader_root in "$TARGET_ROOT/boot/loader" "$TARGET_ROOT/efi/loader"; do
            [[ -d "$loader_root" ]] || continue
            echo "Loader root: ${loader_root#"$TARGET_ROOT"}"
            [[ -f "$loader_root/loader.conf" ]] && sed -n '1,80p' "$loader_root/loader.conf"
            if [[ -d "$loader_root/entries" ]]; then
                find "$loader_root/entries" -maxdepth 1 -type f -name '*.conf' -printf '%f\n' 2>/dev/null | sort | head -120
            fi
        done
    fi

    echo
    echo "Firmware entry inventory (host, selected ${scope_label} ESP, and other disks):"
    if command -v efibootmgr >/dev/null 2>&1 && [[ -n "$EFI_ESP_SOURCE" || -n "$efi_root" ]] \
        && efi_variables_supported; then
        if [[ -z "$EFI_ESP_SOURCE" && -n "$TARGET_ESP_MOUNT" && "$TARGET_ESP_MOUNT" != unresolved ]] \
            && mountpoint -q "$TARGET_ROOT$TARGET_ESP_MOUNT" 2>/dev/null; then
            EFI_ESP_SOURCE="$(findmnt -rn -o SOURCE --target "$TARGET_ROOT$TARGET_ESP_MOUNT" 2>/dev/null | head -1 || true)"
        fi
        efi_nvram_diag="$SESSION_DIR/diag-efi-nvram.txt"
        efibootmgr -v > "$efi_nvram_diag" 2>&1 || true
        efi_print_firmware_inventory "$efi_nvram_diag"
        echo
        echo "Entries referencing the selected ${scope_label} ESP only:"
        partuuid="$(blkid -s PARTUUID -o value "$EFI_ESP_SOURCE" 2>/dev/null || true)"
        if [[ -n "$partuuid" ]]; then
            efibootmgr -v 2>&1 | grep -iF "$partuuid" || echo "No NVRAM entries reference selected ESP PARTUUID $partuuid."
        else
            echo "Selected ESP PARTUUID could not be determined."
        fi
        echo
        efibootmgr 2>/dev/null | grep -E '^Boot(Current|Next|Order):' || true
    elif [[ ! -d /sys/firmware/efi ]]; then
        echo "EFI variables are not supported on this system (legacy BIOS boot); no firmware entry inventory is applicable."
    else
        echo "efibootmgr/UEFI variables are unavailable in the recovery host."
    fi
}

# OpenRC graphical-login diagnostics for Alpine-family targets.  OpenRC has no
# graphical.target or display-manager.service alias; the configured manager is
# the init service that declares "provide display-manager" and is enabled in a
# runlevel.  Every probe is read-only and no target service is started.
diagnostic_display_openrc()
{
    local scope_label="$1" service label package runlevels dm_services marker
    profile_target_backends

    echo "OpenRC service manager:"
    echo "Default runlevel:"
    if (( RUNNING_HOST_MODE == 1 )) && command -v rc-status >/dev/null 2>&1; then
        rc-status default 2>&1 | sed -n '1,120p' || true
    elif [[ -d "$TARGET_ROOT/etc/runlevels/default" ]]; then
        # Read-only equivalent of "rc-update show default" for a target that is
        # not the running system: the enabled state is the runlevel symlink.
        find "$TARGET_ROOT/etc/runlevels/default" -maxdepth 1 -type l -print 2>/dev/null \
            | sed 's#.*/##' | sort || true
    else
        echo "No /etc/runlevels/default directory is visible."
    fi

    dm_services="$(alpine_display_manager_services)"

    echo
    echo "Configured display manager:"
    if [[ -z "$dm_services" ]]; then
        echo "No OpenRC service provides display-manager."
    else
        while IFS= read -r service; do
            [[ -n "$service" ]] || continue
            label="$(display_manager_label_for_service "$service.service")"
            package="$(display_manager_package_for_service "$service.service")"
            runlevels="$(alpine_service_enabled_runlevels "$service" | paste -sd, -)"
            printf '%s: service %s provides display-manager; enabled in runlevel(s): %s; package %s: %s\n' \
                "$label" "$service" "${runlevels:-none}" "$package" "$(alpine_package_state "$package")"
        done <<<"$dm_services"
    fi

    echo
    echo "Installed display managers and OpenRC services:"
    for service in lightdm slim gdm sddm lxdm greetd; do
        package="$(display_manager_package_for_service "$service.service")"
        [[ -n "$package" ]] || package="$service"
        if [[ ! -e "$TARGET_ROOT/etc/init.d/$service" && "$(alpine_package_state "$package")" != installed ]]; then
            continue
        fi
        label="$(display_manager_label_for_service "$service.service")"
        runlevels="$(alpine_service_enabled_runlevels "$service" | paste -sd, -)"
        marker=""
        if grep -Fxq "$service" <<<"$dm_services"; then
            marker=" [configured]"
        fi
        printf '%s: package=%s (%s); init script=%s; enabled runlevels=%s%s\n' \
            "$label" "$package" "$(alpine_package_state "$package")" \
            "$([[ -e "$TARGET_ROOT/etc/init.d/$service" ]] && printf '/etc/init.d/%s' "$service" || printf 'missing')" \
            "${runlevels:-none}" "$marker"
    done

    echo
    if command -v journalctl >/dev/null 2>&1 && target_journal_evidence_present; then
        echo "Recent ${scope_label} display-manager boot evidence (${scope_label} journal, latest boot):"
        journalctl --root="$TARGET_ROOT" -b 0 -o json --no-pager -n 1200 2>&1 \
            | journal_boot_relevant_filter \
            | journal_current_boot_actionable \
            | grep -Ei 'sddm|gdm|lightdm|greetd|(^|[^[:alnum:]])ly([^[:alnum:]]|$)' \
            | grep -iE 'warning|warn|error|failed|failure|crash|signal|timeout|unable|denied|auth' \
            | grep -Eiv 'gkr-pam: unable to locate daemon control file|pam_kwallet5: open_session called without kwallet5_key' \
            | tail -120 \
            | collapse_similar_journal_lines || true
    elif non_journald_log_fallback_ready; then
        echo "Recent ${scope_label} display-manager boot evidence ($(non_journald_log_source_label) log, latest entries):"
        non_systemd_raw_log_lines \
            | syslog_non_user_filter \
            | grep -Ei 'sddm|gdm|lightdm|greetd|(^|[^[:alnum:]])ly([^[:alnum:]]|$)' \
            | grep -iE 'warning|warn|error|failed|failure|crash|signal|timeout|unable|denied|auth' \
            | grep -Eiv 'gkr-pam: unable to locate daemon control file|pam_kwallet5: open_session called without kwallet5_key' \
            | tail -120 \
            | collapse_similar_journal_lines || true
    else
        echo "Recent ${scope_label} display-manager boot evidence ($(non_journald_log_source_label) log, latest entries):"
        echo "No persistent ${scope_label} journal or syslog evidence source is available."
    fi

    echo
    if command -v journalctl >/dev/null 2>&1 && target_journal_evidence_present; then
        echo "Recent ${scope_label} graphics/display errors (${scope_label} journal, latest boot):"
        journalctl --root="$TARGET_ROOT" -b 0 -o json --no-pager -n 1600 2>/dev/null \
            | journal_boot_relevant_filter \
            | journal_current_boot_actionable \
            | grep -Ei 'sddm|gdm|lightdm|greetd|(^|[^[:alnum:]])ly([^[:alnum:]]|$)|plasma|kwin|nvidia|NVRM|nouveau|drm|gpu|display' \
            | grep -iE 'warning|warn|error|failed|failure|crash|signal|timeout|unable|denied|auth' \
            | tail -160 \
            | collapse_similar_journal_lines || true
    elif non_journald_log_fallback_ready; then
        echo "Recent ${scope_label} graphics/display errors ($(non_journald_log_source_label) log, latest entries):"
        non_systemd_raw_log_lines \
            | syslog_non_user_filter \
            | grep -Ei 'sddm|gdm|lightdm|greetd|(^|[^[:alnum:]])ly([^[:alnum:]]|$)|plasma|kwin|nvidia|NVRM|nouveau|drm|gpu|display' \
            | grep -iE 'warning|warn|error|failed|failure|crash|signal|timeout|unable|denied|auth' \
            | tail -160 \
            | collapse_similar_journal_lines || true
    else
        echo "Recent ${scope_label} graphics/display errors ($(non_journald_log_source_label) log, latest entries):"
        echo "No persistent ${scope_label} journal or syslog evidence source is available."
    fi
}

diagnostic_display()
{
    local display_link pkg service unit status configured_manager="none" scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    local -a services=(sddm.service gdm.service gdm3.service lightdm.service greetd.service ly.service)

    profile_target_backends
    if [[ "$TARGET_DISPLAY_BACKEND" == OpenRC ]]; then
        diagnostic_display_openrc "$scope_label"
        return 0
    fi
    if [[ "$TARGET_DISPLAY_BACKEND" == sysvinit ]]; then
        echo "SysVinit display-manager scripts:"
        for service in lightdm slim gdm gdm3 sddm lxdm; do
            if [[ -e "$TARGET_ROOT/etc/init.d/$service" ]]; then
                printf '%s: /etc/init.d/%s present (offline repair is not implemented for sysvinit)\n' \
                    "$(display_manager_label_for_service "$service")" "$service"
            fi
        done
        echo "No systemd graphical.target or display-manager.service is involved on this target."
        return 0
    fi

    echo "Systemd default target:"
    if command -v systemctl >/dev/null 2>&1; then
        systemctl --root="$TARGET_ROOT" get-default 2>&1 || true
    elif [[ -L "$TARGET_ROOT/etc/systemd/system/default.target" ]]; then
        readlink "$TARGET_ROOT/etc/systemd/system/default.target" 2>/dev/null || true
    else
        echo "Unable to determine default target."
    fi

    echo
    echo "Configured display manager:"
    if [[ -L "$TARGET_ROOT/etc/systemd/system/display-manager.service" ]]; then
        display_link="$(readlink "$TARGET_ROOT/etc/systemd/system/display-manager.service" 2>/dev/null || true)"
        configured_manager="$(basename -- "$display_link" 2>/dev/null || true)"
        echo "display-manager.service -> ${display_link:-unresolved}"
    else
        echo "display-manager.service is not enabled."
    fi

    echo
    echo "Installed display managers and units:"
    if [[ -x "$TARGET_ROOT/usr/bin/dpkg-query" ]]; then
        for service in "${services[@]}"; do
            pkg="$(display_manager_package_for_service "$service")"
            status="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
                dpkg-query -W -f='${db:Status-Status} ${Version}' "$pkg" 2>/dev/null || true)"
            unit="$(display_manager_unit_rel "$service" || true)"
            if [[ -n "$status" || -n "$unit" ]]; then
                printf '%s: package=%s unit=%s%s\n' \
                    "$(display_manager_label_for_service "$service")" \
                    "${status:-not-installed}" "${unit:-missing}" \
                    "$([[ "$configured_manager" == "$service" ]] && printf ' [configured]' || true)"
            fi
        done
    elif target_has_executable /usr/bin/rpm /bin/rpm /usr/sbin/rpm; then
        # rpm targets (Fedora/RHEL) use the same probe-based installed-state
        # query as the display preflight; no dpkg-only claim is emitted.
        for service in "${services[@]}"; do
            pkg="$(display_manager_package_for_service "$service")"
            unit="$(display_manager_unit_rel "$service" || true)"
            if target_rpm_package_installed "$pkg" 2>/dev/null; then
                status="installed"
            else
                status="not-installed"
            fi
            if [[ "$status" == "installed" || -n "$unit" ]]; then
                printf '%s: package=%s unit=%s%s\n' \
                    "$(display_manager_label_for_service "$service")" \
                    "$status" "${unit:-missing}" \
                    "$([[ "$configured_manager" == "$service" ]] && printf ' [configured]' || true)"
            fi
        done
    else
        echo "No supported package query backend is available in the ${scope_label}."
    fi

    echo
    echo "Recent ${scope_label} display-manager boot evidence (${scope_label} journal, latest boot):"
    if command -v journalctl >/dev/null 2>&1 && target_journal_evidence_present; then
        # Keep the complete boot stream through the boot-relevant and shutdown
        # filters so the systemd-logind reboot marker remains visible (it is a
        # system unit, so the relevance filter retains it).  A unit-scoped
        # journalctl query omits that marker and would misclassify orderly
        # Display-manager teardown (SIGTERM/auth-helper exit) as a boot failure.
        journalctl --root="$TARGET_ROOT" -b 0 -o json --no-pager -n 1200 2>&1 \
            | journal_boot_relevant_filter \
            | journal_current_boot_actionable \
            | grep -Ei 'sddm|gdm|lightdm|greetd|(^|[^[:alnum:]])ly([^[:alnum:]]|$)' \
            | grep -iE 'warning|error|failed|failure|crash|signal|timeout|unable|denied|auth' \
            | grep -Eiv 'gkr-pam: unable to locate daemon control file|pam_kwallet5: open_session called without kwallet5_key' \
            | tail -120 \
            | collapse_similar_journal_lines || true
    else
        echo "No persistent ${scope_label} journal or syslog evidence source is available."
    fi

    echo
    echo "Recent ${scope_label} graphics/display errors (${scope_label} journal, latest boot):"
    if command -v journalctl >/dev/null 2>&1 && target_journal_evidence_present; then
        # Preserve the full stream until after boot-relevant and shutdown
        # filtering; priority queries do not include the reboot marker used by
        # that filter.
        journalctl --root="$TARGET_ROOT" -b 0 -o json --no-pager -n 1600 2>/dev/null \
            | journal_boot_relevant_filter \
            | journal_current_boot_actionable \
            | grep -Ei 'sddm|gdm|lightdm|greetd|(^|[^[:alnum:]])ly([^[:alnum:]]|$)|plasma|kwin|nvidia|NVRM|nouveau|drm|gpu|display' \
            | grep -iE 'warning|error|failed|failure|crash|signal|timeout|unable|denied|auth' \
            | tail -160 \
            | collapse_similar_journal_lines || true
    else
        echo "No persistent ${scope_label} journal or syslog evidence source is available."
    fi
}

# Filter candidate package-manager log lines for the diagnostics report.
# Known transient mirror-retry noise (per-mirror transfer failures that
# dnf/librepo retries and resolves) is dropped, volatile fields (mount prefix,
# timestamp, pid, URL, mirror IP, checksum) are normalized, identical messages
# are deduplicated and the result is capped.  Real errors (command failures,
# checksum mismatches, unpack/signature failures) are printed first; the
# trailing note always states how many lines the cap suppressed so a bounded
# report is never silent about omitted evidence.
PACKAGE_LOG_MAX_LINES="${PACKAGE_LOG_MAX_LINES:-10}"

package_log_filter()
{
    awk -v max_lines="$PACKAGE_LOG_MAX_LINES" '
        function normalize(line,    s) {
            s = line
            if (s ~ /^\//) sub(/^\/[^:]*\/var\/log\//, "", s)
            gsub(/[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9:+-]+/, "<time>", s)
            gsub(/\[[0-9]+\]/, "[<pid>]", s)
            gsub(/(https?|ftp):\/\/[^ )]+/, "<url>", s)
            gsub(/\(IP: [0-9a-fA-F:.]+\)/, "(IP: <ip>)", s)
            gsub(/[0-9a-fA-F]{16,}/, "<hash>", s)
            gsub(/[[:space:]]+/, " ", s)
            sub(/^ /, "", s)
            sub(/ $/, "", s)
            return s
        }
        function is_transient(line,    l) {
            l = tolower(line)
            return (l ~ /ignore error - try another mirror/ \
                 || l ~ /ignore error - retry download/ \
                 || l ~ /error during transfer:/ \
                 || l ~ /error while downloading: curl error/ \
                 || l ~ /lr_yum_download_url_retry: attempt/ \
                 || l ~ /lro_metalinkurl processing failed: curl error/ \
                 || l ~ /serious error - curl code/ \
                 || l ~ /trying other mirror/ \
                 || l ~ /retrying download/)
        }
        function is_real_error(line,    l) {
            l = tolower(line)
            return (l ~ /command returned error|checksum mismatch|unpacking of archive failed|install failed/ \
                 || l ~ /fatal|panic|no more mirrors|gpg|signature|conflict|broken|error:/)
        }
        {
            if ($0 == "") next
            if (is_transient($0)) { transient++; next }
            key = normalize($0)
            if (key == "" || seen[key]++) next
            unique++
            if (is_real_error($0)) { real[++nreal] = key }
            else { other[++nother] = key }
        }
        END {
            shown = 0
            for (i = 1; i <= nreal && shown < max_lines; i++) { print real[i]; shown++ }
            for (i = 1; i <= nother && shown < max_lines; i++) { print other[i]; shown++ }
            suppressed = unique - shown
            if (shown == 0) {
                if (transient > 0)
                    printf "  Only transient mirror-retry noise matched (%d line(s) suppressed).\n", transient
                else
                    print "  No actionable package-manager log errors were found."
            } else if (suppressed > 0 || transient > 0) {
                printf "  Suppressed: %d transient mirror-retry line(s), %d further unique message(s).\n", transient, suppressed
            }
        }
    '
}

# Bounded package-manager log errors for the detected backend: apk.log on
# Alpine and dnf5.log (with one rotation) on Fedora/RHEL.  The dnf4 log names
# (dnf.log, dnf.rpm.log) are not probed as primary sources because Fedora 44
# writes dnf5.log; scriptlet and kernel-trigger output also lands there.
diagnostic_package_manager_logs()
{
    local scope_label="$1" log
    local -a dnf_logs=()
    if [[ -r "$TARGET_ROOT/var/log/apk.log" ]]; then
        echo
        echo "Recent ${scope_label} apk package-manager log errors (/var/log/apk.log):"
        grep -iE 'error|failed|failure|unable|cannot|conflict|broken|fatal' \
            "$TARGET_ROOT/var/log/apk.log" 2>/dev/null \
            | package_log_filter || true
    fi
    for log in "$TARGET_ROOT/var/log/dnf5.log" "$TARGET_ROOT/var/log/dnf5.log.1"; do
        [[ -r "$log" ]] && dnf_logs+=("$log")
    done
    if ((${#dnf_logs[@]} > 0)); then
        echo
        echo "Recent ${scope_label} dnf5 package-manager log errors (/var/log/dnf5.log):"
        grep -iE 'error|failed|failure|unable|cannot|conflict|broken|fatal|Problem:' \
            "${dnf_logs[@]}" 2>/dev/null \
            | package_log_filter || true
    fi
}

diagnostic_errors()
{
    local scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    profile_target_backends
    if ! command -v journalctl >/dev/null 2>&1 || ! target_journal_evidence_present; then
        if non_journald_log_fallback_ready; then
            echo "Recent ${scope_label} error-priority syslog entries ($(non_journald_log_source_label) log, latest entries):"
            non_systemd_boot_log_lines \
                | grep -iE 'error|failed|failure|warning|warn|critical|fatal|panic|oops|denied|timeout' \
                | tail -200 \
                | collapse_similar_journal_lines || true
            echo
            echo "Recent failure-related ${scope_label} syslog lines:"
            non_systemd_boot_log_lines \
                | grep -iE 'failed|failure|dependency failed|timed out' \
                | tail -100 \
                | collapse_similar_journal_lines || true
            diagnostic_package_manager_logs "$scope_label"
            return 0
        fi
        # A target with no logging evidence at all must say so regardless of
        # the recovery host's tooling: blaming the host here would hide the
        # real reason the diagnostics section is empty.
        if [[ "${TARGET_LOGGING_BACKEND:-none}" == "none" ]]; then
            echo "${scope_label^} has no persistent journal, syslog or dmesg evidence source."
        elif ! command -v journalctl >/dev/null 2>&1; then
            echo "journalctl is not installed in the recovery host; detected ${scope_label} logging backend: ${TARGET_LOGGING_BACKEND}."
        else
            echo "${scope_label^} has no persistent journal evidence under /var/log/journal; detected logging backend: ${TARGET_LOGGING_BACKEND}."
        fi
        return 0
    fi
    echo "Recent ${scope_label} error-priority journal entries:"
    echo "Journal scope: latest ${scope_label} boot (-b 0), excluding intentional shutdown teardown."
    journalctl --root="$TARGET_ROOT" -b 0 -p err -n 200 -o json --no-pager 2>&1 \
        | journal_boot_relevant_filter \
        | journal_current_boot_actionable \
        | collapse_similar_journal_lines || true
    echo
    echo "Recent failure-related ${scope_label} journal lines:"
    journalctl --root="$TARGET_ROOT" -b 0 -o json --no-pager -n 500 2>/dev/null \
        | journal_boot_relevant_filter \
        | journal_current_boot_actionable \
        | grep -iE 'failed|failure|dependency failed|timed out' \
        | tail -100 \
        | collapse_similar_journal_lines || true
    diagnostic_package_manager_logs "$scope_label"
}

diagnostic_usage()
{
    echo "Filesystem usage:"
    df -hT "$MOUNT_BASE" 2>&1 || true
    if [[ "$(findmnt -n -o FSTYPE "$MOUNT_BASE" 2>/dev/null || true)" == "btrfs" ]] \
       && command -v btrfs >/dev/null 2>&1; then
        echo
        echo "Btrfs filesystem usage:"
        btrfs filesystem usage "$MOUNT_BASE" 2>&1 || true
    fi
}

diagnostic_fstab()
{
    if [[ -f "$TARGET_ROOT/etc/fstab" ]]; then
        cat "$TARGET_ROOT/etc/fstab"
    else
        if (( RUNNING_HOST_MODE == 1 )); then
            echo "Running host /etc/fstab is not present."
        else
            echo "Target /etc/fstab is not present."
        fi
        return 1
    fi
}

diagnostic_btrfs()
{
    local fstype scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    fstype="$(findmnt -n -o FSTYPE "$MOUNT_BASE" 2>/dev/null || true)"
    if [[ "$fstype" != "btrfs" ]]; then
        echo "${scope_label^} root filesystem is ${fstype:-unknown}, not Btrfs."
        return 0
    fi
    if ! command -v btrfs >/dev/null 2>&1; then
        echo "btrfs-progs is not installed in the recovery host."
        return 0
    fi
    echo "Btrfs filesystem:"
    btrfs filesystem show "$MOUNT_BASE" 2>&1 || true
    echo
    echo "Btrfs subvolumes:"
    btrfs subvolume list "$MOUNT_BASE" 2>&1 || true
}

diagnostic_mapper()
{
    local mapper=""
    echo "Block dependency tree for selected component:"
    lsblk -s -o NAME,SIZE,TYPE,FSTYPE,LABEL,UUID,MOUNTPOINTS "$ROOT_DEVICE" 2>&1 || true
    if command -v dmsetup >/dev/null 2>&1; then
        echo
        echo "Device-mapper tree:"
        dmsetup ls --tree 2>&1 || true
    fi
    if command -v cryptsetup >/dev/null 2>&1; then
        mapper="$(find_crypt_mapper_for_device "$ROOT_CANONICAL" 2>/dev/null || true)"
        if [[ -n "$mapper" ]]; then
            echo
            echo "cryptsetup status ($(basename -- "$mapper")):"
            cryptsetup status "$(basename -- "$mapper")" 2>&1 || true
        fi
    fi
}

diagnostic_luks()
{
    local scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    echo "Encrypted/mapped ancestry:"
    lsblk -srpo NAME,TYPE,FSTYPE,UUID "$ROOT_DEVICE" 2>&1 || true
    echo
    echo "${scope_label^} /etc/crypttab:"
    if [[ -f "$TARGET_ROOT/etc/crypttab" ]]; then
        # crypttab's third field may contain key files or sensitive options;
        # retain mapper/source identity and replace all remaining fields.
        awk 'BEGIN { OFS="\t" } /^[[:space:]]*#/ { print; next } NF >= 2 { print $1, $2, "<redacted>"; next } { print }' \
            "$TARGET_ROOT/etc/crypttab" | head -200
    else
        echo "not present"
    fi
    echo
    echo "${scope_label^} /etc/fstab mapper references:"
    if [[ -f "$TARGET_ROOT/etc/fstab" ]]; then
        grep -E '/dev/mapper|UUID=' "$TARGET_ROOT/etc/fstab" 2>/dev/null || echo "No mapper/UUID references found."
    else
        echo "fstab not present"
    fi
}

run_one_diagnostic()
{
    local key="$1" scope="${2:-Repair Target}" title rc=0
    title="$(diagnostic_title "$key")"
    echo "========================================"
    echo "$title"
    echo "========================================"
    echo "Diagnostic: $key"
    echo "Scope: $scope"
    echo "Time: $(date --iso-8601=seconds 2>/dev/null || date)"
    echo
    if (( REPORT_SHARED_PREAMBLE == 0 )); then
        diagnostic_repair_capabilities
        echo
    fi
    case "$key" in
        environment) diagnostic_environment || rc=$? ;;
        backend)     diagnostic_backend_profile || rc=$? ;;
        boot)        diagnostic_boot || rc=$? ;;
        boot-evidence) diagnostic_boot_evidence || rc=$? ;;
        kernel)      diagnostic_kernel || rc=$? ;;
        grub)        diagnostic_grub || rc=$? ;;
        uki)         diagnostic_uki || rc=$? ;;
        display)     diagnostic_display || rc=$? ;;
        errors)      diagnostic_errors || rc=$? ;;
        usage)       diagnostic_usage || rc=$? ;;
        fstab)       diagnostic_fstab || rc=$? ;;
        btrfs)       diagnostic_btrfs || rc=$? ;;
        mapper)      diagnostic_mapper || rc=$? ;;
        luks)        diagnostic_luks || rc=$? ;;
        *) fail "Unknown diagnostic: $key" ;;
    esac
    echo
    return "$rc"
}

run_one_target_diagnostic()
{
    run_one_diagnostic "$1" "Repair Target"
}

run_target_diagnostic()
{
    local requested="${1:-}" key
    [[ -n "$requested" ]] || fail "diagnose requires a diagnostic name or 'all'."
    (($# == 1)) || fail "diagnose accepts exactly one diagnostic name or 'all'."

    prepare_target ro
    mount_target_boot_entry "/boot" ro
    mount_target_boot_entry "/boot/efi" ro
    mount_target_boot_entry "/efi" ro

    if [[ "$requested" == "all" || "$requested" == "report" ]]; then
        # The capability preamble is shared by every section: emit it once at
        # the top of the combined report and omit it from each section.
        REPORT_SHARED_PREAMBLE=1
        diagnostic_repair_capabilities || true
        echo
        for key in environment backend boot boot-evidence kernel grub uki display errors usage fstab btrfs mapper luks; do
            run_one_target_diagnostic "$key" || true
        done
        REPORT_SHARED_PREAMBLE=0
    else
        run_one_target_diagnostic "$requested" || true
    fi

    # Diagnostics are informational. Individual audit failures are retained in
    # the output but do not turn a successfully completed read-only inspection
    # into a helper transport failure.
    return 0
}

run_host_diagnostic()
{
    local requested="${1:-}" key
    [[ -n "$requested" ]] || fail "host-diagnose requires a diagnostic name or 'all'."
    (($# == 1)) || fail "host-diagnose accepts exactly one diagnostic name or 'all'."

    CURRENT_STAGE="host read-only diagnostic"
    RUNNING_HOST_MODE=1
    prepare_running_host "$TARGET_DISK" "$ROOT_DEVICE" no
    DIAGNOSTIC_SCOPE="Running Host"
    if [[ "$requested" == all || "$requested" == report ]]; then
        # See run_target_diagnostic: the shared capability preamble is emitted
        # once at the top of the combined running-host report.
        REPORT_SHARED_PREAMBLE=1
        diagnostic_repair_capabilities || true
        echo
        for key in environment backend boot boot-evidence kernel grub uki display errors usage fstab btrfs mapper luks; do
            run_one_diagnostic "$key" "$DIAGNOSTIC_SCOPE" || true
        done
        REPORT_SHARED_PREAMBLE=0
    else
        run_one_diagnostic "$requested" "$DIAGNOSTIC_SCOPE" || true
    fi
    DIAGNOSTIC_SCOPE="Repair Target"
    return 0
}

# ---------------------------------------------------------------------------
# Guarded target configuration read/write
# ---------------------------------------------------------------------------
config_path_for_key()
{
    case "${1:-}" in
        fstab)          printf '%s\n' '/etc/fstab' ;;
        crypttab)       printf '%s\n' '/etc/crypttab' ;;
        grub-defaults)  printf '%s\n' '/etc/default/grub' ;;
        grub-config)    printf '%s\n' '/boot/grub/grub.cfg' ;;
        sddm)           printf '%s\n' '/etc/sddm.conf' ;;
        gdm)            printf '%s\n' '/etc/gdm/custom.conf' ;;
        gdm3)           printf '%s\n' '/etc/gdm3/daemon.conf' ;;
        lightdm)        printf '%s\n' '/etc/lightdm/lightdm.conf' ;;
        greetd)         printf '%s\n' '/etc/greetd/config.toml' ;;
        ly)             printf '%s\n' '/etc/ly/config.ini' ;;
        initramfs)      printf '%s\n' '/etc/initramfs-tools/initramfs.conf' ;;
        *) fail "Unknown target configuration key: ${1:-missing}" ;;
    esac
}

target_config_path()
{
    local virtual_path="$1" candidate root_real parent_real
    validate_virtual_path "$virtual_path"
    candidate="$TARGET_ROOT$virtual_path"
    root_real="$(realpath_existing "$TARGET_ROOT")" || fail "Unable to resolve mounted target root."
    parent_real="$(realpath_existing "$(dirname -- "$candidate")")" || fail "Unable to resolve configuration parent: $virtual_path"
    path_within "$parent_real" "$root_real" || fail "Configuration path escapes the selected target: $virtual_path"
    [[ ! -L "$candidate" ]] || fail "Target configuration must not be a symbolic link: $virtual_path"
    printf '%s\n' "$candidate"
}

run_target_config()
{
    local action="${1:-}" key="${2:-}" content="${3-}" virtual_path path tmp mode owner_group
    [[ "$action" == "read" || "$action" == "write" ]] || fail "config requires read or write."
    if [[ "$action" == "read" ]]; then
        [[ $# -eq 2 ]] || fail "config read received an invalid argument count."
    else
        [[ $# -eq 3 ]] || fail "config write received an invalid argument count."
    fi
    virtual_path="$(config_path_for_key "$key")"
    if [[ "$action" == "read" ]]; then
        prepare_target ro
        maybe_mount_target_path "$virtual_path" ro
        path="$(target_config_path "$virtual_path")"
        [[ -f "$path" && -r "$path" ]] || fail "Target configuration is unavailable or unreadable: $virtual_path"
        printf 'Target configuration: %s\n' "$virtual_path"
        printf 'Inspection is read-only.\n\n'
        cat -- "$path"
        return 0
    fi

    [[ ${#content} -le 262144 ]] || fail "Target configuration is limited to 256 KiB."
    prepare_target rw
    maybe_mount_target_path "$virtual_path" rw
    path="$(target_config_path "$virtual_path")"
    [[ -f "$path" ]] || fail "Target configuration does not exist; refusing to create: $virtual_path"
    mode="$(stat -c '%a' -- "$path")" || fail "Unable to inspect target configuration mode."
    owner_group="$(stat -c '%u:%g' -- "$path")" || fail "Unable to inspect target configuration ownership."
    tmp="$(mktemp "$(dirname -- "$path")/.boot-repair-config.XXXXXX")" || fail "Unable to create a temporary configuration file."
    if ! printf '%s' "$content" > "$tmp"; then
        rm -f -- "$tmp"
        fail "Unable to write temporary target configuration."
    fi
    chmod "$mode" -- "$tmp"
    chown "$owner_group" -- "$tmp"
    mv -f -- "$tmp" "$path"
    sync
    printf 'Target configuration updated: %s\n' "$virtual_path"
    printf 'The target was modified; rerun diagnostics before any repair action.\n'
}

# ---------------------------------------------------------------------------
# Btrfs snapshot inspection and transactional rollback
# ---------------------------------------------------------------------------
snapshot_b64()
{
    printf '%s' "$1" | base64 | tr -d '\n'
}

mount_snapshot_top()
{
    local fstype
    fstype="$(lsblk -ndo FSTYPE "$ROOT_CANONICAL" 2>/dev/null | head -n1 || true)"
    [[ "$fstype" == "btrfs" ]] || fail "Snapshot inspection requires a Btrfs repair root; detected ${fstype:-unknown}."
    need btrfs

    SNAPSHOT_TOP="$SESSION_DIR/snapshot-top"
    mkdir -p -- "$SNAPSHOT_TOP"
    mount_recorded "$ROOT_DEVICE" "$SNAPSHOT_TOP" -o ro,subvolid=5
}

# Enumerate numeric Snapper/Btrfs snapshot directories below every top-level
# @.snapshots directory, printing "<id>\t<absolute path>" in id order.
snapshot_paths()
{
    local base id snap
    local -a bases=()

    while IFS= read -r base; do
        [[ -d "$base" ]] && bases+=("$base")
    done < <(
        find "$SNAPSHOT_TOP" -mindepth 1 -maxdepth 3 -type d \
            \( -name '@.snapshots' -o -name '.snapshots' \) -print 2>/dev/null | sort -u
    )

    for base in "${bases[@]}"; do
        for snap in "$base"/*/snapshot; do
            [[ -d "$snap" ]] || continue
            id="$(basename -- "$(dirname -- "$snap")")"
            [[ "$id" =~ ^[0-9]+$ ]] || continue
            btrfs subvolume show "$snap" >/dev/null 2>&1 || continue
            printf '%s\t%s\n' "$id" "$snap"
        done
    done | sort -n -k1,1 -u
}

snapshot_info_file()
{
    local snap="$1" parent
    parent="$(dirname -- "$snap")"
    [[ -f "$parent/info.xml" ]] && printf '%s\n' "$parent/info.xml"
}

snapshot_xml_value()
{
    local file="$1" tag="$2"
    [[ -f "$file" ]] || return 0
    sed -n "s#.*<$tag>\\(.*\\)</$tag>.*#\\1#p" "$file" | head -n1
}

# Emit one base64-encoded SNAPSHOT protocol record per Snapper/Btrfs root
# snapshot found under the top-level @.snapshots directory (read-only).
list_snapshots()
{
    # The optional active subvolume id marks the snapshot the running system is
    # currently executing from (or that is the Btrfs default) so host scope
    # never offers it as a rollback target.
    local active_id="${1:-}" id snap info created type desc status ro_prop rel
    local count=0

    while IFS=$'\t' read -r id snap; do
        [[ -n "$id" && -n "$snap" ]] || continue
        info="$(snapshot_info_file "$snap" || true)"
        created="$(btrfs subvolume show "$snap" 2>/dev/null \
            | sed -n 's/^[[:space:]]*Creation time:[[:space:]]*//p' | head -n1)"
        [[ -n "$created" ]] || created="$(stat -c '%y' "$snap" 2>/dev/null | cut -d. -f1 || true)"
        type="$(snapshot_xml_value "$info" type || true)"
        desc="$(snapshot_xml_value "$info" description || true)"
        ro_prop="$(btrfs property get -ts "$snap" ro 2>/dev/null | awk -F= '$1=="ro" {print $2; exit}')"
        if [[ -n "$active_id" && "$(btrfs_subvol_id "$snap" 2>/dev/null || true)" == "$active_id" ]]; then
            status="Active root snapshot (currently running)"
        elif [[ -f "$snap/etc/os-release" ]]; then
            status="Linux root snapshot${ro_prop:+; ro=$ro_prop}"
        else
            status="Incomplete/non-root snapshot${ro_prop:+; ro=$ro_prop}"
        fi
        rel="${snap#"$SNAPSHOT_TOP"/}"
        printf 'SNAPSHOT\t%s\t%s\t%s\t%s\t%s\t%s\n' \
            "$id" \
            "$(snapshot_b64 "$created")" \
            "$(snapshot_b64 "$type")" \
            "$(snapshot_b64 "$desc")" \
            "$(snapshot_b64 "$status")" \
            "$(snapshot_b64 "$rel")"
        count=$((count + 1))
    done < <(snapshot_paths)

    log "Snapshot inventory complete: $count snapshot(s) found."
}

snapshot_find_path()
{
    local requested="$1" id candidate
    [[ "$requested" =~ ^[0-9]+$ ]] || return 1
    while IFS=$'\t' read -r id candidate; do
        [[ "$id" == "$requested" ]] || continue
        printf '%s\n' "$candidate"
        return 0
    done < <(snapshot_paths)
    return 1
}

snapshot_root_valid()
{
    local snap="$1"
    [[ -f "$snap/etc/os-release" ]] || return 1
    [[ -f "$snap/etc/fstab" ]] || return 1
    btrfs subvolume show "$snap" >/dev/null 2>&1 || return 1
    return 0
}

snapshot_has_separate_boot()
{
    local snap="$1"
    # /boot/efi being separate is normal and does not explain missing kernel
    # files in the root snapshot. Only a real /boot fstab entry does.
    awk '
        /^[[:space:]]*#/ { next }
        NF >= 3 && $2 == "/boot" { found=1 }
        END { exit(found ? 0 : 1) }
    ' "$snap/etc/fstab" 2>/dev/null
}

validate_snapshot_fstab_for_rollback()
{
    local snap="$1" spec mp fstype options _rest resolved option has_root=0
    [[ -f "$snap/etc/fstab" ]] || return 1

    while read -r spec mp fstype options _rest; do
        [[ -n "$spec" && "${spec:0:1}" != "#" ]] || continue
        mp="$(fstab_unescape "$mp")"
        options="$(fstab_unescape "$options")"
        [[ "$mp" == "/" ]] || continue
        has_root=1
        [[ "$fstype" == "btrfs" ]] || {
            log "Rollback preflight: snapshot root fstab type is '$fstype', not btrfs." | tee -a "$SESSION_LOG"
            return 1
        }

        resolved="$(resolve_fstab_source "$spec")"
        if [[ -n "$resolved" && -b "$resolved" ]]; then
            resolved="$(canonical_block "$resolved")" || return 1
            [[ "$resolved" == "$ROOT_CANONICAL" ]] || {
                log "Rollback preflight: snapshot / fstab source resolves to $resolved rather than selected root $ROOT_CANONICAL." | tee -a "$SESSION_LOG"
                return 1
            }
        elif [[ "$spec" == /dev/* || "$spec" == UUID=* || "$spec" == PARTUUID=* || "$spec" == LABEL=* || "$spec" == PARTLABEL=* ]]; then
            log "Rollback preflight: snapshot / fstab source cannot be resolved: $spec" | tee -a "$SESSION_LOG"
            return 1
        else
            log "Rollback preflight: snapshot / fstab root source has an unsupported format: $spec" | tee -a "$SESSION_LOG"
            return 1
        fi

        IFS=',' read -ra parts <<< "$options"
        for option in "${parts[@]}"; do
            case "$option" in
                subvol=*)
                    option="${option#subvol=}"
                    option="${option#/}"
                    [[ "$option" == "@" ]] || {
                        log "Rollback preflight: snapshot fstab expects root subvolume '$option', but transactional rollback promotes the selected snapshot to @." | tee -a "$SESSION_LOG"
                        return 1
                    }
                    ;;
            esac
        done
        break
    done < "$snap/etc/fstab"

    ((has_root == 1)) || {
        log "Rollback preflight: snapshot fstab has no root (/) entry." | tee -a "$SESSION_LOG"
        return 1
    }
    log "Rollback preflight: snapshot root fstab is compatible with promoted @." | tee -a "$SESSION_LOG"
    return 0
}

validate_snapshot_crypttab_for_rollback()
{
    local snap="$1" rc=0
    [[ -f "$snap/etc/crypttab" ]] || return 0
    set +e
    ( TARGET_ROOT="$snap"; validate_mapper_crypttab )
    rc=$?
    set -e
    ((rc == 0)) || return "$rc"
    return 0
}

snapshot_kernel_pair_audit()
{
    local root="$1" kernel version failures=0
    local -a kernels=()
    shopt -s nullglob
    kernels=("$root"/boot/vmlinuz-*)
    if ((${#kernels[@]} == 0)); then
        if snapshot_has_separate_boot "$root"; then
            echo "  INFO: /boot is separate according to snapshot fstab; kernel files will be validated after target /boot is mounted."
            shopt -u nullglob
            return 0
        fi
        echo "  FAIL: no vmlinuz files are visible in the snapshot root."
        shopt -u nullglob
        return 1
    fi
    for kernel in "${kernels[@]}"; do
        version="${kernel##*/vmlinuz-}"
        # Debian names images initrd.img-<kver>; Arch/mkinitcpio uses the
        # kernel flavor (initramfs-linux.img), so both pairings are accepted.
        if [[ -f "$root/boot/initrd.img-$version" || -f "$root/boot/initramfs-$version.img" ]]; then
            echo "  PASS: $version has matching initramfs."
        else
            echo "  FAIL: $version has no matching initramfs."
            failures=$((failures + 1))
        fi
    done
    shopt -u nullglob
    ((failures == 0))
}

# Print a read-only report for one snapshot: subvolume metadata, root
# validation, kernel/initramfs pairing, fstab and crypttab contents.
inspect_snapshot()
{
    local requested="$1" snap info rel pretty
    [[ "$requested" =~ ^[0-9]+$ ]] || fail "Snapshot id must be numeric."
    snap="$(snapshot_find_path "$requested" || true)"
    [[ -n "$snap" ]] || fail "Snapshot $requested was not found on the selected Btrfs filesystem."

    info="$(snapshot_info_file "$snap" || true)"
    rel="${snap#"$SNAPSHOT_TOP"/}"

    echo "Snapshot: $requested"
    echo "Path: $rel"
    echo
    btrfs subvolume show "$snap" 2>&1 || true
    echo
    if [[ -n "$info" ]]; then
        echo "Snapper metadata:"
        echo "  Type: $(snapshot_xml_value "$info" type || true)"
        echo "  Description: $(snapshot_xml_value "$info" description || true)"
        echo
    fi

    echo "Root validation:"
    if [[ -f "$snap/etc/os-release" ]]; then
        pretty="$(awk -F= '$1=="PRETTY_NAME" {sub(/^[^=]*=/, ""); gsub(/^"|"$/, ""); print; exit}' "$snap/etc/os-release")"
        echo "  PASS: /etc/os-release exists (${pretty:-Linux})"
    else
        echo "  FAIL: /etc/os-release is missing"
    fi
    [[ -f "$snap/etc/fstab" ]] && echo "  PASS: /etc/fstab exists" || echo "  FAIL: /etc/fstab is missing"
    [[ -d "$snap/boot" ]] && echo "  PASS: /boot exists" || echo "  WARN: /boot is not present inside this root snapshot"

    echo
    echo "Kernel/initramfs pairing:"
    snapshot_kernel_pair_audit "$snap" || true

    echo
    echo "Snapshot fstab:"
    sed -n '1,160p' "$snap/etc/fstab" 2>/dev/null || echo "  unavailable"

    echo
    echo "Snapshot crypttab:"
    sed -n '1,120p' "$snap/etc/crypttab" 2>/dev/null || echo "  unavailable"

    echo
    echo "Inspection is read-only. No snapshot, subvolume, boot file or package state was modified."
}

btrfs_subvol_id()
{
    local path="$1" id
    id="$(btrfs inspect-internal rootid "$path" 2>/dev/null || true)"
    if [[ ! "$id" =~ ^[0-9]+$ ]]; then
        id="$(btrfs subvolume show "$path" 2>/dev/null | sed -n 's/^[[:space:]]*Subvolume ID:[[:space:]]*\([0-9][0-9]*\).*/\1/p' | head -1)"
    fi
    [[ "$id" =~ ^[0-9]+$ ]] || return 1
    printf '%s\n' "$id"
}

snapshot_current_root_path()
{
    local path="$SNAPSHOT_TOP/@"
    [[ -d "$path" ]] || return 1
    btrfs subvolume show "$path" >/dev/null 2>&1 || return 1
    printf '%s\n' "$path"
}

snapshot_free_space_ok()
{
    local available_kb
    available_kb="$(df -Pk "$SNAPSHOT_TOP" 2>/dev/null | awk 'NR==2 {print $4}')"
    [[ "$available_kb" =~ ^[0-9]+$ ]] || return 1
    # The rollback is COW, but boot-stack regeneration needs working room.
    (( available_kb >= 1048576 ))
}

snapshot_separate_subvolumes()
{
    local snap="$1" spec mp fstype options
    [[ -f "$snap/etc/fstab" ]] || return 0
    while read -r spec mp fstype options _rest; do
        [[ -n "$spec" && "${spec:0:1}" != "#" ]] || continue
        [[ "$fstype" == "btrfs" && "$mp" != "/" ]] || continue
        mp="$(fstab_unescape "$mp")"
        options="$(fstab_unescape "$options")"
        printf '  %s  (%s)\n' "$mp" "$options"
    done < "$snap/etc/fstab"
}

# Validate one snapshot for a safe @ rollback and print the complete
# transactional plan (read-only; emits the PLAN_OK=1 marker for the GUI).
snapshot_rollback_plan()
{
    local requested="$1" snap current info pretty created root_id default_line rel
    snap="$(snapshot_find_path "$requested" || true)"
    [[ -n "$snap" ]] || fail "Snapshot $requested was not found on the selected Btrfs filesystem."
    snapshot_root_valid "$snap" || fail "Snapshot $requested is not a valid Linux root snapshot (/etc/os-release and /etc/fstab are required)."
    validate_snapshot_fstab_for_rollback "$snap" || fail "Snapshot $requested fstab is not compatible with a safe @ rollback."
    validate_snapshot_crypttab_for_rollback "$snap" || fail "Snapshot $requested crypttab does not resolve safely on the selected target disk."
    current="$(snapshot_current_root_path || true)"
    [[ -n "$current" ]] || fail "The selected Btrfs filesystem has no top-level @ root subvolume; transactional rollback is limited to @-root layouts in this release."
    snapshot_free_space_ok || fail "Less than 1 GiB of free Btrfs space is available; refusing transactional rollback."

    info="$(snapshot_info_file "$snap" || true)"
    pretty="$(awk -F= '$1=="PRETTY_NAME" {sub(/^[^=]*=/, ""); gsub(/^"|"$/, ""); print; exit}' "$snap/etc/os-release")"
    created="$(btrfs subvolume show "$snap" 2>/dev/null | sed -n 's/^[[:space:]]*Creation time:[[:space:]]*//p' | head -1)"
    root_id="$(btrfs_subvol_id "$current" || true)"
    default_line="$(btrfs subvolume get-default "$SNAPSHOT_TOP" 2>/dev/null || true)"
    rel="${snap#"$SNAPSHOT_TOP"/}"

    echo "========================================"
    echo "TRANSACTIONAL BTRFS ROLLBACK PLAN"
    echo "========================================"
    echo "Target disk: $TARGET_DISK"
    echo "Btrfs filesystem: $ROOT_DEVICE"
    echo "Current normal root: @${root_id:+ (subvolume ID $root_id)}"
    echo "Current default subvolume: ${default_line:-unknown}"
    echo
    echo "Selected snapshot: $requested"
    echo "Snapshot path: $rel"
    echo "Snapshot OS: ${pretty:-Linux}"
    [[ -n "$created" ]] && echo "Created: $created"
    [[ -n "$info" ]] && echo "Snapper type: $(snapshot_xml_value "$info" type || true)"
    [[ -n "$info" ]] && echo "Description: $(snapshot_xml_value "$info" description || true)"
    echo
    echo "Preflight kernel/initramfs evidence:"
    snapshot_kernel_pair_audit "$snap" || true
    echo
    echo "Separate Btrfs subvolumes are retained rather than rolled back:"
    snapshot_separate_subvolumes "$snap"
    echo
    echo "Rollback transaction:"
    echo "  1. Keep the selected snapshot unchanged/read-only."
    echo "  2. Create a new writable snapshot candidate."
    echo "  3. Preserve the existing @ under a timestamped @rollback-before-* name."
    echo "  4. Promote the candidate to @ and set it as the Btrfs default subvolume."
    echo "  5. Mount the promoted @ with its real fstab Btrfs subvolumes."
    echo "  6. Validate fstab/crypttab and installed kernel state."
    echo "  7. Rebuild initramfs, TUXEDO UKI when applicable, and GRUB configuration."
    echo "  8. Verify the promoted root and boot artifacts."
    echo "  9. If a critical post-switch stage fails, restore the preserved @ automatically and reconcile its boot stack."
    echo
    echo "No source snapshot will be deleted. The previous @ remains preserved after a successful rollback."
    echo "PLAN_OK=1"
}

# Unmount every target mount except the Btrfs top-level (SNAPSHOT_TOP) before
# the atomic @ name switch.  A busy mount aborts the rollback while the old
# root is still the only active normal root; lazy detach is never used.
snapshot_unmount_target_keep_top()
{
    local idx path
    local -a keep=()
    set +e
    for (( idx=${#MOUNTS[@]}-1; idx>=0; --idx )); do
        path="${MOUNTS[$idx]}"
        [[ -n "$path" ]] || continue
        if [[ "$path" == "$SNAPSHOT_TOP" ]]; then
            keep+=("$path")
            continue
        fi
        if mountpoint -q "$path" 2>/dev/null; then
            # A root-name switch must never proceed after a lazy detach.  If a
            # target mount is busy, abort before renaming @ so the old root is
            # still the only active normal root.
            umount "$path" 2>/dev/null || {
                set -e
                return 1
            }
        fi
    done
    set -e
    MOUNTS=()
    if mountpoint -q "$SNAPSHOT_TOP" 2>/dev/null; then
        MOUNTS+=("$SNAPSHOT_TOP")
    fi
    TARGET_DATA_MOUNTS=()
    TARGET_ROOT=""
    return 0
}

# Mount the promoted @ subvolume read-write with its fstab subvolumes, boot
# entries and chroot pseudo-filesystems so post-switch reconciliation runs
# against the rolled-back root.
snapshot_mount_promoted_root_rw()
{
    ROOT_DEVICE="$(preferred_block_path "$ROOT_DEVICE" "$ROOT_CANONICAL")"
    mkdir -p "$MOUNT_BASE"
    mount_recorded "$ROOT_DEVICE" "$MOUNT_BASE" -o rw,subvol=@
    TARGET_ROOT="$MOUNT_BASE"
    TARGET_SUBVOL="@"
    read_target_os
    prepare_mapper_compatibility_aliases
    mount_target_btrfs_subvolumes rw
    mount_target_boot_entry "/boot" rw
    mount_target_boot_entry "/boot/efi" rw
    mount_special dev-rw none "$TARGET_ROOT/dev"
    mount_special proc proc "$TARGET_ROOT/proc"
    mount_special rbind-ro /sys "$TARGET_ROOT/sys"
    mount_special tmpfs none "$TARGET_ROOT/run"
}

snapshot_verify_installed_kernels()
{
    local kernel version failures=0
    local -a kernels=()
    shopt -s nullglob
    kernels=("$TARGET_ROOT"/boot/vmlinuz-*)
    ((${#kernels[@]} > 0)) || fail "Promoted rollback root has no installed kernel under /boot."
    for kernel in "${kernels[@]}"; do
        version="${kernel##*/vmlinuz-}"
        if [[ -f "$TARGET_ROOT/boot/initramfs-$version.img" ]]; then
            # Arch/mkinitcpio flavor pairing: vmlinuz-<flavor> pairs with
            # initramfs-<flavor>.img, while module directories are named after
            # the full kernel version and cannot be derived from the flavor.
            # The mkinitcpio rebuild below proves the module/image pairing.
            log "Rollback kernel pair before rebuild: $version (mkinitcpio flavor naming)" | tee -a "$SESSION_LOG"
            continue
        fi
        if [[ -f "$TARGET_ROOT/boot/initrd.img-$version" ]]; then
            log "Rollback kernel pair before rebuild: $version" | tee -a "$SESSION_LOG"
        else
            log "Rollback kernel $version is missing initramfs before rebuild; update-initramfs will be asked to regenerate it." | tee -a "$SESSION_LOG"
        fi
        [[ -d "$TARGET_ROOT/usr/lib/modules/$version" || -d "$TARGET_ROOT/lib/modules/$version" ]] || {
            log "Rollback kernel $version has no matching modules directory." | tee -a "$SESSION_LOG"
            failures=$((failures + 1))
        }
    done
    shopt -u nullglob
    ((failures == 0)) || fail "Promoted rollback root has kernel/module inconsistencies."
}

# Validate the promoted @ root (metadata, mapper/crypttab, kernels, dpkg
# audit) and rebuild initramfs, TUXEDO UKI and GRUB against it.  Any failure
# makes the caller restore the preserved pre-rollback root.
snapshot_post_switch_reconcile()
{
    local audit="" current
    [[ "$TARGET_SUBVOL" == "@" ]] || fail "Post-rollback root is not mounted from @."
    [[ -f "$TARGET_ROOT/etc/os-release" && -f "$TARGET_ROOT/etc/fstab" ]] || fail "Promoted @ is missing required Linux root metadata."
    validate_mapper_crypttab
    snapshot_verify_installed_kernels

    if [[ -x "$TARGET_ROOT/usr/bin/dpkg" || -x "$TARGET_ROOT/usr/bin/dpkg-query" ]]; then
        audit="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin dpkg --audit 2>/dev/null || true)"
        if [[ -n "$audit" ]]; then
            printf '%s\n' "$audit" | tee -a "$SESSION_LOG"
            fail "The selected snapshot contains an incomplete dpkg state; repair packages before relying on this rollback."
        fi
    fi

    log "Rollback boot-stack reconciliation: using simulation-first adaptive component workflows." | tee -a "$SESSION_LOG"
    adaptive_initramfs_repair
    if is_tuxedo_uki_layout; then
        preflight_tuxedo_uki
        rebuild_tuxedo_uki
        verify_tuxedo_uki_root_binding
    else
        log "No TUXEDO UKI builder detected; retaining the distribution's existing EFI layout." | tee -a "$SESSION_LOG"
    fi
    adaptive_grub_stage

    local grub_cfg
    grub_cfg="$(grub_config_path)"
    [[ -s "$TARGET_ROOT$grub_cfg" ]] || fail "GRUB regeneration completed but $grub_cfg is missing or empty."
    current="$(current_btrfs_subvol 2>/dev/null || true)"
    [[ "$current" == "@" ]] || fail "Post-rollback mount verification resolved '${current:-unknown}' instead of @."
    log "PASS: promoted rollback root and boot stack validated." | tee -a "$SESSION_LOG"
}

# Recovery path for a failed rollback: detach the promoted mounts, rename the
# failed candidate out of @, promote the preserved @rollback-before-* root back
# to @, restore the previous Btrfs default subvolume and reconcile its boot
# stack.  Returns non-zero when any step leaves the system without a safe root.
snapshot_restore_preserved_root()
{
    local top="$1" backup_name="$2" failed_name="$3" old_default_id="$4" current_id restore_rc=0
    log "ROLLBACK RECOVERY: restoring preserved pre-rollback @." | tee -a "$SESSION_LOG"

    if ! snapshot_unmount_target_keep_top; then
        log "CRITICAL: cannot detach promoted rollback mounts; refusing to rename Btrfs roots during recovery." | tee -a "$SESSION_LOG"
        return 1
    fi
    if ! mount -o remount,rw "$top" 2>/dev/null; then
        log "CRITICAL: could not remount the Btrfs top-level filesystem read-write during rollback recovery." | tee -a "$SESSION_LOG"
        return 1
    fi

    if [[ ! -d "$top/$backup_name" ]]; then
        log "CRITICAL: preserved root $backup_name is missing; refusing to move the active rollback candidate." | tee -a "$SESSION_LOG"
        return 1
    fi
    if [[ -e "$top/@" ]]; then
        if ! mv -- "$top/@" "$top/$failed_name"; then
            log "CRITICAL: could not move failed rollback candidate out of @; refusing to promote the preserved root." | tee -a "$SESSION_LOG"
            return 1
        fi
    fi
    mv -- "$top/$backup_name" "$top/@" || return 1

    if [[ "$old_default_id" =~ ^[0-9]+$ ]]; then
        if ! btrfs subvolume set-default "$old_default_id" "$top" 2>&1 | tee -a "$SESSION_LOG"; then
            log "CRITICAL: could not restore the previous Btrfs default subvolume." | tee -a "$SESSION_LOG"
            return 1
        fi
    else
        current_id="$(btrfs_subvol_id "$top/@" || true)"
        if [[ -n "$current_id" ]]; then
            if ! btrfs subvolume set-default "$current_id" "$top" 2>&1 | tee -a "$SESSION_LOG"; then
                log "CRITICAL: could not restore the previous Btrfs default subvolume." | tee -a "$SESSION_LOG"
                return 1
            fi
        else
            log "CRITICAL: could not determine the preserved root subvolume ID." | tee -a "$SESSION_LOG"
            return 1
        fi
    fi

    snapshot_mount_promoted_root_rw || return 1
    set +e
    ( snapshot_post_switch_reconcile ) 2>&1 | tee -a "$SESSION_LOG"
    restore_rc=${PIPESTATUS[0]}
    set -e
    if ((restore_rc != 0)); then
        log "CRITICAL: original @ was restored, but its boot-stack reconciliation also failed. Manual boot repair is required before reboot." | tee -a "$SESSION_LOG"
        return 1
    fi
    log "PASS: original @ and its boot stack were restored automatically. Failed rollback candidate retained as $failed_name." | tee -a "$SESSION_LOG"
    return 0
}

# Execute the transactional rollback: snapshot to a writable candidate,
# preserve the current @ under @rollback-before-*, promote the candidate,
# reconcile initramfs/UKI/GRUB and automatically restore the preserved root
# when any critical post-switch stage fails.
rollback_snapshot()
{
    local requested="$1" snap current stamp candidate_name backup_name failed_name candidate_path
    local old_default_line old_default_id candidate_id rc output pretty

    snap="$(snapshot_find_path "$requested" || true)"
    [[ -n "$snap" ]] || fail "Snapshot $requested was not found on the selected Btrfs filesystem."
    snapshot_root_valid "$snap" || fail "Snapshot $requested is not a valid Linux root snapshot."
    validate_snapshot_fstab_for_rollback "$snap" || fail "Snapshot $requested fstab is not compatible with a safe @ rollback."
    validate_snapshot_crypttab_for_rollback "$snap" || fail "Snapshot $requested crypttab does not resolve safely on the selected target disk."
    current="$(snapshot_current_root_path || true)"
    [[ -n "$current" ]] || fail "Transactional rollback requires a normal top-level @ root subvolume."
    snapshot_free_space_ok || fail "Less than 1 GiB of free Btrfs space is available; refusing rollback."

    pretty="$(awk -F= '$1=="PRETTY_NAME" {sub(/^[^=]*=/, ""); gsub(/^"|"$/, ""); print; exit}' "$snap/etc/os-release")"
    old_default_line="$(btrfs subvolume get-default "$SNAPSHOT_TOP" 2>/dev/null || true)"
    old_default_id="$(sed -n 's/^ID[[:space:]]\+\([0-9][0-9]*\).*/\1/p' <<< "$old_default_line" | head -1)"
    stamp="$(date +%Y%m%d-%H%M%S)"
    candidate_name="@rollback-new-$stamp"
    backup_name="@rollback-before-$stamp"
    failed_name="@rollback-failed-$stamp"
    candidate_path="$SNAPSHOT_TOP/$candidate_name"

    for path in "$candidate_path" "$SNAPSHOT_TOP/$backup_name" "$SNAPSHOT_TOP/$failed_name"; do
        [[ ! -e "$path" ]] || fail "Rollback staging path already exists: ${path#"$SNAPSHOT_TOP"/}"
    done

    log "Transactional rollback selected: snapshot $requested (${pretty:-Linux})." | tee -a "$SESSION_LOG"
    log "Preserved root name: $backup_name" | tee -a "$SESSION_LOG"
    log "Rollback source snapshot will remain unchanged." | tee -a "$SESSION_LOG"

    # This is the exact point where rollback crosses from read-only planning to
    # a modifying transaction.  Anything before this line must leave the target
    # untouched, including session-log persistence.
    TARGET_WRITE_INTENT=1
    mount -o remount,rw "$SNAPSHOT_TOP"
    log "Creating writable rollback candidate $candidate_name" | tee -a "$SESSION_LOG"
    btrfs subvolume snapshot "$snap" "$candidate_path" 2>&1 | tee -a "$SESSION_LOG"
    snapshot_root_valid "$candidate_path" || {
        btrfs subvolume delete "$candidate_path" >/dev/null 2>&1 || true
        fail "Writable rollback candidate failed Linux-root validation; current @ was not changed."
    }

    log "Detaching target mounts before atomic @ name switch." | tee -a "$SESSION_LOG"
    snapshot_unmount_target_keep_top || {
        btrfs subvolume delete "$candidate_path" >/dev/null 2>&1 || true
        fail "Could not cleanly detach the target; current @ was not changed."
    }

    mv -- "$SNAPSHOT_TOP/@" "$SNAPSHOT_TOP/$backup_name" || {
        btrfs subvolume delete "$candidate_path" >/dev/null 2>&1 || true
        fail "Could not preserve current @; rollback was not performed."
    }
    if ! mv -- "$candidate_path" "$SNAPSHOT_TOP/@"; then
        if mv -- "$SNAPSHOT_TOP/$backup_name" "$SNAPSHOT_TOP/@" 2>/dev/null; then
            fail "Could not promote rollback candidate to @; original @ name was restored."
        fi
        fail "Could not promote rollback candidate or restore the original @ name. Do not reboot."
    fi

    candidate_id="$(btrfs_subvol_id "$SNAPSHOT_TOP/@" || true)"
    [[ -n "$candidate_id" ]] || {
        snapshot_restore_preserved_root "$SNAPSHOT_TOP" "$backup_name" "$failed_name" "$old_default_id" || true
        fail "Unable to determine promoted @ subvolume ID. Original root restoration was attempted."
    }
    set +e
    btrfs subvolume set-default "$candidate_id" "$SNAPSHOT_TOP" 2>&1 | tee -a "$SESSION_LOG"
    rc=${PIPESTATUS[0]}
    set -e
    if ((rc != 0)); then
        log "Rollback candidate default-subvolume update failed; automatically restoring the preserved root." | tee -a "$SESSION_LOG"
        if snapshot_restore_preserved_root "$SNAPSHOT_TOP" "$backup_name" "$failed_name" "$old_default_id"; then
            log "ROLLBACK FAILED SAFELY: original @ restored after default-subvolume failure; failed candidate retained as $failed_name." | tee -a "$SESSION_LOG"
        else
            log "CRITICAL ROLLBACK FAILURE: could not restore original @ after default-subvolume failure. Do not reboot." | tee -a "$SESSION_LOG"
        fi
        return 1
    fi
    sync

    if ! snapshot_mount_promoted_root_rw; then
        log "Rollback candidate mount/preflight failed; automatically restoring the preserved root." | tee -a "$SESSION_LOG"
        if snapshot_restore_preserved_root "$SNAPSHOT_TOP" "$backup_name" "$failed_name" "$old_default_id"; then
            log "ROLLBACK FAILED SAFELY: original @ restored after candidate mount failure; failed candidate retained as $failed_name." | tee -a "$SESSION_LOG"
        else
            log "CRITICAL ROLLBACK FAILURE: could not restore original @ after candidate mount failure. Do not reboot." | tee -a "$SESSION_LOG"
        fi
        return 1
    fi
    set +e
    output="$( ( snapshot_post_switch_reconcile ) 2>&1 )"
    rc=$?
    set -e
    printf '%s\n' "$output" | tee -a "$SESSION_LOG"

    if ((rc != 0)); then
        log "Rollback candidate failed post-switch validation/boot reconciliation; automatically restoring the preserved root." | tee -a "$SESSION_LOG"
        if snapshot_restore_preserved_root "$SNAPSHOT_TOP" "$backup_name" "$failed_name" "$old_default_id"; then
            log "ROLLBACK FAILED SAFELY: original @ restored; failed candidate retained as $failed_name." | tee -a "$SESSION_LOG"
        else
            log "CRITICAL ROLLBACK FAILURE: automatic restoration was incomplete. Do not reboot until Btrfs/boot state is inspected manually." | tee -a "$SESSION_LOG"
        fi
        return 1
    fi

    sync
    log "========================================" | tee -a "$SESSION_LOG"
    log "SNAPSHOT ROLLBACK COMPLETE" | tee -a "$SESSION_LOG"
    log "Selected snapshot: $requested" | tee -a "$SESSION_LOG"
    log "Active writable root: @ (subvolume ID $candidate_id)" | tee -a "$SESSION_LOG"
    log "Previous root retained as: $backup_name" | tee -a "$SESSION_LOG"
    log "Source snapshot retained unchanged." | tee -a "$SESSION_LOG"
    log "Btrfs default subvolume now points to the promoted @." | tee -a "$SESSION_LOG"
    log "Initramfs/UKI/GRUB reconciliation: PASS" | tee -a "$SESSION_LOG"
    log "ROLLBACK_RESULT=SUCCESS" | tee -a "$SESSION_LOG"
}

# snapshots dispatch: list|inspect|plan|rollback against the mounted top-level
# Btrfs filesystem; validates the argument count per action. A non-Btrfs target
# has no Btrfs snapshot inventory: the read-only list reports that as an
# informational outcome (exit 0) so the GUI never shows a failed state for a
# filesystem that cannot carry Btrfs snapshots. Inspect/plan/rollback remain
# explicit errors because they name a concrete Btrfs operation.
run_snapshots()
{
    local action="${1:-}" requested="${2:-}" fstype
    need base64
    need tr
    need find
    need sed
    need stat
    case "$action" in
        list)
            [[ $# -eq 1 ]] || fail "snapshots list does not accept a snapshot id."
            ;;
        inspect|plan|rollback)
            [[ $# -eq 2 ]] || fail "snapshots $action requires exactly one snapshot id."
            ;;
        *)
            fail "Unknown snapshots action: ${action:-missing}. Use list, inspect, plan or rollback."
            ;;
    esac

    prepare_target ro
    fstype="$(lsblk -ndo FSTYPE "$ROOT_CANONICAL" 2>/dev/null | head -n1 || true)"
    if [[ "$fstype" != "btrfs" ]]; then
        if [[ "$action" == "list" ]]; then
            printf 'Snapshot inventory is not applicable: the selected target filesystem is %s, not Btrfs.\n' "${fstype:-unknown}"
            printf 'SNAPSHOT_INVENTORY_NOT_APPLICABLE=1\n'
            log "Snapshot inventory skipped: ${fstype:-unknown} is not Btrfs." | tee -a "$SESSION_LOG"
            return 0
        fi
        fail "Snapshot inspection requires a Btrfs repair root; detected ${fstype:-unknown}."
    fi
    need btrfs
    mount_snapshot_top

    case "$action" in
        list) list_snapshots ;;
        inspect) inspect_snapshot "$requested" ;;
        plan) snapshot_rollback_plan "$requested" ;;
        rollback) rollback_snapshot "$requested" ;;
    esac
}

# ---------------------------------------------------------------------------
# Running-host Btrfs snapshot inventory and name-preserving rollback
# ---------------------------------------------------------------------------
# The host mechanism is deliberately NOT `snapper rollback`: on name-pinned @
# roots (TUXEDO OS/Debian, Arch) snapper only sets the Btrfs default subvolume
# and never edits fstab, GRUB or the UKI, so it would be a boot no-op.  Boot
# Bitch instead reuses the proven target transaction shape live: preserve the
# mounted @ under @rollback-before-*, promote a writable copy of the selected
# snapshot to @, migrate a nested @/.snapshots child subvolume, reconcile the
# boot stack in a scratch chroot and automatically restore the preserved root
# on failure.  The running kernel keeps serving the old subvolume until reboot.

# Path of the running root subvolume relative to the Btrfs top level.  The live
# mount option is preferred; a default-subvolume mount is resolved through the
# running root's subvolume id.  Prints nothing and returns 1 when the running
# root is not a named subvolume (raw top-level root).
host_snapshot_running_path()
{
    local id path
    path="$(current_btrfs_subvol 2>/dev/null || true)"
    if [[ -n "$path" ]]; then
        printf '%s\n' "$path"
        return 0
    fi
    id="$(btrfs_subvol_id / 2>/dev/null || true)"
    [[ "$id" =~ ^[0-9]+$ ]] || return 1
    if [[ "$id" == "5" ]]; then
        printf '/\n'
        return 0
    fi
    path="$(btrfs subvolume list / 2>/dev/null \
        | sed -n "s/^ID[[:space:]]\+${id}[[:space:]].*[[:space:]]path[[:space:]]\+\(.*\)$/\1/p" \
        | head -n1)"
    [[ -n "$path" ]] || return 1
    printf '%s\n' "$path"
}

# The Snapper root configuration is the required snapshot producer/metadata
# source.  A home or foreign configuration is never used as a rollback source.
host_snapshot_snapper_config_ok()
{
    local cfg="/etc/snapper/configs/root"
    [[ -f "$cfg" ]] || return 1
    grep -Eq '^[[:space:]]*SUBVOLUME[[:space:]]*=[[:space:]]*"/"' "$cfg" || return 1
    grep -Eq '^[[:space:]]*FSTYPE[[:space:]]*=[[:space:]]*"btrfs"' "$cfg" || return 1
    return 0
}

# A name-preserving rename cannot change a subvolid= binding: the next boot
# would keep mounting the old subvolume by id.  Refuse any subvolid= pin.
host_snapshot_subvolid_pinned()
{
    if [[ -r /proc/cmdline ]] && grep -Eq '(^|[[:space:]])rootflags=[^[:space:]]*subvolid=' /proc/cmdline; then
        return 0
    fi
    [[ -r /etc/fstab ]] || return 1
    awk '
        /^[[:space:]]*#/ { next }
        NF >= 4 && $4 ~ /(^|,)subvolid=/ { found=1 }
        END { exit(found ? 0 : 1) }
    ' /etc/fstab
}

# A separate /boot filesystem is not part of the root snapshot, so the
# promoted root may not match the running kernels/modules.  Refuse it.
host_snapshot_has_separate_boot()
{
    local root_src boot_src
    root_src="$(findmnt -rn -o SOURCE --target / 2>/dev/null | head -n1 || true)"
    boot_src="$(findmnt -rn -o SOURCE --target /boot 2>/dev/null | head -n1 || true)"
    if [[ -n "$root_src" && -n "$boot_src" ]]; then
        if [[ "${root_src%%\[*}" != "${boot_src%%\[*}" ]]; then
            return 0
        fi
        if [[ "$root_src" == *"["* && "$boot_src" == *"["* \
              && "${root_src##*[}" != "${boot_src##*[}" ]]; then
            return 0
        fi
    fi
    [[ -r /etc/fstab ]] || return 1
    awk '
        /^[[:space:]]*#/ { next }
        NF >= 3 && $2 == "/boot" { found=1 }
        END { exit(found ? 0 : 1) }
    ' /etc/fstab
}

# Timeshift-btrfs snapshots have their own restore tooling and no Snapper
# metadata; a host with that inventory is recognized and refused, never
# silently ignored.
host_snapshot_timeshift_managed()
{
    if [[ -n "${SNAPSHOT_TOP:-}" && -d "$SNAPSHOT_TOP/timeshift-btrfs" ]]; then
        return 0
    fi
    local cfg
    for cfg in /etc/timeshift.json /etc/timeshift/timeshift.json; do
        [[ -f "$cfg" ]] || continue
        grep -Eqi 'btrfs' "$cfg" && return 0
    done
    return 1
}

# Nested child subvolumes of the running @ root.  Only @/.snapshots is
# migrated by this release; any other nested subvolume would be dragged into
# the preserved backup root and is refused instead of silently losing data.
# `btrfs subvolume list -o` prints top-level-relative paths, unlike the default
# list output whose nested paths are relative to their parent subvolume.
host_snapshot_nested_child_paths()
{
    btrfs subvolume list -o / 2>/dev/null \
        | sed -n 's/.*[[:space:]]path[[:space:]]\+\(.*\)$/\1/p'
}

host_snapshot_other_nested_children()
{
    host_snapshot_nested_child_paths \
        | awk '$0 ~ /^@\// && $0 !~ /^@\/\.snapshots(\/|$)/ { print }'
}

# True when the running @ root carries a nested @/.snapshots child subvolume
# (bare `snapper create-config` layout) that must be migrated on promotion.
host_snapshot_nested_snapshots_child()
{
    host_snapshot_nested_child_paths | grep -Fxq '@/.snapshots'
}

host_snapshot_free_space_ok()
{
    local available_kb
    available_kb="$(df -Pk / 2>/dev/null | awk 'NR==2 {print $4}')"
    [[ "$available_kb" =~ ^[0-9]+$ ]] || return 1
    # The rollback is COW, but candidate creation and boot-stack regeneration
    # need working room.
    (( available_kb >= 1048576 ))
}

# Resolve a host rollback target: a numeric Snapper snapshot id under the
# top-level snapshot store, or one of Boot Bitch's own @rollback-before-*
# undo points.  Any other name is refused fail-closed.
host_snapshot_target_is_undo()
{
    [[ "${1:-}" =~ ^@rollback-before-[0-9]{8}-[0-9]{6}$ ]]
}

host_snapshot_resolve_target()
{
    local requested="$1" path
    if host_snapshot_target_is_undo "$requested"; then
        path="$SNAPSHOT_TOP/$requested"
        [[ -d "$path" ]] || return 1
        btrfs subvolume show "$path" >/dev/null 2>&1 || return 1
        printf '%s\n' "$path"
        return 0
    fi
    [[ "$requested" =~ ^[0-9]+$ ]] || return 1
    snapshot_find_path "$requested"
}

# Read-only capability detail line; never fails.
host_snapshot_rollback_evidence()
{
    local version running store
    version="$(LC_ALL=C snapper --version 2>/dev/null | head -n1 || true)"
    running="$(host_snapshot_running_path 2>/dev/null || true)"
    if host_snapshot_nested_snapshots_child 2>/dev/null; then
        store="@/.snapshots (nested; migration supported)"
    elif [[ -n "${SNAPSHOT_TOP:-}" && -e "$SNAPSHOT_TOP/@.snapshots" ]]; then
        store="top-level @.snapshots"
    elif [[ -n "${SNAPSHOT_TOP:-}" && -e "$SNAPSHOT_TOP/.snapshots" ]]; then
        store="top-level .snapshots"
    else
        store="snapshot store not mounted"
    fi
    printf 'snapper %s; running root %s; %s' \
        "${version:-not detected}" "${running:-unknown}" "$store"
}

# Probe-based reason for the `Host snapshot rollback:` capability line.
# Returns 0 (available) with no output, or 1 with the exact missing
# prerequisite on stdout.  Read-only and never fails the calling diagnostic.
host_snapshot_rollback_unavailable_reason()
{
    local fstype running nested

    fstype="$(lsblk -ndo FSTYPE "$ROOT_CANONICAL" 2>/dev/null | head -n1 || true)"
    if [[ "$fstype" != "btrfs" ]]; then
        printf 'the running host root filesystem is %s, not Btrfs' "${fstype:-unknown}"
        return 1
    fi
    command -v btrfs >/dev/null 2>&1 || { printf 'btrfs-progs is not installed'; return 1; }
    command -v snapper >/dev/null 2>&1 || { printf 'snapper is not installed'; return 1; }
    host_snapshot_snapper_config_ok \
        || { printf 'no Snapper root configuration manages / with FSTYPE=btrfs'; return 1; }
    running="$(host_snapshot_running_path 2>/dev/null || true)"
    if [[ "$running" != "@" ]]; then
        printf 'the running root subvolume is %s, not the top-level @' "${running:-unknown}"
        return 1
    fi
    if host_snapshot_subvolid_pinned; then
        printf 'the running host pins subvolid= in fstab or the kernel command line'
        return 1
    fi
    if host_snapshot_has_separate_boot; then
        printf 'the running host has a separate /boot filesystem outside the root snapshot'
        return 1
    fi
    if host_snapshot_timeshift_managed; then
        printf 'a Timeshift btrfs snapshot inventory is present; Snapper @ rollback is not supported'
        return 1
    fi
    nested="$(host_snapshot_other_nested_children 2>/dev/null || true)"
    if [[ -n "$nested" ]]; then
        printf 'the running @ root contains nested subvolumes that this release does not migrate: %s' \
            "$(tr '\n' ' ' <<<"$nested" | sed 's/[[:space:]]*$//')"
        return 1
    fi
    if ! target_path_is_mounted_rw /; then
        printf 'the running host root filesystem is read-only'
        return 1
    fi
    if ! host_snapshot_free_space_ok; then
        printf 'less than 1 GiB of free Btrfs space is available'
        return 1
    fi
    if ! ( host_package_manager_gate ) >/dev/null 2>&1; then
        printf 'a package manager or package-manager lock is active'
        return 1
    fi
    if pgrep -x snapper >/dev/null 2>&1; then
        printf 'another snapper command is running'
        return 1
    fi
    return 0
}

# Probe-based reason for the `Host reboot:` capability line.
host_reboot_unavailable_reason()
{
    if [[ -d /run/systemd/system ]] && command -v systemctl >/dev/null 2>&1; then
        return 0
    fi
    if command -v rc-shutdown >/dev/null 2>&1 || command -v reboot >/dev/null 2>&1; then
        return 0
    fi
    printf 'no supported reboot mechanism was found on the running host'
    return 1
}

# Host inventory: Snapper root snapshots plus Boot Bitch @rollback-before-*
# undo points, with the currently running snapshot marked and never offered as
# a rollback target.  Read-only.
host_list_snapshots()
{
    local active_id output count name path created ro_prop status rel

    active_id="$(btrfs_subvol_id / 2>/dev/null || true)"
    output="$(list_snapshots "$active_id")"
    printf '%s\n' "$output"
    count="$(grep -c '^SNAPSHOT' <<<"$output" || true)"

    while IFS= read -r path; do
        [[ -n "$path" ]] || continue
        name="${path##*/}"
        btrfs subvolume show "$path" >/dev/null 2>&1 || continue
        created="$(btrfs subvolume show "$path" 2>/dev/null \
            | sed -n 's/^[[:space:]]*Creation time:[[:space:]]*//p' | head -n1)"
        [[ -n "$created" ]] || created="$(stat -c '%y' "$path" 2>/dev/null | cut -d. -f1 || true)"
        ro_prop="$(btrfs property get -ts "$path" ro 2>/dev/null | awk -F= '$1=="ro" {print $2; exit}')"
        if snapshot_root_valid "$path"; then
            status="Linux root snapshot; Boot Bitch rollback backup (undo point)${ro_prop:+; ro=$ro_prop}"
        else
            status="Incomplete rollback backup${ro_prop:+; ro=$ro_prop}"
        fi
        rel="${path#"$SNAPSHOT_TOP"/}"
        printf 'SNAPSHOT\t%s\t%s\t%s\t%s\t%s\t%s\n' \
            "$name" \
            "$(snapshot_b64 "$created")" \
            "$(snapshot_b64 "rollback-backup")" \
            "$(snapshot_b64 "Boot Bitch pre-rollback root (undo point)")" \
            "$(snapshot_b64 "$status")" \
            "$(snapshot_b64 "$rel")"
        count=$((count + 1))
    done < <(find "$SNAPSHOT_TOP" -mindepth 1 -maxdepth 1 -type d -name '@rollback-before-*' 2>/dev/null | sort)

    printf 'Host snapshot inventory: %s root snapshot(s)\n' "$count"
}

host_inspect_snapshot()
{
    local requested="$1" snap running default_line

    snap="$(host_snapshot_resolve_target "$requested" || true)"
    [[ -n "$snap" ]] || fail "Snapshot $requested was not found on the running host's Btrfs filesystem."
    running="$(host_snapshot_running_path 2>/dev/null || true)"
    default_line="$(btrfs subvolume get-default "$SNAPSHOT_TOP" 2>/dev/null || true)"

    echo "Running host context:"
    echo "  Running root subvolume: ${running:-unknown}"
    echo "  Current default subvolume: ${default_line:-unknown}"
    if host_snapshot_nested_snapshots_child; then
        echo "  Nested @/.snapshots child subvolume: present (migrated into the promoted @)"
    fi
    echo

    if [[ "$requested" =~ ^[0-9]+$ ]]; then
        inspect_snapshot "$requested"
        return 0
    fi

    echo "Rollback backup: $requested"
    echo "Path: ${snap#"$SNAPSHOT_TOP"/}"
    echo
    btrfs subvolume show "$snap" 2>&1 || true
    echo
    echo "Root validation:"
    if snapshot_root_valid "$snap"; then
        echo "  PASS: /etc/os-release and /etc/fstab exist"
    else
        echo "  FAIL: the rollback backup is not a complete Linux root"
    fi
    echo
    echo "Kernel/initramfs pairing:"
    snapshot_kernel_pair_audit "$snap" || true
    echo
    echo "Backup fstab:"
    sed -n '1,160p' "$snap/etc/fstab" 2>/dev/null || echo "  unavailable"
    echo
    echo "Backup crypttab:"
    sed -n '1,120p' "$snap/etc/crypttab" 2>/dev/null || echo "  unavailable"
    echo
    echo "Inspection is read-only. No snapshot, subvolume, boot file or package state was modified."
}

# Complete read-only preflight and transactional plan for a running-host @
# rollback.  Creates nothing; emits PLAN_OK=1 only after every check passes.
host_snapshot_rollback_plan()
{
    local requested="$1" snap info type current pretty created root_id default_line rel backup_preview
    local running_id default_id snap_id
    local -a other_children=()

    snap="$(host_snapshot_resolve_target "$requested" || true)"
    [[ -n "$snap" ]] || fail "Snapshot $requested was not found on the running host's Btrfs filesystem."

    [[ "$(lsblk -ndo FSTYPE "$ROOT_CANONICAL" 2>/dev/null | head -n1 || true)" == "btrfs" ]] \
        || fail "Host snapshot rollback requires a Btrfs running host root."
    need btrfs
    command -v snapper >/dev/null 2>&1 \
        || fail "snapper is not installed on the running host; Snapper is the required snapshot producer."
    host_snapshot_snapper_config_ok \
        || fail "No Snapper root configuration manages / with FSTYPE=btrfs; host rollback is limited to Snapper-managed @ roots."
    if ! ( LC_ALL=C snapper --no-dbus -c root list >/dev/null 2>&1 ); then
        fail "The Snapper root configuration could not be queried (snapper --no-dbus list failed); refusing an unhealthy or concurrent Snapper state."
    fi
    if pgrep -x snapper >/dev/null 2>&1; then
        fail "Another snapper command is running; refusing a concurrent Snapper transaction."
    fi
    ( host_package_manager_gate ) >/dev/null 2>&1 \
        || fail "A package manager or package-manager lock is active; refusing a concurrent host rollback."

    current="$(host_snapshot_running_path || true)"
    [[ "$current" == "@" ]] \
        || fail "The running root subvolume is '${current:-unknown}', not the top-level @; this release supports Snapper @ roots only."
    host_snapshot_subvolid_pinned \
        && fail "The running host pins subvolid= in fstab or the kernel command line; a name-preserving rollback cannot change it."
    host_snapshot_has_separate_boot \
        && fail "The running host has a separate /boot filesystem outside the root snapshot; refusing host rollback."
    host_snapshot_timeshift_managed \
        && fail "A Timeshift btrfs snapshot inventory is present; Snapper @ rollback is not supported on this host."
    target_path_is_mounted_rw / \
        || fail "The running host root filesystem is read-only; refusing host rollback."
    host_snapshot_free_space_ok \
        || fail "Less than 1 GiB of free Btrfs space is available; refusing host rollback."

    mapfile -t other_children < <(host_snapshot_other_nested_children)
    ((${#other_children[@]} == 0)) \
        || fail "The running @ root contains nested subvolume(s) that this release does not migrate: ${other_children[*]}; refusing host rollback."

    if host_snapshot_target_is_undo "$requested"; then
        type="rollback-backup"
        info=""
        snapshot_root_valid "$snap" \
            || fail "Rollback backup $requested is not a valid Linux root subvolume."
    else
        info="$(snapshot_info_file "$snap" || true)"
        [[ -n "$info" ]] \
            || fail "Snapshot $requested has no Snapper info.xml metadata; non-Snapper Btrfs snapshots are not supported as host rollback targets."
        type="$(snapshot_xml_value "$info" type || true)"
        [[ "$type" == "single" ]] \
            || fail "Snapshot $requested is a Snapper '$type' snapshot; only single snapshots can be promoted to @."
        snapshot_root_valid "$snap" \
            || fail "Snapshot $requested is not a valid Linux root snapshot (/etc/os-release and /etc/fstab are required)."
    fi
    validate_snapshot_fstab_for_rollback "$snap" \
        || fail "Snapshot $requested fstab is not compatible with a safe @ rollback."
    validate_snapshot_crypttab_for_rollback "$snap" \
        || fail "Snapshot $requested crypttab does not resolve safely on the running host disk."
    snapshot_has_separate_boot "$snap" \
        && fail "Snapshot $requested has a separate /boot entry; refusing host rollback."

    running_id="$(btrfs_subvol_id / 2>/dev/null || true)"
    snap_id="$(btrfs_subvol_id "$snap" 2>/dev/null || true)"
    if [[ -n "$running_id" && -n "$snap_id" && "$running_id" == "$snap_id" ]]; then
        fail "Snapshot $requested is the currently running root; refusing to roll back to the running system."
    fi
    default_line="$(btrfs subvolume get-default "$SNAPSHOT_TOP" 2>/dev/null || true)"
    default_id="$(sed -n 's/^ID[[:space:]]\+\([0-9][0-9]*\).*/\1/p' <<< "$default_line" | head -1)"
    if [[ -n "$default_id" && -n "$snap_id" && "$default_id" == "$snap_id" ]]; then
        fail "Snapshot $requested is the current Btrfs default subvolume; refusing to roll back to the active root."
    fi

    root_id="$(btrfs_subvol_id "$SNAPSHOT_TOP/@" || true)"
    pretty="$(awk -F= '$1=="PRETTY_NAME" {sub(/^[^=]*=/, ""); gsub(/^"|"$/, ""); print; exit}' "$snap/etc/os-release")"
    created="$(btrfs subvolume show "$snap" 2>/dev/null | sed -n 's/^[[:space:]]*Creation time:[[:space:]]*//p' | head -1)"
    rel="${snap#"$SNAPSHOT_TOP"/}"
    backup_preview="@rollback-before-$(date +%Y%m%d-%H%M%S)"

    echo "========================================"
    echo "RUNNING-HOST BTRFS ROLLBACK PLAN"
    echo "========================================"
    echo "Host disk: $TARGET_DISK"
    echo "Btrfs filesystem: $ROOT_DEVICE"
    echo "Running root subvolume: @${root_id:+ (subvolume ID $root_id)}"
    echo "Current default subvolume: ${default_line:-unknown}"
    echo
    echo "Selected rollback target: $requested"
    echo "Path: $rel"
    echo "Target OS: ${pretty:-Linux}"
    [[ -n "$created" ]] && echo "Created: $created"
    echo "Snapper type: $type"
    [[ -n "$info" ]] && echo "Description: $(snapshot_xml_value "$info" description || true)"
    echo
    echo "Preflight kernel/initramfs evidence:"
    snapshot_kernel_pair_audit "$snap" || true
    echo
    echo "Separate Btrfs subvolumes are retained rather than rolled back:"
    snapshot_separate_subvolumes "$snap"
    echo
    if host_snapshot_nested_snapshots_child; then
        echo "Nested @/.snapshots child subvolume: will be migrated into the promoted @."
    fi
    echo
    echo "Host rollback transaction:"
    echo "  1. Keep the selected snapshot/backup unchanged."
    echo "  2. Create a new writable snapshot candidate @rollback-new-<stamp>."
    echo "  3. Preserve the running @ as $backup_preview (the automatic undo point)."
    echo "  4. Promote the candidate to @ and set it as the Btrfs default subvolume."
    echo "  5. Migrate a nested @/.snapshots child subvolume when present."
    echo "  6. Mount the promoted @ in a scratch chroot and reconcile initramfs/UKI/GRUB."
    echo "  7. If a critical post-switch stage fails, restore the preserved @ automatically."
    echo
    echo "The running host keeps running the current root until reboot."
    echo "A reboot is required and is never performed automatically."
    echo "PLAN_OK=1"
}

# Detach every helper-owned scratch mount except the Btrfs top-level mount.
host_snapshot_unmount_scratch()
{
    local idx path
    local -a keep=()
    for (( idx=${#MOUNTS[@]}-1; idx>=0; --idx )); do
        path="${MOUNTS[$idx]:-}"
        [[ -n "$path" ]] || continue
        if [[ "$path" == "$SNAPSHOT_TOP" ]]; then
            keep+=("$path")
            continue
        fi
        if mountpoint -q "$path" 2>/dev/null; then
            # The scratch tree contains a recursive /sys bind whose nested
            # submounts can keep the parent busy; the lazy fallback detaches
            # only the helper's private scratch mounts, never the live root.
            umount "$path" 2>/dev/null || umount -l "$path" 2>/dev/null || return 1
        fi
    done
    if ((${#keep[@]} > 0)); then
        MOUNTS=("${keep[@]}")
    else
        MOUNTS=()
    fi
    return 0
}

# Mount the promoted @ read-write at a scratch path (never over the live /)
# with its fstab subvolumes, boot entries and chroot pseudo-filesystems.
host_snapshot_mount_promoted_root_rw()
{
    local scratch="$SESSION_DIR/host-promoted-root"
    ROOT_DEVICE="$(preferred_block_path "$ROOT_DEVICE" "$ROOT_CANONICAL")"
    mkdir -p -- "$scratch"
    mount_recorded "$ROOT_DEVICE" "$scratch" -o rw,subvol=@
    MOUNT_BASE="$scratch"
    TARGET_ROOT="$scratch"
    TARGET_SUBVOL="@"
    read_target_os
    # The shared post-switch reconcile selects the initramfs/GRUB backend from
    # the promoted root's own probes, exactly like a target repair.
    profile_target_backends
    prepare_mapper_compatibility_aliases
    mount_target_btrfs_subvolumes rw
    mount_target_boot_entry "/boot" rw
    mount_target_boot_entry "/boot/efi" rw
    mount_target_boot_entry "/efi" rw
    mount_special dev-rw none "$TARGET_ROOT/dev"
    mount_special proc proc "$TARGET_ROOT/proc"
    mount_special rbind-ro /sys "$TARGET_ROOT/sys"
    mount_special tmpfs none "$TARGET_ROOT/run"
}

# Recovery for a failed running-host rollback: detach the scratch mounts, move
# a migrated nested .snapshots child back into the preserved root, promote the
# preserved @ back to @, restore the previous Btrfs default and reconcile its
# boot stack.  Returns non-zero when any step leaves the host without a safe
# root.
host_snapshot_restore_preserved_root()
{
    local top="$1" backup_name="$2" failed_name="$3" old_default_id="$4" current_id restore_rc=0
    log "HOST ROLLBACK RECOVERY: restoring the preserved pre-rollback @." | tee -a "$SESSION_LOG"

    if ! host_snapshot_unmount_scratch; then
        log "CRITICAL: cannot detach the promoted host rollback mounts; refusing to rename Btrfs roots during recovery." | tee -a "$SESSION_LOG"
        return 1
    fi
    if ! mount -o remount,rw "$top" 2>/dev/null; then
        log "CRITICAL: could not remount the Btrfs top-level filesystem read-write during host rollback recovery." | tee -a "$SESSION_LOG"
        return 1
    fi

    # A nested @/.snapshots child was migrated into the promoted @; move it
    # back so the preserved root is a complete Snapper root again.
    if btrfs subvolume show "$top/@/.snapshots" >/dev/null 2>&1; then
        if [[ -d "$top/$backup_name/.snapshots" ]]; then
            rmdir -- "$top/$backup_name/.snapshots" 2>/dev/null || true
        fi
        if ! mv -- "$top/@/.snapshots" "$top/$backup_name/.snapshots"; then
            log "CRITICAL: could not move the nested @/.snapshots child back into the preserved root." | tee -a "$SESSION_LOG"
            return 1
        fi
    fi

    if [[ ! -d "$top/$backup_name" ]]; then
        log "CRITICAL: preserved root $backup_name is missing; refusing to move the active rollback candidate." | tee -a "$SESSION_LOG"
        return 1
    fi
    if [[ -e "$top/@" ]]; then
        if ! mv -- "$top/@" "$top/$failed_name"; then
            log "CRITICAL: could not move the failed host rollback candidate out of @; refusing to promote the preserved root." | tee -a "$SESSION_LOG"
            return 1
        fi
    fi
    mv -- "$top/$backup_name" "$top/@" || return 1

    if [[ "$old_default_id" =~ ^[0-9]+$ ]]; then
        if ! btrfs subvolume set-default "$old_default_id" "$top" 2>&1 | tee -a "$SESSION_LOG"; then
            log "CRITICAL: could not restore the previous Btrfs default subvolume." | tee -a "$SESSION_LOG"
            return 1
        fi
    else
        current_id="$(btrfs_subvol_id "$top/@" || true)"
        if [[ -n "$current_id" ]]; then
            if ! btrfs subvolume set-default "$current_id" "$top" 2>&1 | tee -a "$SESSION_LOG"; then
                log "CRITICAL: could not restore the previous Btrfs default subvolume." | tee -a "$SESSION_LOG"
                return 1
            fi
        else
            log "CRITICAL: could not determine the preserved root subvolume ID." | tee -a "$SESSION_LOG"
            return 1
        fi
    fi

    host_snapshot_mount_promoted_root_rw || return 1
    set +e
    ( snapshot_post_switch_reconcile ) 2>&1 | tee -a "$SESSION_LOG"
    restore_rc=${PIPESTATUS[0]}
    set -e
    if ((restore_rc != 0)); then
        log "CRITICAL: the original @ was restored, but its boot-stack reconciliation also failed. Manual boot repair is required before reboot." | tee -a "$SESSION_LOG"
        return 1
    fi
    log "PASS: the original @ and its boot stack were restored automatically. Failed rollback candidate retained as $failed_name." | tee -a "$SESSION_LOG"
    return 0
}

# Execute the running-host rollback: snapshot the selected snapshot/undo point
# to a writable candidate, preserve the running @ under @rollback-before-*,
# promote the candidate, migrate a nested @/.snapshots child, reconcile the
# boot stack in a scratch chroot and automatically restore the preserved root
# on any critical failure.  The host is never rebooted here.
host_rollback_snapshot()
{
    local requested="$1" snap stamp candidate_name backup_name failed_name candidate_path
    local old_default_line old_default_id candidate_id rc output nested=0
    local pre_fstab pre_grub pre_cmdline pre_uki path

    # The complete read-only preflight runs again here; the helper never trusts
    # the GUI's earlier plan request.  PLAN_OK=1 is printed again as evidence.
    host_snapshot_rollback_plan "$requested"
    snap="$(host_snapshot_resolve_target "$requested" || true)"
    [[ -n "$snap" ]] || fail "Snapshot $requested was not found on the running host's Btrfs filesystem."

    if host_snapshot_nested_snapshots_child; then
        nested=1
    fi

    stamp="$(date +%Y%m%d-%H%M%S)"
    candidate_name="@rollback-new-$stamp"
    backup_name="@rollback-before-$stamp"
    failed_name="@rollback-failed-$stamp"
    candidate_path="$SNAPSHOT_TOP/$candidate_name"

    for path in "$candidate_path" "$SNAPSHOT_TOP/$backup_name" "$SNAPSHOT_TOP/$failed_name"; do
        [[ ! -e "$path" ]] || fail "Host rollback staging path already exists: ${path#"$SNAPSHOT_TOP"/}"
    done

    old_default_line="$(btrfs subvolume get-default "$SNAPSHOT_TOP" 2>/dev/null || true)"
    old_default_id="$(sed -n 's/^ID[[:space:]]\+\([0-9][0-9]*\).*/\1/p' <<< "$old_default_line" | head -1)"
    pre_fstab="$(repair_file_fingerprint /etc/fstab)"
    pre_grub="$(repair_file_fingerprint /boot/grub/grub.cfg)"
    pre_cmdline="$(repair_file_fingerprint /etc/kernel/cmdline)"
    pre_uki="$(repair_file_fingerprint /boot/efi/EFI/BOOT/TUX.EFI)"

    # Firmware-variable writes stay intercepted for the complete transaction
    # (candidate creation, promotion and the scratch chroot reconcile).
    prepare_host_command_guard

    log "Running-host rollback selected: snapshot $requested." | tee -a "$SESSION_LOG"
    log "Preserved root name: $backup_name" | tee -a "$SESSION_LOG"
    log "Rollback source remains unchanged; the running host keeps the current root until reboot." | tee -a "$SESSION_LOG"
    log "Pre-rollback boot artifact fingerprints: fstab=$pre_fstab grub=$pre_grub cmdline=$pre_cmdline uki=$pre_uki" | tee -a "$SESSION_LOG"

    # This is the exact point where the host rollback crosses from read-only
    # planning to a modifying transaction.
    TARGET_WRITE_INTENT=1
    mount -o remount,rw "$SNAPSHOT_TOP" 2>&1 | tee -a "$SESSION_LOG"

    log "Creating writable rollback candidate $candidate_name" | tee -a "$SESSION_LOG"
    btrfs subvolume snapshot "$snap" "$candidate_path" 2>&1 | tee -a "$SESSION_LOG"
    snapshot_root_valid "$candidate_path" || {
        btrfs subvolume delete "$candidate_path" >/dev/null 2>&1 || true
        fail "Writable host rollback candidate failed Linux-root validation; the running root was not changed."
    }

    mv -- "$SNAPSHOT_TOP/@" "$SNAPSHOT_TOP/$backup_name" || {
        btrfs subvolume delete "$candidate_path" >/dev/null 2>&1 || true
        fail "Could not preserve the running @ subvolume; host rollback was not performed."
    }
    if ! mv -- "$candidate_path" "$SNAPSHOT_TOP/@"; then
        if mv -- "$SNAPSHOT_TOP/$backup_name" "$SNAPSHOT_TOP/@" 2>/dev/null; then
            fail "Could not promote the host rollback candidate to @; the original @ name was restored."
        fi
        fail "Could not promote the host rollback candidate or restore the original @ name. Do not reboot."
    fi

    if (( nested == 1 )); then
        log "Migrating nested @/.snapshots child subvolume into the promoted root." | tee -a "$SESSION_LOG"
        if [[ -d "$SNAPSHOT_TOP/@/.snapshots" && ! -L "$SNAPSHOT_TOP/@/.snapshots" ]]; then
            if ! rmdir -- "$SNAPSHOT_TOP/@/.snapshots" 2>/dev/null; then
                host_snapshot_restore_preserved_root "$SNAPSHOT_TOP" "$backup_name" "$failed_name" "$old_default_id" || true
                fail "The promoted @ contains a non-empty .snapshots directory; cannot migrate the nested child subvolume. Original root restoration was attempted."
            fi
        elif [[ -e "$SNAPSHOT_TOP/@/.snapshots" ]]; then
            host_snapshot_restore_preserved_root "$SNAPSHOT_TOP" "$backup_name" "$failed_name" "$old_default_id" || true
            fail "The promoted @ contains an unexpected .snapshots entry; cannot migrate the nested child subvolume. Original root restoration was attempted."
        fi
        if ! mv -- "$SNAPSHOT_TOP/$backup_name/.snapshots" "$SNAPSHOT_TOP/@/.snapshots"; then
            host_snapshot_restore_preserved_root "$SNAPSHOT_TOP" "$backup_name" "$failed_name" "$old_default_id" || true
            fail "Could not migrate the nested @/.snapshots child subvolume. Original root restoration was attempted."
        fi
    fi

    candidate_id="$(btrfs_subvol_id "$SNAPSHOT_TOP/@" || true)"
    if [[ -z "$candidate_id" ]]; then
        host_snapshot_restore_preserved_root "$SNAPSHOT_TOP" "$backup_name" "$failed_name" "$old_default_id" || true
        fail "Unable to determine the promoted @ subvolume ID. Original root restoration was attempted."
    fi
    set +e
    btrfs subvolume set-default "$candidate_id" "$SNAPSHOT_TOP" 2>&1 | tee -a "$SESSION_LOG"
    rc=${PIPESTATUS[0]}
    set -e
    if ((rc != 0)); then
        log "Host rollback default-subvolume update failed; automatically restoring the preserved root." | tee -a "$SESSION_LOG"
        if host_snapshot_restore_preserved_root "$SNAPSHOT_TOP" "$backup_name" "$failed_name" "$old_default_id"; then
            printf 'HOST_ROLLBACK_RECOVERY=ok\n'
        else
            printf 'HOST_ROLLBACK_RECOVERY=failed\n'
        fi
        fail "Host rollback failed while setting the Btrfs default subvolume; the preserved root restoration was attempted."
    fi
    sync

    if ! host_snapshot_mount_promoted_root_rw; then
        log "Host rollback candidate mount/preflight failed; automatically restoring the preserved root." | tee -a "$SESSION_LOG"
        if host_snapshot_restore_preserved_root "$SNAPSHOT_TOP" "$backup_name" "$failed_name" "$old_default_id"; then
            printf 'HOST_ROLLBACK_RECOVERY=ok\n'
        else
            printf 'HOST_ROLLBACK_RECOVERY=failed\n'
        fi
        fail "Host rollback failed while mounting the promoted @; the preserved root restoration was attempted."
    fi
    set +e
    output="$( ( snapshot_post_switch_reconcile ) 2>&1 )"
    rc=$?
    set -e
    printf '%s\n' "$output" | tee -a "$SESSION_LOG"

    if ((rc != 0)); then
        log "Host rollback candidate failed post-switch validation/boot reconciliation; automatically restoring the preserved root." | tee -a "$SESSION_LOG"
        if host_snapshot_restore_preserved_root "$SNAPSHOT_TOP" "$backup_name" "$failed_name" "$old_default_id"; then
            printf 'HOST_ROLLBACK_RECOVERY=ok\n'
        else
            printf 'HOST_ROLLBACK_RECOVERY=failed\n'
        fi
        fail "Host rollback failed post-switch validation; the preserved root restoration was attempted."
    fi

    sync
    log "========================================" | tee -a "$SESSION_LOG"
    log "RUNNING-HOST SNAPSHOT ROLLBACK COMPLETE" | tee -a "$SESSION_LOG"
    log "Selected rollback target: $requested" | tee -a "$SESSION_LOG"
    log "Promoted root: @ (subvolume ID $candidate_id); the running system keeps the previous root until reboot." | tee -a "$SESSION_LOG"
    log "Previous root retained as: $backup_name" | tee -a "$SESSION_LOG"
    printf 'HOST_ROLLBACK_OLD_DEFAULT=%s\n' "${old_default_id:-unknown}"
    printf 'HOST_ROLLBACK_OLD_ROOT=@\n'
    printf 'HOST_ROLLBACK_NEW_ROOT=@\n'
    printf 'HOST_ROLLBACK_BACKUP_ROOT=%s\n' "$backup_name"
    printf 'HOST_ROLLBACK_REBOOT_REQUIRED=1\n'
    printf 'HOST_ROLLBACK_RESULT=SUCCESS\n'
    repair_change_status snapshots changed
}

# host-snapshots dispatch: list|inspect|plan|rollback against the running
# host.  A non-Btrfs host has no Btrfs snapshot inventory: the read-only list
# reports that as an informational outcome (exit 0), while inspect/plan/
# rollback remain explicit errors because they name a concrete Btrfs
# operation.  A rollback always re-runs every preflight in the helper.
run_host_snapshots()
{
    local action="${1:-}" requested="${2:-}" fstype
    need base64
    need tr
    need find
    need sed
    need stat
    case "$action" in
        list)
            [[ $# -eq 1 ]] || fail "host-snapshots list does not accept a snapshot id."
            ;;
        inspect|plan|rollback)
            [[ $# -eq 2 ]] || fail "host-snapshots $action requires exactly one snapshot id or @rollback-before-* name."
            ;;
        *)
            fail "Unknown host-snapshots action: ${action:-missing}. Use list, inspect, plan or rollback."
            ;;
    esac

    CURRENT_STAGE="host snapshot $action"
    RUNNING_HOST_MODE=1
    if [[ "$action" == "rollback" ]]; then
        prepare_running_host "$TARGET_DISK" "$ROOT_DEVICE" yes
    else
        prepare_running_host "$TARGET_DISK" "$ROOT_DEVICE" no
    fi

    fstype="$(lsblk -ndo FSTYPE "$ROOT_CANONICAL" 2>/dev/null | head -n1 || true)"
    if [[ "$fstype" != "btrfs" ]]; then
        if [[ "$action" == "list" ]]; then
            printf 'Host snapshot inventory is not applicable: the running host root filesystem is %s, not Btrfs.\n' "${fstype:-unknown}"
            printf 'SNAPSHOT_INVENTORY_NOT_APPLICABLE=1\n'
            log "Host snapshot inventory skipped: ${fstype:-unknown} is not Btrfs." | tee -a "$SESSION_LOG"
            return 0
        fi
        fail "Host snapshot $action requires a Btrfs running host root; detected ${fstype:-unknown}."
    fi
    need btrfs
    mount_snapshot_top

    case "$action" in
        list) host_list_snapshots ;;
        inspect) host_inspect_snapshot "$requested" ;;
        plan) host_snapshot_rollback_plan "$requested" ;;
        rollback) host_rollback_snapshot "$requested" ;;
    esac
}

# host-reboot: schedule one reboot of the running host.  The GUI reaches this
# command only after its separate explicit confirmation; the helper never
# reboots on its own and never falls back to an unprivileged mechanism.
run_host_reboot()
{
    local raw_disk="${1:-}" raw_root="${2:-}"
    (($# == 2)) || fail "host-reboot requires a host disk and root component."
    [[ -n "$raw_disk" && -n "$raw_root" ]] || fail "host-reboot requires a host disk and root component."

    CURRENT_STAGE="host reboot"
    RUNNING_HOST_MODE=1
    prepare_running_host "$raw_disk" "$raw_root" no

    if [[ -d /run/systemd/system ]] && command -v systemctl >/dev/null 2>&1; then
        log "Scheduling running-host reboot through systemd." | tee -a "$SESSION_LOG"
        systemctl reboot --no-block || fail "systemd refused the running-host reboot request (inhibitor or policy)."
    elif command -v rc-shutdown >/dev/null 2>&1; then
        log "Scheduling running-host reboot through OpenRC." | tee -a "$SESSION_LOG"
        rc-shutdown -r now || fail "OpenRC refused the running-host reboot request."
    elif command -v reboot >/dev/null 2>&1; then
        log "Scheduling running-host reboot through the system reboot command." | tee -a "$SESSION_LOG"
        reboot || fail "The system reboot command failed."
    else
        fail "No supported reboot mechanism was found on the running host."
    fi
    printf 'HOST_REBOOT_SCHEDULED=1\n'
}

# Interactive-prompt evidence.  dnf5 (and apt/pacman/apk) print their
# confirmation question and then read stdin; they do not skip the question just
# because stdin is not a terminal.  With stdin bound to /dev/null they read EOF
# and abort instead of blocking on the privileged-session protocol pipe, and
# this deliberately narrow pattern turns that abort into an actionable hint.
CHROOT_SHELL_PROMPT_REGEX='(\[[yYnN]/[yYnN]\]|Password:|Enter passphrase|Press any key|\(yes/no\))'
# Bounded runtime for one chroot shell command.  Long enough for ordinary
# repair commands, short enough that a command waiting on something other than
# stdin (a service, a socket) cannot hold the privileged broker forever.
CHROOT_SHELL_TIMEOUT_SECONDS=300

# Some distributions (notably TUXEDO OS) ship an apt wrapper that rejects the
# plain 'apt upgrade' subcommand and prints a policy error directing callers to
# 'full-upgrade'.  A user-run shell command keeps its intent: when the command
# is exactly a plain apt/apt-get upgrade and the run fails with that policy
# message, the shell retries once with the equivalent full-upgrade transaction.
# The recognition is deliberately narrow -- one single-line command whose only
# subcommand is 'upgrade' -- so no other command is rewritten, and
# dnf/pacman/apk commands are never touched.
APT_SHELL_UPGRADE_COMMAND_REGEX='^[[:space:]]*(sudo[[:space:]]+)?([^[:space:]]*/)?apt(-get)?([[:space:]]+-[^[:space:]]+)*[[:space:]]+upgrade([[:space:]]+-[^[:space:]]+)*[[:space:]]*$'
APT_SHELL_UPGRADE_POLICY_REGEX="upgrade.*(disabled|not supported)|use .*(full-upgrade|dist-upgrade)|full-upgrade.*dist-upgrade|dist-upgrade.*full-upgrade"

# Echo the same command with its plain 'upgrade' subcommand replaced by
# 'full-upgrade', or return 1 when the command is not a confidently recognized
# single-line apt/apt-get upgrade.
apt_shell_full_upgrade_command()
{
    local command="${1:-}"
    [[ -n "$command" && "$command" != *$'\n'* ]] || return 1
    grep -Eq "$APT_SHELL_UPGRADE_COMMAND_REGEX" <<<"$command" || return 1
    sed -E 's/(^|[[:space:]])upgrade([[:space:]]|$)/\1full-upgrade\2/' <<<"$command"
}

# Return 0 when a failed run's transcript carries the distribution's policy
# refusal for the plain upgrade subcommand.
apt_shell_upgrade_policy_refused()
{
    local transcript="$1"
    [[ -s "$transcript" ]] || return 1
    grep -Eiq "$APT_SHELL_UPGRADE_POLICY_REGEX" "$transcript"
}

# Execute one reviewed command as root inside a fresh target chroot with a
# bounded runtime; the command text is logged before it runs.
run_chroot_shell()
{
    local command="${1:-}" rc=0 transcript run_command retry_command="" retried=0
    [[ $# -eq 1 ]] || fail "shell requires exactly one command string."
    [[ -n "$command" ]] || fail "shell command cannot be empty."
    need chroot
    need timeout
    # prepare_target rw installs the chroot mounts package managers need
    # (/dev tmpfs, /proc, /sys, /run and the target resolver bind) before the
    # command runs, so dnf/apt/pacman do not fail or wait on missing mounts.
    prepare_target rw
    log "BEGIN: Chroot shell command" | tee -a "$SESSION_LOG"
    log "Command: $command" | tee -a "$SESSION_LOG"
    # A command is deliberately run in a clean target environment.  stdin is
    # /dev/null so a command that asks a question reads EOF and aborts instead
    # of blocking on (or consuming) the privileged-session protocol pipe.
    # --kill-after guarantees the bounded runtime even when the command ignores
    # SIGTERM.  The per-request transcript is scanned after the command ends
    # for interactive-prompt evidence; tee keeps the live output in the helper
    # stdout and in the session log.
    transcript="$SESSION_DIR/chroot-shell-output"
    : > "$transcript"
    run_command="$command"
    while :; do
        set +e
        timeout --foreground --kill-after=10 "$CHROOT_SHELL_TIMEOUT_SECONDS" chroot "$TARGET_ROOT" /usr/bin/env \
            HOME=/root \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            DEBIAN_FRONTEND=noninteractive \
            APT_LISTCHANGES_FRONTEND=none \
            DNF5_FORCE_INTERACTIVE=0 \
            /bin/sh -c "$run_command" < /dev/null 2>&1 | tee -a "$SESSION_LOG" "$transcript"
        rc=${PIPESTATUS[0]}
        set -e
        if (( rc == 0 || retried == 1 )); then
            break
        fi
        retry_command="$(apt_shell_full_upgrade_command "$run_command" || true)"
        if [[ -z "$retry_command" ]] || ! apt_shell_upgrade_policy_refused "$transcript"; then
            break
        fi
        log "apt upgrade is disabled by this distribution; running 'apt full-upgrade' instead" | tee -a "$SESSION_LOG"
        log "Command: $retry_command" | tee -a "$SESSION_LOG"
        run_command="$retry_command"
        retried=1
    done
    log "Chroot shell exit code: $rc" | tee -a "$SESSION_LOG"
    if (( rc != 0 )) && grep -Eq "$CHROOT_SHELL_PROMPT_REGEX" "$transcript" 2>/dev/null; then
        printf '%s\n' "Boot Bitch: the command asked an interactive question, which the Chroot Shell cannot answer (stdin is /dev/null). Re-run it with the non-interactive flag, for example 'dnf update -y', 'apt-get -y upgrade' or 'pacman --noconfirm -Syu'." | tee -a "$SESSION_LOG"
    fi
    if (( rc == 124 || rc == 137 )); then
        printf '%s\n' "Boot Bitch: the command was aborted after ${CHROOT_SHELL_TIMEOUT_SECONDS} seconds. If it was waiting for input, re-run it with the non-interactive flag (for example 'dnf update -y')." | tee -a "$SESSION_LOG"
        fail "Chroot shell command timed out after ${CHROOT_SHELL_TIMEOUT_SECONDS} seconds."
    fi
    (( rc == 0 )) || fail "Chroot shell command failed (exit code $rc)."
}

# Execute one reviewed command directly on the running host through the
# firmware-guarded private namespace (no chroot) with a 300-second timeout.
run_host_shell()
{
    local raw_disk="${1:-}" raw_root="${2:-}" command="${3:-}"
    (($# == 3)) || fail "host-shell requires a host disk, root component and command string."
    [[ -n "$raw_disk" && -n "$raw_root" ]] || fail "host-shell requires a host disk and root component."
    [[ -n "$command" ]] || fail "host-shell command cannot be empty."
    need timeout
    need unshare

    CURRENT_STAGE="host shell"
    RUNNING_HOST_MODE=1
    prepare_running_host "$raw_disk" "$raw_root" no
    DIAGNOSTIC_SCOPE="Running Host"
    prepare_host_command_guard

    log "BEGIN: Running-host shell command" | tee -a "$SESSION_LOG"
    log "Command: $command" | tee -a "$SESSION_LOG"
    # The command runs directly in the live host root through the private
    # firmware-variable namespace with a clean environment.  The timeout
    # prevents an accidental foreground service from blocking the broker
    # indefinitely while still allowing ordinary maintenance commands.  A
    # plain apt upgrade rejected by the host's distribution policy is retried
    # once with the equivalent full-upgrade transaction; the transcript keeps
    # the original failure visible.
    local transcript="$SESSION_DIR/host-shell-output" rc=0
    local run_command="$command" retry_command="" retried=0
    : > "$transcript"
    while :; do
        set +e
        run_host_command_isolated timeout --foreground 300 /usr/bin/env \
            HOME=/root \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            DEBIAN_FRONTEND=noninteractive \
            /bin/bash -lc "$run_command" 2>&1 | tee -a "$SESSION_LOG" "$transcript"
        rc=${PIPESTATUS[0]}
        set -e
        if (( rc == 0 || retried == 1 )); then
            break
        fi
        retry_command="$(apt_shell_full_upgrade_command "$run_command" || true)"
        if [[ -z "$retry_command" ]] || ! apt_shell_upgrade_policy_refused "$transcript"; then
            break
        fi
        log "apt upgrade is disabled by this distribution; running 'apt full-upgrade' instead" | tee -a "$SESSION_LOG"
        log "Command: $retry_command" | tee -a "$SESSION_LOG"
        run_command="$retry_command"
        retried=1
    done
    log "Running-host shell exit code: $rc" | tee -a "$SESSION_LOG"
    if (( rc != 0 )); then
        log "FAIL: Running-host shell command (exit code $rc)" | tee -a "$SESSION_LOG"
        exit "$rc"
    fi
    log "PASS: Running-host shell command" | tee -a "$SESSION_LOG"
}

# ---------------------------------------------------------------------------
# Read-only validation and repair stage ordering
# ---------------------------------------------------------------------------
# validate entry point: mount the target read-only, profile its backends and
# log the complete validation summary; never writes.
validate_target()
{
    prepare_target ro
    mount_target_boot_entry "/boot" ro
    mount_target_boot_entry "/boot/efi" ro
    mount_target_boot_entry "/efi" ro
    profile_target_backends

    log "Validation summary" | tee -a "$SESSION_LOG"
    log "  OS: $TARGET_PRETTY" | tee -a "$SESSION_LOG"
    log "  Root: $ROOT_DEVICE" | tee -a "$SESSION_LOG"
    log "  Root subvolume: ${TARGET_SUBVOL:-default/none}" | tee -a "$SESSION_LOG"
    log "  Repair root mount source: $(findmnt -rn -o SOURCE --target "$TARGET_ROOT" 2>/dev/null | head -n1 || echo unknown)" | tee -a "$SESSION_LOG"
    log "  Root fs: $(lsblk -ndo FSTYPE "$ROOT_CANONICAL" | head -n1)" | tee -a "$SESSION_LOG"
    log "  /etc/fstab: $([[ -s "$TARGET_ROOT/etc/fstab" ]] && echo present || echo missing/empty)" | tee -a "$SESSION_LOG"
    log "  /etc/crypttab: $([[ -s "$TARGET_ROOT/etc/crypttab" ]] && echo present || echo missing/empty)" | tee -a "$SESSION_LOG"
    log "  /boot: $([[ -d "$TARGET_ROOT/boot" ]] && echo present || echo missing)" | tee -a "$SESSION_LOG"
    log "  GRUB config: $([[ -f "$TARGET_ROOT/boot/grub/grub.cfg" ]] && echo present || echo not-visible)" | tee -a "$SESSION_LOG"
    log "  Distribution family: $TARGET_DISTRO_FAMILY" | tee -a "$SESSION_LOG"
    log "  Package manager backend: $TARGET_PACKAGE_MANAGER" | tee -a "$SESSION_LOG"
    log "  Initramfs backend: $TARGET_INITRAMFS_BACKEND" | tee -a "$SESSION_LOG"
    log "  Bootloader backend: $TARGET_BOOTLOADER_BACKEND" | tee -a "$SESSION_LOG"
    log "  ESP mount candidate: $TARGET_ESP_MOUNT" | tee -a "$SESSION_LOG"
    log "  Supported modifying backend: $TARGET_REPAIR_BACKEND" | tee -a "$SESSION_LOG"

    log "Validation complete; no target files were changed." | tee -a "$SESSION_LOG"
    repair_change_status validate "unchanged|validation is read-only"
}

# host-validate entry point: profile the live running host and log the
# validation summary without mounting or modifying anything.
validate_running_host()
{
    CURRENT_STAGE="host validation"
    RUNNING_HOST_MODE=1
    prepare_running_host "$TARGET_DISK" "$ROOT_DEVICE" no
    profile_target_backends
    log "Running-host validation summary" | tee -a "$SESSION_LOG"
    log "  OS: $TARGET_PRETTY" | tee -a "$SESSION_LOG"
    log "  Root: $ROOT_DEVICE" | tee -a "$SESSION_LOG"
    log "  Root filesystem: $(lsblk -ndo FSTYPE "$ROOT_CANONICAL" | head -n1)" | tee -a "$SESSION_LOG"
    log "  /etc/fstab: $([[ -s "$TARGET_ROOT/etc/fstab" ]] && echo present || echo missing/empty)" | tee -a "$SESSION_LOG"
    log "  /etc/crypttab: $([[ -s "$TARGET_ROOT/etc/crypttab" ]] && echo present || echo missing/empty)" | tee -a "$SESSION_LOG"
    log "  /boot: $([[ -d "$TARGET_ROOT/boot" ]] && echo present || echo missing)" | tee -a "$SESSION_LOG"
    log "  /boot/efi: $([[ -n "$EFI_ESP_SOURCE" ]] && echo "$EFI_ESP_SOURCE ($EFI_ESP_FSTYPE)" || echo "not separately mounted")" | tee -a "$SESSION_LOG"
    validate_mapper_crypttab
    log "  Distribution family: $TARGET_DISTRO_FAMILY" | tee -a "$SESSION_LOG"
    log "  Package manager backend: $TARGET_PACKAGE_MANAGER" | tee -a "$SESSION_LOG"
    log "  Initramfs backend: $TARGET_INITRAMFS_BACKEND" | tee -a "$SESSION_LOG"
    log "  Bootloader backend: $TARGET_BOOTLOADER_BACKEND" | tee -a "$SESSION_LOG"
    log "  ESP mount candidate: $TARGET_ESP_MOUNT" | tee -a "$SESSION_LOG"
    log "  Supported modifying backend: $TARGET_REPAIR_BACKEND" | tee -a "$SESSION_LOG"
    log "Running-host validation complete; no host files were changed." | tee -a "$SESSION_LOG"
    repair_change_status validate "unchanged|validation is read-only"
}

validate_stage()
{
    case "$1" in
        dpkg-configure|fix-broken|apt-update|apt-upgrade|dkms|display-manager|initramfs|efi|boot-stack|grub|extlinux) return 0 ;;
        *) fail "Unknown repair stage: $1" ;;
    esac
}

stage_rank()
{
    case "$1" in
        dpkg-configure) printf '10\n' ;;
        fix-broken) printf '20\n' ;;
        apt-update) printf '30\n' ;;
        apt-upgrade) printf '40\n' ;;
        dkms) printf '50\n' ;;
        display-manager) printf '60\n' ;;
        initramfs) printf '70\n' ;;
        efi) printf '80\n' ;;
        boot-stack) printf '85\n' ;;
        grub) printf '90\n' ;;
        extlinux) printf '90\n' ;;
        *) return 1 ;;
    esac
}

# Repair stage arguments shared by repair and host-repair.  An explicit mode
# hint such as --post-efi is a caller statement about work that already ran in
# the same plan/session; the helper never infers it from the stage list.
REPAIR_STAGES=()
REPAIR_POST_EFI=false
BOOT_STACK_POST_EFI=false

parse_repair_arguments()
{
    REPAIR_STAGES=()
    REPAIR_POST_EFI=false
    local argument
    for argument in "$@"; do
        case "$argument" in
            --post-efi) REPAIR_POST_EFI=true ;;
            -*) fail "Unknown repair stage mode hint: $argument" ;;
            *) REPAIR_STAGES+=("$argument") ;;
        esac
    done
}

# Fail closed before any write when a requested stage has no detected guarded
# backend.  Each stage is validated against the same read-only probes that the
# capability lines use; the distribution family is never consulted, so a
# caller that bypasses the GUI still gets a probe-based reason.
validate_repair_stages_against_backends()
{
    local stage reason
    # Probe the target once here so a direct caller gets the same evidence the
    # capability lines use; the callers in run_repair/run_host_repair have
    # already profiled, and profiling again is read-only and idempotent.
    profile_target_backends
    for stage in "$@"; do
        case "$stage" in
            dpkg-configure)
                if ! reason="$(dpkg_unavailable_reason)"; then
                    fail "$reason."
                fi
                ;;
            fix-broken|apt-update|apt-upgrade)
                if ! reason="$(package_stage_unavailable_reason "$stage")"; then
                    fail "$reason."
                fi
                ;;
            dkms)
                if ! reason="$(dkms_unavailable_reason)"; then
                    fail "$reason."
                fi
                ;;
            display-manager)
                if ! reason="$(display_unavailable_reason)"; then
                    fail "$reason."
                fi
                ;;
            initramfs)
                if ! reason="$(initramfs_unavailable_reason)"; then
                    fail "$reason."
                fi
                ;;
            efi)
                if ! reason="$(efi_unavailable_reason)"; then
                    fail "$reason."
                fi
                ;;
            grub)
                if ! reason="$(grub_unavailable_reason)"; then
                    fail "$reason."
                fi
                ;;
            extlinux)
                if ! reason="$(extlinux_unavailable_reason)"; then
                    fail "$reason."
                fi
                ;;
            boot-stack)
                if ! reason="$(bootstack_unavailable_reason)"; then
                    fail "$reason."
                fi
                ;;
        esac
    done
}

# ---------------------------------------------------------------------------
# EFI firmware inventory, bootloader repair and BootOrder reconciliation
# ---------------------------------------------------------------------------
# Derive a safe EFI bootloader directory ID: prefer an existing target-owned
# vendor directory with GRUB/shim binaries, otherwise use the sanitized OS ID.
detect_efi_bootloader_id()
{
    local os_id existing_dir name lower esp_root
    local -a candidates=()

    os_id="${TARGET_OS_ID//[^[:alnum:]_.-]/}"
    esp_root="$TARGET_ROOT${TARGET_ESP_MOUNT:-/boot/efi}/EFI"

    # Prefer an existing target-owned EFI vendor directory so a repair updates
    # the installation the firmware was already using rather than inventing a
    # second entry. Ignore the generic fallback and common foreign OS folders.
    if [[ -d "$esp_root" ]]; then
        while IFS= read -r existing_dir; do
            [[ -d "$existing_dir" ]] || continue
            name="$(basename -- "$existing_dir")"
            lower="${name,,}"
            case "$lower" in
                boot|microsoft|tools) continue ;;
            esac
            if [[ -n "$os_id" && "$lower" == "${os_id,,}" ]]; then
                printf '%s\n' "$name"
                return 0
            fi
            if find "$existing_dir" -maxdepth 1 -type f \
                \( -iname 'grub*.efi' -o -iname 'shim*.efi' \) -print -quit 2>/dev/null \
                | grep -q .; then
                candidates+=("$name")
            fi
        done < <(find "$esp_root" -mindepth 1 -maxdepth 1 -type d -print 2>/dev/null | sort)
    fi

    if ((${#candidates[@]} == 1)); then
        printf '%s\n' "${candidates[0]}"
        return 0
    fi

    [[ -n "$os_id" ]] || os_id="linux"
    printf '%s\n' "$os_id"
}

uefi_nvram_writable()
{
    [[ -d /sys/firmware/efi/efivars ]] || return 1
    findmnt -rn -o OPTIONS --target /sys/firmware/efi/efivars 2>/dev/null \
        | tr ',' '\n' \
        | grep -Fxq rw
}

# Strict Debian-path ESP check: /boot/efi must be a mounted FAT partition that
# belongs to the selected target disk.  Sets EFI_ESP_SOURCE/EFI_ESP_FSTYPE.
validate_target_esp()
{
    local esp_dir
    EFI_ESP_SOURCE=""
    EFI_ESP_FSTYPE=""
    esp_dir="$(target_path /boot/efi)"

    [[ -d "$esp_dir" ]] || fail "Target /boot/efi directory is not available."
    mountpoint -q "$esp_dir" || fail "Target EFI System Partition is not mounted at /boot/efi."

    # A native systemd automount may report autofs before its FAT mount.
    # Select the block-backed mount instead of accepting the synthetic row.
    read -r EFI_ESP_SOURCE EFI_ESP_FSTYPE < <(
        findmnt -rn -o SOURCE,FSTYPE --target "$esp_dir" 2>/dev/null \
            | awk '$1 ~ /^\/dev\// {print $1, $2; exit}'
    ) || true
    [[ -n "$EFI_ESP_SOURCE" ]] && is_block_device "$EFI_ESP_SOURCE" \
        || fail "Unable to identify the mounted EFI System Partition source."
    same_single_top_disk "$TARGET_DISK" "$EFI_ESP_SOURCE" \
        || fail "EFI System Partition resolves outside the selected target disk: $EFI_ESP_SOURCE"
    case "${EFI_ESP_FSTYPE,,}" in
        vfat|fat|fat16|fat32|msdos) ;;
        *) fail "Mounted /boot/efi is not a FAT EFI System Partition (detected ${EFI_ESP_FSTYPE:-unknown})." ;;
    esac
}

# Resolve and validate the ESP at the profile-selected mount path (/boot/efi,
# /boot or /efi) and require it to be a FAT partition of the selected disk.
# The strict /boot/efi validation is selected by the resolved mount path, not
# by the distribution family.
validate_selected_esp()
{
    local mount_path source fstype esp_dir
    if [[ "$TARGET_ESP_MOUNT" == "/boot/efi" ]]; then
        validate_target_esp
        return 0
    fi
    profile_target_backends
    mount_path="${TARGET_ESP_MOUNT:-}"
    [[ "$mount_path" == /boot || "$mount_path" == /boot/efi || "$mount_path" == /efi ]] \
        || fail "Unable to derive a supported EFI System Partition mount for this target."
    esp_dir="$(target_path "$mount_path")"
    [[ -d "$esp_dir" ]] || fail "Target EFI mount directory is not available: $mount_path"
    mountpoint -q "$esp_dir" \
        || fail "Target EFI System Partition is not mounted at $mount_path."
    read -r source fstype < <(
        findmnt -rn -o SOURCE,FSTYPE --target "$esp_dir" 2>/dev/null \
            | awk '$1 ~ /^\/dev\// {print $1, $2; exit}'
    ) || true
    [[ -n "$source" ]] && is_block_device "$source" \
        || fail "Unable to identify the mounted EFI System Partition source at $mount_path."
    same_single_top_disk "$TARGET_DISK" "$source" \
        || fail "EFI System Partition resolves outside the selected target disk: $source"
    case "${fstype,,}" in
        vfat|fat|fat16|fat32|msdos) ;;
        *) fail "Mounted $mount_path is not a FAT EFI System Partition (detected ${fstype:-unknown})." ;;
    esac
    EFI_ESP_SOURCE="$source"
    EFI_ESP_FSTYPE="$fstype"
    log "Selected EFI System Partition: $EFI_ESP_SOURCE ($EFI_ESP_FSTYPE) mounted at $mount_path" | tee -a "$SESSION_LOG"
}

# Read-only ESP discovery used by host diagnostics and backend profiling.  The
# Debian repair path intentionally keeps validate_target_esp() strict at
# /boot/efi, while Arch and other distributions commonly mount their ESP at
# /boot or /efi.  Do not require a separate ESP here: a caller can continue
# with an empty result and report that no FAT ESP is mounted.
detect_mounted_esp()
{
    local esp_mount source fstype esp_dir
    EFI_ESP_SOURCE=""
    EFI_ESP_FSTYPE=""
    TARGET_ESP_MOUNT=""

    for esp_mount in /boot/efi /efi /boot; do
        esp_dir="$(target_path "$esp_mount")"
        [[ -d "$esp_dir" ]] || continue
        mountpoint -q "$esp_dir" 2>/dev/null || continue
        read -r source fstype < <(
            findmnt -rn -o SOURCE,FSTYPE --target "$esp_dir" 2>/dev/null \
                | awk '$1 ~ /^\/dev\// {print $1, $2; exit}'
        ) || true
        [[ -n "$source" ]] || continue
        case "${fstype,,}" in
            vfat|fat|fat16|fat32|msdos) ;;
            *) continue ;;
        esac
        is_block_device "$source" || continue
        same_single_top_disk "$TARGET_DISK" "$source" || continue
        EFI_ESP_SOURCE="$source"
        EFI_ESP_FSTYPE="$fstype"
        TARGET_ESP_MOUNT="$esp_mount"
        return 0
    done
    return 1
}

# Read-only vendor UKI builder check.  The builder path matches
# validate_tuxedo_uki_target(), so capability evidence and the repair preflight
# agree on what "TUXEDO UKI layout" requires.
tuxedo_uki_builder_present()
{
    [[ -x "$TARGET_ROOT/usr/sbin/create_boot_uki_base.sh" ]]
}

is_tuxedo_uki_layout()
{
    [[ "$TARGET_OS_ID" == "tuxedo" ]] && tuxedo_uki_builder_present
}

newest_tuxedo_kernel()
{
    find "$TARGET_ROOT/boot" -maxdepth 1 -type f -name 'vmlinuz-*-tuxedo-amd64' -printf '%f\n' 2>/dev/null \
        | sed 's/^vmlinuz-//' \
        | sort -V \
        | tail -1
}

validate_tuxedo_uki_target()
{
    local kver
    validate_target_esp
    [[ -x "$TARGET_ROOT/usr/sbin/create_boot_uki_base.sh" ]] \
        || fail "TUXEDO UKI builder is not installed in the target: /usr/sbin/create_boot_uki_base.sh"
    kver="$(newest_tuxedo_kernel)"
    [[ -n "$kver" && -f "$TARGET_ROOT/boot/vmlinuz-$kver" ]] \
        || fail "No installed TUXEDO kernel was found for UKI rebuild."
    [[ -d "$TARGET_ROOT/usr/lib/modules/$kver" || -d "$TARGET_ROOT/lib/modules/$kver" ]] \
        || fail "Newest TUXEDO kernel $kver has no matching modules directory in the target."
}

# efibootmgr prints one Boot#### definition per line, followed by optional
# decoded device-path detail lines. Keep the parser limited to the definition
# line: it gives us a stable firmware identity without trying to decode
# vendor-specific binary data.
efi_entry_id_line()
{
    sed -nE 's/^Boot([0-9A-Fa-f]{4})\*?[[:space:]].*/\1/p' <<<"$1" \
        | tr '[:lower:]' '[:upper:]'
}

efi_entry_definition_line()
{
    sed -E 's/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]+//; s/[[:space:]]+/ /g; s/^ //; s/ $//' <<<"$1"
}

efi_entry_label_line()
{
    local definition="$1" label_re='^(.*)[[:space:]]+(HD\(|File\(|PciRoot\(|VenHw\(|MemoryMapped\()'
    definition="$(efi_entry_definition_line "$definition")"
    if [[ "$definition" =~ $label_re ]]; then
        printf '%s\n' "${BASH_REMATCH[1]}"
    else
        # A definition without a decoded path is still useful for inventory
        # and preservation checks. The whole definition is the best safe
        # label available in that case.
        printf '%s\n' "$definition"
    fi
}

efi_entry_partuuid_line()
{
    sed -nE 's/.*HD\([0-9]+,GPT,([[:alnum:]-]{36}),.*/\1/p' <<<"$1" \
        | tr '[:upper:]' '[:lower:]'
}

efi_entry_loader_line()
{
    local line="$1" loader_re='HD\([^)]*\)/([^[:space:]]+)'
    if [[ "$line" =~ $loader_re ]]; then
        # efibootmgr appends optional-data bytes (commonly `0000424f`) after
        # a normal `.EFI` file path. They are not part of the loader name and
        # cannot be passed back through --loader, so preserve the path while
        # deliberately ignoring only that suffix for identity/recreation.
        printf '%s\n' "${BASH_REMATCH[1]}" \
            | sed -E 's/(\.[Ee][Ff][Ii])[0-9A-Fa-f]{8}$/\1/'
    fi
}

efi_selected_system_model()
{
    local esp="$1" disk model

    [[ -n "$esp" ]] || return 1
    disk="$(lsblk -ndo PKNAME "$esp" 2>/dev/null | head -n1 || true)"
    [[ -n "$disk" ]] || return 1
    disk="${disk#/dev/}"
    model="$(lsblk -ndo MODEL "/dev/$disk" 2>/dev/null | head -n1 || true)"
    model="$(sed -E 's/[[:space:]]+/ /g; s/^ //; s/ $//' <<<"$model")"
    [[ -n "$model" ]] || return 1
    printf '%s\n' "$model"
}

efi_disk_and_partnum_for_esp()
{
    local esp="$1" disk partnum sysname disk_path

    # `PARTNUM` is not a portable lsblk column.  Debian util-linux exposes
    # the partition number as `PARTN`; retain a sysfs fallback for older or
    # reduced lsblk builds and for device paths that are not fully represented
    # by lsblk.  Return the canonical disk path and numeric partition number
    # as a tab-separated pair so callers do not duplicate this resolver.
    [[ -n "$esp" ]] || return 1
    disk="$(lsblk -ndo PKNAME "$esp" 2>/dev/null | head -n1 || true)"
    [[ -n "$disk" ]] || return 1
    disk="${disk#/dev/}"
    disk_path="/dev/$disk"
    is_block_device "$disk_path" || return 1

    partnum="$(lsblk -ndo PARTN "$esp" 2>/dev/null | head -n1 || true)"
    if [[ ! "$partnum" =~ ^[0-9]+$ ]]; then
        sysname="$(basename -- "$esp")"
        partnum="$(cat "/sys/class/block/$sysname/partition" 2>/dev/null || true)"
    fi
    [[ "$partnum" =~ ^[0-9]+$ ]] || return 1
    printf '%s\t%s\n' "$disk_path" "$partnum"
}

efi_label_match_text()
{
    # Firmware labels and lsblk models do not use one consistent separator
    # convention (for example a model written with an underscore versus the
    # same model written with a space). Compare a compact, case-insensitive
    # form so a model already present in a vendor label is not appended a
    # second time.
    tr '[:upper:]' '[:lower:]' \
        | sed -E 's/[_-]+/ /g; s/[^[:alnum:]]+/ /g; s/[[:space:]]+/ /g; s/^ //; s/ $//'
}

efi_label_has_selected_model()
{
    local label="$1" model="$2" label_key model_key model_stem
    label_key="$(efi_label_match_text <<<"$label")"
    model_key="$(efi_label_match_text <<<"$model")"
    [[ -n "$model_key" && "$label_key" == *"$model_key"* ]] && return 0

    # Capacity is often omitted from an existing vendor-generated label
    # (for example a model name with a capacity suffix versus the same model
    # without it). Treat that stable model stem as present as well, while
    # requiring at least two meaningful words so a generic label cannot
    # suppress annotation.
    model_stem="$(sed -E 's/[[:space:]]+[0-9]+(gb|tb|gib|tib)$//' <<<"$model_key")"
    [[ "$model_stem" != "$model_key" && "$model_stem" == *' '* \
       && "$label_key" == *"$model_stem"* ]]
}

efi_label_with_selected_model()
{
    local label="$1" model="$2"
    if efi_label_has_selected_model "$label" "$model"; then
        printf '%s\n' "$label"
    else
        printf '%s %s\n' "$label" "$model"
    fi
}

efi_entry_ids_for_partuuid_loader()
{
    local partuuid="${1,,}" wanted_loader="${2,,}" line part loader id
    command -v efibootmgr >/dev/null 2>&1 || return 1
    [[ -n "$partuuid" && -n "$wanted_loader" ]] || return 1

    while IFS= read -r line; do
        part="$(efi_entry_partuuid_line "$line")"
        [[ "$part" == "$partuuid" ]] || continue
        loader="$(efi_entry_loader_line "$line" || true)"
        [[ "${loader,,}" == "$wanted_loader" ]] || continue
        id="$(efi_entry_id_line "$line")"
        [[ -n "$id" ]] && printf '%s\n' "$id"
    done < <(efibootmgr -v 2>/dev/null | sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p')
}

efi_selected_entry_role()
{
    local loader="${1,,}" basename
    basename="${loader##*\\}"
    case "$loader" in
        '\efi\boot\tux.efi') printf 'uki\n' ;;
        *)
            case "$basename" in
                ipxe.efi) printf 'wfai\n' ;;
                bootx64.efi) printf 'fallback\n' ;;
                # EFI vendor directories and loader names vary by
                # distribution. Any remaining decoded EFI file on the
                # selected ESP is a boot entry we can label without needing
                # to guess whether it is Debian, Ubuntu, Fedora, or another
                # distribution. The PARTUUID boundary is enforced by the
                # caller, so another disk's entry is never touched.
                *.efi) printf 'loader\n' ;;
            esac
            ;;
    esac
}

efi_entry_destination_role()
{
    local line="$1" loader basename label label_key
    loader="$(efi_entry_loader_line "$line" || true)"
    loader="${loader,,}"
    basename="${loader##*\\}"
    case "$loader" in
        '\efi\boot\tux.efi') printf 'uki\n'; return 0 ;;
        *) ;;
    esac
    case "$basename" in
        bootx64.efi) printf 'fallback\n'; return 0 ;;
        ipxe.efi) printf 'wfai\n'; return 0 ;;
    esac
    if [[ -n "$loader" ]]; then
        printf 'vendor-loader\n'
        return 0
    fi
    label="$(efi_entry_label_line "$line")"
    label_key="$(efi_label_match_text <<<"$label")"
    if [[ "$label_key" == "uefi os" || ( "$label_key" == uefi\ * && "$label_key" == *partition* ) ]]; then
        printf 'fallback-device-path\n'
    else
        printf 'unknown\n'
    fi
}

efi_selected_wfai_loader()
{
    local efi_root="$TARGET_ROOT${TARGET_ESP_MOUNT:-/boot/efi}/EFI" path relative
    [[ -d "$efi_root" ]] || return 1
    path="$(find "$efi_root" -type f -iname 'iPXE.efi' -print -quit 2>/dev/null || true)"
    [[ -n "$path" ]] || return 1
    relative="${path#"$efi_root"/}"
    [[ "$relative" != "$path" ]] || return 1
    printf '%s\n' "\\EFI\\${relative//\//\\}"
}

efi_selected_generic_loader()
{
    local efi_root vendor_root path relative candidate
    efi_root="$TARGET_ROOT${TARGET_ESP_MOUNT:-/boot/efi}/EFI"
    vendor_root="$efi_root/${EFI_BOOTLOADER_ID:-}"
    [[ -d "$vendor_root" ]] || return 1

    # Prefer the conventional vendor loader names while accepting a
    # distribution-specific filename when the directory provides one. Never
    # select the ESP fallback or a backup artifact as the primary destination.
    for candidate in grubx64.efi shimx64.efi systemd-bootx64.efi; do
        path="$(find "$vendor_root" -maxdepth 1 -type f -iname "$candidate" -print -quit 2>/dev/null || true)"
        [[ -n "$path" ]] || continue
        relative="${path#"$efi_root"/}"
        [[ "$relative" != "$path" ]] || continue
        printf '%s\n' "\\EFI\\${relative//\//\\}"
        return 0
    done
    while IFS= read -r -d '' path; do
        [[ "${path,,}" != *.bak && "${path,,}" != *.backup ]] || continue
        relative="${path#"$efi_root"/}"
        [[ "$relative" != "$path" ]] || continue
        printf '%s\n' "\\EFI\\${relative//\//\\}"
        return 0
    done < <(find "$vendor_root" -maxdepth 1 -type f -iname '*.efi' ! -iname 'bootx64.efi' -print0 2>/dev/null)
    return 1
}

efi_ensure_selected_generic_entry()
{
    local partuuid="$1" model="$2" loader esp disk partnum label resolved current
    local -a ids=()

    loader="$(efi_selected_generic_loader || true)"
    [[ -n "$loader" ]] || {
        log "Selected EFI vendor directory has no primary loader; no generic firmware entry was created." | tee -a "$SESSION_LOG"
        return 0
    }
    mapfile -t ids < <(efi_entry_ids_for_partuuid_loader "$partuuid" "${loader,,}" || true)
    if ((${#ids[@]} > 1)); then
        log "Multiple generic EFI entries already point to the selected system loader (${ids[*]}); destination maintenance will retain one after the repair." | tee -a "$SESSION_LOG"
        return 0
    fi
    ((${#ids[@]} == 0)) || return 0

    uefi_nvram_writable || {
        log "Selected EFI loader is present at $loader, but firmware variables are not writable; no generic firmware entry was created." | tee -a "$SESSION_LOG"
        return 0
    }
    command -v efibootmgr >/dev/null 2>&1 || return 0
    esp="$(canonical_block "$EFI_ESP_SOURCE" 2>/dev/null || true)"
    [[ -n "$esp" ]] && is_block_device "$esp" || {
        log "ERROR: selected EFI loader is present, but its ESP could not be resolved as a block device." | tee -a "$SESSION_LOG" >&2
        return 1
    }
    resolved="$(efi_disk_and_partnum_for_esp "$esp" 2>/dev/null || true)"
    IFS=$'\t' read -r disk partnum <<< "$resolved"
    [[ -n "$disk" && "$partnum" =~ ^[0-9]+$ ]] || {
        log "ERROR: unable to derive disk and partition for selected ESP while restoring its generic EFI entry." | tee -a "$SESSION_LOG" >&2
        return 1
    }
    label="${EFI_BOOTLOADER_ID:-Linux}"
    [[ -n "$model" ]] && label="$(efi_label_with_selected_model "$label" "$model")"
    log "Restoring missing generic EFI firmware entry on selected system ESP $esp as '$label' ($loader)." | tee -a "$SESSION_LOG"
    efibootmgr --create --disk "$disk" --part "$partnum" \
        --label "$label" --loader "$loader" 2>&1 \
        | tee -a "$SESSION_LOG" || return 1
    current="$SESSION_DIR/efi-nvram-generic.txt"
    efibootmgr -v > "$current" 2>&1 || return 1
    mapfile -t ids < <(efi_entry_ids_for_partuuid_loader "$partuuid" "${loader,,}" || true)
    ((${#ids[@]} == 1)) || {
        log "ERROR: generic EFI registration completed but could not be verified uniquely on selected system ESP $esp." | tee -a "$SESSION_LOG" >&2
        return 1
    }
    log "PASS: generic EFI firmware entry restored as Boot${ids[0]} on selected system ESP $esp." | tee -a "$SESSION_LOG"
}

efi_ensure_selected_wfai_entry()
{
    local partuuid="$1" model="$2" esp disk partnum label current loader entry_name os_id resolved
    local -a ids=()

    esp="$(canonical_block "$EFI_ESP_SOURCE" 2>/dev/null || true)"
    [[ -n "$esp" ]] || return 0
    loader="$(efi_selected_wfai_loader || true)"
    [[ -n "$loader" ]] || {
        log "iPXE/WebFAI EFI loader is absent on selected system ESP; no recovery firmware entry was created." | tee -a "$SESSION_LOG"
        return 0
    }

    mapfile -t ids < <(efi_entry_ids_for_partuuid_loader "$partuuid" "${loader,,}" || true)
    if ((${#ids[@]} > 1)); then
        log "Multiple iPXE/WebFAI entries already point to the selected system ESP (${ids[*]}); destination maintenance will retain one after the repair." | tee -a "$SESSION_LOG"
        return 0
    fi
    if ((${#ids[@]} == 1)); then
        return 0
    fi

    uefi_nvram_writable || {
        log "iPXE/WebFAI EFI loader is present on the selected system ESP, but firmware variables are not writable; no recovery firmware entry was created." | tee -a "$SESSION_LOG"
        return 0
    }
    resolved="$(efi_disk_and_partnum_for_esp "$esp" 2>/dev/null || true)"
    IFS=$'\t' read -r disk partnum <<< "$resolved"
    [[ -n "$disk" && "$partnum" =~ ^[0-9]+$ ]] || {
        log "ERROR: unable to derive disk and partition for selected system ESP while restoring WebFAI." | tee -a "$SESSION_LOG" >&2
        return 1
    }
    entry_name="iPXE"
    os_id="${TARGET_OS_ID:-}"
    [[ "${os_id,,}" == tuxedo ]] && entry_name="WFAI"
    label="$entry_name $model"
    log "Restoring missing ${entry_name} firmware entry on selected system ESP $esp as '$label' ($loader)." | tee -a "$SESSION_LOG"
    efibootmgr --create --disk "$disk" --part "$partnum" \
        --label "$label" --loader "$loader" 2>&1 \
        | tee -a "$SESSION_LOG" || return 1
    current="$SESSION_DIR/efi-nvram-wfai.txt"
    efibootmgr -v > "$current" 2>&1 || return 1
    mapfile -t ids < <(efi_entry_ids_for_partuuid_loader "$partuuid" "${loader,,}" || true)
    ((${#ids[@]} == 1)) || {
        log "ERROR: iPXE/WebFAI registration completed but could not be verified uniquely on selected system ESP $esp." | tee -a "$SESSION_LOG" >&2
        return 1
    }
    log "PASS: ${entry_name} firmware entry restored as Boot${ids[0]} on selected system ESP $esp." | tee -a "$SESSION_LOG"
}

# Remove duplicate or legacy firmware destinations for the selected ESP while
# keeping exactly one entry per destination class (UKI, fallback, WFAI and
# vendor loader).  Active/next entries and unknown device paths are never
# removed; the final NVRAM state is re-read and verified from firmware.
efi_prune_selected_duplicate_destinations()
{
    local esp partuuid current_file line id loader label label_key basename key priority
    local current_id next_id order new_order="" removed_ids="" remove_id keep_id keep_priority
    local verify_file verify_key verify_loader verify_basename verify_label_key
    local -a ids=()
    local -a legacy_shim_ids=()
    local -A destination_id=() destination_priority=()
    local -a remove_list=()

    command -v efibootmgr >/dev/null 2>&1 || return 0
    esp="$(canonical_block "$EFI_ESP_SOURCE" 2>/dev/null || true)"
    [[ -n "$esp" ]] || return 0
    efi_set_inventory_esp_ids
    partuuid="${EFI_TARGET_ESP_PARTUUID,,}"
    [[ -n "$partuuid" ]] || return 0

    current_file="$SESSION_DIR/efi-nvram-destination-maintenance.txt"
    efibootmgr -v > "$current_file" 2>&1 || return 1
    current_id="$(sed -nE 's/^BootCurrent: ([0-9A-Fa-f]{4}).*/\1/p' "$current_file" | head -n1 | tr '[:lower:]' '[:upper:]')"
    next_id="$(sed -nE 's/^BootNext: ([0-9A-Fa-f]{4}).*/\1/p' "$current_file" | head -n1 | tr '[:lower:]' '[:upper:]')"

    # A destination is identified by selected ESP PARTUUID plus its decoded
    # loader path. Firmware-generated partition-only fallback records are
    # treated as the conventional BOOTX64.EFI destination only when their
    # label clearly identifies a UEFI partition/OS entry. Unknown device paths
    # remain untouched rather than being guessed.
    while IFS= read -r line; do
        id="$(efi_entry_id_line "$line")"
        [[ -n "$id" ]] || continue
        [[ "$(efi_entry_partuuid_line "$line")" == "$partuuid" ]] || continue
        loader="$(efi_entry_loader_line "$line" || true)"
        label="$(efi_entry_label_line "$line")"
        key=""
        priority=10
        if [[ -n "$loader" ]]; then
            loader="${loader,,}"
            basename="${loader##*\\}"
            case "$loader" in
                '\efi\boot\tux.efi') key="uki"; priority=0 ;;
                '\efi\boot\bootx64.efi') key="fallback"; priority=0 ;;
                *)
                    if [[ "$basename" == ipxe.efi ]]; then
                        key="wfai"
                        priority=0
                    elif [[ "${TARGET_OS_ID,,}" == tuxedo && "$basename" == shimx64.efi \
                          && "$loader" == *"\tuxedo\shimx64.efi" ]]; then
                        # Once a verified TUXEDO UKI exists, the old signed
                        # shim route is a legacy route for the same system.
                        key="legacy-tuxedo-shim"
                        priority=5
                    else
                        key="loader:$loader"
                        priority=0
                    fi
                    ;;
            esac
        else
            label_key="$(efi_label_match_text <<<"$label")"
            if [[ "$label_key" == "uefi os" || ( "$label_key" == uefi\ * && "$label_key" == *partition* ) ]]; then
                key="fallback"
                priority=1
            fi
        fi
        [[ -n "$key" ]] || continue

        if [[ "$key" == legacy-tuxedo-shim ]]; then
            # Defer this decision until every selected-ESP entry has been
            # inspected. A firmware order can list the legacy shim before the
            # canonical UKI; the final pass removes it once that UKI is known.
            legacy_shim_ids+=("$id")
            continue
        fi

        keep_id="${destination_id[$key]:-}"
        if [[ -z "$keep_id" ]]; then
            destination_id["$key"]="$id"
            destination_priority["$key"]="$priority"
            continue
        fi
        keep_priority="${destination_priority[$key]}"
        if [[ "$id" == "$current_id" || "$id" == "$next_id" ]]; then
            remove_list+=("$keep_id")
            destination_id["$key"]="$id"
            destination_priority["$key"]="$priority"
        elif [[ "$keep_id" == "$current_id" || "$keep_id" == "$next_id" ]]; then
            remove_list+=("$id")
        elif (( priority < keep_priority )); then
            remove_list+=("$keep_id")
            destination_id["$key"]="$id"
            destination_priority["$key"]="$priority"
        else
            remove_list+=("$id")
        fi
    done < <(sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p' "$current_file")

    # A TUXEDO UKI and the old signed shim are two ways into the same target
    # installation. Keep the shim only when no UKI exists, or while firmware
    # is actively using it and deleting it would disrupt the current boot.
    if [[ -n "${destination_id[uki]:-}" ]]; then
        for id in "${legacy_shim_ids[@]}"; do
            [[ "$id" == "$current_id" || "$id" == "$next_id" ]] || remove_list+=("$id")
        done
    fi

    ((${#remove_list[@]} > 0)) || {
        log "PASS: selected ESP firmware destinations are unique; no duplicate entries removed." | tee -a "$SESSION_LOG"
        return 0
    }

    # Avoid issuing the same delete twice when an active-entry preference and
    # a destination collision identify the same firmware number.
    local -A removed_seen=()
    for remove_id in "${remove_list[@]}"; do
        [[ -n "${removed_seen[$remove_id]:-}" ]] && continue
        removed_seen["$remove_id"]=1
        [[ "$remove_id" != "$current_id" && "$remove_id" != "$next_id" ]] || {
            log "ERROR: refusing to remove active/next firmware entry Boot$remove_id while pruning selected ESP destinations." | tee -a "$SESSION_LOG" >&2
            return 1
        }
        log "Removing duplicate/legacy selected-ESP firmware destination Boot$remove_id." | tee -a "$SESSION_LOG"
        efibootmgr -b "$remove_id" -B 2>&1 \
            | tee -a "$SESSION_LOG" || return 1
        removed_ids+="${removed_ids:+,}$remove_id"
    done

    # Keep the current order relative to every retained entry, filtering only
    # the entries explicitly removed above.
    current_file="$SESSION_DIR/efi-nvram-destination-maintenance-after.txt"
    efibootmgr -v > "$current_file" 2>&1 || return 1
    order="$(sed -n 's/^BootOrder: //p' "$current_file" | head -n1 || true)"
    if [[ -n "$order" ]]; then
        IFS=',' read -ra ids <<< "$order"
        for id in "${ids[@]}"; do
            id="${id^^}"
            [[ -n "${removed_seen[$id]:-}" ]] && continue
            grep -Eq "^Boot${id}\*?[[:space:]]" "$current_file" || continue
            new_order+="${new_order:+,}$id"
        done
        if [[ -n "$new_order" ]]; then
            efibootmgr -o "$new_order" 2>&1 | tee -a "$SESSION_LOG" || return 1
        fi
    fi

    # Verify the post-state from firmware itself. This catches a helper or
    # firmware implementation that accepted a delete request but left a
    # second record behind, before the repair is reported successful.
    verify_file="$SESSION_DIR/efi-nvram-destination-maintenance-verified.txt"
    efibootmgr -v > "$verify_file" 2>&1 || return 1
    for remove_id in "${!removed_seen[@]}"; do
        ! grep -Eq "^Boot${remove_id}\*?[[:space:]]" "$verify_file" || {
            log "ERROR: firmware still reports removed selected-ESP entry Boot$remove_id." | tee -a "$SESSION_LOG" >&2
            return 1
        }
    done
    local -A final_destination_count=()
    while IFS= read -r line; do
        id="$(efi_entry_id_line "$line")"
        [[ -n "$id" ]] || continue
        [[ "$(efi_entry_partuuid_line "$line")" == "$partuuid" ]] || continue
        verify_loader="$(efi_entry_loader_line "$line" || true)"
        verify_loader="${verify_loader,,}"
        verify_basename="${verify_loader##*\\}"
        verify_key=""
        case "$verify_loader" in
            '\efi\boot\tux.efi') verify_key="uki" ;;
            '\efi\boot\bootx64.efi') verify_key="fallback" ;;
            *) [[ "$verify_basename" == ipxe.efi ]] && verify_key="wfai" ;;
        esac
        if [[ -z "$verify_key" && -z "$verify_loader" ]]; then
            verify_label_key="$(efi_label_match_text <<<"$(efi_entry_label_line "$line")")"
            [[ "$verify_label_key" == "uefi os" || ( "$verify_label_key" == uefi\ * && "$verify_label_key" == *partition* ) ]] && verify_key="fallback"
        fi
        [[ -n "$verify_key" ]] || continue
        final_destination_count["$verify_key"]=$(( ${final_destination_count[$verify_key]:-0} + 1 ))
    done < <(sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p' "$verify_file")
    for verify_key in uki fallback wfai; do
        (( ${final_destination_count[$verify_key]:-0} <= 1 )) || {
            log "ERROR: selected ESP still has duplicate $verify_key destinations after maintenance." | tee -a "$SESSION_LOG" >&2
            return 1
        }
    done
    log "PASS: selected ESP firmware destinations reconciled; removed Boot$removed_ids." | tee -a "$SESSION_LOG"
}

# Reorder BootOrder so each disk's entries are grouped by normal boot use
# (host ESP first, then repair ESP, then foreign), with UKI before vendor
# loader, fallback and WFAI.  Entries intentionally omitted from BootOrder and
# all unknown paths are preserved exactly.
efi_group_firmware_boot_order()
{
    local current_file line id part loader basename role_key group rank seq current_order sorted_order candidate_file label_key
    local -a ids=()
    local -A order_index=() seen=()

    command -v efibootmgr >/dev/null 2>&1 || return 0
    efi_set_inventory_esp_ids
    current_file="$SESSION_DIR/efi-nvram-grouped-order.txt"
    efibootmgr -v > "$current_file" 2>&1 || return 1
    current_order="$(sed -n 's/^BootOrder: //p' "$current_file" | head -n1 || true)"

    if [[ -n "$current_order" ]]; then
        IFS=',' read -ra ids <<< "$current_order"
        seq=0
        for id in "${ids[@]}"; do
            id="${id^^}"
            [[ "$id" =~ ^[0-9A-F]{4}$ ]] || continue
            order_index["$id"]="$seq"
            seq=$((seq + 1))
        done
    fi

    # Sort complete ESP groups without inventing any new paths. UKI is the
    # primary route where present, followed by the conventional vendor
    # loader, the firmware fallback, and WebFAI. Unknown entries retain their
    # membership and are placed after recognized routes for that ESP.
    candidate_file="$SESSION_DIR/efi-nvram-grouped-candidates.tsv"
    : > "$candidate_file"
    seq=0
    while IFS= read -r line; do
        id="$(efi_entry_id_line "$line")"
        [[ -n "$id" && -z "${seen[$id]:-}" ]] || continue
        seen["$id"]=1
        # BootOrder may intentionally omit an otherwise valid, inactive
        # firmware record. Reordering must preserve that omission.
        if [[ -n "$current_order" && -z "${order_index[$id]+present}" ]]; then
            continue
        fi
        part="$(efi_entry_partuuid_line "$line")"
        loader="$(efi_entry_loader_line "$line" || true)"
        loader="${loader,,}"
        basename="${loader##*\\}"
        role_key="other"
        if [[ "$loader" == '\efi\boot\tux.efi' ]]; then
            role_key="uki"
        elif [[ "$basename" == bootx64.efi ]]; then
            role_key="fallback"
        elif [[ "$basename" == ipxe.efi ]]; then
            role_key="wfai"
        elif [[ -n "$loader" ]]; then
            role_key="loader"
        else
            label_key="$(efi_label_match_text <<<"$(efi_entry_label_line "$line")")"
            [[ "$label_key" == "uefi os" || ( "$label_key" == uefi\ * && "$label_key" == *partition* ) ]] \
                && role_key="fallback"
        fi

        if [[ -n "$part" && "$part" == "${EFI_HOST_ESP_PARTUUID,,}" ]]; then
            group=0
        elif [[ -n "$part" && "$part" == "${EFI_TARGET_ESP_PARTUUID,,}" ]]; then
            group=1
        else
            group=2
        fi
        case "$role_key" in
            uki) rank=0 ;;
            loader) rank=1 ;;
            fallback) rank=2 ;;
            wfai) rank=3 ;;
            *) rank=4 ;;
        esac
        if [[ -z "${order_index[$id]:-}" ]]; then
            order_index["$id"]=$((100000 + seq))
        fi
        printf '%03d\t%06d\t%s\n' "$((group * 10 + rank))" "${order_index[$id]}" "$id" >> "$candidate_file"
        seq=$((seq + 1))
    done < <(sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p' "$current_file")

    sorted_order="$(sort -n -k1,1 -k2,2 "$candidate_file" | cut -f3 | paste -sd, -)"
    [[ -n "$sorted_order" && "$sorted_order" != "$current_order" ]] || {
        log "PASS: EFI BootOrder already groups each ESP's vendor destinations by normal use." | tee -a "$SESSION_LOG"
        return 0
    }
    log "Grouping EFI BootOrder by ESP and normal boot use: $sorted_order" | tee -a "$SESSION_LOG"
    efibootmgr -o "$sorted_order" 2>&1 | tee -a "$SESSION_LOG" || return 1
    log "PASS: EFI BootOrder grouped by drive; all retained firmware entries remain present." | tee -a "$SESSION_LOG"
}

# Append the selected drive's model to firmware labels of selected-ESP entries
# (through the root-owned label updater with per-entry backups) and restore a
# missing iPXE/WebFAI recovery registration.  Host and foreign ESPs untouched.
efi_annotate_selected_entries()
{
    local esp partuuid model line id part loader role label new_label updater backup_dir backup
    local nvram="$SESSION_DIR/efi-nvram-annotate-before.txt"

    command -v efibootmgr >/dev/null 2>&1 || return 0
    esp="$(canonical_block "$EFI_ESP_SOURCE" 2>/dev/null || true)"
    [[ -n "$esp" ]] || return 0
    partuuid="$(blkid -s PARTUUID -o value "$esp" 2>/dev/null || true)"
    partuuid="${partuuid,,}"
    [[ -n "$partuuid" ]] || return 0
    model="$(efi_selected_system_model "$esp" 2>/dev/null || true)"
    [[ -n "$model" ]] || return 0

    efibootmgr -v > "$nvram" 2>&1 || return 1
    log "Annotating selected-system EFI entries with OS/model identity: ${TARGET_PRETTY:-${TARGET_OS_ID:-Linux}} / $model (PARTUUID $partuuid)." | tee -a "$SESSION_LOG"
    while IFS= read -r line; do
        id="$(efi_entry_id_line "$line")"
        [[ -n "$id" ]] || continue
        part="$(efi_entry_partuuid_line "$line")"
        [[ "$part" == "$partuuid" ]] || continue
        loader="$(efi_entry_loader_line "$line" || true)"
        role="$(efi_selected_entry_role "$loader" || true)"
        [[ -n "$role" ]] || continue
        label="$(efi_entry_label_line "$line")"
        new_label="$(efi_label_with_selected_model "$label" "$model")"
        if [[ "$new_label" == "$label" ]]; then
            log "EFI Boot$id already names selected model; leaving label '$label' unchanged." | tee -a "$SESSION_LOG"
            continue
        fi
        updater="${EFI_LABEL_UPDATER:-/usr/libexec/boot-repair/boot-repair-efi-label.py}"
        if [[ ! -x "$updater" ]]; then
            updater="$(dirname "${BASH_SOURCE[0]}")/boot-repair-efi-label.py"
        fi
        [[ -x "$updater" ]] || {
            log "ERROR: EFI label updater is not installed; refusing to report an unverified label change." | tee -a "$SESSION_LOG" >&2
            return 1
        }
        backup_dir="$SESSION_DIR/efi-label-backups"
        backup="$backup_dir/Boot${id^^}.bin"
        "$updater" --bootnum "$id" --partuuid "$partuuid" \
            --loader "$loader" --label "$new_label" --backup "$backup" 2>&1 \
            | tee -a "$SESSION_LOG" || return 1
        log "EFI Boot$id ($role) label updated to '$new_label' on selected system ESP only." | tee -a "$SESSION_LOG"
    done < <(sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p' "$nvram")

    efi_ensure_selected_wfai_entry "$partuuid" "$model"
}

efi_verify_selected_model_labels()
{
    local esp partuuid model nvram line id part loader role label

    command -v efibootmgr >/dev/null 2>&1 || return 0
    esp="$(canonical_block "$EFI_ESP_SOURCE" 2>/dev/null || true)"
    [[ -n "$esp" ]] || return 0
    partuuid="$(blkid -s PARTUUID -o value "$esp" 2>/dev/null || true)"
    partuuid="${partuuid,,}"
    [[ -n "$partuuid" ]] || return 0
    model="$(efi_selected_system_model "$esp" 2>/dev/null || true)"
    [[ -n "$model" ]] || return 0

    nvram="$SESSION_DIR/efi-nvram-labels-verified.txt"
    efibootmgr -v > "$nvram" 2>&1 || return 1
    while IFS= read -r line; do
        id="$(efi_entry_id_line "$line")"
        [[ -n "$id" ]] || continue
        part="$(efi_entry_partuuid_line "$line")"
        [[ "$part" == "$partuuid" ]] || continue
        loader="$(efi_entry_loader_line "$line" || true)"
        role="$(efi_selected_entry_role "$loader" || true)"
        [[ -n "$role" ]] || continue
        label="$(efi_entry_label_line "$line")"
        efi_label_has_selected_model "$label" "$model" || {
            log "ERROR: selected ESP Boot$id label '$label' still lacks drive model '$model' after EFI order maintenance." | tee -a "$SESSION_LOG" >&2
            return 1
        }
    done < <(sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p' "$nvram")
    log "PASS: selected ESP EFI labels retain the drive model after final BootOrder maintenance." | tee -a "$SESSION_LOG"
}

efi_entry_key_line()
{
    local line="$1" part label loader definition
    part="$(efi_entry_partuuid_line "$line")"
    label="$(efi_entry_label_line "$line")"
    loader="$(efi_entry_loader_line "$line" || true)"
    definition="$(efi_entry_definition_line "$line")"
    if [[ -n "$part" ]]; then
        # PARTUUID + label + loader distinguishes two valid loaders on one
        # ESP while remaining stable when firmware assigns a new Boot#### ID.
        printf '%s|%s|%s\n' "$part" "${label,,}" "${loader,,}"
    else
        # Keep entries whose firmware path cannot be decoded in the identity
        # model. They can be preserved exactly, but are intentionally not
        # reconstructed from a guessed path if a vendor tool deletes them.
        printf 'unknown|%s\n' "${definition,,}"
    fi
}

efi_entry_partition_label_key_line()
{
    local line="$1" part label
    part="$(efi_entry_partuuid_line "$line")"
    label="$(efi_entry_label_line "$line")"
    [[ -n "$part" ]] || return 1
    printf '%s|%s\n' "$part" "${label,,}"
}

efi_entry_class_line()
{
    local line="$1" host_partuuid="$2" target_partuuid="$3" part
    part="$(efi_entry_partuuid_line "$line")"
    if [[ -n "$host_partuuid" && "$part" == "${host_partuuid,,}" \
          && ( "$RUNNING_HOST_MODE" == 1 || -z "$target_partuuid" || "$part" != "${target_partuuid,,}" ) ]]; then
        printf 'host\n'
    elif [[ -n "$target_partuuid" && "$part" == "${target_partuuid,,}" ]]; then
        printf 'repair\n'
    elif [[ -n "$part" ]]; then
        printf 'foreign\n'
    else
        printf 'unknown\n'
    fi
}

efi_host_esp_source()
{
    local source esp_mount="/boot/efi"
    if [[ "$RUNNING_HOST_MODE" == 1 && -n "$TARGET_ESP_MOUNT" ]]; then
        esp_mount="$TARGET_ESP_MOUNT"
    fi
    # systemd automounts expose an `autofs` row before the actual vfat row;
    # choose the block-backed mount rather than the synthetic systemd-1 source.
    source="$(findmnt -rn -o SOURCE,FSTYPE --target "$esp_mount" 2>/dev/null \
        | awk '$1 ~ /^\/dev\// && $2 ~ /^(vfat|fat|fat16|fat32|msdos)$/ {print $1; exit}' || true)"
    source="${source%%\[*}"
    [[ -n "$source" && -b "$source" ]] || return 1
    canonical_block "$source"
}

efi_set_inventory_esp_ids()
{
    local host_source=""
    EFI_TARGET_ESP_PARTUUID=""
    EFI_HOST_ESP_SOURCE=""
    EFI_HOST_ESP_PARTUUID=""
    EFI_HOST_ESP_MOUNT=""

    if [[ -n "$EFI_ESP_SOURCE" && -b "$EFI_ESP_SOURCE" ]]; then
        EFI_TARGET_ESP_PARTUUID="$(blkid -s PARTUUID -o value "$EFI_ESP_SOURCE" 2>/dev/null || true)"
        EFI_TARGET_ESP_PARTUUID="${EFI_TARGET_ESP_PARTUUID,,}"
    fi
    host_source="$(efi_host_esp_source 2>/dev/null || true)"
    if [[ -n "$host_source" ]]; then
        EFI_HOST_ESP_SOURCE="$host_source"
        EFI_HOST_ESP_MOUNT="${TARGET_ESP_MOUNT:-/boot/efi}"
        EFI_HOST_ESP_PARTUUID="$(blkid -s PARTUUID -o value "$host_source" 2>/dev/null || true)"
        EFI_HOST_ESP_PARTUUID="${EFI_HOST_ESP_PARTUUID,,}"
    fi
}

efi_print_firmware_inventory()
{
    local nvram="$1" line id class part label loader role selected_label="repair-ESP"
    [[ -s "$nvram" ]] || return 0
    efi_set_inventory_esp_ids
    [[ "$RUNNING_HOST_MODE" == 1 ]] && selected_label="selected-system-ESP"
    printf 'EFI inventory (all firmware entries): host-ESP=%s (%s) %s=%s (%s)\n' \
        "${EFI_HOST_ESP_PARTUUID:-unknown}" "${EFI_HOST_ESP_SOURCE:-unresolved}" \
        "$selected_label" \
        "${EFI_TARGET_ESP_PARTUUID:-unknown}" "${EFI_ESP_SOURCE:-unresolved}"
    while IFS= read -r line; do
        id="$(efi_entry_id_line "$line")"
        [[ -n "$id" ]] || continue
        class="$(efi_entry_class_line "$line" "$EFI_HOST_ESP_PARTUUID" "$EFI_TARGET_ESP_PARTUUID")"
        part="$(efi_entry_partuuid_line "$line")"
        label="$(efi_entry_label_line "$line")"
        loader="$(efi_entry_loader_line "$line" || true)"
        role="$(efi_entry_destination_role "$line")"
        printf '  Boot%s class=%s role=%s partuuid=%s label=%s loader=%s\n' \
            "$id" "$class" "$role" "${part:-unknown}" "$label" "${loader:-device-path-only}"
    done < <(sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p' "$nvram")
}

efi_find_entry_by_key()
{
    local nvram="$1" wanted="$2" line key id
    while IFS= read -r line; do
        key="$(efi_entry_key_line "$line")"
        [[ "$key" == "$wanted" ]] || continue
        id="$(efi_entry_id_line "$line")"
        [[ -n "$id" ]] && { printf '%s\n' "$id"; return 0; }
    done < <(sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p' "$nvram")
    return 1
}

# Recreate one removed firmware entry from its captured efibootmgr definition.
# Only ordinary EFI file-path entries are reconstructed; vendor-specific device
# paths fail closed.  Prints the verified new Boot#### ID on stdout while all
# diagnostics go to stderr so the caller can capture the ID safely.
efi_create_entry_from_definition()
{
    local line="$1" class="$2" partuuid label loader esp disk partnum current id resolved
    partuuid="$(efi_entry_partuuid_line "$line")"
    label="$(efi_entry_label_line "$line")"
    loader="$(efi_entry_loader_line "$line" || true)"
    [[ -n "$partuuid" ]] || {
        log "ERROR: cannot restore $class firmware entry without a GPT PARTUUID: $line" | tee -a "$SESSION_LOG" >&2
        return 1
    }
    # Reconstruct only ordinary EFI file-path entries. PCI-only or vendor
    # paths (for example a firmware-generated `UEFI OS` entry) have no safe
    # efibootmgr command-line equivalent and must never be guessed.
    [[ "$loader" =~ ^\\EFI\\[^[:space:]]+\.[Ee][Ff][Ii]$ ]] || {
        log "ERROR: $class firmware entry $label disappeared, but its device path is vendor-specific; refusing to guess a replacement." | tee -a "$SESSION_LOG" >&2
        return 1
    }
    esp="$(blkid -t "PARTUUID=$partuuid" -o device 2>/dev/null | head -n1 || true)"
    [[ -n "$esp" && -b "$esp" ]] || {
        log "ERROR: cannot resolve ESP PARTUUID $partuuid while restoring $class firmware entry $label." | tee -a "$SESSION_LOG" >&2
        return 1
    }
    resolved="$(efi_disk_and_partnum_for_esp "$esp" 2>/dev/null || true)"
    IFS=$'\t' read -r disk partnum <<< "$resolved"
    [[ -n "$disk" && "$partnum" =~ ^[0-9]+$ ]] || {
        log "ERROR: cannot derive disk/partition for ESP $esp while restoring firmware entry $label." | tee -a "$SESSION_LOG" >&2
        return 1
    }
    log "Restoring $class firmware entry '$label' for PARTUUID $partuuid (EFI path $loader)." | tee -a "$SESSION_LOG" >&2
    # The caller captures this function's stdout as the restored Boot#### ID.
    # Keep every diagnostic on stderr: efibootmgr warns on stderr when another
    # entry already uses the label (a two-ESP system has one `arch` entry per
    # disk), and that warning must never be captured as the new ID.
    efibootmgr --create --disk "$disk" --part "$partnum" \
        --label "$label" --loader "$loader" 2>&1 | tee -a "$SESSION_LOG" >&2 \
        || return 1
    current="$SESSION_DIR/efi-nvram-recreate.$partuuid"
    efibootmgr -v > "$current" 2>&1 || return 1
    id="$(efi_find_entry_by_key "$current" "$(efi_entry_key_line "$line")" || true)"
    [[ "$id" =~ ^[0-9A-Fa-f]{4}$ ]] || {
        log "ERROR: efibootmgr completed but restored $class entry '$label' could not be verified by identity (got '${id:-empty}'); refusing to use an unverified firmware ID." | tee -a "$SESSION_LOG" >&2
        return 1
    }
    printf '%s\n' "$id"
}

efi_target_uki_entry_ids()
{
    local partuuid
    command -v efibootmgr >/dev/null 2>&1 || return 1
    partuuid="$(blkid -s PARTUUID -o value "$EFI_ESP_SOURCE" 2>/dev/null || true)"
    [[ -n "$partuuid" ]] || return 1

    efi_uki_entry_ids_for_partuuid "$partuuid"
}

efi_uki_entry_ids_for_partuuid()
{
    local partuuid="${1,,}"
    command -v efibootmgr >/dev/null 2>&1 || return 1
    [[ -n "$partuuid" ]] || return 1

    efibootmgr -v 2>/dev/null \
        | awk -v partuuid="$partuuid" '
            BEGIN { IGNORECASE=1 }
            /^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/ \
                && index(tolower($0), tolower(partuuid)) \
                && index($0, "TUXEDO UKI") \
                && index(toupper($0), "TUX.EFI") {
                    id=$1
                    sub(/^Boot/, "", id)
                    sub(/\*.*/, "", id)
                    print toupper(id)
                }
        ' | sort -u
}

efi_entry_ids_for_partuuid_role()
{
    local partuuid="${1,,}" wanted_role="$2" nvram line part role id
    command -v efibootmgr >/dev/null 2>&1 || return 1
    [[ -n "$partuuid" ]] || return 1
    nvram="$(efibootmgr -v 2>/dev/null || true)"
    while IFS= read -r line; do
        part="$(efi_entry_partuuid_line "$line")"
        [[ "$part" == "$partuuid" ]] || continue
        role="$(efi_entry_destination_role "$line")"
        [[ "$role" == "$wanted_role" ]] || continue
        id="$(efi_entry_id_line "$line")"
        [[ -n "$id" ]] && printf '%s\n' "${id^^}"
    done < <(sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p' <<<"$nvram")
}

restore_missing_host_tuxedo_uki_entry()
{
    local allow_same_esp="${1:-0}" host_mount host_uki host_esp disk partnum current resolved model label
    local -a ids=()

    # This is deliberately limited to the running host ESP.  A repair of a
    # second disk may restore a missing host registration, but it must never
    # scan arbitrary ESPs or infer a TUXEDO installation from a disk model.
    efi_set_inventory_esp_ids
    host_esp="$EFI_HOST_ESP_SOURCE"
    host_mount="${EFI_HOST_ESP_MOUNT:-/boot/efi}"
    [[ -n "$host_esp" && -n "$EFI_HOST_ESP_PARTUUID" ]] || return 0
    [[ "$allow_same_esp" == 1 || "$host_esp" != "$EFI_ESP_SOURCE" ]] || return 0
    [[ -d "$host_mount/EFI/TUXEDO" ]] || return 0
    host_uki="$host_mount/EFI/BOOT/TUX.EFI"
    [[ -s "$host_uki" ]] || return 0

    mapfile -t ids < <(efi_uki_entry_ids_for_partuuid "$EFI_HOST_ESP_PARTUUID" 2>/dev/null || true)
    if ((${#ids[@]} == 1)); then
        log "Host TUXEDO UKI firmware entry already present: Boot${ids[0]} on $host_esp." | tee -a "$SESSION_LOG"
        return 0
    fi
    if ((${#ids[@]} > 1)); then
        log "ERROR: more than one host TUXEDO UKI entry points to $host_esp: ${ids[*]}; refusing to remove or choose one." | tee -a "$SESSION_LOG" >&2
        return 1
    fi

    uefi_nvram_writable || {
        log "Host TUXEDO UKI file is present on $host_esp, but firmware variables are not writable; host registration was not changed." | tee -a "$SESSION_LOG"
        return 0
    }
    command -v efibootmgr >/dev/null 2>&1 || return 0
    host_esp="$(canonical_block "$host_esp" 2>/dev/null || true)"
    [[ -n "$host_esp" ]] && is_block_device "$host_esp" || {
        log "ERROR: host TUXEDO UKI file is present, but its ESP could not be resolved as a block device." | tee -a "$SESSION_LOG" >&2
        return 1
    }
    resolved="$(efi_disk_and_partnum_for_esp "$host_esp" 2>/dev/null || true)"
    IFS=$'\t' read -r disk partnum <<< "$resolved"
    [[ -n "$disk" && "$partnum" =~ ^[0-9]+$ ]] || {
        log "ERROR: unable to derive the host disk and partition for ESP $host_esp." | tee -a "$SESSION_LOG" >&2
        return 1
    }

    label="TUXEDO UKI"
    model="$(efi_selected_system_model "$host_esp" 2>/dev/null || true)"
    if [[ -n "$model" ]]; then
        label="$(efi_label_with_selected_model "$label" "$model")"
    fi
    log "Restoring missing host TUXEDO UKI firmware entry on $host_esp as '$label'; existing host, repair, and foreign entries are untouched." | tee -a "$SESSION_LOG"
    efibootmgr --create --disk "$disk" --part "$partnum" \
        --label "$label" --loader '\EFI\BOOT\TUX.EFI' 2>&1 \
        | tee -a "$SESSION_LOG" || return 1
    current="$SESSION_DIR/efi-nvram-host-uki.txt"
    efibootmgr -v > "$current" 2>&1 || return 1
    mapfile -t ids < <(efi_uki_entry_ids_for_partuuid "$EFI_HOST_ESP_PARTUUID" 2>/dev/null || true)
    ((${#ids[@]} == 1)) || {
        log "ERROR: host TUXEDO UKI registration completed but could not be verified uniquely on $host_esp." | tee -a "$SESSION_LOG" >&2
        return 1
    }
    log "PASS: host TUXEDO UKI firmware entry restored as Boot${ids[0]} on $host_esp." | tee -a "$SESSION_LOG"
}

# Compare the pre-repair firmware inventory with the current one, map every
# pre-existing entry to its current identity and recreate entries a vendor tool
# removed.  Writes an old-ID -> new-ID map file for BootOrder restoration.
efi_reconcile_firmware_inventory()
{
    local before="$1" map_file="$2" current_file line id old_id key class mapped definition current_line
    local partition_label_key
    local -A current_by_id=() current_by_key=() current_by_partition_label=()
    local -a old_ids=()

    [[ -s "$before" ]] || return 0
    efi_set_inventory_esp_ids
    current_file="$SESSION_DIR/efi-nvram-current-before-reconcile.txt"
    efibootmgr -v > "$current_file" 2>&1 \
        || fail "Unable to capture firmware entries for post-operation reconciliation."
    : > "$map_file"
    mapfile -t old_ids < <(sed -nE 's/^Boot([0-9A-Fa-f]{4})\*?[[:space:]].*/\1/p' "$before" | tr '[:lower:]' '[:upper:]' | sort -u)

    while IFS= read -r line; do
        id="$(efi_entry_id_line "$line")"
        [[ -n "$id" ]] || continue
        current_by_id["$id"]="$line"
        key="$(efi_entry_key_line "$line")"
        [[ -n "${current_by_key[$key]:-}" ]] || current_by_key["$key"]="$id"
        partition_label_key="$(efi_entry_partition_label_key_line "$line" || true)"
        [[ -n "$partition_label_key" && -n "${current_by_partition_label[$partition_label_key]:-}" ]] \
            || [[ -z "$partition_label_key" ]] \
            || current_by_partition_label["$partition_label_key"]="$id"
    done < <(sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p' "$current_file")

    # First map exact IDs. If firmware/vendor tooling reused an ID, fall back
    # to the stable PARTUUID/label/loader identity before recreating anything.
    for old_id in "${old_ids[@]}"; do
        line="$(grep -E "^Boot${old_id}\*?[[:space:]]" "$before" | head -n1 || true)"
        [[ -n "$line" ]] || continue
        key="$(efi_entry_key_line "$line")"
        class="$(efi_entry_class_line "$line" "$EFI_HOST_ESP_PARTUUID" "$EFI_TARGET_ESP_PARTUUID")"
        definition="$(efi_entry_definition_line "$line")"
        current_line="${current_by_id[$old_id]:-}"
        mapped=""
        if [[ -n "$current_line" && "$(efi_entry_definition_line "$current_line")" == "$definition" ]]; then
            mapped="$old_id"
        elif [[ -n "${current_by_key[$key]:-}" ]]; then
            mapped="${current_by_key[$key]}"
            log "EFI entry identity preserved while firmware ID changed: Boot$old_id -> Boot$mapped." | tee -a "$SESSION_LOG"
        elif [[ "$class" == repair ]]; then
            # A conventional GRUB install may intentionally replace only the
            # loader file on the selected repair ESP (for example shim ->
            # grubx64.efi) while retaining the same partition and entry label.
            # Accept that target-side replacement; host and foreign entries
            # still require an exact loader identity and are reconstructed.
            partition_label_key="$(efi_entry_partition_label_key_line "$line" || true)"
            if [[ -n "$partition_label_key" && -n "${current_by_partition_label[$partition_label_key]:-}" ]]; then
                mapped="${current_by_partition_label[$partition_label_key]}"
                log "Repair-target EFI entry label preserved while loader changed: Boot$old_id -> Boot$mapped." | tee -a "$SESSION_LOG"
            fi
        fi
        if [[ -z "$mapped" ]]; then
            mapped="$(efi_create_entry_from_definition "$line" "$class" || true)"
            [[ -n "$mapped" ]] || return 1
            current_file="$SESSION_DIR/efi-nvram-current-before-reconcile.txt"
            efibootmgr -v > "$current_file" 2>&1 || return 1
            current_by_id=()
            current_by_key=()
            current_by_partition_label=()
            while IFS= read -r current_line; do
                id="$(efi_entry_id_line "$current_line")"
                [[ -n "$id" ]] || continue
                current_by_id["$id"]="$current_line"
                key="$(efi_entry_key_line "$current_line")"
                [[ -n "${current_by_key[$key]:-}" ]] || current_by_key["$key"]="$id"
                partition_label_key="$(efi_entry_partition_label_key_line "$current_line" || true)"
                [[ -n "$partition_label_key" && -n "${current_by_partition_label[$partition_label_key]:-}" ]] \
                    || [[ -z "$partition_label_key" ]] \
                    || current_by_partition_label["$partition_label_key"]="$id"
            done < <(sed -nE '/^Boot[0-9A-Fa-f]{4}\*?[[:space:]]/p' "$current_file")
        fi
        printf '%s\t%s\t%s\n' "$old_id" "$mapped" "$key" >> "$map_file"
    done

    efi_print_firmware_inventory "$current_file" | tee -a "$SESSION_LOG"
}

# Restore BootOrder and BootNext from the pre-repair capture through the
# reconciled identity map, preserving pre-existing membership and appending
# entries created during the repair (extra_id is placed last).
efi_restore_reconciled_order()
{
    local before="$1" map_file="$2" extra_id="${3:-}" old_order old_next mapped id current out_csv="" seen_csv="," mapped_csv="," next_mapped=""
    local -a ids=()
    old_order="$(sed -n 's/^BootOrder: //p' "$before" | head -n1 || true)"
    old_next="$(sed -nE 's/^BootNext: ([0-9A-Fa-f]{4}).*/\1/p' "$before" | head -n1 | tr '[:lower:]' '[:upper:]' || true)"
    current="$(efibootmgr -v 2>/dev/null || true)"

    if [[ -n "$old_order" ]]; then
        IFS=',' read -ra ids <<< "$old_order"
        for id in "${ids[@]}"; do
            id="${id^^}"
            [[ "$id" =~ ^[0-9A-F]{4}$ ]] || continue
            mapped="$(awk -F '\t' -v old="$id" '$1 == old {print $2; exit}' "$map_file" 2>/dev/null || true)"
            [[ -n "$mapped" ]] || {
                log "ERROR: no reconciled firmware identity for pre-existing EFI entry Boot$id; refusing to guess a BootOrder position." | tee -a "$SESSION_LOG" >&2
                return 1
            }
            [[ "$mapped" =~ ^[0-9A-Fa-f]{4}$ ]] || {
                log "ERROR: pre-existing EFI entry Boot$id mapped to invalid firmware ID '${mapped}'; refusing to restore a partially reconciled BootOrder." | tee -a "$SESSION_LOG" >&2
                return 1
            }
            grep -Eq "^Boot${mapped}\*?[[:space:]]" <<<"$current" || {
                log "ERROR: reconciled EFI entry Boot$mapped (from pre-existing Boot$id) is absent from firmware; refusing to restore BootOrder without it." | tee -a "$SESSION_LOG" >&2
                return 1
            }
            [[ "$seen_csv" == *",$mapped,"* ]] && continue
            seen_csv+="$mapped,"
            out_csv+="${out_csv:+,}$mapped"
        done
    fi

    while IFS=$'\t' read -r _old_id mapped _key; do
        [[ -n "$mapped" ]] || continue
        mapped_csv+="$mapped,"
    done < "$map_file"

    # Preserve entries created by a repair tool after the pre-state capture.
    # Entries that existed before but were intentionally absent from
    # BootOrder stay absent; only genuinely new entries are appended.
    while IFS= read -r id; do
        [[ -n "$id" ]] || continue
        [[ "$mapped_csv" == *",$id,"* ]] && continue
        [[ "$seen_csv" == *",$id,"* ]] && continue
        seen_csv+="$id,"
        out_csv+="${out_csv:+,}$id"
    done < <(sed -nE 's/^Boot([0-9A-Fa-f]{4})\*?[[:space:]].*/\1/p' <<<"$current" | tr '[:lower:]' '[:upper:]' | sort -u)

    if [[ -n "$extra_id" && "$seen_csv" != *",${extra_id^^},"* ]]; then
        out_csv+="${out_csv:+,}${extra_id^^}"
    fi
    if [[ -n "$out_csv" ]]; then
        log "Restoring reconciled EFI BootOrder (all pre-existing entries retained): $out_csv" | tee -a "$SESSION_LOG"
        efibootmgr -o "$out_csv" 2>&1 | tee -a "$SESSION_LOG" || return 1
    fi

    if [[ -n "$old_next" ]]; then
        next_mapped="$(awk -F '\t' -v old="$old_next" '$1 == old {print $2; exit}' "$map_file" 2>/dev/null || true)"
        [[ -n "$next_mapped" ]] || return 1
        log "Restoring reconciled BootNext: $next_mapped" | tee -a "$SESSION_LOG"
        efibootmgr -n "$next_mapped" 2>&1 | tee -a "$SESSION_LOG" || return 1
    elif grep -q '^BootNext:' <<<"$before"; then
        # The pre-state explicitly had no active BootNext; clear any value a
        # builder may have introduced while preserving the rest of NVRAM.
        log "Clearing BootNext introduced during EFI repair." | tee -a "$SESSION_LOG"
        efibootmgr -N 2>&1 | tee -a "$SESSION_LOG" || return 1
    fi
}

# Return the selected ESP's single TUXEDO UKI firmware entry, creating and
# verifying one when absent.  Prints the Boot#### ID on success; returns
# non-zero when firmware is unwritable or the result cannot be verified.
ensure_target_tuxedo_uki_entry()
{
    local -a ids=()
    local esp disk partnum resolved model label

    mapfile -t ids < <(efi_target_uki_entry_ids 2>/dev/null || true)
    if ((${#ids[@]} == 1)); then
        printf '%s\n' "${ids[0]}"
        return 0
    fi
    if ((${#ids[@]} > 1)); then
        # Keep the first verified path long enough for BootOrder reconciliation;
        # the selected-ESP destination pass removes the remaining duplicate
        # after all vendor changes have completed.
        log "Multiple TUXEDO UKI entries already point to the selected ESP (${ids[*]}); destination maintenance will retain one after the repair." | tee -a "$SESSION_LOG"
        printf '%s\n' "${ids[0]}"
        return 0
    fi

    uefi_nvram_writable || return 1
    command -v efibootmgr >/dev/null 2>&1 || return 1
    esp="$(canonical_block "$EFI_ESP_SOURCE" 2>/dev/null || true)"
    is_block_device "$esp" || return 1
    resolved="$(efi_disk_and_partnum_for_esp "$esp" 2>/dev/null || true)"
    IFS=$'\t' read -r disk partnum <<< "$resolved"
    [[ -n "$disk" && "$partnum" =~ ^[0-9]+$ ]] || return 1

    label="TUXEDO UKI"
    model="$(efi_selected_system_model "$esp" 2>/dev/null || true)"
    if [[ -n "$model" ]]; then
        label="$(efi_label_with_selected_model "$label" "$model")"
    fi
    log "TUXEDO UKI firmware entry is absent for the selected system ESP; creating only that selected-system entry as '$label'." | tee -a "$SESSION_LOG" >&2
    efibootmgr --create --disk "$disk" --part "$partnum" \
        --label "$label" --loader '\EFI\BOOT\TUX.EFI' 2>&1 \
        | tee -a "$SESSION_LOG" >&2 || return 1
    mapfile -t ids < <(efi_target_uki_entry_ids 2>/dev/null || true)
    ((${#ids[@]} == 1)) || return 1
    printf '%s\n' "${ids[0]}"
}

run_tuxedo_uki_builder()
{
    local label="$1"; shift
    local session_tag="${SESSION_DIR##*/}" guard_parent guard_dir wrapper real_efibootmgr="" rc
    local target_real parent_real

    [[ "$session_tag" =~ ^session\.[[:alnum:]_-]+$ ]] \
        || fail "Unable to derive a safe request identifier for the temporary EFI guard."
    guard_parent="$TARGET_ROOT/usr/local/libexec"
    guard_dir="$guard_parent/boot-repair-efi-guard.$session_tag"
    wrapper="$guard_dir/efibootmgr"
    target_real="$(realpath_existing "$TARGET_ROOT" 2>/dev/null || true)"
    parent_real="$(realpath -m "$guard_parent" 2>/dev/null || true)"
    [[ -n "$target_real" && -n "$parent_real" ]] \
        || fail "Unable to resolve the target path for the temporary EFI guard."
    path_within "$parent_real" "$target_real" \
        || fail "Temporary EFI guard path escapes the selected target: $guard_parent"

    # The vendor script currently deletes the first globally matching
    # `TUXEDO UKI` entry before creating a new one.  With two TUXEDO ESPs that
    # can remove the other disk's valid UKI.  Put a request-scoped shim first
    # in PATH: reads still use efibootmgr, but the vendor's delete/create calls
    # are no-ops.  The helper performs the selected-target registration itself
    # after the UKI image has been verified.
    if [[ -x "$TARGET_ROOT/usr/bin/efibootmgr" ]]; then
        real_efibootmgr="/usr/bin/efibootmgr"
    elif [[ -x "$TARGET_ROOT/usr/sbin/efibootmgr" ]]; then
        real_efibootmgr="/usr/sbin/efibootmgr"
    fi

    if [[ -n "$real_efibootmgr" ]]; then
        [[ ! -e "$guard_dir" && ! -L "$guard_dir" ]] \
            || fail "Temporary EFI guard path already exists: $guard_dir"
        mkdir -p -- "$guard_dir"
        TEMP_TARGET_PATHS+=("$guard_dir")
        cat > "$wrapper" <<EOF
#!/bin/sh
set -eu
for arg in "\$@"; do
    case "\$arg" in
        -B|--create|-c)
            echo "Boot Bitch EFI guard: vendor NVRAM mutation suppressed; helper will reconcile the selected system." >&2
            exit 0
            ;;
    esac
done
exec $real_efibootmgr "\$@"
EOF
        chmod 0755 -- "$wrapper"
    fi

    log "BEGIN: $label" | tee -a "$SESSION_LOG"
    set +e
    if [[ -n "$real_efibootmgr" ]]; then
        run_selected_chroot /usr/bin/env \
            HOME=/root \
            PATH="/usr/local/libexec/boot-repair-efi-guard.$session_tag:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" \
            DEBIAN_FRONTEND=noninteractive \
            APT_LISTCHANGES_FRONTEND=none \
            "$@" 2>&1 | tee -a "$SESSION_LOG"
    else
        run_selected_chroot /usr/bin/env \
            HOME=/root \
            PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            DEBIAN_FRONTEND=noninteractive \
            APT_LISTCHANGES_FRONTEND=none \
            "$@" 2>&1 | tee -a "$SESSION_LOG"
    fi
    rc=${PIPESTATUS[0]}
    set -e
    rm -rf -- "$guard_dir"
    ((rc == 0)) || fail "$label failed with exit code $rc"
    log "Vendor command completed successfully: $label" | tee -a "$SESSION_LOG"
}

# Rebuild the vendor TUXEDO UKI with the request-scoped NVRAM guard, reconcile
# the complete firmware inventory around the suppressed vendor NVRAM calls and
# verify the embedded kernel matches the newest installed one.
rebuild_tuxedo_uki()
{
    local kver new_uki="" uki tmp embedded old_uki_sha="" new_uki_sha=""
    local nvram_pre="" nvram_post="" nvram_map=""

    validate_tuxedo_uki_target
    kver="$(newest_tuxedo_kernel)"
    uki="$TARGET_ROOT/boot/efi/EFI/BOOT/TUX.EFI"
    if [[ -s "$uki" ]] && command -v sha256sum >/dev/null 2>&1; then
        old_uki_sha="$(sha256sum "$uki" | awk '{print $1}')"
    fi

    if uefi_nvram_writable && command -v efibootmgr >/dev/null 2>&1; then
        nvram_pre="$SESSION_DIR/efi-nvram-pre.txt"
        nvram_post="$SESSION_DIR/efi-nvram-post.txt"
        efibootmgr -v > "$nvram_pre" 2>&1 || fail "Unable to capture firmware entries before TUXEDO UKI rebuild."
        efi_print_firmware_inventory "$nvram_pre" | tee -a "$SESSION_LOG"
        log "Captured complete firmware entry state before UKI rebuild: $nvram_pre" | tee -a "$SESSION_LOG"
    else
        log "Writable UEFI variables/efibootmgr are unavailable; UKI file rebuild will proceed without BootOrder restoration." | tee -a "$SESSION_LOG"
    fi

    run_tuxedo_uki_builder "Rebuild TUXEDO UKI for $kver" /usr/sbin/create_boot_uki_base.sh "$kver"

    [[ -s "$uki" ]] || fail "TUXEDO UKI builder completed but /boot/efi/EFI/BOOT/TUX.EFI is missing or empty."
    if command -v sha256sum >/dev/null 2>&1; then
        new_uki_sha="$(sha256sum "$uki" | awk '{print $1}')"
    fi
    if [[ -n "$old_uki_sha" && "$old_uki_sha" == "$new_uki_sha" ]]; then
        log "WARNING: vendor UKI command completed without changing TUX.EFI; the existing image will be validated against the selected target before this repair is reported successful." | tee -a "$SESSION_LOG"
    else
        log "PASS: vendor UKI command produced a changed TUX.EFI image." | tee -a "$SESSION_LOG"
    fi

    if [[ -n "$nvram_pre" ]]; then
        nvram_map="$SESSION_DIR/efi-nvram-map-uki.tsv"
        efi_reconcile_firmware_inventory "$nvram_pre" "$nvram_map" \
            || fail "Firmware inventory reconciliation could not restore every host, repair-target, and foreign entry safely."

        # In running-host mode the selected ESP is also the host ESP.  Restore
        # that installation's UKI registration before asking the generic
        # selected-target lookup to resolve it.  This avoids treating the
        # host/target overlap as an ambiguous foreign entry after the vendor
        # builder's NVRAM calls have been suppressed.
        if [[ "$RUNNING_HOST_MODE" == 1 ]]; then
            restore_missing_host_tuxedo_uki_entry 1 \
                || fail "Unable to restore the running host TUXEDO UKI entry without risking existing firmware entries."
        fi
        new_uki="$(ensure_target_tuxedo_uki_entry || true)"
        [[ -n "$new_uki" ]] \
            || fail "TUXEDO UKI was rebuilt but its firmware entry could not be found or created on the selected system ESP."
        if [[ "$RUNNING_HOST_MODE" != 1 ]]; then
            restore_missing_host_tuxedo_uki_entry "$RUNNING_HOST_MODE" \
                || fail "Unable to restore a missing host TUXEDO UKI entry without risking existing firmware entries."
        fi
        efi_annotate_selected_entries \
            || fail "Unable to annotate selected-system EFI entries or restore its iPXE/WebFAI registration safely."
        efi_restore_reconciled_order "$nvram_pre" "$nvram_map" "$new_uki" \
            || fail "Unable to restore the reconciled EFI BootOrder without risking loss of another disk's entry."
        efi_prune_selected_duplicate_destinations \
            || fail "Unable to reconcile duplicate selected-system EFI destinations safely."
        efi_group_firmware_boot_order \
            || fail "Unable to group the retained EFI entries by drive and boot use."
        # BootOrder maintenance must not silently undo selected-drive labels;
        # re-read and verify the final NVRAM state after all mutations.
        efi_annotate_selected_entries \
            || fail "Unable to retain selected-system EFI labels after BootOrder maintenance."
        efi_verify_selected_model_labels \
            || fail "Selected-system EFI labels could not be verified after BootOrder maintenance."
        efibootmgr -v > "$nvram_post" 2>&1 \
            || fail "Unable to capture firmware entries after TUXEDO UKI rebuild."
    fi

    if command -v objcopy >/dev/null 2>&1; then
        tmp="$(mktemp "$SESSION_DIR/uki-uname.XXXXXX")"
        if objcopy --dump-section ".uname=$tmp" "$uki" >/dev/null 2>&1; then
            embedded="$(tr '\0' '\n' < "$tmp" | head -1)"
            log "TUXEDO UKI embedded kernel: ${embedded:-unknown}" | tee -a "$SESSION_LOG"
            [[ "$embedded" == "$kver" ]] \
                || fail "Rebuilt TUXEDO UKI embeds '${embedded:-unknown}' but newest installed kernel is '$kver'."
        else
            log "WARNING: objcopy could not inspect the rebuilt UKI .uname section." | tee -a "$SESSION_LOG"
        fi
    fi

    if [[ -n "$old_uki_sha" && "$old_uki_sha" == "$new_uki_sha" ]]; then
        log "PASS: existing TUXEDO UKI validated for $kver on selected ESP $EFI_ESP_SOURCE; no replacement image was needed." | tee -a "$SESSION_LOG"
    else
        log "PASS: TUXEDO UKI rebuilt for $kver on selected ESP $EFI_ESP_SOURCE" | tee -a "$SESSION_LOG"
    fi
}

# Decode the rebuilt UKI command line and fail unless it references the
# promoted root UUID, the Btrfs @ subvolume and the target LUKS UUID.
verify_tuxedo_uki_root_binding()
{
    local uki="$TARGET_ROOT/boot/efi/EFI/BOOT/TUX.EFI" tmp cmdline root_uuid fstype backing luks_uuid
    [[ -s "$uki" ]] || fail "TUXEDO UKI is missing or empty: /boot/efi/EFI/BOOT/TUX.EFI"
    need objcopy
    tmp="$(mktemp "$SESSION_DIR/uki-cmdline.XXXXXX")"
    objcopy --dump-section ".cmdline=$tmp" "$uki" >/dev/null 2>&1 \
        || fail "Unable to inspect the rebuilt TUXEDO UKI .cmdline section."
    cmdline="$(tr '\0' ' ' < "$tmp")"
    log "TUXEDO UKI cmdline: $cmdline" | tee -a "$SESSION_LOG"

    root_uuid="$(blkid -s UUID -o value "$ROOT_CANONICAL" 2>/dev/null || true)"
    if [[ -n "$root_uuid" && "$cmdline" != *"root=UUID=$root_uuid"* ]]; then
        fail "Rebuilt TUXEDO UKI does not reference the promoted root filesystem UUID $root_uuid."
    fi

    fstype="$(lsblk -ndo FSTYPE "$ROOT_CANONICAL" 2>/dev/null | head -n1 || true)"
    if [[ "$fstype" == "btrfs" && "$TARGET_SUBVOL" == "@" ]]; then
        if [[ "$cmdline" != *"rootflags=subvol=/@"* && "$cmdline" != *"subvol=/@"* ]]; then
            fail "Rebuilt TUXEDO UKI does not explicitly select the promoted Btrfs @ root."
        fi
    fi

    backing="$(crypt_backing_device "$ROOT_CANONICAL" 2>/dev/null || true)"
    if [[ -n "$backing" ]] && command -v cryptsetup >/dev/null 2>&1 && cryptsetup isLuks "$backing" >/dev/null 2>&1; then
        luks_uuid="$(cryptsetup luksUUID "$backing" 2>/dev/null || true)"
        if [[ -n "$luks_uuid" \
              && "$cmdline" != *"rd.luks.uuid=$luks_uuid"* \
              && "$cmdline" != *"cryptdevice=UUID=$luks_uuid"* \
              && "$cmdline" != *"rd.luks.name=$luks_uuid="* ]]; then
            fail "Rebuilt TUXEDO UKI does not reference the target LUKS UUID $luks_uuid."
        fi
    fi

    log "PASS: TUXEDO UKI root/LUKS/subvolume binding verified." | tee -a "$SESSION_LOG"
}

# Conventional GRUB EFI preflight: validate the selected ESP, locate
# grub-install and derive the bootloader ID.  Sets EFI_GRUB_INSTALL_PATH and
# EFI_BOOTLOADER_ID; used again immediately before the write.
validate_efi_bootloader_target()
{
    EFI_GRUB_INSTALL_PATH=""
    EFI_BOOTLOADER_ID=""
    validate_selected_esp

    if [[ -x "$TARGET_ROOT/usr/sbin/grub-install" ]]; then
        EFI_GRUB_INSTALL_PATH="/usr/sbin/grub-install"
    elif [[ -x "$TARGET_ROOT/usr/bin/grub-install" ]]; then
        EFI_GRUB_INSTALL_PATH="/usr/bin/grub-install"
    else
        fail "grub-install is not installed in the target system."
    fi

    EFI_BOOTLOADER_ID="$(detect_efi_bootloader_id)"
    [[ -n "$EFI_BOOTLOADER_ID" ]] || fail "Unable to derive a safe EFI bootloader ID."
    [[ "$EFI_BOOTLOADER_ID" =~ ^[[:alnum:]_.-]+$ ]] \
        || fail "Derived EFI bootloader ID contains unsupported characters: $EFI_BOOTLOADER_ID"
}

# Emit the EFI/UKI change status from the artifact fingerprint captured before
# the repair started.  Shared by the TUXEDO UKI and conventional GRUB paths so
# every EFI action reports exactly one status line.
efi_repair_emit_change_status()
{
    local before="$1" after=""
    if [[ -z "$before" ]]; then
        repair_change_status efi changed
        return 0
    fi
    after="$(efi_boot_artifact_fingerprint)"
    if [[ "$before" == "$after" ]]; then
        repair_change_status efi "unchanged|EFI boot artifacts and firmware entries are byte-identical"
    else
        repair_change_status efi changed
    fi
}

# ---------------------------------------------------------------------------
# Alpine EFI repair (guarded GRUB EFI reinstall and EFI-stub reconciliation)
# ---------------------------------------------------------------------------
# Session-scoped ESP loader backup.  The pre-repair EFI/<id> directory and the
# optional firmware fallback copy are captured before any Alpine EFI write and
# restored verbatim when the install or its verification fails.  The firmware
# state is captured separately by the stage; this is the file-level rollback the
# conventional Debian/Arch path does not need.
ALPINE_EFI_BACKUP_DIR=""
ALPINE_EFI_FALLBACK_PRESENT=false

alpine_efi_backup_state()
{
    local esp_root id vendor_dir backup_dir
    esp_root="$TARGET_ROOT${TARGET_ESP_MOUNT:-/boot/efi}"
    id="${EFI_BOOTLOADER_ID:-}"
    [[ -n "$id" ]] || return 1
    vendor_dir="$esp_root/EFI/$id"
    [[ -d "$vendor_dir" ]] || return 1

    backup_dir="$SESSION_DIR/alpine-efi-backup"
    rm -rf -- "$backup_dir"
    mkdir -p -- "$backup_dir/EFI" || return 1
    cp -a -- "$vendor_dir" "$backup_dir/EFI/$id" || return 1

    ALPINE_EFI_FALLBACK_PRESENT=false
    if [[ -f "$esp_root/EFI/boot/bootx64.efi" ]]; then
        mkdir -p -- "$backup_dir/EFI/boot" || return 1
        cp -a -- "$esp_root/EFI/boot/bootx64.efi" "$backup_dir/EFI/boot/bootx64.efi" || return 1
        ALPINE_EFI_FALLBACK_PRESENT=true
    fi
    find "$backup_dir/EFI" -type f -exec sha256sum {} + > "$backup_dir/SHA256SUMS" 2>/dev/null || true
    ALPINE_EFI_BACKUP_DIR="$backup_dir"
    log "Backed up Alpine EFI loader files to $backup_dir (firmware fallback present before repair: $ALPINE_EFI_FALLBACK_PRESENT)." | tee -a "$SESSION_LOG"
}

alpine_efi_restore_backup()
{
    local esp_root id backup_dir
    backup_dir="${ALPINE_EFI_BACKUP_DIR:-}"
    [[ -n "$backup_dir" && -d "$backup_dir/EFI" ]] || return 1
    esp_root="$TARGET_ROOT${TARGET_ESP_MOUNT:-/boot/efi}"
    id="${EFI_BOOTLOADER_ID:-}"
    if [[ -n "$id" && -d "$backup_dir/EFI/$id" ]]; then
        rm -rf -- "$esp_root/EFI/$id"
        mkdir -p -- "$esp_root/EFI" || return 1
        cp -a -- "$backup_dir/EFI/$id" "$esp_root/EFI/$id" || return 1
    fi
    if [[ -f "$backup_dir/EFI/boot/bootx64.efi" ]]; then
        mkdir -p -- "$esp_root/EFI/boot" || return 1
        cp -a -- "$backup_dir/EFI/boot/bootx64.efi" "$esp_root/EFI/boot/bootx64.efi" || return 1
    elif [[ "${ALPINE_EFI_FALLBACK_PRESENT:-false}" != true && -f "$esp_root/EFI/boot/bootx64.efi" ]]; then
        # The pre-repair layout had no firmware fallback; do not leave one
        # behind when the failed repair created it.
        rm -f -- "$esp_root/EFI/boot/bootx64.efi"
    fi
    log "Restored the Alpine EFI loader files from the session backup $backup_dir." | tee -a "$SESSION_LOG"
}

# Recreate/refresh Alpine's firmware fallback loader (EFI/boot/bootx64.efi)
# from the detected named vendor loader.  Alpine's installer always writes this
# firmware fallback path, so a repair restores it when a user or a partial
# update deleted it; a failed repair removes a fallback the pre-repair layout
# did not have (alpine_efi_restore_backup).
alpine_efi_refresh_fallback_loader()
{
    local esp_root source fallback
    esp_root="$TARGET_ROOT${TARGET_ESP_MOUNT:-/boot/efi}"
    fallback="$esp_root/EFI/boot/bootx64.efi"
    source="$(efi_selected_generic_loader || true)"
    [[ -n "$source" ]] || return 1
    source="$esp_root${source//\\//}"
    [[ -s "$source" ]] || return 1
    install -D -m 0644 -- "$source" "$fallback" || return 1
    cmp -s -- "$source" "$fallback" || return 1
    log "PASS: refreshed the firmware fallback loader EFI/boot/bootx64.efi from ${source#"$esp_root"}." | tee -a "$SESSION_LOG"
}

# Read-only Alpine EFI-stub preflight.  Shared by the mandatory preflight in
# run_repair/run_host_repair and by the stage itself so the checks cannot
# drift.  Sets EFI_ESP_SOURCE/EFI_ESP_FSTYPE through validate_selected_esp.
alpine_efi_stub_preflight()
{
    [[ "$(alpine_efi_backend)" == efi-stub ]] \
        || fail "The detected Alpine EFI backend is not EFI-stub."
    alpine_efi_firmware_available \
        || fail "EFI-stub entry repair requires UEFI firmware; the recovery host booted in legacy BIOS mode."
    command -v efibootmgr >/dev/null 2>&1 \
        || fail "EFI-stub entry repair requires efibootmgr in the recovery host."
    validate_selected_esp
    [[ -n "$(alpine_efi_esp_kernel_images)" ]] \
        || fail "No EFI-stub kernel image (vmlinuz-*) is present at the EFI System Partition root."
    [[ -n "$(alpine_efi_esp_initramfs_images)" ]] \
        || fail "No EFI-stub initramfs image (initramfs-*) is present at the EFI System Partition root."
    uefi_nvram_writable \
        || fail "EFI-stub entry repair requires writable UEFI variables; there are no loader files on the ESP to reinstall."
}

# EFI-stub-only systems have no loader files to reinstall, so the MVP stage is
# deliberately limited to firmware-entry reconciliation around the detected
# stub definitions: recreate entries lost during this run from the pre-state
# capture, restore BootOrder and prune duplicates.  No kernel cmdline is ever
# synthesised; a missing or unprovable stub definition fails closed.
alpine_efi_stub_reconcile()
{
    local nvram_pre="" nvram_map="" partuuid="" efi_before=""
    local -a stub_ids=()

    alpine_efi_stub_preflight
    efi_set_inventory_esp_ids
    partuuid="${EFI_TARGET_ESP_PARTUUID,,}"
    [[ -n "$partuuid" ]] \
        || fail "Unable to identify the selected ESP PARTUUID for EFI-stub reconciliation."

    if command -v sha256sum >/dev/null 2>&1; then
        efi_before="$(efi_boot_artifact_fingerprint)"
    fi

    nvram_pre="$SESSION_DIR/efi-nvram-pre-alpine-stub.txt"
    efibootmgr -v > "$nvram_pre" 2>&1 \
        || fail "Unable to capture firmware entries before Alpine EFI-stub reconciliation."
    efi_print_firmware_inventory "$nvram_pre" | tee -a "$SESSION_LOG"
    mapfile -t stub_ids < <(alpine_efi_stub_entry_ids_for_partuuid "$partuuid" || true)
    ((${#stub_ids[@]} > 0)) \
        || fail "No EFI-stub firmware entry on the selected ESP references a kernel present on the ESP; EFI-stub entry synthesis is not implemented."

    log "Detected ${#stub_ids[@]} EFI-stub firmware entry(ies) on the selected ESP (${stub_ids[*]}); reconciling firmware state without file synthesis." | tee -a "$SESSION_LOG"
    nvram_map="$SESSION_DIR/efi-nvram-map-alpine-stub.tsv"
    efi_reconcile_firmware_inventory "$nvram_pre" "$nvram_map" \
        || fail "Firmware inventory reconciliation could not restore every host, repair-target, and foreign entry safely during Alpine EFI-stub reconciliation."
    efi_annotate_selected_entries \
        || fail "Unable to annotate selected-system EFI entries safely during Alpine EFI-stub reconciliation."
    efi_restore_reconciled_order "$nvram_pre" "$nvram_map" \
        || fail "Unable to restore the reconciled EFI BootOrder safely during Alpine EFI-stub reconciliation."
    efi_prune_selected_duplicate_destinations \
        || fail "Unable to reconcile duplicate selected-system EFI destinations safely during Alpine EFI-stub reconciliation."
    efi_group_firmware_boot_order \
        || fail "Unable to group the retained EFI entries by drive and boot use during Alpine EFI-stub reconciliation."
    efi_annotate_selected_entries \
        || fail "Unable to retain selected-system EFI labels after Alpine EFI-stub BootOrder maintenance."
    efi_verify_selected_model_labels \
        || fail "Selected-system EFI labels could not be verified after Alpine EFI-stub BootOrder maintenance."

    log "PASS: Alpine EFI-stub firmware entries verified; no ESP files were changed." | tee -a "$SESSION_LOG"
    efi_repair_emit_change_status "$efi_before"
}

# Alpine GRUB EFI install body.  Runs inside a subshell so any failure returns
# to the caller with the pre-repair ESP backup still available for rollback.
alpine_grub_efi_apply()
{
    local efi_dir
    run_chroot_try "Reinstall Alpine GRUB EFI bootloader (--no-nvram)" "$@"
    if ((CHROOT_TRY_RC != 0)); then
        log "ERROR: Alpine grub-install failed with exit code $CHROOT_TRY_RC." >&2
        return 1
    fi
    alpine_efi_refresh_fallback_loader || {
        log "ERROR: unable to refresh the Alpine firmware fallback loader." >&2
        return 1
    }
    efi_dir="$TARGET_ROOT${TARGET_ESP_MOUNT:-/boot/efi}/EFI/$EFI_BOOTLOADER_ID"
    [[ -d "$efi_dir" ]] || {
        log "ERROR: grub-install completed but EFI vendor directory ${TARGET_ESP_MOUNT:-/boot/efi}/EFI/$EFI_BOOTLOADER_ID is missing." >&2
        return 1
    }
    find "$efi_dir" -maxdepth 1 -type f \
        \( -iname 'grub*.efi' -o -iname 'shim*.efi' \) -print -quit 2>/dev/null \
        | grep -q . || {
        log "ERROR: grub-install completed but no GRUB EFI binary was found under ${TARGET_ESP_MOUNT:-/boot/efi}/EFI/$EFI_BOOTLOADER_ID." >&2
        return 1
    }
    log "PASS: Alpine EFI loader files verified under ${TARGET_ESP_MOUNT:-/boot/efi}/EFI/$EFI_BOOTLOADER_ID" | tee -a "$SESSION_LOG"
}

# Firmware-entry reconciliation for the Alpine GRUB EFI stage.  Runs after the
# --no-nvram install so all firmware mutations stay inside the helper.  Kept in
# its own function so the caller can restore the ESP file backup if any
# reconciliation step fails.
alpine_grub_efi_reconcile()
{
    local nvram_pre="$1" nvram_map="$2"
    efi_reconcile_firmware_inventory "$nvram_pre" "$nvram_map" \
        || fail "Firmware inventory reconciliation could not restore every host, repair-target, and foreign entry safely after Alpine GRUB EFI install."
    efi_ensure_selected_generic_entry \
        "${EFI_TARGET_ESP_PARTUUID,,}" \
        "$(efi_selected_system_model "$EFI_ESP_SOURCE" 2>/dev/null || true)" \
        || fail "Unable to restore and verify the selected system's Alpine GRUB EFI firmware entry."
    restore_missing_host_tuxedo_uki_entry "$RUNNING_HOST_MODE" \
        || fail "Unable to restore a missing host TUXEDO UKI entry without risking existing firmware entries."
    efi_annotate_selected_entries \
        || fail "Unable to annotate selected-system EFI entries or restore its iPXE/WebFAI registration safely after Alpine GRUB EFI install."
    efi_restore_reconciled_order "$nvram_pre" "$nvram_map" \
        || fail "Unable to restore the reconciled EFI BootOrder safely after Alpine GRUB EFI install."
    efi_prune_selected_duplicate_destinations \
        || fail "Unable to reconcile duplicate selected-system EFI destinations safely after Alpine GRUB EFI install."
    efi_group_firmware_boot_order \
        || fail "Unable to group the retained EFI entries by drive and boot use after Alpine GRUB EFI install."
    efi_annotate_selected_entries \
        || fail "Unable to retain selected-system EFI labels after Alpine GRUB EFI BootOrder maintenance."
    efi_verify_selected_model_labels \
        || fail "Selected-system EFI labels could not be verified after Alpine GRUB EFI BootOrder maintenance."
}

# Alpine GRUB EFI stage: back up the ESP loader files, reinstall GRUB with the
# installer convention (--no-nvram), refresh the fallback copy, reconcile the
# firmware entry/BootOrder through the shared reconciler and report the change
# status.  The ESP backup is restored when the install, verification or
# firmware reconciliation fails.
alpine_grub_efi_repair()
{
    local nvram_pre="" nvram_map="" efi_before=""
    local -a install_args=()

    [[ "$(alpine_efi_backend)" == grub ]] \
        || fail "Alpine GRUB EFI repair requires the detected GRUB bootloader backend."
    alpine_efi_firmware_available \
        || fail "Alpine GRUB EFI repair requires UEFI firmware; the recovery host booted in legacy BIOS mode."
    alpine_grub_install_present \
        || fail "grub-install is not installed in the Alpine target system."
    target_apk_package_installed grub \
        || fail "The grub package is not installed in the Alpine target."
    target_apk_package_installed grub-efi \
        || fail "grub-efi is not installed in the Alpine target."
    alpine_grub_module_dir_present \
        || fail "The x86_64-efi GRUB module directory is missing from the Alpine target."
    alpine_grub_config_tool_present \
        || fail "Neither grub-mkconfig nor update-grub is installed in the Alpine target system."
    validate_mapper_crypttab
    validate_efi_bootloader_target

    log "SIMULATE/PREFLIGHT: Alpine GRUB EFI install target=$EFI_ESP_SOURCE fs=$EFI_ESP_FSTYPE id=$EFI_BOOTLOADER_ID mode=--no-nvram (helper-managed firmware entries)" | tee -a "$SESSION_LOG"
    run_chroot_try "Check Alpine grub-install availability/version" "$EFI_GRUB_INSTALL_PATH" --version
    ((CHROOT_TRY_RC == 0)) \
        || fail "grub-install is present but could not execute during the Alpine EFI preflight."

    if command -v sha256sum >/dev/null 2>&1; then
        efi_before="$(efi_boot_artifact_fingerprint)"
    fi
    alpine_efi_backup_state \
        || fail "Unable to back up the Alpine EFI loader files before repair."

    if uefi_nvram_writable && command -v efibootmgr >/dev/null 2>&1; then
        nvram_pre="$SESSION_DIR/efi-nvram-pre-alpine-grub.txt"
        efibootmgr -v > "$nvram_pre" 2>&1 \
            || fail "Unable to capture firmware entries before Alpine GRUB EFI install."
        efi_print_firmware_inventory "$nvram_pre" | tee -a "$SESSION_LOG"
        log "Captured complete firmware entry state before Alpine GRUB EFI install: $nvram_pre" | tee -a "$SESSION_LOG"
    else
        log "Writable UEFI efivars/efibootmgr are unavailable; Alpine GRUB EFI repair will update loader files and the fallback copy only (read-only firmware-variable mode)." | tee -a "$SESSION_LOG"
    fi

    log "EFI System Partition: $EFI_ESP_SOURCE ($EFI_ESP_FSTYPE)" | tee -a "$SESSION_LOG"
    log "EFI bootloader ID: $EFI_BOOTLOADER_ID" | tee -a "$SESSION_LOG"
    install_args=(
        "$EFI_GRUB_INSTALL_PATH"
        --target=x86_64-efi
        --efi-directory="${TARGET_ESP_MOUNT:-/boot/efi}"
        --bootloader-id="$EFI_BOOTLOADER_ID"
        --boot-directory=/boot
        --recheck
        --no-nvram
    )
    if ! ( alpine_grub_efi_apply "${install_args[@]}" ); then
        alpine_efi_restore_backup \
            || log "WARNING: the Alpine ESP backup could not be fully restored; inspect ${ALPINE_EFI_BACKUP_DIR:-the session backup}." | tee -a "$SESSION_LOG"
        fail "Alpine GRUB EFI repair failed; the ESP loader files were restored from the session backup."
    fi

    if [[ -n "$nvram_pre" ]]; then
        nvram_map="$SESSION_DIR/efi-nvram-map-alpine-grub.tsv"
        if ! ( alpine_grub_efi_reconcile "$nvram_pre" "$nvram_map" ); then
            alpine_efi_restore_backup \
                || log "WARNING: the Alpine ESP backup could not be fully restored; inspect ${ALPINE_EFI_BACKUP_DIR:-the session backup}." | tee -a "$SESSION_LOG"
            fail "Alpine GRUB EFI firmware reconciliation failed; the ESP loader files were restored from the session backup."
        fi
    fi

    log "EFI registration mode: Alpine --no-nvram install with helper-managed firmware entries (writable efivars: $([[ "$nvram_pre" != "" ]] && printf yes || printf no))." | tee -a "$SESSION_LOG"
    efi_repair_emit_change_status "$efi_before"
}

# Alpine EFI stage dispatcher: the detected Alpine EFI backend decides between
# the guarded GRUB EFI reinstall and the EFI-stub firmware-entry reconciliation.
# Anything else fails closed with the capability reason.
adaptive_alpine_efi_repair()
{
    local reason
    case "$(alpine_efi_backend)" in
        grub) alpine_grub_efi_repair ;;
        efi-stub) alpine_efi_stub_reconcile ;;
        *)
            if ! reason="$(efi_unavailable_reason)"; then
                fail "Alpine EFI repair is not available: $reason."
            fi
            fail "Alpine EFI repair is not available for the detected EFI backend."
            ;;
    esac
}

# EFI stage entry point: rebuild the TUXEDO UKI when that layout is detected,
# otherwise reinstall conventional GRUB EFI files (with a --no-nvram fallback),
# reconcile firmware entries/BootOrder and emit the EFI change status.
reinstall_efi_bootloader()
{
    local nvram_mode efi_dir
    local nvram_pre="" nvram_map="" efi_before=""
    local -a install_args

    # Alpine uses its own guarded path: grub-install --no-nvram plus an ESP
    # file backup/rollback and the shared firmware reconciler, or the
    # EFI-stub firmware-entry reconciliation when GRUB is not present.
    if is_alpine_family; then
        adaptive_alpine_efi_repair
        return 0
    fi

    if command -v sha256sum >/dev/null 2>&1; then
        efi_before="$(efi_boot_artifact_fingerprint)"
    fi

    # Current encrypted TUXEDO Debian systems use a UKI primary boot path. Use
    # the vendor-supplied builder, but first run a read-mostly prerequisite
    # pass that can safely correct a missing initramfs before the vendor builder
    # is allowed to touch TUX.EFI or firmware state.
    if is_tuxedo_uki_layout; then
        log "TUXEDO UKI layout detected; simulating/prereflighting the vendor boot path before rebuild." | tee -a "$SESSION_LOG"
        preflight_tuxedo_uki
        rebuild_tuxedo_uki
        verify_tuxedo_uki_root_binding
        efi_repair_emit_change_status "$efi_before"
        return 0
    fi

    preflight_generic_efi
    # Re-run the target validation immediately before the write. The preflight
    # above is intentionally advisory/read-only; this second gate protects
    # against device topology changes between planning and execution.
    validate_efi_bootloader_target

    if uefi_nvram_writable && command -v efibootmgr >/dev/null 2>&1; then
        nvram_pre="$SESSION_DIR/efi-nvram-pre-grub-install.txt"
        efibootmgr -v > "$nvram_pre" 2>&1 \
            || fail "Unable to capture firmware entries before conventional GRUB EFI install."
        efi_print_firmware_inventory "$nvram_pre" | tee -a "$SESSION_LOG"
        log "Captured complete firmware entry state before conventional GRUB EFI install: $nvram_pre" | tee -a "$SESSION_LOG"
    fi

    log "EFI System Partition: $EFI_ESP_SOURCE ($EFI_ESP_FSTYPE)" | tee -a "$SESSION_LOG"
    log "EFI bootloader ID: $EFI_BOOTLOADER_ID" | tee -a "$SESSION_LOG"

    install_args=(
        "$EFI_GRUB_INSTALL_PATH"
        --target=x86_64-efi
        --efi-directory="${TARGET_ESP_MOUNT:-/boot/efi}"
        --bootloader-id="$EFI_BOOTLOADER_ID"
        --recheck
    )

    if uefi_nvram_writable; then
        nvram_mode="firmware NVRAM update enabled"
        log "Writable UEFI efivars detected; first attempt will permit firmware registration." | tee -a "$SESSION_LOG"
        run_chroot_try "Reinstall GRUB EFI bootloader" "${install_args[@]}"
        if ((CHROOT_TRY_RC != 0)) \
           && grep -Eiq 'EFI variables are not supported|efivar|NVRAM|could not prepare.*Boot|failed to register|failed to set EFI variable' <<<"$CHROOT_TRY_OUTPUT"; then
            log "KNOWN ISSUE: firmware NVRAM registration failed; retrying once as a file-only --no-nvram reinstall." | tee -a "$SESSION_LOG"
            nvram_mode="file reinstall only (--no-nvram fallback)"
            run_chroot_try "Retry GRUB EFI bootloader files without NVRAM update" \
                "${install_args[@]}" --no-nvram
        fi
    else
        nvram_mode="file reinstall only (--no-nvram)"
        log "Writable UEFI efivars are unavailable; using the preflight-selected --no-nvram path." | tee -a "$SESSION_LOG"
        run_chroot_try "Reinstall GRUB EFI bootloader files" \
            "${install_args[@]}" --no-nvram
    fi

    ((CHROOT_TRY_RC == 0)) || fail "EFI bootloader reinstall failed after preflight/known NVRAM fallback."

    if [[ -n "$nvram_pre" ]]; then
        nvram_map="$SESSION_DIR/efi-nvram-map-grub.tsv"
        efi_reconcile_firmware_inventory "$nvram_pre" "$nvram_map" \
            || fail "Firmware inventory reconciliation could not restore every host, repair-target, and foreign entry safely after conventional GRUB EFI install."
        efi_ensure_selected_generic_entry \
            "${EFI_TARGET_ESP_PARTUUID,,}" \
            "$(efi_selected_system_model "$EFI_ESP_SOURCE" 2>/dev/null || true)" \
            || fail "Unable to restore and verify the selected system's generic EFI firmware entry."
        restore_missing_host_tuxedo_uki_entry "$RUNNING_HOST_MODE" \
            || fail "Unable to restore a missing host TUXEDO UKI entry without risking existing firmware entries."
        efi_annotate_selected_entries \
            || fail "Unable to annotate selected-system EFI entries or restore its iPXE/WebFAI registration safely after conventional GRUB EFI install."
        efi_restore_reconciled_order "$nvram_pre" "$nvram_map" \
            || fail "Unable to restore the reconciled EFI BootOrder safely after conventional GRUB EFI install."
        efi_prune_selected_duplicate_destinations \
            || fail "Unable to reconcile duplicate selected-system EFI destinations safely after conventional GRUB EFI install."
        efi_group_firmware_boot_order \
            || fail "Unable to group the retained EFI entries by drive and boot use after conventional GRUB EFI install."
        efi_annotate_selected_entries \
            || fail "Unable to retain selected-system EFI labels after GRUB BootOrder maintenance."
        efi_verify_selected_model_labels \
            || fail "Selected-system EFI labels could not be verified after GRUB BootOrder maintenance."
    fi

    efi_dir="$TARGET_ROOT${TARGET_ESP_MOUNT:-/boot/efi}/EFI/$EFI_BOOTLOADER_ID"
    [[ -d "$efi_dir" ]] \
        || fail "grub-install completed but EFI vendor directory was not found: ${TARGET_ESP_MOUNT:-/boot/efi}/EFI/$EFI_BOOTLOADER_ID"
    if ! find "$efi_dir" -maxdepth 1 -type f \
        \( -iname 'grub*.efi' -o -iname 'shim*.efi' \) -print -quit 2>/dev/null \
        | grep -q .; then
        fail "grub-install completed but no GRUB/shim EFI binary was found under ${TARGET_ESP_MOUNT:-/boot/efi}/EFI/$EFI_BOOTLOADER_ID."
    fi

    log "PASS: EFI loader files verified under ${TARGET_ESP_MOUNT:-/boot/efi}/EFI/$EFI_BOOTLOADER_ID" | tee -a "$SESSION_LOG"
    log "EFI registration mode: $nvram_mode" | tee -a "$SESSION_LOG"
    efi_repair_emit_change_status "$efi_before"
}

# Offline graphical-login repair: set graphical.target as default, enable the
# detected display manager and (re)point display-manager.service at its unit.
# The manager is never started inside the repair chroot.
restore_display_manager()
{
    local display_link default_link
    need systemctl

    detect_display_manager
    if ! package_query_available; then
        fail "No detected package manager can verify display-manager package state."
    fi
    if [[ -n "$DISPLAY_MANAGER_PACKAGE" ]] && ! target_package_installed "$DISPLAY_MANAGER_PACKAGE"; then
        fail "$DISPLAY_MANAGER_LABEL is not fully installed in the target. Run package repair/reinstall before restoring graphical login."
    fi

    log "Restoring graphical.target as the target default." | tee -a "$SESSION_LOG"
    if ! systemctl --root="$TARGET_ROOT" set-default graphical.target 2>&1 | tee -a "$SESSION_LOG"; then
        fail "Unable to set graphical.target as the target default."
    fi

    log "Enabling $DISPLAY_MANAGER_LABEL in the offline target (it will NOT be started inside the repair chroot)." | tee -a "$SESSION_LOG"
    if ! systemctl --root="$TARGET_ROOT" enable "$DISPLAY_MANAGER_SERVICE" 2>&1 | tee -a "$SESSION_LOG"; then
        # Some custom display-manager units are intentionally static and have
        # no [Install] section.  The display-manager.service alias below is
        # sufficient for graphical.target to pull those units in; only treat
        # an enable failure as fatal when the unit advertises install rules.
        if grep -Eq '^\[Install\][[:space:]]*$' "$TARGET_ROOT$DISPLAY_MANAGER_UNIT_REL"; then
            fail "Unable to enable $DISPLAY_MANAGER_SERVICE in the target."
        fi
        log "$DISPLAY_MANAGER_SERVICE is a static unit; preserving it through the display-manager.service alias." | tee -a "$SESSION_LOG"
    fi

    mkdir -p "$TARGET_ROOT/etc/systemd/system"
    ln -sfn "$DISPLAY_MANAGER_UNIT_REL" "$TARGET_ROOT/etc/systemd/system/display-manager.service"

    display_link="$(readlink "$TARGET_ROOT/etc/systemd/system/display-manager.service" 2>/dev/null || true)"
    default_link="$(readlink "$TARGET_ROOT/etc/systemd/system/default.target" 2>/dev/null || true)"
    [[ "$display_link" == *"$DISPLAY_MANAGER_SERVICE" ]] \
        || fail "display-manager.service does not resolve to $DISPLAY_MANAGER_LABEL after repair."
    [[ "$default_link" == *graphical.target ]] \
        || fail "default.target does not resolve to graphical.target after repair."
    if grep -Eq '^\[Install\][[:space:]]*$' "$TARGET_ROOT$DISPLAY_MANAGER_UNIT_REL"; then
        systemctl --root="$TARGET_ROOT" is-enabled "$DISPLAY_MANAGER_SERVICE" 2>&1 \
            | grep -Fxq enabled \
            || fail "$DISPLAY_MANAGER_SERVICE is not enabled after graphical-login repair."
    fi

    log "PASS: display-manager.service -> $display_link" | tee -a "$SESSION_LOG"
    log "PASS: default.target -> $default_link" | tee -a "$SESSION_LOG"
    log "$DISPLAY_MANAGER_LABEL was configured offline only; Boot Bitch intentionally did not start a graphical session inside the chroot." | tee -a "$SESSION_LOG"
}

# boot-stack stage: reconcile mapper/crypttab, initramfs, EFI/UKI and GRUB in
# one run, each through its own guarded preflight.  With post_efi=true the
# caller states the EFI/UKI stage already rebuilt and verified this layout, so
# only mapper/crypttab and initramfs are reconciled here.  The change status is
# aggregated from initramfs, EFI and GRUB artifact fingerprints.
repair_boot_stack()
{
    local post_efi="${BOOT_STACK_POST_EFI:-false}"
    local hash_available=false boot_changed=false bios_grub2=false
    local initramfs_before="" initramfs_after="" efi_before="" efi_after=""
    local grub_before="" grub_after=""

    # A Fedora/RHEL grub2 layout on legacy BIOS reconciles dracut -> GRUB2
    # only: there are no EFI/UKI artifacts to fingerprint or rebuild.
    if grub2_layout_detected && bios_firmware_mode; then
        bios_grub2=true
        if [[ "$post_efi" == true ]]; then
            log "NOTE: --post-efi is an EFI-only hint; the Fedora BIOS boot stack reconciles dracut and GRUB2 without an EFI stage." | tee -a "$SESSION_LOG"
        fi
    fi

    if command -v sha256sum >/dev/null 2>&1; then
        hash_available=true
        initramfs_before="$(initramfs_image_fingerprint)"
        if [[ "$bios_grub2" == true ]]; then
            grub_before="$(grub_artifact_fingerprint)"
        else
            efi_before="$(efi_boot_artifact_fingerprint)"
            grub_before="$(repair_file_fingerprint "$TARGET_ROOT/boot/grub/grub.cfg")"
        fi
    fi

    log "SIMULATE/PREFLIGHT: complete boot-stack reconciliation" | tee -a "$SESSION_LOG"
    validate_mapper_crypttab

    # Each component performs its own trial/preflight, deterministic correction
    # pass and post-write verification. This keeps the compound operation from
    # hiding which layer required correction.  With --post-efi the caller
    # states that the EFI/UKI stage already rebuilt and verified this layout,
    # including its GRUB follow-up, so those two components are not repeated.
    adaptive_initramfs_repair
    if [[ "$bios_grub2" == true ]]; then
        log "Fedora BIOS GRUB2 layout detected; no EFI/UKI artifacts are part of boot-stack reconciliation." | tee -a "$SESSION_LOG"
        fedora_bootstack_pairing_check
    elif [[ "$post_efi" == true ]]; then
        log "SKIP: boot-stack EFI/UKI rebuild skipped because the EFI / UKI bootloader stage already rebuilt and verified this layout in the same run (--post-efi)." | tee -a "$SESSION_LOG"
    elif is_tuxedo_uki_layout; then
        preflight_tuxedo_uki
        rebuild_tuxedo_uki
        verify_tuxedo_uki_root_binding
    elif [[ "$TARGET_DISTRO_FAMILY" == arch && "$TARGET_BOOTLOADER_BACKEND" == grub ]]; then
        log "Arch GRUB layout detected; applying the guarded conventional EFI reinstall after initramfs preflight." | tee -a "$SESSION_LOG"
        reinstall_efi_bootloader
    else
        log "No TUXEDO UKI builder detected; preserving the distribution's existing EFI layout during boot-stack reconciliation." | tee -a "$SESSION_LOG"
    fi
    if [[ "$post_efi" == true && "$bios_grub2" != true ]]; then
        log "SKIP: boot-stack GRUB regeneration skipped because the EFI / UKI bootloader stage already regenerated the GRUB configuration in the same run (--post-efi)." | tee -a "$SESSION_LOG"
    elif [[ "$bios_grub2" == true ]]; then
        # Boot-stack reconciliation is non-destructive: it never performs the
        # evidence-triggered grub2-install boot-code reinstall.
        adaptive_grub_stage config-only
    else
        adaptive_grub_stage
    fi
    if [[ "$post_efi" == true && "$bios_grub2" != true ]]; then
        log "PASS: boot stack reconciliation completed; EFI/UKI and GRUB were reused from the earlier EFI / UKI stage while mapper/crypttab and initramfs were reconciled." | tee -a "$SESSION_LOG"
    else
        log "PASS: boot stack reconciliation completed after component simulations and verification." | tee -a "$SESSION_LOG"
    fi
    # Aggregate the compound stage from its own artifact fingerprints so the
    # status reflects every component even when one of them reported no change.
    if [[ "$hash_available" != true ]]; then
        boot_changed=true
    else
        initramfs_after="$(initramfs_image_fingerprint)"
        if [[ "$bios_grub2" == true ]]; then
            grub_after="$(grub_artifact_fingerprint)"
        else
            efi_after="$(efi_boot_artifact_fingerprint)"
            grub_after="$(repair_file_fingerprint "$TARGET_ROOT/boot/grub/grub.cfg")"
            [[ "$efi_before" == "$efi_after" ]] || boot_changed=true
        fi
        [[ "$initramfs_before" == "$initramfs_after" ]] || boot_changed=true
        [[ "$grub_before" == "$grub_after" ]] || boot_changed=true
    fi
    if [[ "$boot_changed" == true ]]; then
        repair_change_status bootstack changed
    elif [[ "$bios_grub2" == true ]]; then
        repair_change_status bootstack "unchanged|initramfs and GRUB2 artifacts are byte-identical"
    else
        repair_change_status bootstack "unchanged|initramfs, EFI/UKI and GRUB artifacts are byte-identical"
    fi
}

# Move one existing EFI entry to the front of BootOrder while retaining every
# other firmware entry (including valid entries absent from the old BootOrder)
# and verify that firmware accepted the new order.
efi_promote_entry_first()
{
    local wanted="${1^^}" current old_order id out_csv="" seen_csv=","
    local -a ids=()

    current="$(efibootmgr -v 2>/dev/null || true)"
    grep -Eq "^Boot${wanted}\\*?[[:space:]]" <<<"$current" \
        || fail "Requested default EFI entry Boot$wanted is not present."
    old_order="$(sed -n 's/^BootOrder: //p' <<<"$current" | head -n1 || true)"

    out_csv="$wanted"
    seen_csv+=",$wanted,"
    if [[ -n "$old_order" ]]; then
        IFS=',' read -ra ids <<< "$old_order"
        for id in "${ids[@]}"; do
            id="${id^^}"
            [[ "$id" =~ ^[0-9A-F]{4}$ ]] || continue
            grep -Eq "^Boot${id}\\*?[[:space:]]" <<<"$current" || continue
            [[ "$seen_csv" == *",$id,"* ]] && continue
            seen_csv+=",$id,"
            out_csv+=",$id"
        done
    fi

    # Firmware may expose valid entries that were not in BootOrder. Preserve
    # those entries by appending them in their current efibootmgr listing order.
    while IFS= read -r id; do
        id="${id^^}"
        [[ "$id" =~ ^[0-9A-F]{4}$ ]] || continue
        [[ "$seen_csv" == *",$id,"* ]] && continue
        seen_csv+=",$id,"
        out_csv+=",$id"
    done < <(sed -nE 's/^Boot([0-9A-Fa-f]{4})\\*?[[:space:]].*/\1/p' <<<"$current" | tr '[:lower:]' '[:upper:]' | sort -u)

    log "Making Boot$wanted the explicit default while preserving all other EFI entries: $out_csv" | tee -a "$SESSION_LOG"
    efibootmgr -o "$out_csv" 2>&1 | tee -a "$SESSION_LOG" || return 1
    current="$(efibootmgr -v 2>/dev/null || true)"
    [[ "$(sed -n 's/^BootOrder: //p' <<<"$current" | head -n1 || true)" == "$out_csv" ]] \
        || fail "Firmware did not retain the requested default EFI entry Boot$wanted."
    log "PASS: Boot$wanted is first in BootOrder; all other entries were retained." | tee -a "$SESSION_LOG"
}

# Run one modifying package stage across every detected package-manager
# backend, in native-first deterministic order.  Each backend performs its own
# mandatory preflight, simulation and guards; a backend failure aborts the
# stage with the backend named in the stage context.  The stage emits exactly
# one combined change status: changed when any backend changed, unchanged only
# when every backend proved unchanged.  Single-backend targets keep the exact
# backend status line.
run_package_stage()
{
    local stage="$1" tool_key="$2" backend entry key state backend_status backend_feedback reason i
    local any_changed=false detail="" combined_state=""
    local -a backends=() runnable=() skipped_backends=() skipped_reasons=() collected=()

    mapfile -t backends < <(package_stage_backends "$stage")
    ((${#backends[@]} > 0)) \
        || fail "No guarded package-manager backend is available for stage '$stage'. Detected package managers: ${TARGET_PACKAGE_MANAGERS[*]:-none}."

    # A detected backend whose stage prerequisites are missing (for example rpm
    # on a Debian-family target that merely ships the rpm package) must not
    # abort a stage another backend can complete: it is skipped and named in
    # the combined status.  When no backend is runnable the stage fails with
    # the exact prerequisite reason.
    for backend in "${backends[@]}"; do
        if reason="$(package_backend_unavailable_reason "$stage" "$backend")"; then
            runnable+=("$backend")
        else
            skipped_backends+=("$backend")
            skipped_reasons+=("$reason")
        fi
    done
    if ((${#runnable[@]} == 0)); then
        fail "${skipped_reasons[0]:-no guarded package-manager backend is runnable for stage '$stage'}."
    fi
    for i in "${!skipped_backends[@]}"; do
        log "SKIP: package backend ${skipped_backends[$i]} is not runnable for stage '$stage': ${skipped_reasons[$i]}" | tee -a "$SESSION_LOG"
    done

    log "Package stage '$stage': running ${#runnable[@]} runnable backend(s): ${runnable[*]}" | tee -a "$SESSION_LOG"

    REPAIR_CHANGE_STATUS_COLLECTED=()
    REPAIR_CHANGE_STATUS_COLLECT=1
    for backend in "${runnable[@]}"; do
        CURRENT_STAGE="$stage ($backend)"
        log "BEGIN: package backend $backend ($stage)" | tee -a "$SESSION_LOG"
        # Each backend publishes its own held-back/skipped feedback; the
        # dispatcher collects it so the combined status names the packages too.
        package_feedback_reset
        case "$stage:$backend" in
            dpkg-configure:apt/dpkg) dpkg_configure_stage ;;
            fix-broken:apt/dpkg) adaptive_fix_broken ;;
            fix-broken:apk) adaptive_alpine_apk_fix_broken ;;
            fix-broken:pacman) adaptive_arch_pacman_repair "Repair Arch package dependencies" fixbroken ;;
            fix-broken:rpm) adaptive_rpm_fix_broken ;;
            apt-update:apt/dpkg) run_apt_update ;;
            apt-update:rpm) adaptive_rpm_metadata_refresh ;;
            apt-upgrade:apt/dpkg) adaptive_apt_upgrade ;;
            apt-upgrade:apk) adaptive_alpine_apk_upgrade ;;
            apt-upgrade:pacman) adaptive_arch_pacman_repair "Upgrade installed Arch packages" upgrade ;;
            apt-upgrade:rpm) adaptive_rpm_upgrade ;;
            *) fail "No guarded implementation for package stage '$stage' backend '$backend'." ;;
        esac
        backend_feedback="$PACKAGE_FEEDBACK_SUMMARY"
        collected=("${REPAIR_CHANGE_STATUS_COLLECTED[@]}")
        backend_status=""
        if ((${#collected[@]} > 0)); then
            entry="${collected[${#collected[@]}-1]}"
            key="${entry%%$'\t'*}"
            state="${entry#*$'\t'}"
            if [[ "$key" == "$tool_key" ]]; then
                backend_status="$state"
            fi
        fi
        REPAIR_CHANGE_STATUS_COLLECTED=()
        if [[ -z "$backend_status" ]]; then
            # A backend that did not report a status cannot be proven unchanged.
            any_changed=true
            detail+="${detail:+; }$backend changed (no change status reported)"
            if [[ -n "$backend_feedback" ]]; then
                detail+=" ($backend_feedback)"
            fi
            log "Package backend $backend did not report a change status; treating the stage as changed." | tee -a "$SESSION_LOG"
            continue
        fi
        if [[ "$backend_status" == unchanged || "$backend_status" == unchanged\|* ]]; then
            detail+="${detail:+; }$backend unchanged"
        else
            any_changed=true
            detail+="${detail:+; }$backend changed"
        fi
        if [[ -n "$backend_feedback" ]]; then
            detail+=" ($backend_feedback)"
        fi
        log "Package backend $backend change status: $backend_status" | tee -a "$SESSION_LOG"
    done
    REPAIR_CHANGE_STATUS_COLLECT=0

    for i in "${!skipped_backends[@]}"; do
        detail+="${detail:+; }${skipped_backends[$i]} skipped (${skipped_reasons[$i]})"
    done
    if ((${#backends[@]} == 1)); then
        [[ -n "$backend_status" ]] \
            || fail "Package backend ${backends[0]} completed without a change status."
        repair_change_status "$tool_key" "$backend_status"
        return 0
    fi
    if [[ "$any_changed" == true ]]; then
        combined_state="changed|backends: $detail"
    else
        combined_state="unchanged|backends: $detail"
    fi
    repair_change_status "$tool_key" "$combined_state"
}

# ---------------------------------------------------------------------------
# Repair orchestration (target, native host and host default selection)
# ---------------------------------------------------------------------------
# host-repair entry point: validate stage order, run the native safety
# preflights (identity, backend, package locks, EFI, command guard) and execute
# every requested stage against the running host.
run_host_repair()
{
    local raw_disk="$1" raw_root="$2" stage previous_rank=0 current_rank
    local package_stage=false efi_requested=false grub_requested=false boot_stack_requested=false
    shift 2
    parse_repair_arguments "$@"
    ((${#REPAIR_STAGES[@]} > 0)) || fail "host-repair requires at least one repair stage."
    BOOT_STACK_POST_EFI="$REPAIR_POST_EFI"

    for stage in "${REPAIR_STAGES[@]}"; do
        validate_stage "$stage"
        current_rank="$(stage_rank "$stage")"
        (( current_rank >= previous_rank )) \
            || fail "Repair stages are out of safe dependency order: $stage must run after earlier stages."
        previous_rank="$current_rank"
        [[ "$stage" == efi ]] && efi_requested=true
        [[ "$stage" == grub ]] && grub_requested=true
        [[ "$stage" == boot-stack ]] && boot_stack_requested=true
        case "$stage" in
            dpkg-configure|fix-broken|apt-update|apt-upgrade|dkms|display-manager) package_stage=true ;;
        esac
    done

    CURRENT_STAGE="host safety preflight"
    RUNNING_HOST_MODE=1
    prepare_running_host "$raw_disk" "$raw_root" yes
    profile_target_backends
    # Stage applicability is probe-based: no distribution-family prefilter.
    validate_repair_stages_against_backends "${REPAIR_STAGES[@]}"
    if [[ "$package_stage" == true ]]; then
        host_package_manager_gate
    fi
    if [[ "$efi_requested" == true ]]; then
        if is_tuxedo_uki_layout; then
            validate_tuxedo_uki_target
            log "Host TUXEDO UKI read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE)" | tee -a "$SESSION_LOG"
        elif is_alpine_family && [[ "$(alpine_efi_backend)" == efi-stub ]]; then
            alpine_efi_stub_preflight
            log "Host Alpine EFI-stub read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE)" | tee -a "$SESSION_LOG"
        else
            validate_efi_bootloader_target
            log "Host EFI read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE, id=$EFI_BOOTLOADER_ID)" | tee -a "$SESSION_LOG"
        fi
    fi
    if [[ "$boot_stack_requested" == true ]]; then
        if is_tuxedo_uki_layout; then
            validate_tuxedo_uki_target
            log "Host boot-stack TUXEDO UKI read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE)" | tee -a "$SESSION_LOG"
        elif is_arch_family; then
            validate_efi_bootloader_target
            log "Host boot-stack Arch EFI read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE, id=$EFI_BOOTLOADER_ID)" | tee -a "$SESSION_LOG"
        fi
    fi
    # Every native modifying stage runs through the private firmware-variable
    # namespace.  Package, kernel, display, GRUB and vendor hooks can all
    # indirectly invoke efibootmgr; keeping the guard active for the complete
    # request prevents an unrelated hook from changing host NVRAM behind the
    # selected-system reconciler.
    prepare_host_command_guard
    uefi_nvram_writable || {
        log "Writable UEFI variables are unavailable; host maintenance will preserve files and BootOrder without firmware registration." | tee -a "$SESSION_LOG"
    }
    log "Native running-host repair preflight: PASS" | tee -a "$SESSION_LOG"

    for stage in "${REPAIR_STAGES[@]}"; do
        CURRENT_STAGE="$stage"
        case "$stage" in
            dpkg-configure) run_package_stage dpkg-configure dpkg ;;
            fix-broken) run_package_stage fix-broken fixbroken ;;
            apt-update) run_package_stage apt-update aptupdate ;;
            apt-upgrade) run_package_stage apt-upgrade upgrade ;;
            dkms) adaptive_dkms_repair ;;
            display-manager) adaptive_display_manager_repair ;;
            initramfs) adaptive_initramfs_repair ;;
            efi) reinstall_efi_bootloader ;;
            grub) adaptive_grub_stage ;;
            extlinux) adaptive_extlinux_repair ;;
            boot-stack) repair_boot_stack ;;
        esac
    done

    # Keep the same EFI follow-up behavior as target repair: a standalone EFI
    # repair regenerates the menu after the loader files have been updated.
    # With --post-efi the boot-stack stage reuses that EFI/UKI repair and skips
    # its own GRUB regeneration, so the follow-up must still run here.
    if [[ "$efi_requested" == true && "$grub_requested" != true \
        && ( "$boot_stack_requested" != true || "$REPAIR_POST_EFI" == true ) ]]; then
        CURRENT_STAGE="grub (EFI follow-up)"
        log "EFI repair completed; regenerating the running host GRUB fallback configuration." | tee -a "$SESSION_LOG"
        adaptive_grub_stage
    fi

    sync
    log "All requested running-host repair stages completed successfully." | tee -a "$SESSION_LOG"
}

# host-default entry point: restore the running host's canonical EFI entry when
# missing, promote it to the front of BootOrder and reconcile labels/duplicates
# while proving the complete pre-change firmware state was preserved.
run_host_default()
{
    local raw_disk="$1" raw_root="$2" pre current partuuid role reason
    local -a ids=()

    CURRENT_STAGE="host default EFI entry"
    RUNNING_HOST_MODE=1
    prepare_running_host "$raw_disk" "$raw_root" yes
    profile_target_backends
    if ! reason="$(efi_unavailable_reason)"; then
        fail "Host default EFI entry selection is not available: $reason."
    fi
    if is_alpine_family && [[ "$(alpine_efi_backend)" == efi-stub ]]; then
        # Selecting a default EFI-stub entry is a phase-2 capability: without
        # the captured cmdline parser the canonical entry cannot be recreated
        # safely, so fail closed instead of guessing.
        fail "Alpine EFI-stub host default selection is not implemented; use the Alpine EFI repair stage for entry reconciliation."
    fi
    uefi_nvram_writable || fail "UEFI variables are not writable; cannot change the running host's default EFI entry."
    command -v efibootmgr >/dev/null 2>&1 || fail "efibootmgr is required to change the running host's default EFI entry."

    pre="$SESSION_DIR/efi-nvram-host-default-before.txt"
    efibootmgr -v > "$pre" 2>&1 || fail "Unable to capture firmware entries before changing the host default."
    efi_print_firmware_inventory "$pre" | tee -a "$SESSION_LOG"
    if is_tuxedo_uki_layout; then
        restore_missing_host_tuxedo_uki_entry 1 \
            || fail "Unable to restore a missing host TUXEDO UKI entry without risking existing firmware entries."
        efi_annotate_selected_entries \
            || fail "Unable to annotate the running host's selected ESP or restore its iPXE/WebFAI registration safely."
    else
        validate_efi_bootloader_target
        partuuid="$(blkid -s PARTUUID -o value "$EFI_ESP_SOURCE" 2>/dev/null || true)"
        partuuid="${partuuid,,}"
        [[ -n "$partuuid" ]] || fail "Unable to identify the running host ESP PARTUUID for default selection."
        for role in uki vendor-loader fallback wfai; do
            mapfile -t ids < <(efi_entry_ids_for_partuuid_role "$partuuid" "$role" 2>/dev/null || true)
            if ((${#ids[@]} == 1)); then
                break
            fi
            ids=()
        done
        if ((${#ids[@]} != 1)); then
            log "No unique firmware entry exists for the running host ESP; creating the canonical host loader entry before selecting the default." | tee -a "$SESSION_LOG"
            reinstall_efi_bootloader
            mapfile -t ids < <(efi_entry_ids_for_partuuid_role "$partuuid" vendor-loader 2>/dev/null || true)
            ((${#ids[@]} == 1)) || fail "Unable to resolve a unique bootloader entry for the running host ESP after canonical EFI registration."
        fi
        efi_annotate_selected_entries \
            || fail "Unable to annotate the running host's selected ESP entries safely."
    fi
    current="$SESSION_DIR/efi-nvram-host-default-after-create.txt"
    efibootmgr -v > "$current" 2>&1 || fail "Unable to capture firmware entries after host entry restoration."
    if is_tuxedo_uki_layout; then
        mapfile -t ids < <(efi_uki_entry_ids_for_partuuid "$EFI_HOST_ESP_PARTUUID" 2>/dev/null || true)
        ((${#ids[@]} == 1)) || fail "Host TUXEDO UKI entry is not uniquely identifiable; refusing to change BootOrder."
    fi
    efi_promote_entry_first "${ids[0]}"
    efi_prune_selected_duplicate_destinations \
        || fail "Unable to reconcile duplicate host-ESP EFI destinations safely."
    efi_group_firmware_boot_order \
        || fail "Unable to group the retained host EFI entries by drive and boot use."
    efi_annotate_selected_entries \
        || fail "Unable to retain host EFI labels after BootOrder maintenance."
    efi_verify_selected_model_labels \
        || fail "Host EFI labels could not be verified after BootOrder maintenance."
    log "PASS: running host default EFI entry is Boot${ids[0]} on $EFI_ESP_SOURCE." | tee -a "$SESSION_LOG"
    # The pre-change NVRAM capture is the proof: an identical firmware state
    # after all reconciliation means BootOrder, labels and registrations
    # already matched what the operation would have written.
    if [[ -s "$pre" ]] && cmp -s "$pre" <(efibootmgr -v 2>/dev/null || true); then
        repair_change_status host-default "unchanged|BootOrder, labels and registrations already correct"
    else
        repair_change_status host-default changed
    fi
}

# repair entry point: enforce safe stage ordering, run every mandatory
# read-only safety preflight (including EFI/UKI and boot-stack checks) before
# promoting the target to read-write, then execute the requested stages.
run_repair()
{
    local efi_requested=false grub_requested=false boot_stack_requested=false previous_rank=0 current_rank stage

    parse_repair_arguments "$@"
    ((${#REPAIR_STAGES[@]} > 0)) || fail "No repair stages were requested."
    BOOT_STACK_POST_EFI="$REPAIR_POST_EFI"
    for stage in "${REPAIR_STAGES[@]}"; do
        validate_stage "$stage"
        current_rank="$(stage_rank "$stage")"
        (( current_rank >= previous_rank )) || fail "Repair stages are out of safe dependency order: $stage must run after earlier stages."
        previous_rank="$current_rank"
        [[ "$stage" == "efi" ]] && efi_requested=true
        [[ "$stage" == "grub" ]] && grub_requested=true
        [[ "$stage" == "boot-stack" ]] && boot_stack_requested=true
    done

    # Keep failures attributable to the operation that owns them.  The GUI
    # receives the helper transcript after the progress dialog closes, so this
    # context turns a bare exit code into a directly actionable stage report.
    CURRENT_STAGE="mandatory preflight"

    # Inspect the target read-only first. Merely mounting an unsupported
    # filesystem read-write can replay a journal, so stage applicability is
    # decided by the read-only backend probes before promoting the mount to
    # read-write.  No distribution-family prefilter is applied.
    prepare_target ro
    # Resolve and inspect separate boot filesystems while they are still
    # read-only. promote_target_rw() will remount only these recorded target
    # mounts after every selected-disk check has succeeded.  The backend probe
    # runs after /boot is visible so kernel/initramfs/bootloader evidence on a
    # separate boot partition is not missed.
    mount_target_boot_entry "/boot" ro
    mount_target_boot_entry "/boot/efi" ro
    mount_target_boot_entry "/efi" ro
    profile_target_backends
    validate_repair_stages_against_backends "${REPAIR_STAGES[@]}"
    if [[ "$efi_requested" == true ]]; then
        if is_tuxedo_uki_layout; then
            validate_tuxedo_uki_target
            log "TUXEDO UKI repair read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE)" | tee -a "$SESSION_LOG"
        elif is_alpine_family && [[ "$(alpine_efi_backend)" == efi-stub ]]; then
            alpine_efi_stub_preflight
            log "Alpine EFI-stub repair read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE)" | tee -a "$SESSION_LOG"
        else
            validate_efi_bootloader_target
            log "EFI repair read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE, id=$EFI_BOOTLOADER_ID)" | tee -a "$SESSION_LOG"
        fi
    fi
    if [[ "$boot_stack_requested" == true ]]; then
        if is_tuxedo_uki_layout; then
            validate_tuxedo_uki_target
            log "Boot-stack TUXEDO UKI read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE)" | tee -a "$SESSION_LOG"
        elif is_arch_family; then
            validate_efi_bootloader_target
            log "Boot-stack Arch EFI read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE, id=$EFI_BOOTLOADER_ID)" | tee -a "$SESSION_LOG"
        fi
    fi
    need chroot
    promote_target_rw

    log "Mandatory safety preflight: PASS" | tee -a "$SESSION_LOG"

    for stage in "${REPAIR_STAGES[@]}"; do
        CURRENT_STAGE="$stage"
        case "$stage" in
            dpkg-configure)
                run_package_stage dpkg-configure dpkg
                ;;
            fix-broken)
                run_package_stage fix-broken fixbroken
                ;;
            apt-update)
                run_package_stage apt-update aptupdate
                ;;
            apt-upgrade)
                run_package_stage apt-upgrade upgrade
                ;;
            dkms)
                adaptive_dkms_repair
                ;;
            display-manager)
                adaptive_display_manager_repair
                ;;
            initramfs)
                adaptive_initramfs_repair
                ;;
            efi)
                reinstall_efi_bootloader
                ;;
            boot-stack)
                repair_boot_stack
                ;;
            grub)
                adaptive_grub_stage
                ;;
            extlinux)
                adaptive_extlinux_repair
                ;;
        esac
    done

    # An individual EFI reinstall should leave a freshly generated GRUB menu.
    # When the caller already requested the GRUB stage, avoid running it twice.
    # With --post-efi the boot-stack stage reuses that EFI/UKI repair and skips
    # its own GRUB regeneration, so the follow-up must still run here.
    if [[ "$efi_requested" == true && "$grub_requested" != true \
        && ( "$boot_stack_requested" != true || "$REPAIR_POST_EFI" == true ) ]]; then
        CURRENT_STAGE="grub (EFI follow-up)"
        log "EFI repair completed; simulating and regenerating the GRUB fallback configuration." | tee -a "$SESSION_LOG"
        adaptive_grub_stage
    fi

    sync
    log "All requested repair stages completed successfully." | tee -a "$SESSION_LOG"
}

# ---------------------------------------------------------------------------
# Privileged session broker and command dispatch
# ---------------------------------------------------------------------------
session_protocol_error()
{
    local request_id="$1"
    shift
    printf 'SESSION_ERROR\t%s\t%s\n' "$request_id" "$*"
    printf 'DONE\t%s\t2\n' "$request_id"
}

# Long-lived pkexec broker: snapshot this helper into the root-owned state
# directory, then read tab-separated BEGIN/ARG/SECRET/END requests and run each
# whitelisted command through the authenticated copy, streaming OUT lines and
# a final DONE line.  Secrets are only ever read from the request stream.
session_server()
{
    local tag request_id argc has_secret i field_tag field_id encoded decoded end_tag end_id
    local command rc secret_b64 secret pre_mapper post_mapper unlock_device mapper_name
    local -a fields op_args

    need base64
    need tr
    need cp
    need chmod
    need bash

    # Do not repeatedly execute the user-writable source-tree helper after one
    # authorization. Snapshot the authenticated helper into a root-owned,
    # mode-0700 file for the lifetime of this broker. Child requests are
    # interpreted explicitly by bash rather than execve'd directly because
    # several recovery/live distributions mount /run with noexec. Installed
    # builds are already root-owned, but using the same rule keeps both paths
    # equivalent.
    ensure_state_root
    # mktemp atomically creates the destination, avoiding a symlink race on a
    # predictable PID-based filename before the helper source is copied.
    SESSION_HELPER_COPY="$(mktemp "$STATE_ROOT/session-helper.XXXXXX")"
    cp -- "$SELF_PATH" "$SESSION_HELPER_COPY"
    chown root:root -- "$SESSION_HELPER_COPY"
    chmod 0700 -- "$SESSION_HELPER_COPY"

    printf 'SESSION_READY\t1\n'

    while IFS=$'\t' read -r tag request_id argc has_secret; do
        [[ -n "$tag" ]] || continue
        if [[ "$tag" == "QUIT" ]]; then
            return 0
        fi
        if [[ "$tag" != "BEGIN" || ! "$request_id" =~ ^[0-9]+$ || ! "$argc" =~ ^[0-9]+$ || ! "$has_secret" =~ ^[01]$ ]]; then
            session_protocol_error "${request_id:-0}" "Malformed privileged-session request header."
            continue
        fi
        if (( argc < 1 || argc > 4096 )); then
            session_protocol_error "$request_id" "Privileged-session request has an invalid field count."
            continue
        fi

        fields=()
        for ((i=0; i<argc; ++i)); do
            if ! IFS=$'\t' read -r field_tag field_id encoded; then
                session_protocol_error "$request_id" "Unexpected end of privileged-session request."
                return 1
            fi
            if [[ "$field_tag" != "ARG" || "$field_id" != "$request_id" ]]; then
                session_protocol_error "$request_id" "Malformed privileged-session argument record."
                return 1
            fi
            if ! decoded="$(printf '%s' "$encoded" | base64 -d 2>/dev/null)"; then
                session_protocol_error "$request_id" "Unable to decode a privileged-session argument."
                return 1
            fi
            fields+=("$decoded")
        done

        secret_b64=""
        secret=""
        if [[ "$has_secret" == "1" ]]; then
            if ! IFS=$'\t' read -r field_tag field_id secret_b64; then
                session_protocol_error "$request_id" "Missing privileged-session secret record."
                return 1
            fi
            if [[ "$field_tag" != "SECRET" || "$field_id" != "$request_id" ]]; then
                session_protocol_error "$request_id" "Malformed privileged-session secret record."
                return 1
            fi
            if ! secret="$(printf '%s' "$secret_b64" | base64 -d 2>/dev/null)"; then
                session_protocol_error "$request_id" "Unable to decode privileged-session secret."
                return 1
            fi
            secret_b64=""
        fi

        if ! IFS=$'\t' read -r end_tag end_id; then
            session_protocol_error "$request_id" "Missing privileged-session end record."
            return 1
        fi
        if [[ "$end_tag" != "END" || "$end_id" != "$request_id" ]]; then
            session_protocol_error "$request_id" "Malformed privileged-session end record."
            return 1
        fi

        command="${fields[0]}"
        op_args=("${fields[@]:1}")
        case "$command" in
            unlock|validate|diagnose|config-read|config-write|snapshots|repair|shell|browse-target|copy-preview|copy|fs-inspect|fs-repair) ;;
            host-validate|host-diagnose|host-repair|host-default|host-shell|host-fs-inspect|host-fs-repair|host-snapshots|host-reboot) ;;
            *)
                secret=""
                session_protocol_error "$request_id" "Command is not permitted by the privileged-session broker: $command"
                continue
                ;;
        esac

        pre_mapper=""
        unlock_device=""
        if [[ "$command" == "unlock" && ${#op_args[@]} -ge 2 ]]; then
            unlock_device="${op_args[1]}"
            pre_mapper="$(find_crypt_mapper_for_device "$unlock_device" 2>/dev/null || true)"
        fi

        set +e
        if [[ "$has_secret" == "1" ]]; then
            printf '%s' "$secret" | bash "$SESSION_HELPER_COPY" "$command" "${op_args[@]}" 2>&1 |
                while IFS= read -r line || [[ -n "$line" ]]; do
                    printf 'OUT\t%s\t%s\n' "$request_id" "$line"
                done
            rc=${PIPESTATUS[1]}
        else
            bash "$SESSION_HELPER_COPY" "$command" "${op_args[@]}" 2>&1 |
                while IFS= read -r line || [[ -n "$line" ]]; do
                    printf 'OUT\t%s\t%s\n' "$request_id" "$line"
                done
            rc=${PIPESTATUS[0]}
        fi
        set -e

        if [[ "$command" == "unlock" && "$rc" -eq 0 && -n "$unlock_device" && -z "$pre_mapper" ]]; then
            post_mapper="$(find_crypt_mapper_for_device "$unlock_device" 2>/dev/null || true)"
            if [[ -n "$post_mapper" ]]; then
                mapper_name="$(basename -- "$post_mapper")"
                case " ${SESSION_OWNED_MAPPERS[*]:-} " in
                    *" $mapper_name "*) ;;
                    *) SESSION_OWNED_MAPPERS+=("$mapper_name") ;;
                esac
            fi
        fi

        secret=""
        fields=()
        op_args=()
        printf 'DONE\t%s\t%s\n' "$request_id" "$rc"
    done
}

# Helper entry point: require root, validate the command's argument count and
# dispatch to the single owning function.  Unknown or malformed commands print
# usage and fail before any target is touched.
main()
{
    [[ ${EUID:-$(id -u)} -eq 0 ]] || fail "This helper must run as root (normally through pkexec)."
    ensure_state_root

    (($# >= 1)) || { usage; exit 2; }
    local command="$1"; shift
    case "$command" in
        -h|--help|help)
            usage
            return 0
            ;;
        session)
            (($# == 0)) || fail "session does not accept command-line arguments."
            session_server
            return $?
            ;;
    esac

    (($# >= 2)) || { usage; exit 2; }
    TARGET_DISK="$1"; shift
    ROOT_DEVICE="$1"; shift

    case "$command" in
        unlock)
            (($# == 0)) || fail "unlock does not accept extra arguments."
            unlock_target
            ;;
        validate)
            (($# == 0)) || fail "validate does not accept repair stages."
            validate_target
            ;;
        diagnose)
            run_target_diagnostic "$@"
            ;;
        host-diagnose)
            [[ $# -eq 1 ]] || fail "host-diagnose requires exactly one diagnostic name or 'all'."
            run_host_diagnostic "$1"
            ;;
        host-validate)
            [[ $# -eq 0 ]] || fail "host-validate does not accept repair stages."
            validate_running_host
            ;;
        config-read)
            [[ $# -eq 1 ]] || fail "config-read requires exactly one configuration key."
            run_target_config read "$1"
            ;;
        config-write)
            [[ $# -eq 2 ]] || fail "config-write requires a configuration key and content."
            run_target_config write "$1" "$2"
            ;;
        snapshots)
            run_snapshots "$@"
            ;;
        host-snapshots)
            run_host_snapshots "$@"
            ;;
        host-reboot)
            [[ $# -eq 0 ]] || fail "host-reboot does not accept extra arguments."
            run_host_reboot "$TARGET_DISK" "$ROOT_DEVICE"
            ;;
        shell)
            [[ $# -eq 1 ]] || fail "shell requires exactly one command string."
            run_chroot_shell "$1"
            ;;
        browse-target)
            [[ $# -eq 1 ]] || fail "browse-target requires exactly one absolute directory path."
            browse_target_directory "$1"
            ;;
        repair)
            run_repair "$@"
            ;;
        fs-inspect)
            [[ $# -eq 0 ]] || fail "fs-inspect does not accept extra arguments."
            fs_inspect "$TARGET_DISK" "$ROOT_DEVICE"
            ;;
        fs-repair)
            [[ $# -eq 2 ]] || fail "fs-repair requires exactly one device and one repair mode."
            fs_repair "$TARGET_DISK" "$ROOT_DEVICE" "$1" "$2"
            ;;
        host-fs-inspect)
            [[ $# -eq 0 ]] || fail "host-fs-inspect does not accept extra arguments."
            RUNNING_HOST_MODE=1
            fs_inspect "$TARGET_DISK" "$ROOT_DEVICE"
            ;;
        host-fs-repair)
            [[ $# -eq 2 ]] || fail "host-fs-repair requires exactly one device and one repair mode."
            RUNNING_HOST_MODE=1
            fs_repair "$TARGET_DISK" "$ROOT_DEVICE" "$1" "$2"
            ;;
        host-repair)
            run_host_repair "$TARGET_DISK" "$ROOT_DEVICE" "$@"
            ;;
        host-default)
            [[ $# -eq 0 ]] || fail "host-default does not accept extra arguments."
            run_host_default "$TARGET_DISK" "$ROOT_DEVICE"
            ;;
        host-shell)
            [[ $# -eq 1 ]] || fail "host-shell requires exactly one command string."
            run_host_shell "$TARGET_DISK" "$ROOT_DEVICE" "$1"
            ;;
        copy-preview|copy)
            run_file_copy "$command" "$@"
            ;;
        *)
            usage
            fail "Unknown command: $command"
            ;;
    esac
}

main "$@"
