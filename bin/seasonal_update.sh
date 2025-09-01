#!/usr/bin/env bash
set -euo pipefail

# ---------- helpers first (so we can call them anywhere) ----------
log() { echo "$(date '+%Y-%m-%d %H:%M:%S') seasonal: $*" | tee -a "$SEASONAL_LOG"; }

get_season() {
    local day=$1
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

require() { command -v "$1" >/dev/null 2>&1 || { echo "Missing dependency: $1"; exit 1; }; }

# ---------- config ----------
ROOT="/home/dayzserver/dayz"
CFG="$ROOT/serverDZ.cfg"
GAME_TIME_FILE="$ROOT/game_time.txt"
SEASONAL_LOG="$ROOT/seasonal.log"
LATITUDE_FILE="$ROOT/latitude.txt"
REBOOT_HOURS=10

require bc

# ---------- latitude init ----------
if [ -f "$LATITUDE_FILE" ]; then
    LATITUDE=$(cat "$LATITUDE_FILE")
else
    LATITUDE=57
    echo "$LATITUDE" > "$LATITUDE_FILE"
    log "Initialized latitude to ${LATITUDE}°N"
fi

# ---------- read time accel ----------
TIME_ACCEL=$(grep -o 'serverTimeAcceleration[[:space:]]*=[[:space:]]*[0-9]\+' "$CFG" 2>/dev/null | grep -o '[0-9]\+' || echo "6")
log "Time acceleration: ${TIME_ACCEL}x"
log "Latitude: ${LATITUDE}°N"

# ---------- current game hours ----------
if [ -f "$GAME_TIME_FILE" ]; then
    CURRENT_GAME_HOURS=$(cat "$GAME_TIME_FILE")
    log "Loaded existing game time: $CURRENT_GAME_HOURS hours"
else
    REAL_DAY_OF_YEAR=$(date +%j)
    CURRENT_GAME_HOURS=$(( (REAL_DAY_OF_YEAR - 1) * 24 ))
    log "Initializing from system date: Day $REAL_DAY_OF_YEAR of year"
    log "Starting game time: $CURRENT_GAME_HOURS hours"
fi

# ---------- advance by reboot window ----------
HOURS_TO_ADD=$((REBOOT_HOURS * TIME_ACCEL))
CURRENT_GAME_HOURS=$((CURRENT_GAME_HOURS + HOURS_TO_ADD))

# ---------- wrap yearly ----------
YEAR_LENGTH_HOURS=$((365 * 24))
if [ "$CURRENT_GAME_HOURS" -ge "$YEAR_LENGTH_HOURS" ]; then
    YEARS_ELAPSED=$((CURRENT_GAME_HOURS / YEAR_LENGTH_HOURS))
    CURRENT_GAME_HOURS=$((CURRENT_GAME_HOURS % YEAR_LENGTH_HOURS))
    log "Yearly cycle complete! Years elapsed: $YEARS_ELAPSED, reset to hour $CURRENT_GAME_HOURS"
fi

CURRENT_DAY=$((CURRENT_GAME_HOURS / 24 + 1))
[ "$CURRENT_DAY" -gt 365 ] && CURRENT_DAY=365

echo "$CURRENT_GAME_HOURS" > "$GAME_TIME_FILE"

log "Game hours elapsed: $CURRENT_GAME_HOURS (+$HOURS_TO_ADD)"
log "Current seasonal day: $CURRENT_DAY/365 ($(date -d "Jan 1 + $((CURRENT_DAY - 1)) days" +"%b %d"))"

# ---------- daylight calc ----------
DAY_OF_YEAR=$CURRENT_DAY
# declination (deg)
DECLINATION=$(echo "scale=6; -23.45 * c(2 * 3.14159 * ($DAY_OF_YEAR + 10) / 365)" | bc -l)

# hour angle (radians) with clamps to avoid NaNs
HOUR_ANGLE_RAD=$(echo 'scale=8;
    pi=3.1415926535;
    ddeg='$DECLINATION';
    latdeg='$LATITUDE';
    d=ddeg*pi/180; b=latdeg*pi/180;
    x = (-s(b)*s(d))/(c(b)*c(d));
    if (x>1) x=1;
    if (x<-1) x=-1;
    # acos(x) via a() and sqrt
    acosx = a(sqrt(1-x*x)/(x==1?1: (1/sqrt(1-x*x)) ))  # not used directly, bc’s a() is atan; compute via identity:
' | bc -l 2>/dev/null) # placeholder to keep bc happy

# Simpler: use sunrise duration formula safely:
HOUR_ANGLE_RAD=$(echo 'scale=8;
    pi=3.1415926535;
    ddeg='$DECLINATION';
    latdeg='$LATITUDE';
    d=ddeg*pi/180; b=latdeg*pi/180;
    y = -t(b)*t(d);
    # acos(-tan φ * tan δ) = arccos(y_clamped)
    # implement acos via atan: acos(y)=atan2(sqrt(1-y^2), y)
    y_clamp=y; if (y_clamp>1) y_clamp=1; if (y_clamp<-1) y_clamp=-1;
    sqrt(1 - y_clamp*y_clamp);  # we’ll pass this back to shell and compute daylength differently
' | bc -l)

# Fallback if weird: default to 12/12
if [ -z "$HOUR_ANGLE_RAD" ]; then
    DAYLIGHT_HOURS=12
else
    # Use standard approximation: daylength = 24 * (1/pi) * arccos(-tan φ * tan δ)
    RAW=$(echo 'scale=8;
        pi=3.1415926535;
        ddeg='$DECLINATION';
        latdeg='$LATITUDE';
        d=ddeg*pi/180; b=latdeg*pi/180;
        y = -s(b)*s(d)/(c(b)*c(d));
        if (y>1) y=1; if (y<-1) y=-1;
        # acos(y) using atan2 form
        acos = a(sqrt(1-y*y)/y);
        if (y==0) { acos=pi/2; }
        if (y<0)  { acos=acos+pi; }
        24*(acos/pi)
    ' | bc -l)
    DAYLIGHT_HOURS=$(printf "%.2f" "${RAW:-12}")
fi

# clamp by latitude buckets
MAX_DAYLIGHT=$(echo "scale=1; if ($LATITUDE > 60) 20 else if ($LATITUDE > 50) 18 else 16" | bc -l)
MIN_DAYLIGHT=$(echo "scale=1; if ($LATITUDE > 60) 4 else if ($LATITUDE > 50) 6 else 8" | bc -l)

cmp=$(echo "$DAYLIGHT_HOURS < $MIN_DAYLIGHT" | bc -l); if [ "$cmp" -eq 1 ]; then DAYLIGHT_HOURS=$MIN_DAYLIGHT; fi
cmp=$(echo "$DAYLIGHT_HOURS > $MAX_DAYLIGHT" | bc -l); if [ "$cmp" -eq 1 ]; then DAYLIGHT_HOURS=$MAX_DAYLIGHT; fi

NIGHT_HOURS=$(echo "scale=2; 24 - $DAYLIGHT_HOURS" | bc -l)

log "Daylight hours: $DAYLIGHT_HOURS, Night hours: $NIGHT_HOURS"

# ---------- ratio calc ----------
DEFAULT_NIGHT_HOURS=12
NIGHT_RATIO=$(echo "scale=6; $DEFAULT_NIGHT_HOURS / $NIGHT_HOURS" | bc -l)
# guard against empty/NaN
: "${NIGHT_RATIO:?night ratio calc failed}"

# bounds
if (( $(echo "$NIGHT_RATIO < 0.1" | bc -l) )); then NIGHT_RATIO=0.1; fi
if (( $(echo "$NIGHT_RATIO > 64"  | bc -l) )); then NIGHT_RATIO=64.0; fi

# pretty
NIGHT_RATIO_FMT=$(printf "%.2f" "$NIGHT_RATIO")
log "Calculated night acceleration multiplier: $NIGHT_RATIO_FMT"

# ---------- config write ----------
cp "$CFG" "$CFG.bak"
# only update if we have a sane number
if [[ "$NIGHT_RATIO_FMT" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
    sed -i "s/serverNightTimeAcceleration[[:space:]]*=[[:space:]]*[^;]*/serverNightTimeAcceleration=$NIGHT_RATIO_FMT/" "$CFG"
    log "Updated serverNightTimeAcceleration to $NIGHT_RATIO_FMT"
else
    log "Skipped config update: invalid ratio '$NIGHT_RATIO_FMT'"
fi

SEASON=$(get_season "$CURRENT_DAY")
log "Current season: $SEASON (Day $CURRENT_DAY)"

exit 0
