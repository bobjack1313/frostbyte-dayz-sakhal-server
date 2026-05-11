#!/usr/bin/env bash
set -euo pipefail

BE=/home/dayzserver/dayz/battleye
echo "[BE cfg]"

ACTIVE="$(ls -1t "$BE"/beserver_x64_active_*.cfg 2>/dev/null | head -1 || true)"

if [[ -n "${ACTIVE}" && -f "${ACTIVE}" ]]; then
  echo "Active: ${ACTIVE}"
  sed -n '1,40p' "${ACTIVE}" || true
elif [[ -f "$BE/beserver_x64.cfg" ]]; then
  echo "Base:   $BE/beserver_x64.cfg"
  sed -n '1,40p' "$BE/beserver_x64.cfg" || true
else
  echo "WARN: no beserver_x64*.cfg found (RCON will be disabled until created)"
fi
