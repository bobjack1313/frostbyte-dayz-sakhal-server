#!/usr/bin/env bash
# Latitude Configuration Helper for DayZ Seasonal System

ROOT="/home/dayzserver/dayz"
LATITUDE_FILE="$ROOT/latitude.txt"

echo "DayZ Seasonal System - Latitude Configuration"
echo "=============================================="
echo ""
echo "Current latitude: $(cat "$LATITUDE_FILE" 2>/dev/null || echo "Not set")"
echo ""
echo "Common Map Latitudes:"
echo "  35°N - Minimal seasonal variation (Mediterranean)"
echo "  45°N - Moderate seasons (Central Europe/Northern US)"
echo "  50°N - Noticeable variation (Southern UK/Northern France)"
echo "  55°N - Strong seasons (Scotland/Southern Scandinavia)"
echo "  57°N - Sakhal/Kamchatka (Current default)"
echo "  60°N - Very strong seasons (Southern Alaska/Oslo)"
echo "  65°N - Extreme variation (Northern Scandinavia)"
echo "  70°N+ - Polar extremes (not recommended for gameplay)"
echo ""
echo "Seasonal Intensity Guide:"
echo "  35°N: Night ratio ~0.9 to 1.1 (subtle)"
echo "  45°N: Night ratio ~0.8 to 1.3 (moderate)" 
echo "  57°N: Night ratio ~0.7 to 2.2 (strong)"
echo "  65°N: Night ratio ~0.5 to 4.0 (very strong)"
echo ""

read -p "Enter new latitude (35-70): " NEW_LATITUDE

# Validate input
if ! [[ "$NEW_LATITUDE" =~ ^[0-9]+$ ]] || [ "$NEW_LATITUDE" -lt 35 ] || [ "$NEW_LATITUDE" -gt 70 ]; then
    echo "Error: Please enter a number between 35 and 70"
    exit 1
fi

# Update latitude
echo "$NEW_LATITUDE" > "$LATITUDE_FILE"
echo ""
echo "Latitude updated to ${NEW_LATITUDE}°N"
echo ""
echo "Preview of seasonal variation at ${NEW_LATITUDE}°N:"

# Quick calculation preview
WINTER_DAYLIGHT=$(echo "scale=1; 12 - (23.45 * s(($NEW_LATITUDE * 3.14159/180)) * 2.4)" | bc -l)
SUMMER_DAYLIGHT=$(echo "scale=1; 12 + (23.45 * s(($NEW_LATITUDE * 3.14159/180)) * 2.4)" | bc -l)

# Clamp to reasonable bounds
if (( $(echo "$WINTER_DAYLIGHT < 4" | bc -l) )); then WINTER_DAYLIGHT=4.0; fi
if (( $(echo "$SUMMER_DAYLIGHT > 20" | bc -l) )); then SUMMER_DAYLIGHT=20.0; fi

WINTER_NIGHT=$(echo "24 - $WINTER_DAYLIGHT" | bc -l)
SUMMER_NIGHT=$(echo "24 - $SUMMER_DAYLIGHT" | bc -l)

WINTER_RATIO=$(echo "scale=2; 12 / $WINTER_NIGHT" | bc -l)
SUMMER_RATIO=$(echo "scale=2; 12 / $SUMMER_NIGHT" | bc -l)

echo "  Winter: ${WINTER_DAYLIGHT}h day, ${WINTER_NIGHT}h night (ratio: $WINTER_RATIO)"
echo "  Summer: ${SUMMER_DAYLIGHT}h day, ${SUMMER_NIGHT}h night (ratio: $SUMMER_RATIO)"
echo ""
echo "Changes will take effect on next server restart."
