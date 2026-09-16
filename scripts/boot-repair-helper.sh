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
TARGET_PACKAGE_MANAGER=""
TARGET_INITRAMFS_BACKEND=""
TARGET_BOOTLOADER_BACKEND=""
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

# Display-manager selection is discovered from the target's existing
# display-manager.service link and installed units.  Keep this state scoped to
# one helper invocation so graphical-login repair never silently switches a
# target from its configured login manager to SDDM.
DISPLAY_MANAGER_SERVICE=""
DISPLAY_MANAGER_PACKAGE=""
DISPLAY_MANAGER_LABEL=""
DISPLAY_MANAGER_UNIT_REL=""

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
  $PROGRAM_NAME repair       <target-disk> <root-device> <stage> [stage ...]
  $PROGRAM_NAME host-repair  <host-disk> <root-device> <stage> [stage ...]
  $PROGRAM_NAME host-validate <host-disk> <root-device>
  $PROGRAM_NAME host-diagnose <host-disk> <root-device> <diagnostic|all>
  $PROGRAM_NAME host-default <host-disk> <root-device>
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

Shell:
  shell            Execute one reviewed command as root inside a fresh target chroot

LUKS unlock is supported for non-host target devices. The passphrase is read
from standard input and is never accepted as a command-line argument. File copy
uses rsync without --delete and independently validates host/target containment.
Modifying repair stages use a transaction-specific backend preflight. Debian/
Ubuntu uses APT/dpkg; Arch uses a sandboxed full pacman transaction, mkinitcpio,
and its detected EFI/GRUB layout. Unsupported package or boot layouts remain
hard-gated. EFI bootloader reinstall is an explicit stage and is never selected
implicitly.
Read-only diagnostics also profile Arch-family targets (pacman, initramfs
generator, GRUB/systemd-boot/UKI layout, ESP mount and kernel naming). Arch
modifying stages are enabled only where the corresponding guarded preflight
supports the detected layout.
Host maintenance is a separate native-running-system path. It accepts all
repair stages supported by the detected backend with the same stage-specific
checks; unsupported package or boot stages are rejected before any write.
Host validation and diagnostics are read-only; snapshot, shell and file-copy
workflows remain separate target tools.
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

