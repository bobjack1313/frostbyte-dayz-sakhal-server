#!/usr/bin/env bash
set -u  # (not -e) we want to continue even if RCON fails

RCON="/home/dayzserver/dayz/bin/rcon.sh"

say() {
  local msg="$1"
  [ -x "$RCON" ] && timeout 4s "$RCON" "say -1 $msg" >/dev/null 2>&1 || true
}

# 1) Gentle: notify + BE shutdown
say "Server restarting in 20 seconds..."
sleep 15
[ -x "$RCON" ] && timeout 4s "$RCON" "#shutdown" >/dev/null 2>&1 || true
sleep 5

# 2) Kill tmux session if it exists (covers your managed instance)
tmux has-session -t dayz 2>/dev/null && tmux kill-session -t dayz || true

# 3) Safety net: if the real host is still up, TERM the actual process
#    (enfMain is the host on Linux; DayZServer may not exist)
pgrep -x enfMain >/dev/null && kill -TERM $(pgrep -x enfMain) || true

# 4) Hard stop if it ignored TERM after a bit
for i in {1..10}; do pgrep -x enfMain >/dev/null || break; sleep 1; done
pgrep -x enfMain >/dev/null && kill -KILL $(pgrep -x enfMain) || true

exit 0
