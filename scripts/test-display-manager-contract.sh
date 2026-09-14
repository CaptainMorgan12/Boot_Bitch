#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
HELPER="$ROOT_DIR/scripts/boot-repair-helper.sh"

[[ -x "$HELPER" ]] || { echo "FAIL: helper is not executable" >&2; exit 1; }
bash -n "$HELPER"

# The graphical-login stage must discover the configured manager and support
# common Debian-family managers without silently forcing SDDM.
grep -q '^detect_display_manager()' "$HELPER"
grep -q 'display_manager_package_for_service()' "$HELPER"
grep -q 'sddm.service).*sddm' "$HELPER"
grep -q 'gdm.service|gdm3.service).*gdm3' "$HELPER"
grep -q 'lightdm.service).*lightdm' "$HELPER"
grep -q 'greetd.service).*greetd' "$HELPER"
grep -q 'ly.service).*ly' "$HELPER"
grep -q 'Multiple display managers are installed' "$HELPER"
grep -q 'DISPLAY_MANAGER_SERVICE' "$HELPER"
grep -q 'enable "\$DISPLAY_MANAGER_SERVICE"' "$HELPER"
grep -q 'display-manager.service does not resolve to \$DISPLAY_MANAGER_LABEL' "$HELPER"

# Diagnostic evidence must include all supported manager names and graphics
# journal signals, while repair remains offline and never starts a GUI.
grep -q 'Installed display managers and units' "$HELPER"
grep -q 'Recent .*display-manager boot evidence' "$HELPER"
grep -q 'sddm|gdm|lightdm|greetd' "$HELPER"
grep -q 'will NOT be started inside the repair chroot' "$HELPER"
grep -q '^trial_display_manager_headless()' "$HELPER"
grep -q 'systemd-analyze --root=' "$HELPER"
grep -q 'headless systemd verification' "$HELPER"

echo "PASS: graphical login manager detection, diagnostics, and repair contract is wired."
