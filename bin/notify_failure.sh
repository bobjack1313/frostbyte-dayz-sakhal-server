#!/usr/bin/env bash
set -euo pipefail

WEBHOOK_URL="https://discord.com/api/webhooks/1506552595240587375/Y6NKRjl5U5YSFOtkkRpjWjG1I-7VkkjrgHONFv4R7wK5uZEMfywLK_VRKRkcqE2JpHqr"

HOSTNAME="$(hostname)"
TIME="$(date '+%Y-%m-%d %H:%M:%S %Z')"
MESSAGE="${1:-DayZ server failure detected}"

payload=$(cat <<EOF
{
  "content": "🚨 **DayZ Alert on ${HOSTNAME}**\n${MESSAGE}\nTime: ${TIME}"
}
EOF
)

curl -sS \
  -H "Content-Type: application/json" \
  -d "$payload" \
  "$WEBHOOK_URL" >/dev/null
