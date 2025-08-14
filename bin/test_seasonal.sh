#!/usr/bin/env bash
# Test Suite for DayZ Seasonal System

ROOT="/home/dayzserver/dayz"
TEST_DIR="$ROOT/test_seasonal"
ORIGINAL_FILES=()

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}================================${NC}"
}

print_test() {
    echo -e "${YELLOW}TEST: $1${NC}"
}

print_pass() {
    echo -e "${GREEN}✅ PASS: $1${NC}"
}

print_fail() {
    echo -e "${RED}❌ FAIL: $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ️  INFO: $1${NC}"
}

# Backup existing files
backup_files() {
    print_header "SETTING UP TEST ENVIRONMENT"
    
    mkdir -p "$TEST_DIR"
    
    FILES=("game_time.txt" "latitude.txt" "seasonal.log" "serverDZ.cfg")
    
    for file in "${FILES[@]}"; do
        if [ -f "$ROOT/$file" ]; then
            cp "$ROOT/$file" "$TEST_DIR/${file}.backup"
            ORIGINAL_FILES+=("$file")
            print_info "Backed up $file"
        fi
    done
}

# Restore files after testing
restore_files() {
    print_header "RESTORING ORIGINAL FILES"
    
    for file in "${ORIGINAL_FILES[@]}"; do
        if [ -f "$TEST_DIR/${file}.backup" ]; then
            cp "$TEST_DIR/${file}.backup" "$ROOT/$file"
            print_info "Restored $file"
        fi
    done
    
    # Clean up test files
    TEST_FILES=("game_time.txt" "latitude.txt" "seasonal.log")
    for file in "${TEST_FILES[@]}"; do
        if [ -f "$ROOT/$file" ] && [[ ! " ${ORIGINAL_FILES[@]} " =~ " $file " ]]; then
            rm "$ROOT/$file"
            print_info "Cleaned up test file $file"
        fi
    done
    
    rm -rf "$TEST_DIR"
}

# Test 1: File initialization
test_initialization() {
    print_test "File Initialization"
    
    # Remove any existing files
    rm -f "$ROOT/game_time.txt" "$ROOT/latitude.txt"
    
    # Run seasonal script
    cd "$ROOT" && ./bin/seasonal_update.sh > /dev/null 2>&1
    
    if [ -f "$ROOT/game_time.txt" ] && [ -f "$ROOT/latitude.txt" ]; then
        GAME_TIME=$(cat "$ROOT/game_time.txt")
        LATITUDE=$(cat "$ROOT/latitude.txt")
        
        if [ "$GAME_TIME" -gt 0 ] && [ "$LATITUDE" = "57" ]; then
            print_pass "Files initialized correctly (game_time: $GAME_TIME, latitude: $LATITUDE)"
        else
            print_fail "Files created but values incorrect (game_time: $GAME_TIME, latitude: $LATITUDE)"
        fi
    else
        print_fail "Required files not created"
    fi
}

# Test 2: Time progression
test_time_progression() {
    print_test "Time Progression"
    
    # Set initial time
    echo "100" > "$ROOT/game_time.txt"
    
    # Run seasonal script
    cd "$ROOT" && ./bin/seasonal_update.sh > /dev/null 2>&1
    
    NEW_TIME=$(cat "$ROOT/game_time.txt")
    EXPECTED_TIME=142  # 100 + (7 * 6) = 142
    
    if [ "$NEW_TIME" = "$EXPECTED_TIME" ]; then
        print_pass "Time progression correct ($NEW_TIME hours)"
    else
        print_fail "Time progression incorrect (expected $EXPECTED_TIME, got $NEW_TIME)"
    fi
}

# Test 3: Yearly cycling
test_yearly_cycling() {
    print_test "Yearly Cycling"
    
    # Set time near end of year
    NEAR_END=$((365 * 24 - 10))  # 8750 hours (10 hours before year end)
    echo "$NEAR_END" > "$ROOT/game_time.txt"
    
    # Run seasonal script twice
    cd "$ROOT" && ./bin/seasonal_update.sh > /dev/null 2>&1
    cd "$ROOT" && ./bin/seasonal_update.sh > /dev/null 2>&1
    
    NEW_TIME=$(cat "$ROOT/game_time.txt")
    
    # Should have cycled back (was 8750, +42, +42 = 8834, which should become 74)
    if [ "$NEW_TIME" -lt 100 ]; then
        print_pass "Yearly cycling works (reset to $NEW_TIME hours)"
    else
        print_fail "Yearly cycling failed (time: $NEW_TIME, should be < 100)"
    fi
}

