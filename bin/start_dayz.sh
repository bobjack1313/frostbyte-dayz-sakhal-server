#!/usr/bin/env bash
set -euo pipefail

ROOT="/home/dayzserver/dayz"
LOG="$ROOT/profiles/console.log"

cd "$ROOT"
tmux kill-session -t dayz 2>/dev/null || true

exec tmux new-session -d -s dayz -- bash -lc '
  export LD_LIBRARY_PATH="/home/dayzserver/dayz/battleye:${LD_LIBRARY_PATH:-}";
  ./DayZServer \
    -config=serverDZ.cfg \
    -port=2302 \
    -dologs -adminlog -netlog -scrAllowFileWrite \
    -profiles=/home/dayzserver/dayz/profiles \
    -mission=./mpmissions/dayzOffline.sakhal \
    -BEpath=/home/dayzserver/dayz/battleye |& tee -a /home/dayzserver/dayz/profiles/console.log'
