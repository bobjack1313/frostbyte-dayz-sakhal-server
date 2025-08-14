#!/usr/bin/env bash
set -euo pipefail

BE_DIR="/home/dayzserver/dayz/battleye"
ACTIVE_CFG="$(ls -1t "$BE_DIR"/beserver_x64_active_*.cfg 2>/dev/null | head -1)"
STATIC_CFG="$BE_DIR/beserver_x64.cfg"
CFG=""

if [[ -f "$ACTIVE_CFG" ]]; then
  CFG="$ACTIVE_CFG"
elif [[ -f "$STATIC_CFG" ]]; then
  CFG="$STATIC_CFG"
else
  echo "rcon.sh: no BE cfg found in $BE_DIR" >&2
  exit 1
fi

PORT=$(sed -n 's/^RConPort[[:space:]]\+//p' "$CFG" | head -1)
PASS=$(sed -n 's/^RConPassword[[:space:]]\+//p' "$CFG" | head -1)
HOST="127.0.0.1"

CMD="${1:-}"
if [[ -z "$CMD" ]]; then
  echo "rcon.sh: usage: rcon.sh \"say -1 hello\" | \"players\" | \"#shutdown\"" >&2
  exit 1
fi

echo "rcon.sh: using cfg=$(basename "$CFG") host=$HOST port=$PORT cmd=$CMD"

# retry with backoff up to ~1m total
for d in 2 3 4 5 6 8 10 12 12 12; do
  if /usr/bin/bercon-cli -i "$HOST" -p "$PORT" -P "$PASS" -- "$CMD"; then
    exit 0
  fi
  sleep "$d"
done

echo "rcon.sh: failed to deliver command after retries" >&2
exit 1
