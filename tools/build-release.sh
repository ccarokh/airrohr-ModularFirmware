#!/usr/bin/env bash
# Builds the release binaries into dist/ and names them after the version in
# src/Board.h - so the file name always matches what the device reports on
# /status and over MQTT (device_firmware).
#
#   ./tools/build-release.sh
#
# dist/ is in .gitignore: the binaries do not belong in the repository, they
# are attached to a GitHub release.

set -euo pipefail

cd "$(dirname "$0")/.."

VERSION=$(sed -n 's/.*AIRROHR_VERSION "AR-modular-\(.*\)".*/\1/p' src/Board.h)
if [ -z "$VERSION" ]; then
	echo "Version not found in src/Board.h" >&2
	exit 1
fi

# env -> file name suffix
ENVS=(
	"esp32dev:esp32"
	"esp32_carrier:esp32-carrier"
	"esp8266test:esp8266-nodemcu"
)

PIO=${PIO:-"python3 -m platformio"}

mkdir -p dist
echo "airRohr $VERSION"

for entry in "${ENVS[@]}"; do
	env="${entry%%:*}"
	name="${entry##*:}"
	echo "--- $env"
	$PIO run -e "$env"
	cp ".pio/build/$env/firmware.bin" "dist/airrohr-$name-v$VERSION.bin"
done

echo
ls -l dist/*"v$VERSION"*.bin
echo
echo "Checksums:"
( cd dist && sha256sum ./*"v$VERSION"*.bin | tee "airrohr-v$VERSION.sha256" )
