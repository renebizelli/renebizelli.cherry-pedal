#!/usr/bin/env bash
# Applies an update package built by scripts/package-release.sh. Run this
# from inside the package folder (e.g. after copying it from a USB drive),
# on the Raspberry Pi itself.
#
# Usage: ./install-update.sh [target-dir]
#   target-dir defaults to ~/renebizelli.cherry-pedal

set -euo pipefail

PACKAGE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TARGET_DIR="${1:-$HOME/renebizelli.cherry-pedal}"

for required in build/cherry_pedal bands assets source.json; do
    if [ ! -e "$PACKAGE_DIR/$required" ]; then
        echo "Error: '$required' not found in $PACKAGE_DIR — is this a valid update package?" >&2
        exit 1
    fi
done

echo "Installing update into $TARGET_DIR ..."
mkdir -p "$TARGET_DIR/build"
cp "$PACKAGE_DIR/build/cherry_pedal" "$TARGET_DIR/build/cherry_pedal"
chmod +x "$TARGET_DIR/build/cherry_pedal"
rsync -a --delete "$PACKAGE_DIR/bands/" "$TARGET_DIR/bands/"
rsync -a --delete "$PACKAGE_DIR/assets/" "$TARGET_DIR/assets/"
cp "$PACKAGE_DIR/source.json" "$TARGET_DIR/source.json"

echo "Done."

if pgrep -x cherry_pedal > /dev/null; then
    echo "cherry_pedal is currently running — stopping it so the new build takes effect."
    killall cherry_pedal
    echo "Not restarted automatically: relaunch it from the desktop icon, or it will"
    echo "start on its own the next time the Pi is powered on (autostart)."
fi
