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

# best-effort pre-announce
log "announce: 15m"; "$ROOT/bin/rcon.sh" 'say -1 *** Server restarting in 15 minutes ***' || true
sleep 300

log "announce: 10m"; "$ROOT/bin/rcon.sh" 'say -1 *** Server restarting in 10 minutes ***' || true
sleep 300

log "announce: 5m"; "$ROOT/bin/rcon.sh" 'say -1 *** Server restarting in 5 minutes ***' || true
sleep 240

log "announce: 1m"; "$ROOT/bin/rcon.sh" 'say -1 *** Server restarting in 1 minute ***' || true
sleep 30

log "announce: 30s"; "$ROOT/bin/rcon.sh" 'say -1 *** Server restarting in 30 seconds ***' || true
sleep 20

log "announce: 10s";  "$ROOT/bin/rcon.sh" 'say -1 *** Restarting in 10 seconds ***' || true
sleep 10

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

log "wait: settle"
sleep 15   # give BE/RCON time to actually accept logins

# prove RCON is alive (non-fatal if it isn’t yet)
TOKEN="RESTART_OK_$(date +%H%M%S)"
"$ROOT/bin/rcon.sh" "say -1 ✅ Server back up. Token: $TOKEN" || true

log "done"