resolve_fstab_source()
{
    local spec="$1"
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

mount_special()
{
    local kind="$1" source="$2" destination="$3" root_real parent_real
    root_real="$(realpath -e -- "$TARGET_ROOT" 2>/dev/null)" \
        || fail "Unable to resolve target root before mounting $kind."
    [[ "$destination" == "$TARGET_ROOT/"* && ! -L "$destination" ]] \
        || fail "Refusing to mount $kind through an unsafe target path: $destination"
    parent_real="$(realpath -e -- "$(dirname -- "$destination")" 2>/dev/null)" \
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
    root_real="$(realpath -e -- "$TARGET_ROOT" 2>/dev/null || true)"
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
    root_real="$(realpath -e -- "$TARGET_ROOT" 2>/dev/null)" \
        || fail "Unable to resolve target root before mounting $mp."
    dest_parent_real="$(realpath -e -- "$(dirname -- "$dest")" 2>/dev/null)" \
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

mount_target_btrfs_subvolumes()
{
    local requested_mode="$1" depth spec mountpoint_name fstype options resolved dest mount_options option filtered_options
    local -a option_parts=()

    [[ "$requested_mode" == "ro" || "$requested_mode" == "rw" ]] \
        || fail "Internal Btrfs subvolume mount mode error: $requested_mode"
    [[ -f "$TARGET_ROOT/etc/fstab" ]] || return 0
    [[ "$(lsblk -ndo FSTYPE "$ROOT_CANONICAL" 2>/dev/null | head -n1 || true)" == "btrfs" ]] || return 0

    log "Mounting target Btrfs fstab subvolumes ($requested_mode)" | tee -a "$SESSION_LOG"

    while IFS=$'\t' read -r depth spec mountpoint_name fstype options; do
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
    done < <(find "$base" -mindepth 1 -maxdepth 3 -type f -path '*/etc/os-release' -printf '%h\n' 2>/dev/null | sed 's#/etc$##')

    return 1
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

target_distro_family()
{
    if is_debian_family; then
        printf '%s\n' "debian"
    elif is_arch_family; then
        printf '%s\n' "arch"
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

profile_target_backends()
{
    local family="${TARGET_DISTRO_FAMILY:-}" esp_path
    [[ -n "$family" ]] || family="$(target_distro_family)"
    TARGET_DISTRO_FAMILY="$family"

    if target_has_executable /usr/bin/pacman /usr/bin/pacman-static; then
        TARGET_PACKAGE_MANAGER="pacman"
    elif target_has_executable /usr/bin/apt-get /usr/bin/apt; then
        TARGET_PACKAGE_MANAGER="apt/dpkg"
    elif target_has_executable /usr/bin/dnf /usr/bin/yum; then
        TARGET_PACKAGE_MANAGER="dnf/rpm"
    elif target_has_executable /usr/bin/zypper; then
        TARGET_PACKAGE_MANAGER="zypper/rpm"
    else
        TARGET_PACKAGE_MANAGER="unknown"
    fi

    if target_has_executable /usr/bin/mkinitcpio /usr/sbin/mkinitcpio \
        || target_has_path /etc/mkinitcpio.conf /etc/mkinitcpio.d; then
        TARGET_INITRAMFS_BACKEND="mkinitcpio"
    elif target_has_executable /usr/bin/dracut /usr/sbin/dracut \
        || target_has_path /etc/dracut.conf /etc/dracut.conf.d; then
        TARGET_INITRAMFS_BACKEND="dracut"
    elif target_has_executable /usr/bin/booster /usr/lib/booster/booster; then
        TARGET_INITRAMFS_BACKEND="booster"
    elif target_has_executable /usr/sbin/update-initramfs /usr/bin/update-initramfs \
        || target_has_executable /usr/sbin/mkinitramfs /usr/bin/mkinitramfs; then
        TARGET_INITRAMFS_BACKEND="initramfs-tools"
    else
        TARGET_INITRAMFS_BACKEND="unknown"
    fi

    if target_has_path /boot/grub/grub.cfg \
        || target_has_executable /usr/sbin/grub-mkconfig /usr/bin/grub-mkconfig \
        || target_has_executable /usr/sbin/update-grub /usr/bin/update-grub; then
        TARGET_BOOTLOADER_BACKEND="grub"
    elif target_has_path /boot/loader /efi/loader /boot/EFI/systemd /efi/EFI/systemd \
        || target_has_executable /usr/bin/bootctl /usr/bin/kernel-install; then
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
    for esp_path in /efi /boot/efi /boot; do
        if mountpoint -q "$TARGET_ROOT$esp_path" 2>/dev/null \
            && findmnt -rn -o FSTYPE --target "$TARGET_ROOT$esp_path" 2>/dev/null \
                | grep -Eiq '^(vfat|fat|fat16|fat32|msdos)$'; then
            TARGET_ESP_MOUNT="$esp_path"
            break
        fi
    done
    if [[ -z "$TARGET_ESP_MOUNT" ]]; then
        for esp_path in /efi /boot/efi /boot; do
            if [[ -d "$TARGET_ROOT$esp_path/EFI" || -d "$TARGET_ROOT$esp_path/loader" ]]; then
                TARGET_ESP_MOUNT="$esp_path"
                break
            fi
        done
    fi
    [[ -n "$TARGET_ESP_MOUNT" ]] || TARGET_ESP_MOUNT="unresolved"

    if find "$TARGET_ROOT/boot" -maxdepth 1 -type f -name 'vmlinuz-linux*' -print -quit 2>/dev/null | grep -q .; then
        TARGET_KERNEL_LAYOUT="Arch-style named kernels (vmlinuz-linux*)"
    elif find "$TARGET_ROOT/boot" -maxdepth 1 -type f -name 'vmlinuz-*' -print -quit 2>/dev/null | grep -q .; then
        TARGET_KERNEL_LAYOUT="versioned vmlinuz-* kernels"
    else
        TARGET_KERNEL_LAYOUT="no conventional vmlinuz files detected"
    fi

    if [[ "$family" == arch ]]; then
        TARGET_REPAIR_BACKEND="Arch profile — guarded pacman/mkinitcpio/GRUB/EFI repairs when transaction preflights pass"
    elif [[ "$family" == debian ]]; then
        TARGET_REPAIR_BACKEND="Debian/APT profile — existing guarded modifying backend"
    else
        TARGET_REPAIR_BACKEND="${family^} profile — diagnostics only (modifying backend not enabled)"
    fi
}

profile_esp_root()
{
    profile_target_backends
    [[ "$TARGET_ESP_MOUNT" != unresolved ]] || return 1
    printf '%s\n' "$TARGET_ROOT$TARGET_ESP_MOUNT"
}

prepare_target()
{
    local mode="$1" fstype mounted_root raw_target raw_root mount_mode discovered_subvol="" root_mount_options
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
        [[ -n "$mounted_root" ]] || fail "Btrfs filesystem mounted, but no Linux root with /etc/os-release was found."
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
        mount_special rbind-ro /dev "$TARGET_ROOT/dev"
        mount_special proc proc "$TARGET_ROOT/proc"
        mount_special rbind-ro /sys "$TARGET_ROOT/sys"
        mount_special tmpfs none "$TARGET_ROOT/run"
        mount_target_resolver
    fi
}

prepare_running_host()
{
    local raw_target="$1" raw_root="$2" require_debian="${3:-yes}" require_rw="${4:-no}" fstype

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
    if [[ "$require_debian" == yes ]]; then
        is_debian_family || fail "Native host maintenance is limited to Debian/Ubuntu-family systems. Detected: $TARGET_PRETTY"
    fi
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
    mount_special rbind-ro /dev "$TARGET_ROOT/dev"
    mount_special proc proc "$TARGET_ROOT/proc"
    mount_special rbind-ro /sys "$TARGET_ROOT/sys"
    mount_special tmpfs none "$TARGET_ROOT/run"
    # prepare_target is deliberately read-only during repair preflight.  The
    # resolver bind therefore cannot be installed until this promotion step;
    # without it apt-update inside the repair chroot sees the target's stale
    # systemd-resolved path and fails DNS resolution.
    mount_target_resolver
}


promote_target_data_rw()
{
    [[ -n "$MOUNT_BASE" && -d "$MOUNT_BASE" ]] || fail "Internal target mount is not prepared."
    TARGET_WRITE_INTENT=1
    log "Remounting confirmed target filesystem read-write for file copy" | tee -a "$SESSION_LOG"
    mount -o remount,rw "$MOUNT_BASE"
    remount_target_data_rw
}

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
    root_real="$(realpath -e -- "$TARGET_ROOT")" \
        || fail "Unable to resolve mounted target root."
    candidate_real="$(realpath -e -- "$candidate")" \
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

    root_real="$(realpath -e -- "$TARGET_ROOT")" || fail "Unable to resolve mounted target root."
    parent_real="$(realpath -e -- "$(dirname -- "$candidate")")" || fail "Unable to resolve repair-system source parent: $virtual_path"
    path_within "$parent_real" "$root_real" || fail "Repair-system source escapes the selected target through a symlink: $virtual_path"
    printf '%s\n' "$candidate"
}

target_destination_path()
{
    local virtual_path="$1" create="$2" candidate root_real probe probe_real owner_uid owner_gid missing_path
    local -a missing_paths=()
    validate_virtual_path "$virtual_path"
    candidate="$TARGET_ROOT$virtual_path"
    root_real="$(realpath -e -- "$TARGET_ROOT")" || fail "Unable to resolve mounted target root."

    if [[ -L "$candidate" ]]; then
        fail "Repair-system destination must not be a symbolic link: $virtual_path"
    fi
    if [[ -e "$candidate" && ! -d "$candidate" ]]; then
        fail "Repair-system destination exists but is not a directory: $virtual_path"
    fi

    probe="$(nearest_existing_directory "$candidate")" || fail "No existing parent directory was found for target destination: $virtual_path"
    probe_real="$(realpath -e -- "$probe")" || fail "Unable to resolve target destination parent: $virtual_path"
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
        probe_real="$(realpath -e -- "$candidate")" || fail "Unable to resolve target destination: $virtual_path"
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
    parent_real="$(realpath -e -- "$(dirname -- "$source")")" || fail "Unable to resolve host source parent: $source"
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
    real="$(realpath -e -- "$destination")" || fail "Unable to resolve host destination: $destination"

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
    # Native package actions must not race apt, dpkg, unattended-upgrades, or
    # another package frontend already using the live host. Stale lock files
    # alone are not considered active; inspect processes and lock holders.
    for proc in apt apt-get dpkg pacman makepkg yay paru unattended-upgrade packagekitd; do
        if pgrep -x "$proc" >/dev/null 2>&1; then
            fail "Package manager process '$proc' is already running; refusing a concurrent host package repair."
        fi
    done
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
    log "Host package-manager concurrency gate: PASS" | tee -a "$SESSION_LOG"
}

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

run_apt_update()
{
    local output rc
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
    ((rc == 0)) || fail "Refresh package metadata failed with exit code $rc."
    # apt-get can return success while retaining stale indexes when every
    # repository fetch fails. Treat that as a failed refresh so a subsequent
    # package repair never proceeds on misleading metadata.
    if grep -Eiq 'failed to fetch|some index files failed|temporary failure resolving|could not resolve|err:[[:space:]]' <<<"$output"; then
        fail "Refresh package metadata did not complete; repository indexes could not be refreshed. Check target networking or repository configuration."
    fi
    log "PASS: Refresh package metadata" | tee -a "$SESSION_LOG"
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

adaptive_apt_upgrade()
{
    local chosen="" upgrade_output="" full_output="" dist_output=""

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

    log "APT upgrade decision: '$chosen' selected from simulation results." | tee -a "$SESSION_LOG"
    run_chroot "Upgrade installed packages ($chosen)" apt-get -y "$chosen"

    # dpkg --audit is non-destructive and gives an immediate post-upgrade sanity
    # check. Any output is logged for the recovery record without turning a
    # harmless informational audit into a second package operation.
    log "Post-upgrade dpkg audit:" | tee -a "$SESSION_LOG"
    run_selected_chroot /usr/bin/env \
        HOME=/root \
        PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
        dpkg --audit 2>&1 | tee -a "$SESSION_LOG" || true
}

adaptive_fix_broken()
{
    local output rc

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

    run_chroot "Repair broken package dependencies" apt-get -y -f install
}

ARCH_PACMAN_TRY_OUTPUT=""
ARCH_PACMAN_TRY_RC=0

arch_pacman_prepare_sandbox()
{
    local tag
    [[ "$TARGET_DISTRO_FAMILY" == arch && "$TARGET_PACKAGE_MANAGER" == pacman ]] \
        || fail "Arch pacman backend is not selected for this target."
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

arch_pacman_transaction_is_safe()
{
    local output="$1" package_count
    if grep -Eiq 'failed retrieving file|failed to synchronize|failed to download|could not resolve host|could not resolve address|invalid or corrupted package|failed to prepare transaction|failed to commit transaction' <<<"$output"; then
        log "REFUSED: pacman preflight reported repository, download or transaction integrity errors." | tee -a "$SESSION_LOG"
        return 1
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
    arch_pacman_transaction_try "Arch pacman full transaction preflight" -Syu --downloadonly --noconfirm
    ((ARCH_PACMAN_TRY_RC == 0)) \
        || fail "Arch pacman transaction preflight failed; no target packages were changed."
    arch_pacman_transaction_is_safe "$ARCH_PACMAN_TRY_OUTPUT" \
        || fail "Arch pacman transaction was rejected by Boot Bitch safety policy."
    log "PASS: Arch pacman transaction preflight resolved without removals." | tee -a "$SESSION_LOG"
}

adaptive_arch_pacman_repair()
{
    local label="${1:-Upgrade installed packages}"
    preflight_arch_pacman_transaction
    run_chroot "$label (pacman -Syu)" pacman --noconfirm -Syu
    log "PASS: $label completed through one full pacman transaction." | tee -a "$SESSION_LOG"
}

CHROOT_TRY_OUTPUT=""
CHROOT_TRY_RC=0

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

preflight_dkms()
{
    local kver header_pkg
    local -a missing_headers=() available_headers=()

    if [[ "$TARGET_DISTRO_FAMILY" == arch ]]; then
        [[ -x "$TARGET_ROOT/usr/bin/dkms" || -x "$TARGET_ROOT/usr/sbin/dkms" ]] \
            || fail "DKMS is not installed in the Arch target system."
        log "SIMULATE/PREFLIGHT: Arch DKMS rebuild (headers must already be installed; no package guessing is performed)" | tee -a "$SESSION_LOG"
        while IFS= read -r kver; do
            [[ -n "$kver" ]] || continue
            run_selected_chroot /usr/bin/env PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
                test -e "/lib/modules/$kver/build" \
                || fail "Arch DKMS preflight found no /lib/modules/$kver/build tree; install the matching headers and retry."
            log "DKMS preflight: headers/build tree present for $kver" | tee -a "$SESSION_LOG"
        done < <(arch_kernel_versions)
        run_chroot_try "Inspect Arch DKMS state" dkms status
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
}

display_manager_package_for_service()
{
    if [[ "$TARGET_DISTRO_FAMILY" == arch ]]; then
        case "$1" in
            gdm.service|gdm3.service) printf '%s\n' 'gdm' ;;
            *) printf '%s\n' "${1%.service}" ;;
        esac
        return 0
    fi
    case "$1" in
        sddm.service) printf '%s\n' 'sddm' ;;
        gdm.service|gdm3.service) printf '%s\n' 'gdm3' ;;
        lightdm.service) printf '%s\n' 'lightdm' ;;
        greetd.service) printf '%s\n' 'greetd' ;;
        ly.service) printf '%s\n' 'ly' ;;
        *) printf '%s\n' '' ;;
    esac
}

