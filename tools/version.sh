#!/usr/bin/env bash
# Compute the rolling project version: YY.MM.NN
#   YY.MM : year.month of the current date (e.g. 26.09)
#   NN    : number of commits since the version tag vYY.MM (e.g. v26.09).
#           00 when the tag does not exist yet.
# Output: 26.09.00
set -euo pipefail

YYMM="$(date +%y.%m)"
TAG="v${YYMM}"

if command -v git >/dev/null 2>&1 && git rev-parse --git-dir >/dev/null 2>&1; then
    if git rev-parse --verify "refs/tags/${TAG}" >/dev/null 2>&1; then
        NN="$(git rev-list "${TAG}..HEAD" --count 2>/dev/null || echo 0)"
    else
        NN=0
    fi
else
    NN=0
fi

printf '%s.%02d\n' "${YYMM}" "${NN:-0}"
