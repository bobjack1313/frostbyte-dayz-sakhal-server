#!/usr/bin/env bash
set -euo pipefail

ROOT="/home/dayzserver/dayz"
PROFILES="$ROOT/profiles"
LOG="$PROFILES/console.log"
SESSION="dayz"

cd "$ROOT"

# Make sure the app id file exists so Steam login doesn’t act weird
[[ -f "$ROOT/steam_appid.txt" ]] || echo 221100 > "$ROOT/steam_appid.txt"

# Kill any old session
tmux kill-session -t "$SESSION" >/dev/null 2>&1 || true

# Launch using the ROOT binary (not bin/)
tmux new-session -d -s "$SESSION" bash -lc '
  set -euo pipefail
  export LD_LIBRARY_PATH="'"$ROOT"':'"$ROOT"'/bin:'"$ROOT"'/battleye:${LD_LIBRARY_PATH:-}"

  exec '"$ROOT"'/DayZServer \
    -config=serverDZ.cfg \
    -port=2302 \
    -steamPort=2304 \
    -steamQueryPort=2305 \
    -ip=0.0.0.0 \
    -profiles="'"$PROFILES"'" \
    -mission="./mpmissions/dayzOffline.sakhal" \
    -BEpath="'"$ROOT"'/battleye" \
    -dologs -adminlog -netlog -scrAllowFileWrite \
    |& tee -a "'"$LOG"'"
'

echo "Started in tmux: $SESSION  (tmux attach -t $SESSION)"
echo "Log: $LOG"

sleep 120

# Start radio watcher after DayZ server launch
BE_DIR="$ROOT/battleye"
CFG="$(ls -1t "$BE_DIR"/beserver_x64_active_*.cfg 2>/dev/null | head -1)"; [[ -z "$CFG" ]] && CFG="$BE_DIR/beserver_x64.cfg"
RCON_PASS="$(sed -n 's/^RConPassword[[:space:]]\+//p' "$CFG" | head -1)"
RCON_PORT="$(sed -n 's/^RConPort[[:space:]]\+//p' "$CFG" | head -1)"

tmux kill-session -t radio >/dev/null 2>&1 || true
sleep 5
tmux new -s radio -d "RCON_ADDR=127.0.0.1:${RCON_PORT} RCON_PASS='${RCON_PASS}' $ROOT/tools/radio_watcher.sh 2>&1 | tee -a $PROFILES/radio_watcher.out"
