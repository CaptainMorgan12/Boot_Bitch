#!/usr/bin/env bash
set -euo pipefail

# Static contract for the icon set compiled into the binary.  Every literal
# icon requested by MainWindow must resolve to one of the bundled semantic
# SVGs when a desktop theme is absent or returns a placeholder.
root_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
source_file="$root_dir/src/MainWindow.cpp"
atlas_dir="$root_dir/resources/icons/atlas"

map_icon() {
  local name="$1"
  case "$name" in
    kernel) echo kernel ;;
    initramfs|mkinitcpio) echo initramfs ;;
    grub) echo grub ;;
    distribution|distro) echo distribution ;;
    task-complete|dialog-ok-apply) echo check ;;
    system-run|tools-wizard) echo run ;;
    view-refresh) echo refresh ;;
    document-edit|edit-clear) echo edit ;;
    document-save) echo save ;;
    document-open|folder-open) echo open ;;
    document-open-recent) echo snapshot ;;
    list-remove|application-exit) echo trash ;;
    security-high) echo security ;;
    dialog-information|help-*) echo info ;;
    dialog-password) echo question ;;
    system-software-install) echo install ;;
    system-software-update) echo update ;;
    *drive*|computer) echo drive ;;
    filesystem-*|*btrfs*|*ext4*) echo filesystem ;;
    *configure*|*settings*|*preferences*|*repair*|*wizard*) echo gear ;;
    *terminal*) echo terminal ;;
    *lock*|*encrypt*|*password*) echo lock ;;
    *warning*|*error*|*report-bug*) echo warning ;;
    *display*|*video*) echo display ;;
    applications-development|*dkms*) echo development ;;
    *copy*) echo copy ;;
    folder-*|text-x-log|*log) echo folder ;;
    *snapshot*|*revert*) echo snapshot ;;
    *help*|*information*) echo info ;;
    *) echo document ;;
  esac
}

mapfile -t requested < <(grep -oE 'themedIcon\(QStringLiteral\("[^"]+"' "$source_file" \
  | sed -E 's/.*QStringLiteral\("([^"]+)"/\1/' | sort -u)
# These icons are selected from data tables and therefore do not appear as
# literal themedIcon() calls in the source scan above.
requested+=(applications-development kernel initramfs grub distribution)
mapfile -t requested < <(printf '%s\n' "${requested[@]}" | sort -u)
failures=0
printf '%-30s %-12s %s\n' "Requested icon" "Bundled atlas" "Status"
printf '%-30s %-12s %s\n' "--------------" "-------------" "------"
for icon in "${requested[@]}"; do
  atlas="$(map_icon "$icon")"
  if [[ -f "$atlas_dir/$atlas.svg" ]]; then
    printf '%-30s %-12s PASS\n' "$icon" "$atlas.svg"
  else
    printf '%-30s %-12s FAIL\n' "$icon" "$atlas.svg"
    failures=$((failures + 1))
  fi
done

if (( failures )); then
  echo "Icon atlas audit failed: $failures icon request(s) have no bundled semantic asset." >&2
  exit 1
fi
echo "Icon atlas audit passed: ${#requested[@]} icon request(s) have a bundled asset."

# The atlas assets are SVG, so the runtime needs Qt's SVG image-format plugin
# (QIcon reads them through QImageReader). Every package profile must declare
# the plugin package, otherwise the atlas silently degrades to generic style
# icons. Debian moved the plugin out of libqt6svg6 into qt6-svg-plugins in
# qt6-svg 6.7.2-5 (trixie and newer, TUXEDO OS); bookworm and Ubuntu 24.04
# still ship it inside libqt6svg6, hence the versioned alternative. Fedora's
# and Alpine's qt6-qtsvg, openSUSE's libQt6Svg6 and Arch's qt6-svg all include
# the plugin.
runtime_failures=0
check_runtime_dependency() {
  local profile="$1" file="$2" needle="$3"
  if [[ -f "$file" ]] && grep -qF -- "$needle" "$file"; then
    printf '%-12s %-4s %s\n' "$profile" "PASS" "$needle"
  else
    printf '%-12s %-4s %s\n' "$profile" "FAIL" "missing: $needle"
    runtime_failures=$((runtime_failures + 1))
  fi
}
printf '\n%-12s %-4s %s\n' "Profile" "State" "Qt SVG image-format plugin dependency"
printf '%-12s %-4s %s\n' "-------" "-----" "----------------------------------------"
check_runtime_dependency "deb" "$root_dir/CMakeLists.txt" "qt6-svg-plugins | libqt6svg6 (<< 6.7.2-5~)"
check_runtime_dependency "rpm" "$root_dir/scripts/package-rpm.sh" "qt6-qtsvg"
check_runtime_dependency "suse" "$root_dir/scripts/package-rpm.sh" "libQt6Svg6"
check_runtime_dependency "arch" "$root_dir/scripts/package-arch.sh" "'qt6-base' 'qt6-svg'"
check_runtime_dependency "apk" "$root_dir/scripts/package-alpine.sh" "qt6-qtsvg"

if (( runtime_failures )); then
  echo "Icon atlas runtime audit failed: $runtime_failures package profile(s) do not declare the Qt SVG image-format plugin." >&2
  exit 1
fi
echo "Icon atlas runtime audit passed: every package profile declares the Qt SVG image-format plugin."
