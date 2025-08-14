#!/usr/bin/env bash
set -euo pipefail

ROOT="/home/dayzserver/dayz"
BIN="$ROOT/DayZServer"
CFG="$ROOT/serverDZ.cfg"
PROFILES="$ROOT/profiles"
MISSION="./mpmissions/dayzOffline.sakhal"
BE_DIR="$ROOT/battleye"

export LD_LIBRARY_PATH="$BE_DIR:${LD_LIBRARY_PATH:-}"

resolve_be_field() {
  local field="$1"
  local active
  active=$(ls -1t "$BE_DIR"/beserver_x64_active_*.cfg 2>/dev/null | head -1 || true)
  if [ -n "${active:-}" ] && [ -r "$active" ]; then
    sed -n "s/^[[:space:]]*$field[[:space:]]\\+//p" "$active" | head -1
    return 0
  fi
  if [ -r "$BE_DIR/beserver_x64.cfg" ]; then
    sed -n "s/^[[:space:]]*$field[[:space:]]\\+//p" "$BE_DIR/beserver_x64.cfg" | head -1
    return 0
  fi
  return 1
}

RCON_PASS="${RCON_PASS:-$(resolve_be_field RConPassword || true)}"
RCON_PORT="${RCON_PORT:-$(resolve_be_field RConPort || echo 2306)}"

# Final fallback for password: .rcon_pass if present
if [ -z "${RCON_PASS:-}" ] && [ -r "$BE_DIR/.rcon_pass" ]; then
  RCON_PASS="$(cat "$BE_DIR/.rcon_pass")"
fi
: "${RCON_PASS:=testpass123}"

start_server() {
  tmux kill-session -t dayz 2>/dev/null || true
  cd "$ROOT"
  tmux new-session -d -s dayz -- \
    "$BIN" \
      -config="$CFG" \
      -port=2302 \
      -dologs -adminlog -netlog -scrAllowFileWrite \
      -profiles="$PROFILES" \
      -mission="$MISSION"
}

stop_server_soft() {
  if command -v bercon-cli >/dev/null 2>&1; then
    timeout 3s bercon-cli -i 127.0.0.1 -p "$RCON_PORT" -P "$RCON_PASS" -- "say -1 Server restarting in 15s..." || true
    sleep 12
    timeout 3s bercon-cli -i 127.0.0.1 -p "$RCON_PORT" -P "$RCON_PASS" -- "#shutdown" || true
    sleep 3
  fi
  tmux kill-session -t dayz 2>/dev/null || true
}

wait_for_be() {
  for _ in $(seq 1 30); do
    ss -lunp 2>/dev/null | grep -q ":$RCON_PORT.*enfMain" && { echo "[loop] BE on UDP $RCON_PORT"; return 0; }
    sleep 1
  done
  echo "[loop] WARN: BE not bound on UDP $RCON_PORT after 30s"
  return 1
}

health_check() {
  command -v bercon-cli >/dev/null 2>&1 || return 0
  timeout 3s bercon-cli -i 127.0.0.1 -p "$RCON_PORT" -P "$RCON_PASS" -- "players" >/dev/null
}

trap 'echo "[loop] signal -> stop"; stop_server_soft; exit 0' INT TERM

start_epoch=$(date +%s)
start_server
wait_for_be || true

while true; do
  if ! health_check; then
    echo "[loop] RCON heartbeat failed → restart"
    stop_server_soft
    start_server
    wait_for_be || true
    start_epoch=$(date +%s)
  fi
  now=$(date +%s)
  if (( now - start_epoch > 7*3600 )); then
    echo "[loop] 7h rolling restart"
    stop_server_soft
    start_server
    wait_for_be || true
    start_epoch=$(date +%s)
  fi
  sleep 600
done