display_manager_label_for_service()
{
    case "$1" in
        sddm.service) printf '%s\n' 'SDDM' ;;
        gdm.service|gdm3.service) printf '%s\n' 'GDM3' ;;
        lightdm.service) printf '%s\n' 'LightDM' ;;
        greetd.service) printf '%s\n' 'greetd' ;;
        ly.service) printf '%s\n' 'ly' ;;
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
    if [[ "$TARGET_DISTRO_FAMILY" == arch ]]; then
        [[ -n "$package" && ( -x "$TARGET_ROOT/usr/bin/pacman" || -x "$TARGET_ROOT/usr/bin/pacman-static" ) ]] || return 1
        run_selected_chroot /usr/bin/env PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
            pacman --root / -Q "$package" >/dev/null 2>&1
        return $?
    fi
    [[ -n "$package" && -x "$TARGET_ROOT/usr/bin/dpkg-query" ]] || return 1
    run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
        dpkg-query -W -f='${db:Status-Status}' "$package" 2>/dev/null \
        | grep -Fxq installed
}

last_boot_display_manager_candidates()
{
    local candidate
    local -a known_services=(sddm.service gdm3.service lightdm.service greetd.service ly.service)
    command -v journalctl >/dev/null 2>&1 || return 0
    [[ -d "$TARGET_ROOT/var/log/journal" ]] || return 0
    for candidate in "${known_services[@]}"; do
        # A unit-scoped query is used only for selection.  The complete
        # graphics journal remains visible in diagnostic_display below.
        if journalctl --root="$TARGET_ROOT" -b 0 -u "$candidate" --no-pager -n 5 2>/dev/null | grep -q .; then
            printf '%s\n' "$candidate"
        fi
    done
}

detect_display_manager()
{
    local configured_service="" link="" service="" package="" unit=""
    local candidate installed_count=0 installed_service="" journal_count=0 journal_service=""
    local -a known_services=(sddm.service gdm3.service lightdm.service greetd.service ly.service)

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

    [[ -n "$service" ]] || fail "No configured graphical login manager was found. Install and configure SDDM, GDM3, LightDM, greetd, or another display manager first."
    DISPLAY_MANAGER_SERVICE="$service"
    DISPLAY_MANAGER_UNIT_REL="${unit:-$(display_manager_unit_rel "$service" || true)}"
    DISPLAY_MANAGER_PACKAGE="$(display_manager_package_for_service "$service")"
    DISPLAY_MANAGER_LABEL="$(display_manager_label_for_service "$service")"
    [[ -n "$DISPLAY_MANAGER_UNIT_REL" ]] || fail "Display manager $DISPLAY_MANAGER_SERVICE has no unit file in the target."
}

preflight_display_manager()
{
    local manager_status="" desktop_status=""
    need systemctl

    log "SIMULATE/PREFLIGHT: graphical login / display manager" | tee -a "$SESSION_LOG"
    if [[ "$TARGET_DISTRO_FAMILY" == arch ]]; then
        detect_display_manager
        log "Detected Arch display manager: $DISPLAY_MANAGER_LABEL ($DISPLAY_MANAGER_SERVICE)" | tee -a "$SESSION_LOG"
        target_package_installed "$DISPLAY_MANAGER_PACKAGE" \
            || fail "$DISPLAY_MANAGER_LABEL is not installed according to pacman; refusing to enable an unverified display manager."
        [[ -f "$TARGET_ROOT/usr/lib/systemd/system/graphical.target" || -f "$TARGET_ROOT/lib/systemd/system/graphical.target" ]] \
            || fail "graphical.target is missing from the Arch target system."
        [[ -f "$TARGET_ROOT$DISPLAY_MANAGER_UNIT_REL" ]] \
            || fail "$DISPLAY_MANAGER_LABEL service unit is missing from the Arch target."
        trial_display_manager_headless
        log "Arch display-manager preflight passed; repair will only adjust offline systemd links." | tee -a "$SESSION_LOG"
        return 0
    fi
    [[ -x "$TARGET_ROOT/usr/bin/dpkg-query" ]] \
        || fail "dpkg-query is unavailable in the target; cannot validate graphical-login packages."

    detect_display_manager
    log "Detected graphical login manager: $DISPLAY_MANAGER_LABEL ($DISPLAY_MANAGER_SERVICE)" | tee -a "$SESSION_LOG"
    manager_status="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
        dpkg-query -W -f='${db:Status-Status}' "$DISPLAY_MANAGER_PACKAGE" 2>/dev/null || true)"
    if [[ -n "$DISPLAY_MANAGER_PACKAGE" && "$manager_status" != "installed" ]]; then
        safe_apt_install_packages "Install missing $DISPLAY_MANAGER_LABEL display manager" no "$DISPLAY_MANAGER_PACKAGE"
        manager_status="installed"
    fi

    if [[ "$TARGET_OS_ID" == "tuxedo" ]]; then
        desktop_status="$(run_selected_chroot /usr/bin/env PATH=/usr/sbin:/usr/bin:/sbin:/bin \
            dpkg-query -W -f='${db:Status-Status}' tuxedoos-desktop 2>/dev/null || true)"
        if [[ "$desktop_status" != "installed" ]] && apt_package_available tuxedoos-desktop; then
            safe_apt_install_packages "Restore the TUXEDO desktop meta-package required for graphical login" no tuxedoos-desktop
        fi
    fi

    [[ -f "$TARGET_ROOT/usr/lib/systemd/system/graphical.target" || -f "$TARGET_ROOT/lib/systemd/system/graphical.target" ]] \
        || fail "graphical.target is missing from the target system; repair systemd packages before restoring graphical login."
    [[ -f "$TARGET_ROOT$DISPLAY_MANAGER_UNIT_REL" ]] \
        || fail "$DISPLAY_MANAGER_LABEL service unit is still missing after known package correction."

    trial_display_manager_headless

    log "Display-manager preflight plan: set graphical.target default, enable $DISPLAY_MANAGER_SERVICE, and repair display-manager.service offline." | tee -a "$SESSION_LOG"
}