# Test 4: Different latitudes
test_latitudes() {
    print_test "Latitude Variations"
    
    LATITUDES=(35 45 57 65)
    RESULTS=()
    
    for lat in "${LATITUDES[@]}"; do
        echo "$lat" > "$ROOT/latitude.txt"
        echo "4380" > "$ROOT/game_time.txt"  # Day 182.5 (around summer solstice)
        
        cd "$ROOT" && ./bin/seasonal_update.sh > /dev/null 2>&1
        
        # Extract night ratio from log
        RATIO=$(grep "night acceleration multiplier" "$ROOT/seasonal.log" | tail -1 | grep -o '[0-9.]*$')
        RESULTS+=("${lat}°N: $RATIO")
    done
    
    print_pass "Latitude variations calculated:"
    for result in "${RESULTS[@]}"; do
        echo "    $result"
    done
}

# Test 5: Config file updates
test_config_updates() {
    print_test "Config File Updates"
    
    # Create test config
    cat > "$ROOT/serverDZ.cfg" << EOF
serverTimeAcceleration=6;
serverNightTimeAcceleration=1;
hostname="Test Server";
EOF
    
    echo "57" > "$ROOT/latitude.txt"
    echo "1000" > "$ROOT/game_time.txt"
    
    cd "$ROOT" && ./bin/seasonal_update.sh > /dev/null 2>&1
    
    # Check if serverNightTimeAcceleration was updated
    NEW_RATIO=$(grep "serverNightTimeAcceleration=" "$ROOT/serverDZ.cfg" | grep -o '[0-9.]*')
    
    if [ -f "$ROOT/serverDZ.cfg.bak" ] && [ "$NEW_RATIO" != "1" ]; then
        print_pass "Config updated (new ratio: $NEW_RATIO) and backed up"
    else
        print_fail "Config update failed"
    fi
}

# Test 6: Edge cases
test_edge_cases() {
    print_test "Edge Cases"
    
    # Test extreme latitude
    echo "70" > "$ROOT/latitude.txt" 
    echo "0" > "$ROOT/game_time.txt"  # Winter solstice
    
    cd "$ROOT" && ./bin/seasonal_update.sh > /dev/null 2>&1
    
    RATIO=$(grep "night acceleration multiplier" "$ROOT/seasonal.log" | tail -1 | grep -o '[0-9.]*$')
    
    if (( $(echo "$RATIO >= 0.1 && $RATIO <= 64" | bc -l) )); then
        print_pass "Extreme latitude handled (ratio: $RATIO within bounds)"
    else
        print_fail "Extreme latitude produced invalid ratio: $RATIO"
    fi
}

# Test utilities
test_utilities() {
    print_test "Utility Scripts"
    
    # Test latitude configurator
    if [ -x "$ROOT/bin/configure_latitude.sh" ]; then
        print_pass "configure_latitude.sh is executable"
    else
        print_fail "configure_latitude.sh not found or not executable"
    fi
    
    # Test season sync
    if [ -x "$ROOT/bin/sync_seasons.sh" ]; then
        print_pass "sync_seasons.sh is executable"  
    else
        print_fail "sync_seasons.sh not found or not executable"
    fi
}

# Main test runner
run_tests() {
    print_header "DayZ SEASONAL SYSTEM TEST SUITE"
    
    # Check dependencies
    if ! command -v bc &> /dev/null; then
        print_fail "bc (calculator) not installed. Run: sudo apt install bc"
        exit 1
    fi
    
    if [ ! -f "$ROOT/bin/seasonal_update.sh" ]; then
        print_fail "seasonal_update.sh not found in $ROOT/bin/"
        exit 1
    fi
    
    backup_files
    
    test_initialization
    test_time_progression  
    test_yearly_cycling
    test_latitudes
    test_config_updates
    test_edge_cases
    test_utilities
    
    restore_files
    
    print_header "TEST SUMMARY"
    echo "All tests completed!"
    echo ""
    echo "Manual verification steps:"
    echo "1. Check ~/dayzserver/dayz/seasonal.log for detailed logs"
    echo "2. Run: ~/dayzserver/dayz/bin/sync_seasons.sh (option 3) to preview current settings"
    echo "3. Run your normal restart script to see it in action"
}

# Run the tests
run_tests
# test edit
