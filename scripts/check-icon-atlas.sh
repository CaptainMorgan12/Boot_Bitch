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
    *lock*|*encrypt*|*password*|*unlocked*) echo lock ;;
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
requested+=(applications-development)
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
