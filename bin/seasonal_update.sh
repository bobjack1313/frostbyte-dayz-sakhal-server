#!/usr/bin/env bash
set -euo pipefail

ROOT="/home/dayzserver/dayz"
CFG="$ROOT/serverDZ.cfg"
SEASONAL_LOG="$ROOT/seasonal.log"
LATITUDE_FILE="$ROOT/latitude.txt"

log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') seasonal: $*" | tee -a "$SEASONAL_LOG"
}

require() {
    command -v "$1" >/dev/null 2>&1 || {
        echo "Missing dependency: $1"
        exit 1
    }
}

get_season() {
    local day="$1"

    if [ "$day" -lt 80 ] || [ "$day" -gt 355 ]; then
        echo "Winter"
    elif [ "$day" -lt 172 ]; then
        echo "Spring"
    elif [ "$day" -lt 266 ]; then
        echo "Summer"
    else
        echo "Autumn"
    fi
}

require bc

if [ ! -f "$CFG" ]; then
    log "ERROR: serverDZ.cfg not found at $CFG"
    exit 1
fi

# Latitude for Sakhal-ish northern daylight behavior.
if [ -f "$LATITUDE_FILE" ]; then
    LATITUDE="$(cat "$LATITUDE_FILE")"
else
    LATITUDE=57
    echo "$LATITUDE" > "$LATITUDE_FILE"
    log "Initialized latitude to ${LATITUDE}°N"
fi

# Use real-world date, not accumulated fake game time.
CURRENT_DAY=$((10#$(date +%j)))

# Clamp leap day to day 365 for this simple model.
if [ "$CURRENT_DAY" -gt 365 ]; then
    CURRENT_DAY=365
fi

DATE_LABEL="$(date -d "Jan 1 + $((CURRENT_DAY - 1)) days" +"%b %d")"

log "Using system date: day $CURRENT_DAY/365 ($DATE_LABEL)"
log "Latitude: ${LATITUDE}°N"

# Solar declination approximation.
DECLINATION=$(echo "scale=8; -23.45 * c(2 * 3.1415926535 * ($CURRENT_DAY + 10) / 365)" | bc -l)

# Daylight hours approximation:
# daylength = 24/pi * acos(-tan(latitude) * tan(declination))
RAW_DAYLIGHT=$(echo '
scale=8
pi=3.1415926535
latdeg='"$LATITUDE"'
ddeg='"$DECLINATION"'

lat=latdeg*pi/180
dec=ddeg*pi/180

y = -s(lat)*s(dec)/(c(lat)*c(dec))

if (y > 1) y = 1
if (y < -1) y = -1

# acos(y) using atan form.
if (y == 0) {
    acos = pi / 2
} else {
    acos = a(sqrt(1 - y*y) / y)
    if (y < 0) acos = acos + pi
}

24 * (acos / pi)
' | bc -l)

DAYLIGHT_HOURS=$(printf "%.2f" "${RAW_DAYLIGHT:-12}")

# Clamp extremes so Sakhal does not get absurd seasonal timing.
MAX_DAYLIGHT=$(echo "scale=1; if ($LATITUDE > 60) 20 else if ($LATITUDE > 50) 18 else 16" | bc -l)
MIN_DAYLIGHT=$(echo "scale=1; if ($LATITUDE > 60) 4 else if ($LATITUDE > 50) 6 else 8" | bc -l)

if [ "$(echo "$DAYLIGHT_HOURS < $MIN_DAYLIGHT" | bc -l)" -eq 1 ]; then
    DAYLIGHT_HOURS="$MIN_DAYLIGHT"
fi

if [ "$(echo "$DAYLIGHT_HOURS > $MAX_DAYLIGHT" | bc -l)" -eq 1 ]; then
    DAYLIGHT_HOURS="$MAX_DAYLIGHT"
fi

NIGHT_HOURS=$(echo "scale=2; 24 - $DAYLIGHT_HOURS" | bc -l)

log "Daylight hours: $DAYLIGHT_HOURS, Night hours: $NIGHT_HOURS"

# Keep real night duration near a 12-hour baseline by adjusting night acceleration.
DEFAULT_NIGHT_HOURS=12
NIGHT_RATIO=$(echo "scale=6; $DEFAULT_NIGHT_HOURS / $NIGHT_HOURS" | bc -l)

if [ -z "$NIGHT_RATIO" ]; then
    log "ERROR: night ratio calculation failed"
    exit 1
fi

if [ "$(echo "$NIGHT_RATIO < 0.1" | bc -l)" -eq 1 ]; then
    NIGHT_RATIO=0.1
fi

if [ "$(echo "$NIGHT_RATIO > 64" | bc -l)" -eq 1 ]; then
    NIGHT_RATIO=64.0
fi

NIGHT_RATIO_FMT=$(printf "%.2f" "$NIGHT_RATIO")

log "Calculated night acceleration multiplier: $NIGHT_RATIO_FMT"

cp "$CFG" "$CFG.bak"

if grep -q "serverNightTimeAcceleration" "$CFG"; then
    sed -i "s/serverNightTimeAcceleration[[:space:]]*=[[:space:]]*[^;]*/serverNightTimeAcceleration=$NIGHT_RATIO_FMT/" "$CFG"
    log "Updated serverNightTimeAcceleration to $NIGHT_RATIO_FMT"
else
    log "ERROR: serverNightTimeAcceleration not found in $CFG"
    exit 1
fi

SEASON=$(get_season "$CURRENT_DAY")
log "Current season: $SEASON (Day $CURRENT_DAY)"

exit 0