adaptive_display_manager_repair()
{
    preflight_display_manager
    restore_display_manager
}

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

adaptive_initramfs_repair()
{
    if [[ "$TARGET_DISTRO_FAMILY" == arch ]]; then
        adaptive_arch_initramfs_repair
        return 0
    fi
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
}

preflight_grub()
{
    local grub_mkconfig="" sim_path target_sim_path err_path output err_output rc generator_mode

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
        generator_mode="grub-mkconfig"
        log "Using grub-mkconfig with an isolated output path for preflight." | tee -a "$SESSION_LOG"
    elif [[ -x "$TARGET_ROOT/usr/sbin/update-grub" || -x "$TARGET_ROOT/usr/bin/update-grub" ]]; then
        generator_mode="update-grub"
        log "grub-mkconfig unavailable; using update-grub output for preflight." | tee -a "$SESSION_LOG"
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
        if [[ "$generator_mode" == update-grub ]]; then
            output="$(
                run_selected_chroot /usr/bin/env \
                    HOME=/root PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
                    update-grub 2>"$err_path"
            )"
        else
            output="$(
                run_selected_chroot /usr/bin/env \
                    HOME=/root PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
                    "$grub_mkconfig" -o "$target_sim_path" 2>"$err_path"
            )"
        fi
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

    if [[ "$generator_mode" == update-grub ]]; then
        printf '%s\n' "$output" > "$sim_path"
    else
        [[ -s "$TARGET_ROOT$target_sim_path" ]] || fail "grub-mkconfig completed but produced no temporary configuration."
        cp -- "$TARGET_ROOT$target_sim_path" "$sim_path"
    fi
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

guard_grub_candidate_preserves_entries()
{
    local existing="$1" candidate="$2" old_keys new_keys missing

    [[ -s "$existing" && -s "$candidate" ]] || return 0
    old_keys="$SESSION_DIR/grub-existing-entries"
    new_keys="$SESSION_DIR/grub-candidate-entries"
    missing="$SESSION_DIR/grub-missing-entries"
    grub_entry_keys "$existing" > "$old_keys"
    grub_entry_keys "$candidate" > "$new_keys"
    comm -23 "$old_keys" "$new_keys" > "$missing" || true
    if [[ -s "$missing" ]]; then
        log "ERROR: GRUB preflight candidate would remove existing menu entries; the target configuration was left unchanged." | tee -a "$SESSION_LOG"
        sed 's/^/  preserved-entry-required: /' "$missing" | tee -a "$SESSION_LOG"
        return 1
    fi
}

adaptive_grub_repair()
{
    local old_cfg="$SESSION_DIR/grub-before-update.cfg" generator="update-grub"

    if [[ ! -x "$TARGET_ROOT/usr/sbin/update-grub" && ! -x "$TARGET_ROOT/usr/bin/update-grub" ]]; then
        generator="grub-mkconfig"
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
}

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
    local scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    profile_target_backends

    echo "${scope_label^} backend profile (read-only):"
    echo "Distribution ID: ${TARGET_OS_ID:-unknown}"
    echo "Distribution family: ${TARGET_DISTRO_FAMILY:-unknown}"
    echo "Distribution name: ${TARGET_PRETTY:-unknown}"
    echo "Package manager backend: ${TARGET_PACKAGE_MANAGER:-unknown}"
    echo "Initramfs backend: ${TARGET_INITRAMFS_BACKEND:-unknown}"
    echo "Bootloader backend: ${TARGET_BOOTLOADER_BACKEND:-unknown}"
    echo "ESP mount candidate: ${TARGET_ESP_MOUNT:-unresolved}"
    echo "Kernel layout: ${TARGET_KERNEL_LAYOUT:-unknown}"
    echo "Repair capability: ${TARGET_REPAIR_BACKEND:-unknown}"

    if [[ "$TARGET_DISTRO_FAMILY" == arch ]]; then
        echo "Arch policy: package changes require an explicit full pacman transaction; partial metadata refresh is not treated as a repair."
        echo "Arch status: modifying package, initramfs, GRUB and conventional EFI actions require their transaction-specific preflight."
    elif [[ "$TARGET_DISTRO_FAMILY" == debian ]]; then
        echo "Debian policy: existing guarded APT/dpkg repair backend selected when its stage-specific preflight passes."
    else
        echo "Policy: this profile is currently diagnostics-only."
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

