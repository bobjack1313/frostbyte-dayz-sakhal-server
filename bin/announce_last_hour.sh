#!/usr/bin/env bash
set -uo pipefail   # best-effort; don't die on one miss
RCON="/home/dayzserver/dayz/bin/rcon.sh"

# DRY=1: use small-but-real pauses so BE answers consistently
if [ "${DRY:-0}" = "1" ]; then
  _60m=5 _30m=5 _15m=5 _10m=5 _5m=5 _2m=5 _1m=5
else
  _60m=30m _30m=15m _15m=10m _10m=5m _5m=3m _2m=1m _1m=60s
fi

say() {
  # warm-up ping; ignore failure
  "$RCON" "players" >/dev/null 2>&1 || true
  # real message; retry handled inside rcon.sh; ignore failure here
  "$RCON" "say -1 $*" || true
}

say "Server restarting in 60 minutes..."; sleep "$_60m"
say "Server restarting in 30 minutes..."; sleep "$_30m"
say "Server restarting in 15 minutes..."; sleep "$_15m"
say "Server restarting in 10 minutes..."; sleep "$_10m"
say "Server restarting in 5 minutes...";  sleep "$_5m"
say "Server restarting in 2 minutes...";  sleep "$_2m"
say "Server restarting in 1 minute...";   sleep "$_1m"
