#!/usr/bin/env bash
set -euo pipefail

ROOT="/home/dayzserver/dayz"
MPMISS="$ROOT/mpmissions/dayzOffline.sakhal"

TEMPLATE="$MPMISS/db/messages_template.xml"
HUMOR="$MPMISS/db/message_humor.txt"
OUT="$MPMISS/db/messages.xml"

log(){ echo "shuffle-messages: $*"; }

log "generating rotating messages"

if [[ ! -f "$TEMPLATE" ]]; then
  log "missing template: $TEMPLATE"
  exit 1
fi

if [[ ! -f "$HUMOR" ]]; then
  log "missing humor file: $HUMOR"
  exit 1
fi

python3 - "$TEMPLATE" "$HUMOR" "$OUT" <<'PY'
import random
import re
import sys
import html
from pathlib import Path

template_path = Path(sys.argv[1])
humor_path = Path(sys.argv[2])
out_path = Path(sys.argv[3])

template = template_path.read_text(encoding="utf-8")
lines = [
    line.strip()
    for line in humor_path.read_text(encoding="utf-8").splitlines()
    if line.strip() and not line.strip().startswith("#")
]

placeholders = sorted(
    set(re.findall(r"MESSAGE_HUMOR_(\d+)", template)),
    key=lambda x: int(x)
)

if not placeholders:
    raise SystemExit("No MESSAGE_HUMOR_* placeholders found")

if len(lines) < len(placeholders):
    raise SystemExit(f"Not enough humor lines: need {len(placeholders)}, found {len(lines)}")

picked = random.sample(lines, len(placeholders))

for key, line in zip(placeholders, picked):
    template = template.replace(f"MESSAGE_HUMOR_{key}", html.escape(line, quote=False))

out_path.write_text(template, encoding="utf-8")
PY

log "wrote $OUT"