diagnostic_boot_chain()
{
    local uki="" esp_root=""
    local grub_cfg="$TARGET_ROOT/boot/grub/grub.cfg"
    local root_type backing luks_uuid crypttab_line crypttab_key
    local uki_cmdline="" uki_luks_uuid="" uki_luks_name="" cryptdevice_uuid=""
    local tmp
    local has_uki=false has_grub=false

    profile_target_backends
    esp_root="$(profile_esp_root 2>/dev/null || true)"
    [[ -n "$esp_root" ]] && uki="$esp_root/EFI/BOOT/TUX.EFI"

    [[ -s "$uki" ]] && has_uki=true
    [[ -s "$grub_cfg" ]] && has_grub=true
    echo "Detected boot chain (read-only):"
    if [[ "$TARGET_BOOTLOADER_BACKEND" == "systemd-boot + UKI" ]]; then
        echo "Primary: firmware EFI entry -> systemd-boot -> UKI or loader entry -> initramfs -> root filesystem -> graphical login."
        [[ "$has_grub" == true ]] && echo "Fallback: firmware fallback/GRUB entry -> GRUB menu -> initramfs -> root filesystem -> graphical login."
    elif [[ "$TARGET_BOOTLOADER_BACKEND" == "systemd-boot" ]]; then
        echo "Primary: firmware EFI entry -> systemd-boot loader entry -> initramfs -> root filesystem -> graphical login."
    elif [[ "$TARGET_BOOTLOADER_BACKEND" == "generic UKI" ]]; then
        echo "Primary: firmware EFI entry -> distribution UKI -> embedded initramfs -> root filesystem -> graphical login."
    elif [[ "$has_uki" == true && -x "$TARGET_ROOT/usr/sbin/create_boot_uki_base.sh" ]]; then
        echo "Primary: firmware EFI entry -> TUXEDO UKI (TUX.EFI) -> initramfs -> root filesystem -> graphical login."
        [[ "$has_grub" == true ]] && echo "Fallback: firmware fallback/GRUB entry -> GRUB menu -> initramfs -> root filesystem -> graphical login."
    elif [[ "$has_grub" == true ]]; then
        echo "Primary: firmware EFI entry -> GRUB menu -> initramfs -> root filesystem -> graphical login."
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
    local journal_dir="$TARGET_ROOT/var/log/journal"
    local grub_cfg="$TARGET_ROOT/boot/grub/grub.cfg"
    local cmdline_file="$TARGET_ROOT/proc/cmdline"
    local evidence_rc=0 efi_nvram_evidence scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"

    echo "Boot evidence (read-only):"
    echo "${scope_label^} root: $TARGET_ROOT"
    echo "Captured: $(date --iso-8601=seconds 2>/dev/null || date)"
    echo "Mounted ${scope_label} source/options: $(findmnt -rn -o SOURCE,OPTIONS --target "$TARGET_ROOT" 2>/dev/null | head -1 || echo unknown)"
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
    echo
    diagnostic_boot_chain
    echo
    if [[ -f "$TARGET_ROOT/boot/grub/grubenv" ]]; then
        echo "GRUB environment (saved/next selection):"
        if command -v grub-editenv >/dev/null 2>&1; then
            grub-editenv "$TARGET_ROOT/boot/grub/grubenv" list 2>&1 || true
        else
            strings "$TARGET_ROOT/boot/grub/grubenv" 2>/dev/null \
                | grep -E '^(saved_entry|next_entry|prev_saved_entry)=' || echo "grub-editenv is unavailable."
        fi
    fi
    if [[ -d "$TARGET_ROOT/boot/loader" ]]; then
        echo "systemd-boot loader selection:"
        [[ -f "$TARGET_ROOT/boot/loader/loader.conf" ]] \
            && sed -n '1,80p' "$TARGET_ROOT/boot/loader/loader.conf" || echo "loader.conf is not present."
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
        echo "Firmware boot selection and ownership (host NVRAM context):"
        efi_nvram_evidence="$SESSION_DIR/boot-evidence-efi-nvram.txt"
        efibootmgr -v > "$efi_nvram_evidence" 2>&1 || true
        efi_print_firmware_inventory "$efi_nvram_evidence"
        efibootmgr -v 2>&1 | grep -E '^(Boot(Current|Next|Order):)' | head -20 || true
    fi
    echo

    echo "${scope_label^} kernel and initramfs selection candidates:"
    find "$TARGET_ROOT/boot" -maxdepth 1 -type f \
        \( -name 'vmlinuz-*' -o -name 'initrd.img-*' -o -name 'config-*' \) \
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
    if [[ -d "$journal_dir" ]] && command -v journalctl >/dev/null 2>&1; then
        # Restrict issue evidence to the most recent selected-system boot. Older boots
        # are retained in the boot-ID inventory below, but their resolved
        # failures should not drive a repair decision for the current boot.
        journalctl --root="$TARGET_ROOT" -b 0 --no-pager -n 1200 2>/dev/null \
            | journal_current_boot_actionable \
            | grep -Ei 'cryptsetup|systemd-cryptsetup|luks|passphrase|password|unlock|keyslot|dracut|initramfs' \
            | sed -E 's/(password|passphrase|passwd|key)[=:][[:space:]]*[^[:space:]]+/\1=[REDACTED]/Ig' \
            | tail -260 || echo "No unlock-related ${scope_label} journal entries found."
    else
        echo "No persistent target journal is available."
    fi
    echo

    echo "Boot-selection and kernel messages (${scope_label} journal):"
    if [[ -d "$journal_dir" ]] && command -v journalctl >/dev/null 2>&1; then
        echo "Journal scope: latest ${scope_label} boot (-b 0); older boot failures are omitted from inspection evidence."
        journalctl --root="$TARGET_ROOT" -b 0 --no-pager -n 1600 2>/dev/null \
            | journal_current_boot_actionable \
            | grep -Ei 'kernel command line|BOOT_IMAGE|selected|default entry|menuentry|grub|systemd-boot|efiboot|efi|initramfs|mount.*(root|boot)|failed|timeout|dependency' \
            | tail -360 || echo "No boot-selection messages found."
        echo
        echo "${scope_label^} journal boot IDs (if available):"
        journalctl --root="$TARGET_ROOT" --list-boots --no-pager 2>&1 | tail -40 || true
    else
        echo "No persistent target journal is available."
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
    for initrd in "$TARGET_ROOT"/boot/initrd.img "$TARGET_ROOT"/boot/initrd.img-* \
                  "$TARGET_ROOT"/boot/initramfs-*.img; do
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
    local cfg="$TARGET_ROOT/boot/grub/grub.cfg" scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    profile_target_backends
    echo "Detected bootloader backend: ${TARGET_BOOTLOADER_BACKEND:-unknown}"
    if [[ "$TARGET_BOOTLOADER_BACKEND" != grub ]]; then
        echo "GRUB is not the selected backend; showing any visible GRUB files for comparison only."
    fi
    echo "GRUB configuration: $cfg"
    if [[ -f "$cfg" ]]; then
        grep -E '^[[:space:]]*(menuentry|submenu)|linux[[:space:]]|linuxefi[[:space:]]|initrd[[:space:]]|initrdefi[[:space:]]|root=|subvol' "$cfg" \
            | head -300 || true
    else
        echo "GRUB configuration is not visible."
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
        echo "EFI fallback binary:"
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

    echo "Selected ${scope_label} EFI System Partition:"
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
    if command -v efibootmgr >/dev/null 2>&1 && [[ -n "$EFI_ESP_SOURCE" || -n "$efi_root" ]]; then
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
    else
        echo "efibootmgr/UEFI variables are unavailable in the recovery host."
    fi
}

diagnostic_display()
{
    local display_link pkg service unit status configured_manager="none" scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    local -a services=(sddm.service gdm3.service lightdm.service greetd.service ly.service)

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
    else
        echo "dpkg-query is not available in the ${scope_label}."
    fi

    echo
    echo "Recent ${scope_label} display-manager boot evidence (${scope_label} journal, latest boot):"
    if command -v journalctl >/dev/null 2>&1 && [[ -d "$TARGET_ROOT/var/log/journal" ]]; then
        # Keep the complete boot stream through the shutdown filter so the
        # systemd-logind reboot marker remains visible.  A unit-scoped
        # journalctl query omits that marker and would misclassify orderly
        # Display-manager teardown (SIGTERM/auth-helper exit) as a boot failure.
        journalctl --root="$TARGET_ROOT" -b 0 --no-pager -n 1200 2>&1 \
            | journal_current_boot_actionable \
            | grep -Ei 'sddm|gdm|lightdm|greetd|(^|[^[:alnum:]])ly([^[:alnum:]]|$)' \
            | grep -iE 'warning|error|failed|failure|crash|signal|timeout|unable|denied|auth' \
            | grep -Eiv 'gkr-pam: unable to locate daemon control file|pam_kwallet5: open_session called without kwallet5_key' \
            | tail -120 || true
    else
        echo "No persistent ${scope_label} journal is available."
    fi

    echo
    echo "Recent ${scope_label} graphics/display errors (${scope_label} journal, latest boot):"
    if command -v journalctl >/dev/null 2>&1 && [[ -d "$TARGET_ROOT/var/log/journal" ]]; then
        # Preserve the full stream until after shutdown filtering; priority
        # queries do not include the reboot marker used by that filter.
        journalctl --root="$TARGET_ROOT" -b 0 --no-pager -n 1600 2>/dev/null \
            | journal_current_boot_actionable \
            | grep -Ei 'sddm|gdm|lightdm|greetd|(^|[^[:alnum:]])ly([^[:alnum:]]|$)|plasma|kwin|nvidia|NVRM|nouveau|drm|gpu|display' \
            | grep -iE 'warning|error|failed|failure|crash|signal|timeout|unable|denied|auth' \
            | tail -160 || true
    else
        echo "No persistent ${scope_label} journal is available."
    fi
}

