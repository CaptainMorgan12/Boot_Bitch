#!/usr/bin/env bash
set -euo pipefail

# Report the diagnostic/tab icon names available in the local freedesktop
# themes and suggest a semantically equivalent name when one is missing.
roots=(/usr/share/icons /usr/local/share/icons "$HOME/.local/share/icons")
declare -A aliases=(
  [task-complete]="dialog-ok-apply emblem-ok"
  [system-run]="debug-run media-playback-start"
  [dialog-information]="emblem-information dialog-question"
  [preferences-system]="applications-system preferences-desktop"
  [drive-removable-media]="drive-harddisk drive-partition"
  [video-display]="computer-laptop video-display"
  [dialog-warning]="emblem-warning dialog-error"
  [drive-harddisk]="drive-harddisk-solidstate drive-multidisk"
  [text-x-generic]="document-properties document-open"
  [document-encrypt]="application-pgp-encrypted document-encrypted"
  [document-preview]="document-print-preview document-properties"
  [tools-report-bug]="dialog-warning applications-development"
  [tools-wizard]="applications-utilities configure"
  [document-revert]="document-open-recent view-refresh"
  [utilities-terminal]="terminal konsole"
  [edit-copy]="copy edit-copy-symbolic"
  [text-x-log]="folder-log document-properties"
  [settings-configure]="configure applications-system"
)

find_icon() {
  local name=$1 root path
  for root in "${roots[@]}"; do
    [[ -d $root ]] || continue
    path=$(find "$root" -type f \( -name "$name.svg" -o -name "$name.png" -o -name "$name.xpm" \) -print -quit 2>/dev/null || true)
    [[ -n $path ]] && { printf '%s\n' "$path"; return 0; }
  done
  return 1
}

icons=(task-complete system-run dialog-information preferences-system drive-removable-media video-display dialog-warning drive-harddisk text-x-generic document-encrypt document-preview tools-report-bug tools-wizard document-revert utilities-terminal edit-copy text-x-log settings-configure)
tab_icons=(drive-harddisk tools-report-bug tools-wizard document-revert utilities-terminal edit-copy text-x-log settings-configure)
printf '%-26s %-9s %s\n' "Requested icon" "Status" "Path or equivalent"
printf '%-26s %-9s %s\n' "--------------" "------" "------------------"
for icon in "${icons[@]}"; do
  if path=$(find_icon "$icon"); then
    printf '%-26s %-9s %s\n' "$icon" "present" "$path"
    continue
  fi
  replacement="none"
  for candidate in ${aliases[$icon]-}; do
    if path=$(find_icon "$candidate"); then
      replacement="$candidate ($path)"
      break
    fi
  done
  printf '%-26s %-9s %s\n' "$icon" "missing" "$replacement"
done

printf '\nTab icon names:\n'
for icon in "${tab_icons[@]}"; do
  if path=$(find_icon "$icon"); then
    printf '%-26s %-9s %s\n' "$icon" "present" "$path"
    continue
  fi
  replacement="none"
  for candidate in ${aliases[$icon]-}; do
    if path=$(find_icon "$candidate"); then
      replacement="$candidate ($path)"
      break
    fi
  done
  printf '%-26s %-9s %s\n' "$icon" "missing" "$replacement"
done
