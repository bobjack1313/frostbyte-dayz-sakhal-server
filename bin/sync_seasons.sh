#!/usr/bin/env bash
# Season Sync Utility - Reset seasons to match real-world date

ROOT="/home/dayzserver/dayz"
GAME_TIME_FILE="$ROOT/game_time.txt"
LATITUDE_FILE="$ROOT/latitude.txt"

echo "DayZ Seasonal Sync Utility"
echo "=========================="
echo ""

# Get current system info
REAL_DAY_OF_YEAR=$(date +%j)
REAL_DATE=$(date +"%B %d, %Y")
LATITUDE=$(cat "$LATITUDE_FILE" 2>/dev/null || echo "57")

echo "Current system date: $REAL_DATE"
echo "Day of year: $REAL_DAY_OF_YEAR/365"
echo "Configured latitude: ${LATITUDE}°N"
echo ""

if [ -f "$GAME_TIME_FILE" ]; then
    CURRENT_GAME_HOURS=$(cat "$GAME_TIME_FILE")
    CURRENT_GAME_DAY=$((CURRENT_GAME_HOURS / 24 + 1))
    if [ $CURRENT_GAME_DAY -gt 365 ]; then CURRENT_GAME_DAY=365; fi
    
    GAME_DATE=$(date -d "Jan 1 + $((CURRENT_GAME_DAY - 1)) days" +"%B %d")
    echo "Current game season: Day $CURRENT_GAME_DAY ($GAME_DATE)"
    echo ""
else
    echo "No existing game time found."
    echo ""
fi

echo "Options:"
echo "1) Sync to current real-world date (Day $REAL_DAY_OF_YEAR)"
echo "2) Set to specific date"
echo "3) Preview seasonal info for current date"
echo "4) Cancel"
echo ""

read -p "Choose option (1-4): " CHOICE

case $CHOICE in
    1)
        # Sync to current system date
        NEW_GAME_HOURS=$(( (REAL_DAY_OF_YEAR - 1) * 24 ))
        echo "$NEW_GAME_HOURS" > "$GAME_TIME_FILE"
        echo "Synced! Game season now matches $REAL_DATE"
        echo "Game time set to: $NEW_GAME_HOURS hours (Day $REAL_DAY_OF_YEAR)"
        ;;
        
    2)
        # Set to specific date
        echo ""
        echo "Enter target date:"
        read -p "Month (1-12): " MONTH
        read -p "Day (1-31): " DAY
        
        if ! [[ "$MONTH" =~ ^[0-9]+$ ]] || [ "$MONTH" -lt 1 ] || [ "$MONTH" -gt 12 ]; then
            echo "Error: Invalid month"
            exit 1
        fi
        
        if ! [[ "$DAY" =~ ^[0-9]+$ ]] || [ "$DAY" -lt 1 ] || [ "$DAY" -gt 31 ]; then
            echo "Error: Invalid day"
            exit 1
        fi
        
        # Calculate day of year for target date
        TARGET_DATE=$(printf "2024-%02d-%02d" $MONTH $DAY)
        TARGET_DAY_OF_YEAR=$(date -d "$TARGET_DATE" +%j 2>/dev/null || echo "0")
        
        if [ "$TARGET_DAY_OF_YEAR" = "0" ]; then
            echo "Error: Invalid date"
            exit 1
        fi
        
        NEW_GAME_HOURS=$(( (TARGET_DAY_OF_YEAR - 1) * 24 ))
        echo "$NEW_GAME_HOURS" > "$GAME_TIME_FILE"
        
        TARGET_DATE_STR=$(date -d "$TARGET_DATE" +"%B %d")
        echo "Game season set to $TARGET_DATE_STR"
        echo "Game time set to: $NEW_GAME_HOURS hours (Day $TARGET_DAY_OF_YEAR)"
        ;;
        
    3)
        # Preview seasonal info
        echo ""
        echo "Seasonal Preview for Day $REAL_DAY_OF_YEAR at ${LATITUDE}°N:"
        echo "============================================="
        
        # Calculate daylight for current date
        DECLINATION=$(echo "scale=4; -23.45 * c(2 * 3.14159 * ($REAL_DAY_OF_YEAR + 10) / 365)" | bc -l)
        HOUR_ANGLE_RAD=$(echo "scale=6; a = -$DECLINATION * 3.14159/180; b = $LATITUDE * 3.14159/180; if ((-1 * (c(b) * c(a))) <= -1) { 3.14159 } else if ((-1 * (c(b) * c(a))) >= 1) { 0 } else { a(-1 * (s(b) * s(a)) / sqrt(1 - (s(b) * s(a))^2)) }" | bc -l)
        DAYLIGHT_HOURS=$(echo "scale=2; 24 * $HOUR_ANGLE_RAD / 3.14159" | bc -l)
        
        # Apply latitude-based clamping
        MAX_DAYLIGHT=$(echo "scale=1; if ($LATITUDE > 60) 20 else if ($LATITUDE > 50) 18 else 16" | bc -l)
        MIN_DAYLIGHT=$(echo "scale=1; if ($LATITUDE > 60) 4 else if ($LATITUDE > 50) 6 else 8" | bc -l)
        
        if (( $(echo "$DAYLIGHT_HOURS < $MIN_DAYLIGHT" | bc -l) )); then
            DAYLIGHT_HOURS=$MIN_DAYLIGHT
        elif (( $(echo "$DAYLIGHT_HOURS > $MAX_DAYLIGHT" | bc -l) )); then
            DAYLIGHT_HOURS=$MAX_DAYLIGHT
        fi
        
        NIGHT_HOURS=$(echo "scale=2; 24 - $DAYLIGHT_HOURS" | bc -l)
        NIGHT_RATIO=$(echo "scale=2; 12 / $NIGHT_HOURS" | bc -l)
        
        echo "Daylight: ${DAYLIGHT_HOURS} hours"
        echo "Night: ${NIGHT_HOURS} hours"  
        echo "Night acceleration ratio: $NIGHT_RATIO"
        
        if (( $(echo "$NIGHT_RATIO < 1" | bc -l) )); then
            echo "Effect: Nights will feel longer (slower)"
        elif (( $(echo "$NIGHT_RATIO > 1" | bc -l) )); then
            echo "Effect: Nights will pass quickly"
        else
            echo "Effect: Normal 12/12 day/night balance"
        fi
        ;;
        
    4)
        echo "Cancelled."
        ;;
        
    *)
        echo "Invalid option."
        exit 1
        ;;
esac

echo ""
echo "Next server restart will apply any changes."