diagnostic_errors()
{
    local scope_label="target"
    (( RUNNING_HOST_MODE == 1 )) && scope_label="running host"
    if ! command -v journalctl >/dev/null 2>&1; then
        echo "journalctl is not installed in the recovery host."
        return 0
    fi
    if [[ ! -d "$TARGET_ROOT/var/log/journal" ]]; then
        echo "${scope_label^} has no persistent /var/log/journal directory."
        return 0
    fi
    echo "Recent ${scope_label} error-priority journal entries:"
    echo "Journal scope: latest ${scope_label} boot (-b 0), excluding intentional shutdown teardown."
    journalctl --root="$TARGET_ROOT" -b 0 -p err -n 200 --no-pager 2>&1 \
        | journal_current_boot_actionable || true
    echo
    echo "Recent failure-related ${scope_label} journal lines:"
    journalctl --root="$TARGET_ROOT" -b 0 --no-pager -n 500 2>/dev/null \
        | journal_current_boot_actionable \
        | grep -iE 'failed|failure|dependency failed|timed out' \
        | tail -100 || true
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
    local requested="${1:-}" key overall=0
    [[ -n "$requested" ]] || fail "diagnose requires a diagnostic name or 'all'."
    (($# == 1)) || fail "diagnose accepts exactly one diagnostic name or 'all'."

    prepare_target ro
    mount_target_boot_entry "/boot" ro
    mount_target_boot_entry "/boot/efi" ro
    mount_target_boot_entry "/efi" ro

    if [[ "$requested" == "all" || "$requested" == "report" ]]; then
        for key in environment backend boot boot-evidence kernel grub uki display errors usage fstab btrfs mapper luks; do
            run_one_target_diagnostic "$key" || overall=1
        done
    else
        run_one_target_diagnostic "$requested" || overall=1
    fi

    # Diagnostics are informational. Individual audit failures are retained in
    # the output but do not turn a successfully completed read-only inspection
    # into a helper transport failure.
    return 0
}

run_host_diagnostic()
{
    local requested="${1:-}" key overall=0
    [[ -n "$requested" ]] || fail "host-diagnose requires a diagnostic name or 'all'."
    (($# == 1)) || fail "host-diagnose accepts exactly one diagnostic name or 'all'."

    CURRENT_STAGE="host read-only diagnostic"
    RUNNING_HOST_MODE=1
    prepare_running_host "$TARGET_DISK" "$ROOT_DEVICE" no no
    DIAGNOSTIC_SCOPE="Running Host"
    if [[ "$requested" == all || "$requested" == report ]]; then
        for key in environment backend boot boot-evidence kernel grub uki display errors usage fstab btrfs mapper luks; do
            run_one_diagnostic "$key" "$DIAGNOSTIC_SCOPE" || overall=1
        done
    else
        run_one_diagnostic "$requested" "$DIAGNOSTIC_SCOPE" || overall=1
    fi
    DIAGNOSTIC_SCOPE="Repair Target"
    return 0
}

config_path_for_key()
{
    case "${1:-}" in
        fstab)          printf '%s\n' '/etc/fstab' ;;
        crypttab)       printf '%s\n' '/etc/crypttab' ;;
        grub-defaults)  printf '%s\n' '/etc/default/grub' ;;
        grub-config)    printf '%s\n' '/boot/grub/grub.cfg' ;;
        sddm)           printf '%s\n' '/etc/sddm.conf' ;;
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
    root_real="$(realpath -e -- "$TARGET_ROOT")" || fail "Unable to resolve mounted target root."
    parent_real="$(realpath -e -- "$(dirname -- "$candidate")")" || fail "Unable to resolve configuration parent: $virtual_path"
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

list_snapshots()
{
    local id snap info created type desc status ro_prop rel
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
        if [[ -f "$snap/etc/os-release" ]]; then
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
        if [[ -f "$root/boot/initrd.img-$version" ]]; then
            echo "  PASS: $version has matching initramfs."
        else
            echo "  FAIL: $version has no matching initramfs."
            failures=$((failures + 1))
        fi
    done
    shopt -u nullglob
    ((failures == 0))
}

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

snapshot_rollback_plan()
{
    local requested="$1" snap current info pretty created root_id default_line default_id rel
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
    default_id="$(sed -n 's/^ID[[:space:]]\+\([0-9][0-9]*\).*/\1/p' <<< "$default_line" | head -1)"
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
    mount_special rbind-ro /dev "$TARGET_ROOT/dev"
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
    adaptive_grub_repair

    [[ -s "$TARGET_ROOT/boot/grub/grub.cfg" ]] || fail "GRUB regeneration completed but /boot/grub/grub.cfg is missing or empty."
    current="$(current_btrfs_subvol 2>/dev/null || true)"
    [[ "$current" == "@" ]] || fail "Post-rollback mount verification resolved '${current:-unknown}' instead of @."
    log "PASS: promoted rollback root and boot stack validated." | tee -a "$SESSION_LOG"
}

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

rollback_snapshot()
{
    local requested="$1" snap current stamp candidate_name backup_name failed_name candidate_path
    local old_default_line old_default_id candidate_id free_kb rc output pretty

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

run_snapshots()
{
    local action="${1:-}" requested="${2:-}"
    need base64
    need tr
    need find
    need sed
    need stat
    need btrfs
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
    mount_snapshot_top

    case "$action" in
        list) list_snapshots ;;
        inspect) inspect_snapshot "$requested" ;;
        plan) snapshot_rollback_plan "$requested" ;;
        rollback) rollback_snapshot "$requested" ;;
    esac
}

run_chroot_shell()
{
    local command="${1:-}"
    [[ $# -eq 1 ]] || fail "shell requires exactly one command string."
    [[ -n "$command" ]] || fail "shell command cannot be empty."
    need chroot
    need timeout
    prepare_target rw
    log "BEGIN: Chroot shell command" | tee -a "$SESSION_LOG"
    log "Command: $command" | tee -a "$SESSION_LOG"
    # A command is deliberately run in a clean target environment.  The
    # timeout prevents an accidental foreground service from blocking the
    # broker indefinitely while still allowing ordinary repair commands.
    set +e
    timeout --foreground 300 chroot "$TARGET_ROOT" /usr/bin/env \
        HOME=/root \
        PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin \
        DEBIAN_FRONTEND=noninteractive \
        APT_LISTCHANGES_FRONTEND=none \
        /bin/sh -c "$command" 2>&1 | tee -a "$SESSION_LOG"
    local rc=${PIPESTATUS[0]}
    set -e
    log "Chroot shell exit code: $rc" | tee -a "$SESSION_LOG"
    (( rc == 0 )) || fail "Chroot shell command failed (exit code $rc)."
}

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
}

validate_running_host()
{
    CURRENT_STAGE="host validation"
    RUNNING_HOST_MODE=1
    prepare_running_host "$TARGET_DISK" "$ROOT_DEVICE" no no
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
}

validate_stage()
{
    case "$1" in
        dpkg-configure|fix-broken|apt-update|apt-upgrade|dkms|display-manager|initramfs|efi|boot-stack|grub) return 0 ;;
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
        *) return 1 ;;
    esac
}


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

