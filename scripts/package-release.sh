#!/usr/bin/env bash
# Cross-compiles cherry_pedal for the Pi's actual architecture (armhf) via
# Docker Buildx and bundles it with everything needed to run — but not
# build — on the Raspberry Pi: bands/, assets/, source.json. The result in
# dist/ can be copied to a USB drive or sent over the network (scp/rsync)
# and applied on the Pi with scripts/install-update.sh, without needing
# git or a C++ toolchain there.
#
# Usage: run from the repo root.
#   ./scripts/package-release.sh

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT"

DIST_DIR="dist/cherry-pedal-update"

echo "Cross-compiling cherry_pedal for linux/arm/v7 (armhf)..."
docker buildx build --platform linux/arm/v7 \
    -f docker/Dockerfile.armhf-build --target export -o docker/out-armhf .

echo "Assembling update package in $DIST_DIR ..."
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR/build"
cp docker/out-armhf/cherry_pedal "$DIST_DIR/build/cherry_pedal"
cp -r bands "$DIST_DIR/bands"
cp -r assets "$DIST_DIR/assets"
cp source.json "$DIST_DIR/source.json"
cp scripts/install-update.sh "$DIST_DIR/install-update.sh"
chmod +x "$DIST_DIR/install-update.sh"

echo "Done. Copy the '$DIST_DIR' folder to a USB drive (or scp it to the Pi),"
echo "then on the Pi run: ./install-update.sh from inside that folder."
