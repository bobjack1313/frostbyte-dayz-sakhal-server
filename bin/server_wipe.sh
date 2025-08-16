#!/usr/bin/env bash
set -euo pipefail

# --- Config you probably won't need to change ---
ROOT="${HOME}/dayz"
MISSION="${ROOT}/mpmissions/dayzOffline.sakhal"
STORE="${MISSION}/storage_1"   # persistence lives here on your setup

# Time-state files we want to wipe for a "time reset"
TIME_FILES=(
  "${ROOT}/seasonal.log"
  "${ROOT}/game_time.txt"
  "${ROOT}/latitude.txt"
)

# Player DB files live in the mission store on your setup
# Include SQLite sidecars just in case
PLAYERS_DB_GLOBS=(
  "${STORE}/players.db"
  "${STORE}/players.db-*"
  "${STORE}/players.*.db"
  "${STORE}/players.db-journal"
  "${STORE}/players.db-shm"
  "${STORE}/players.db-wal"
)

banner() { echo -e "\n=== $* ===\n"; }

# Return 0 only if user confirms twice; otherwise print why and return 1
confirm_twice() {
  local prompt="$1"
  read -r -p "${prompt} (yes/NO): " a
  case "${a,,}" in
    y|yes) ;;
    *) echo "Cancelled: first confirmation declined."; return 1 ;;
  esac
  read -r -p "Really sure? Type YES to proceed: " b
  if [[ "$b" != "YES" ]]; then
    echo 'Cancelled: second confirmation failed (expected YES).'
    return 1
  fi
  return 0
}

DID_ANYTHING=0

# --- Preview helpers (context-aware) ---
show_time_targets() {
  echo "Will remove time refs (if present):"
  printf "  - %s\n" \
    "$ROOT/seasonal.log" \
    "$ROOT/game_time.txt" \
    "$ROOT/latitude.txt"
}

show_player_targets() {
  echo "Players DB location:"
  echo "  - $STORE"
  echo "Will remove player DB files (if present):"
  printf "  - %s\n" "$STORE"/players.db "$STORE"/players.db-*
}

show_store_targets() {
  echo "Mission dir : $MISSION"
  echo "Store dir   : $STORE"
}

active_store_hint() {
  local d="$STORE"
  [[ -d $d ]] || return 0
  if [[ -f "$d/data/vehicles.bin" || -f "$d/data/building.bin" ]]; then
    echo "Active-ish store candidate: $d"
  fi
}

stop_server_hint() {
  if pgrep -af DayZServer >/dev/null 2>&1; then
    echo "DayZServer appears to be running."
    echo "   Stop it before wiping:"
    echo "     tmux attach -t dayz   # then Ctrl+C"
    echo "     # or: pkill -f DayZServer"
    read -r -p "Proceed anyway? (NOT RECOMMENDED) (y/N): " go
    [[ "${go:-}" =~ ^[yY]$ ]] || { echo "Aborted."; exit 1; }
  fi
}

wipe_time() {
  banner "Resetting time-state files"
  local found=0
  for f in "${TIME_FILES[@]}"; do
    if [[ -e "$f" ]]; then
      echo "Deleting: $f"
      rm -f -- "$f"
      found=1
    fi
  done
  (( found == 0 )) && echo "No time files found. Nothing to do."
}

wipe_players() {
  banner "Wiping player databases"
  local found=0
  shopt -s nullglob
  for g in "${PLAYERS_DB_GLOBS[@]}"; do
    # Expand each glob; with nullglob on, missing patterns drop out
    for f in $g; do
      [[ -e "$f" ]] || continue
      echo "Deleting: $f"
      rm -f -- "$f"
      found=1
    done
  done
  shopt -u nullglob
  (( found == 0 )) && echo "No player DB files found. Nothing to do."
}

wipe_server() {
  banner "Wiping persistence (storage_1)"
  if [[ -d "$STORE" ]]; then
    echo "Deleting: $STORE"
    rm -rf -- "$STORE"
  else
    echo "No storage_1 dir found at: $STORE"
  fi
}

menu() {
  cat <<'EOF'
Choose an action:
  1) Reset time only  (delete seasonal.log, game_time.txt, latitude.txt)
  2) Wipe players     (delete players.db*)
  3) Wipe server      (delete storage_1)
  4) Wipe ALL         (1 + 2 + 3)
  q) Quit
EOF
  read -r -p "> " choice
  case "${choice}" in
    1)
      show_time_targets
      if confirm_twice "Reset time files now?"; then
        stop_server_hint
        wipe_time
        DID_ANYTHING=1
      else
        echo "No action taken."
      fi
      ;;
    2)
      show_player_targets
      if confirm_twice "Wipe ALL player data now?"; then
        stop_server_hint
        wipe_players
        DID_ANYTHING=1
      else
        echo "No action taken."
      fi
      ;;
    3)
      show_store_targets
      active_store_hint
      if confirm_twice "Wipe server persistence (storage_1) now?"; then
        stop_server_hint
        wipe_server
        DID_ANYTHING=1
      else
        echo "No action taken."
      fi
      ;;
    4)
      show_time_targets
      show_player_targets
      show_store_targets
      active_store_hint
      if confirm_twice "WIPE EVERYTHING (time files + players + storage)?"; then
        stop_server_hint
        wipe_time
        wipe_players
        wipe_server
        DID_ANYTHING=1
      else
        echo "No action taken."
      fi
      ;;
    q|Q) echo "Bye."; exit 0 ;;
    *) echo "Invalid choice."; exit 1 ;;
  esac

  if [[ $DID_ANYTHING -eq 1 ]]; then
    echo -e "\nDone. On next start, DayZ will rebuild persistence."
    echo "Restart like usual (tmux script or your systemd unit)."
  else
    echo -e "\nNo changes made."
  fi
}

# sanity checks
[[ -d "$ROOT" ]]    || { echo "ROOT not found: $ROOT"; exit 1; }
[[ -d "$MISSION" ]] || { echo "Mission dir not found: $MISSION"; exit 1; }
[[ -d "$STORE" ]]   || echo "WARN: Store dir not found yet (fresh server?): $STORE"

menu
