#!/usr/bin/env bash
set -euo pipefail

ROOT="/home/dayzserver/dayz"
BIN="$ROOT/DayZServer"
PROFILES="$ROOT/profiles"
CFG="$ROOT/serverDZ.cfg"
MPMISS="$ROOT/mpmissions/dayzOffline.sakhal"
BE_DIR="$ROOT/battleye"
PORT=2302
HOST="0.0.0.0"
CPUCOUNT=$(nproc)

log(){ echo "dayz-restart: $*"; }

is_dayz_listening() {
  ss -lunp | grep -qE ':(2302|2304|2305)\b'
}

announce_if_running() {
  local label="$1"
  local message="$2"

  if is_dayz_listening; then
    log "announce: $label"
    "$ROOT/bin/rcon.sh" "say -1 $message" || true
  else
    log "announce: $label skipped, server not listening"
  fi
}

SERVER_WAS_RUNNING=false

if is_dayz_listening; then
  SERVER_WAS_RUNNING=true
  log "server is running; using graceful restart countdown"
else
  log "server is not running; skipping announcements and countdown"
fi

# best-effort pre-announce
if [[ "$SERVER_WAS_RUNNING" == true ]]; then
  announce_if_running "15m" "*** Server restarting in 15 minutes ***"
  sleep 300

  announce_if_running "10m" "*** Server restarting in 10 minutes ***"
  sleep 300

  announce_if_running "5m" "*** Server restarting in 5 minutes ***"
  sleep 240

  announce_if_running "1m" "*** Server restarting in 1 minute ***"
  sleep 30

  announce_if_running "30s" "*** Server restarting in 30 seconds ***"
  sleep 20

  announce_if_running "10s" "*** Restarting in 10 seconds ***"
  sleep 10
fi

log "stop: calling stop_dayz.sh"
"$ROOT/bin/stop_dayz.sh" || true

# UPDATE SEASONAL SETTINGS BEFORE RESTART
log "seasonal: updating day/night cycle"
"$ROOT/bin/seasonal_update.sh" || true

sleep 3

# UPDATE ROTATING SERVER MESSAGES BEFORE RESTART
log "messages: shuffling rotating messages"
"$ROOT/bin/shuffle_messages.sh" || true

sleep 3

# start fresh
log "start: invoking start_dayz.sh"
"$ROOT/bin/start_dayz.sh"

# wait for listeners (game + BE)
log "wait: listeners"
for i in {1..60}; do
  ss -lunp | grep -E ':(2302|2304|2305|2306)\b' || true

  game=$(ss -lunp | grep -c ':2302') || true
  steam=$(ss -lunp | grep -c ':2304') || true
  query=$(ss -lunp | grep -c ':2305') || true

  if [[ $game -gt 0 && $steam -gt 0 && $query -gt 0 ]]; then
    log "listeners ready: 2302/2304/2305"
    break
  fi

  sleep 1
done

if ! is_dayz_listening; then
  log "failure: listeners did not come up"
  "$ROOT/bin/notify_failure.sh" "DayZ restart failed: ports 2302/2304/2305 did not come up."
  exit 1
fi

log "wait: settle"
sleep 15   # give BE/RCON time to actually accept logins

# prove RCON is alive (non-fatal if it isn’t yet)
TOKEN="RESTART_OK_$(date +%H%M%S)"
"$ROOT/bin/rcon.sh" "say -1 Server back up. Token: $TOKEN" || true

log "done"

