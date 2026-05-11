#!/usr/bin/env bash
set -euo pipefail

ROOT="/home/dayzserver/dayz"
PROFILES="$ROOT/profiles"
LOG="$PROFILES/console.log"
BE_DIR="$ROOT/battleye"

CFG=""
for i in {1..30}; do
  CFG="$(ls -1t "$BE_DIR"/beserver_x64_active_*.cfg 2>/dev/null | head -1 || true)"
  [[ -n "$CFG" ]] && break
  sleep 2
done
[[ -z "$CFG" ]] && CFG="$BE_DIR/beserver_x64.cfg"

if [[ -f "$CFG" ]]; then
  RCON_PASS="$(sed -n 's/^RConPassword[[:space:]]\+//p' "$CFG" | head -1 || true)"
  RCON_PORT="$(sed -n 's/^RConPort[[:space:]]\+//p' "$CFG" | head -1 || true)"

  if [[ -n "${RCON_PASS:-}" && -n "${RCON_PORT:-}" ]]; then
    tmux kill-session -t radio >/dev/null 2>&1 || true
    sleep 2
    tmux new -s radio -d "RCON_ADDR=127.0.0.1:${RCON_PORT} RCON_PASS='${RCON_PASS}' $ROOT/tools/radio_watcher.sh 2>&1 | tee -a $PROFILES/radio_watcher.out"
    echo "radio watcher: started (port=$RCON_PORT)" | tee -a "$LOG"
  else
    echo "radio watcher: RCON_PASS or RCON_PORT missing in $CFG, skipping" | tee -a "$LOG"
  fi
else
  echo "radio watcher: no Battleye cfg found, skipping" | tee -a "$LOG"
fi