validate_target_esp()
{
    EFI_ESP_SOURCE=""
    EFI_ESP_FSTYPE=""

    [[ -d "$TARGET_ROOT/boot/efi" ]] || fail "Target /boot/efi directory is not available."
    mountpoint -q "$TARGET_ROOT/boot/efi" || fail "Target EFI System Partition is not mounted at /boot/efi."

    # A native systemd automount may report autofs before its FAT mount.
    # Select the block-backed mount instead of accepting the synthetic row.
    read -r EFI_ESP_SOURCE EFI_ESP_FSTYPE < <(
        findmnt -rn -o SOURCE,FSTYPE --target "$TARGET_ROOT/boot/efi" 2>/dev/null \
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

validate_selected_esp()
{
    local mount_path source fstype
    if [[ "$TARGET_DISTRO_FAMILY" != arch && "$TARGET_ESP_MOUNT" == "/boot/efi" ]]; then
        validate_target_esp
        return 0
    fi
    profile_target_backends
    mount_path="${TARGET_ESP_MOUNT:-}"
    [[ "$mount_path" == /boot || "$mount_path" == /boot/efi || "$mount_path" == /efi ]] \
        || fail "Unable to derive a supported EFI System Partition mount for this target."
    [[ -d "$TARGET_ROOT$mount_path" ]] || fail "Target EFI mount directory is not available: $mount_path"
    mountpoint -q "$TARGET_ROOT$mount_path" \
        || fail "Target EFI System Partition is not mounted at $mount_path."
    read -r source fstype < <(
        findmnt -rn -o SOURCE,FSTYPE --target "$TARGET_ROOT$mount_path" 2>/dev/null \
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
    local esp_mount source fstype
    EFI_ESP_SOURCE=""
    EFI_ESP_FSTYPE=""
    TARGET_ESP_MOUNT=""

    for esp_mount in /boot/efi /efi /boot; do
        [[ -d "$TARGET_ROOT$esp_mount" ]] || continue
        mountpoint -q "$TARGET_ROOT$esp_mount" 2>/dev/null || continue
        read -r source fstype < <(
            findmnt -rn -o SOURCE,FSTYPE --target "$TARGET_ROOT$esp_mount" 2>/dev/null \
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

is_tuxedo_uki_layout()
{
    [[ "$TARGET_OS_ID" == "tuxedo" ]] \
        && [[ -x "$TARGET_ROOT/usr/sbin/create_boot_uki_base.sh" ]]
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

efi_entry_id_for_target_label()
{
    local label="$1" partuuid
    command -v efibootmgr >/dev/null 2>&1 || return 1
    partuuid="$(blkid -s PARTUUID -o value "$EFI_ESP_SOURCE" 2>/dev/null || true)"
    [[ -n "$partuuid" ]] || return 1

    efibootmgr -v 2>/dev/null \
        | grep -iF "$partuuid" \
        | grep -F "$label" \
        | { if [[ "$label" == "TUXEDO UKI" ]]; then grep -F 'TUX.EFI'; else cat; fi; } \
        | sed -nE 's/^Boot([0-9A-Fa-f]{4})\*?[[:space:]]+.*/\1/p' \
        | head -1
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
    # convention (for example WD_BLACK versus WD BLACK). Compare a compact,
    # case-insensitive form so a model already present in a vendor label is
    # not appended a second time.
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
    # (for example WD_BLACK SN8100 HS versus WD_BLACK SN8100 HS 4000GB). Treat
    # that stable model stem as present as well, while requiring at least two
    # meaningful words so a generic label cannot suppress annotation.
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
    efibootmgr --create --disk "$disk" --part "$partnum" \
        --label "$label" --loader "$loader" 2>&1 | tee -a "$SESSION_LOG" \
        || return 1
    current="$SESSION_DIR/efi-nvram-recreate.$partuuid"
    efibootmgr -v > "$current" 2>&1 || return 1
    id="$(efi_find_entry_by_key "$current" "$(efi_entry_key_line "$line")" || true)"
    [[ -n "$id" ]] || {
        log "ERROR: efibootmgr completed but restored entry '$label' could not be found by identity." | tee -a "$SESSION_LOG" >&2
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

efi_host_tuxedo_uki_entry_ids()
{
    efi_set_inventory_esp_ids
    [[ -n "$EFI_HOST_ESP_PARTUUID" ]] || return 1
    efi_uki_entry_ids_for_partuuid "$EFI_HOST_ESP_PARTUUID"
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
            [[ -n "$mapped" ]] || return 1
            grep -Eq "^Boot${mapped}\*?[[:space:]]" <<<"$current" || return 1
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
    target_real="$(realpath -e "$TARGET_ROOT" 2>/dev/null || true)"
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

reinstall_efi_bootloader()
{
    local nvram_mode efi_dir
    local nvram_pre="" nvram_map=""
    local -a install_args

    # Current encrypted TUXEDO Debian systems use a UKI primary boot path. Use
    # the vendor-supplied builder, but first run a read-mostly prerequisite
    # pass that can safely correct a missing initramfs before the vendor builder
    # is allowed to touch TUX.EFI or firmware state.
    if is_tuxedo_uki_layout; then
        log "TUXEDO UKI layout detected; simulating/prereflighting the vendor boot path before rebuild." | tee -a "$SESSION_LOG"
        preflight_tuxedo_uki
        rebuild_tuxedo_uki
        verify_tuxedo_uki_root_binding
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
}

restore_display_manager()
{
    local display_link default_link
    need systemctl

    detect_display_manager
    if [[ "$TARGET_DISTRO_FAMILY" != arch && ! -x "$TARGET_ROOT/usr/bin/dpkg-query" ]]; then
        fail "dpkg-query is unavailable in the target; cannot verify display-manager package state."
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

repair_boot_stack()
{
    log "SIMULATE/PREFLIGHT: complete boot-stack reconciliation" | tee -a "$SESSION_LOG"
    validate_mapper_crypttab

    # Each component performs its own trial/preflight, deterministic correction
    # pass and post-write verification. This keeps the compound operation from
    # hiding which layer required correction.
    adaptive_initramfs_repair
    if is_tuxedo_uki_layout; then
        preflight_tuxedo_uki
        rebuild_tuxedo_uki
        verify_tuxedo_uki_root_binding
    elif [[ "$TARGET_DISTRO_FAMILY" == arch && "$TARGET_BOOTLOADER_BACKEND" == grub ]]; then
        log "Arch GRUB layout detected; applying the guarded conventional EFI reinstall after initramfs preflight." | tee -a "$SESSION_LOG"
        reinstall_efi_bootloader
    else
        log "No TUXEDO UKI builder detected; preserving the distribution's existing EFI layout during boot-stack reconciliation." | tee -a "$SESSION_LOG"
    fi
    adaptive_grub_repair
    log "PASS: boot stack reconciliation completed after component simulations and verification." | tee -a "$SESSION_LOG"
}

efi_promote_entry_first()
{
    local wanted="${1^^}" current old_order id out_csv="" seen="," all_ids
    local -a ids=()

    current="$(efibootmgr -v 2>/dev/null || true)"
    grep -Eq "^Boot${wanted}\\*?[[:space:]]" <<<"$current" \
        || fail "Requested default EFI entry Boot$wanted is not present."
    old_order="$(sed -n 's/^BootOrder: //p' <<<"$current" | head -n1 || true)"

    out_csv="$wanted"
    seen+=",$wanted,"
    if [[ -n "$old_order" ]]; then
        IFS=',' read -ra ids <<< "$old_order"
        for id in "${ids[@]}"; do
            id="${id^^}"
            [[ "$id" =~ ^[0-9A-F]{4}$ ]] || continue
            grep -Eq "^Boot${id}\\*?[[:space:]]" <<<"$current" || continue
            [[ "$seen" == *",$id,"* ]] && continue
            seen+=",$id,"
            out_csv+=",$id"
        done
    fi

    # Firmware may expose valid entries that were not in BootOrder. Preserve
    # those entries by appending them in their current efibootmgr listing order.
    while IFS= read -r id; do
        id="${id^^}"
        [[ "$id" =~ ^[0-9A-F]{4}$ ]] || continue
        [[ "$seen" == *",$id,"* ]] && continue
        seen+=",$id,"
        out_csv+=",$id"
    done < <(sed -nE 's/^Boot([0-9A-Fa-f]{4})\\*?[[:space:]].*/\1/p' <<<"$current" | tr '[:lower:]' '[:upper:]' | sort -u)

    log "Making Boot$wanted the explicit default while preserving all other EFI entries: $out_csv" | tee -a "$SESSION_LOG"
    efibootmgr -o "$out_csv" 2>&1 | tee -a "$SESSION_LOG" || return 1
    current="$(efibootmgr -v 2>/dev/null || true)"
    [[ "$(sed -n 's/^BootOrder: //p' <<<"$current" | head -n1 || true)" == "$out_csv" ]] \
        || fail "Firmware did not retain the requested default EFI entry Boot$wanted."
    log "PASS: Boot$wanted is first in BootOrder; all other entries were retained." | tee -a "$SESSION_LOG"
}

run_host_repair()
{
    local raw_disk="$1" raw_root="$2" stage previous_rank=0 current_rank
    local package_stage=false efi_requested=false grub_requested=false boot_stack_requested=false
    shift 2
    (($# > 0)) || fail "host-repair requires at least one repair stage."

    for stage in "$@"; do
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
    prepare_running_host "$raw_disk" "$raw_root" no yes
    profile_target_backends
    if ! is_debian_family && ! is_arch_family; then
        fail "Native host repairs require a supported Debian/Ubuntu or Arch backend. Detected: $TARGET_PRETTY"
    fi
    if is_arch_family; then
        for stage in "$@"; do
            case "$stage" in
                dpkg-configure)
                    fail "dpkg configuration is not available on Arch; use the Arch package transaction stages instead." ;;
                apt-update)
                    fail "Standalone APT metadata refresh is not available on Arch; use Upgrade installed packages for one full pacman transaction." ;;
            esac
        done
    fi
    if [[ "$package_stage" == true ]]; then
        host_package_manager_gate
    fi
    if [[ "$efi_requested" == true ]]; then
        if is_tuxedo_uki_layout; then
            validate_tuxedo_uki_target
            log "Host TUXEDO UKI read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE)" | tee -a "$SESSION_LOG"
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

    for stage in "$@"; do
        CURRENT_STAGE="$stage"
        case "$stage" in
            dpkg-configure) run_chroot "Complete interrupted package configuration" dpkg --configure -a ;;
            fix-broken)
                if is_arch_family; then adaptive_arch_pacman_repair "Repair Arch package dependencies"; else adaptive_fix_broken; fi ;;
            apt-update)
                run_apt_update ;;
            apt-upgrade)
                if is_arch_family; then adaptive_arch_pacman_repair "Upgrade installed Arch packages"; else adaptive_apt_upgrade; fi ;;
            dkms) adaptive_dkms_repair ;;
            display-manager) adaptive_display_manager_repair ;;
            initramfs) adaptive_initramfs_repair ;;
            efi) reinstall_efi_bootloader ;;
            grub) adaptive_grub_repair ;;
            boot-stack) repair_boot_stack ;;
        esac
    done

    # Keep the same EFI follow-up behavior as target repair: a standalone EFI
    # repair regenerates the menu after the loader files have been updated.
    if [[ "$efi_requested" == true && "$grub_requested" != true && "$boot_stack_requested" != true ]]; then
        CURRENT_STAGE="grub (EFI follow-up)"
        log "EFI repair completed; regenerating the running host GRUB fallback configuration." | tee -a "$SESSION_LOG"
        adaptive_grub_repair
    fi

    sync
    log "All requested running-host repair stages completed successfully." | tee -a "$SESSION_LOG"
}

