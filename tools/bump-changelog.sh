#!/usr/bin/env bash
# Bring debian/changelog in line with the rolling version (YY.MM.NN).
# Idempotent: a no-op when the changelog already carries that version.
# Run this before `dpkg-buildpackage` so the produced .deb is stamped with
# the version computed at build time.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="$(bash "${ROOT}/tools/version.sh")"
CURRENT="$(head -n1 "${ROOT}/debian/changelog" | sed -nE 's/^[A-Za-z0-9._+-]+ \(([^)]*)\).*$/\1/p')"

if [ "${VERSION}" = "${CURRENT}" ]; then
    echo "Changelog already at ${VERSION}."
else
    dch --distribution noble --ignoretime -v "${VERSION}" \
        "Bumped to rolling version ${VERSION}."
fi
