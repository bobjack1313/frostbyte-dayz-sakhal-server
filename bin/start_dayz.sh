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