run_host_default()
{
    local raw_disk="$1" raw_root="$2" pre current partuuid role
    local -a ids=()

    CURRENT_STAGE="host default EFI entry"
    RUNNING_HOST_MODE=1
    prepare_running_host "$raw_disk" "$raw_root" no yes
    profile_target_backends
    if ! is_debian_family && ! is_arch_family; then
        fail "Native host default selection requires a supported Debian/Ubuntu or Arch backend. Detected: $TARGET_PRETTY"
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
}

run_repair()
{
    local efi_requested=false grub_requested=false boot_stack_requested=false previous_rank=0 current_rank stage

    (($# > 0)) || fail "No repair stages were requested."
    for stage in "$@"; do
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
    # filesystem read-write can replay a journal, so family support is decided
    # before promoting the mount to read-write.
    prepare_target ro
    profile_target_backends
    if ! is_debian_family && ! is_arch_family; then
        fail "Modifying repairs require a supported Debian/Ubuntu or Arch backend. Detected: $TARGET_PRETTY"
    fi
    if is_arch_family; then
        for stage in "$@"; do
            case "$stage" in
                dpkg-configure)
                    fail "dpkg configuration is not available on Arch; use the Arch package transaction stages instead." ;;
                apt-update)
                    fail "Standalone APT metadata refresh is not available on Arch; use Upgrade installed packages for one full pacman transaction." ;;
            esac
        done
    fi
    # Resolve and inspect separate boot filesystems while they are still
    # read-only. promote_target_rw() will remount only these recorded target
    # mounts after every selected-disk check has succeeded.
    mount_target_boot_entry "/boot" ro
    mount_target_boot_entry "/boot/efi" ro
    mount_target_boot_entry "/efi" ro
    if [[ "$efi_requested" == true ]]; then
        if is_tuxedo_uki_layout; then
            validate_tuxedo_uki_target
            log "TUXEDO UKI repair read-only preflight: PASS ($EFI_ESP_SOURCE, $EFI_ESP_FSTYPE)" | tee -a "$SESSION_LOG"
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

    for stage in "$@"; do
        CURRENT_STAGE="$stage"
        case "$stage" in
            dpkg-configure)
                run_chroot "Complete interrupted package configuration" dpkg --configure -a
                ;;
            fix-broken)
                if is_arch_family; then adaptive_arch_pacman_repair "Repair Arch package dependencies"; else adaptive_fix_broken; fi
                ;;
            apt-update)
                run_apt_update
                ;;
            apt-upgrade)
                if is_arch_family; then adaptive_arch_pacman_repair "Upgrade installed Arch packages"; else adaptive_apt_upgrade; fi
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
                adaptive_grub_repair
                ;;
        esac
    done

    # An individual EFI reinstall should leave a freshly generated GRUB menu.
    # When the caller already requested the GRUB stage, avoid running it twice.
    if [[ "$efi_requested" == true && "$grub_requested" != true && "$boot_stack_requested" != true ]]; then
        CURRENT_STAGE="grub (EFI follow-up)"
        log "EFI repair completed; simulating and regenerating the GRUB fallback configuration." | tee -a "$SESSION_LOG"
        adaptive_grub_repair
    fi

    sync
    log "All requested repair stages completed successfully." | tee -a "$SESSION_LOG"
}

session_protocol_error()
{
    local request_id="$1"
    shift
    printf 'SESSION_ERROR\t%s\t%s\n' "$request_id" "$*"
    printf 'DONE\t%s\t2\n' "$request_id"
}

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
            unlock|validate|diagnose|config-read|config-write|snapshots|repair|shell|browse-target|copy-preview|copy) ;;
            host-validate|host-diagnose|host-repair|host-default) ;;
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
        host-repair)
            run_host_repair "$TARGET_DISK" "$ROOT_DEVICE" "$@"
            ;;
        host-default)
            [[ $# -eq 0 ]] || fail "host-default does not accept extra arguments."
            run_host_default "$TARGET_DISK" "$ROOT_DEVICE"
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
