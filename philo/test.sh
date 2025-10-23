#!/bin/bash

# Colors for readability
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
BLUE="\033[0;34m"
CYAN="\033[0;36m"
GRAY="\033[0;90m"
NC="\033[0m" # No Color

# Locate the executable
if [ -x "./bin/philo" ]; then
	PHILO="./bin/philo"
elif [ -x "./philo" ]; then
	PHILO="./philo"
else
	echo -e "${RED}Executable not found. Build first (make).${NC}"
	exit 1
fi

# Test cases format: "n_philo time_to_die time_to_eat time_to_sleep [n_meals]|EXPECTED_BEHAVIOR|TIMEOUT|DESCRIPTION"
declare -a TESTS=(

    "4 800 200 200 50|COMPLETE_MEALS|15|Long test - 50 meals"
    "5 800 200 200 30|COMPLETE_MEALS|15|Long test - 30 meals"

    # Basic cases
    "1 800 200 200|SHOULD_DIE|2|Single philosopher should die (no second fork)"
    "2 410 200 200|NO_DEATH|3|Two philosophers - should survive"
    "2 402 200 200|NO_DEATH|3|Two philosophers - perfect alternation, no death expected"
    "4 310 200 100|SHOULD_DIE|2|Four philosophers - tight timing, death expected"

    # Edge cases - very tight timing
    "2 310 200 100|SHOULD_DIE|2|Two philosophers - very tight (310ms die, 200ms eat)"
    "3 310 103 140|NO_DEATH|3|Three philosophers - exact minimum timing"
    "4 130 60 60|NO_DEATH|3|Four philosophers - exact minimum timing"
    "5 310 103 103|NO_DEATH|3|Five philosophers - exact minimum timing"

    # Edge cases - death timing validation
    "3 500 200 200|SHOULD_DIE|3|Three philosophers - should die at ~500ms"
    "4 200 205 200|SHOULD_DIE|2|Four philosophers - impossible timing (eat > die)"

    # Odd number edge cases
    "3 310 103 103|NO_DEATH|3|Three philosophers - odd number edge case"
    "5 310 103 103|NO_DEATH|3|Five philosophers - odd number edge case"
    "7 310 100 100|NO_DEATH|3|Seven philosophers - odd number edge case"

    # Meal completion tests
    "2 800 200 200 5|COMPLETE_MEALS|10|Two philosophers - 5 meals each"
    "3 800 200 200 5|COMPLETE_MEALS|10|Three philosophers - 5 meals each"
    "4 410 200 200 10|COMPLETE_MEALS|15|Four philosophers - 10 meals each"
    "5 600 150 150 7|COMPLETE_MEALS|15|Five philosophers - 7 meals each"

    # Single meal edge cases
    "2 800 200 200 1|COMPLETE_MEALS|5|Two philosophers - 1 meal only"
    "4 800 200 200 1|COMPLETE_MEALS|5|Four philosophers - 1 meal only"

    # High philosopher count stress tests
    "100 800 200 200|NO_DEATH|3|Stress: 100 philosophers"
    "155 800 200 200|NO_DEATH|3|Stress: 150 philosophers"
    "199 310 103 103|NO_DEATH|3|Stress: 199 philosophers edge case"
    "200 800 200 200|NO_DEATH|3|Stress: 200 philosophers"
    "200 600 200 200 10|NO_DEATH|15|Stress: 200 philosophers with 10 meals"
    "200 180 60 60 15|NO_DEATH|15|Stress: 200 philosophers fast pace"

    # Extreme timing cases
    "4 410 200 200|NO_DEATH|3|Tight window - 400ms die, 200ms cycle"
    "5 400 100 100|NO_DEATH|3|Tight window - 5 philosophers"
    "6 400 100 100|NO_DEATH|3|Tight window - 6 philosophers"   
)

DEATH_TOLERANCE_MS=10
MAX_PARALLEL=5

echo -e "${BLUE}╔════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║    Philosophers Validation Tester          ║${NC}"
echo -e "${BLUE}║         (Parallel Execution)               ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════╝${NC}"
echo

# Use a temporary directory for logs and status
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"; pkill -P $$' EXIT

TOTAL_TESTS=${#TESTS[@]}
PASSED_TESTS=0
FAILED_TESTS=0
CURRENT_TEST=0

# Track how many progress lines are currently displayed
DISPLAYED_LINES=0

# Progress bar function
show_progress() {
    local current=$1
    local total=$2
    local width=28

    # Guard against zero/negative totals and clamp current
    if [ -z "$total" ] || [ "$total" -le 0 ]; then
        total=1
    fi
    if [ -z "$current" ] || [ "$current" -lt 0 ]; then
        current=0
    fi
    if [ "$current" -gt "$total" ]; then
        current=$total
    fi

    local percentage=$((current * 100 / total))
    local completed=$((width * current / total))
    if [ $completed -gt $width ]; then completed=$width; fi
    local remaining=$((width - completed))

    printf "["
    printf "%${completed}s" | tr ' ' '='
    printf "%${remaining}s" | tr ' ' '.'
    printf "] %3d%%" "$percentage"
}

# Run and validate a single test
run_single_test() {
    local test_idx=$1
    local test_case="${TESTS[$test_idx]}"

    IFS='|' read -r ARGS EXPECTED TIMEOUT DESC <<< "$test_case"
    LOG="$TMPDIR/test_$((test_idx + 1)).log"
    STATUS_FILE="$TMPDIR/status_$((test_idx + 1)).txt"
    PROGRESS_FILE="$TMPDIR/progress_$((test_idx + 1)).txt"

    # Initialize progress file
    echo "0|$TIMEOUT" > "$PROGRESS_FILE"

    # Parse arguments (needed for dynamic timeout)
    read -r N_PHILO TT_DIE TT_EAT TT_SLEEP N_MEALS <<< "$ARGS"
    N_MEALS=${N_MEALS:-0}

    # Adjust timeout for COMPLETE_MEALS to avoid premature kill
    ADJ_TIMEOUT=$TIMEOUT
    if [ "$EXPECTED" = "COMPLETE_MEALS" ] && [ "$N_MEALS" -gt 0 ]; then
        # Estimate minimal total time: waves * TT_EAT, where waves = ceil((N_PHILO*N_MEALS)/max(1, floor(N_PHILO/2)))
        CONC=$((N_PHILO / 2))
        if [ $CONC -lt 1 ]; then CONC=1; fi
        TOTAL_MEALS=$((N_PHILO * N_MEALS))
        WAVES=$(((TOTAL_MEALS + CONC - 1) / CONC))
        EST_MS=$((WAVES * TT_EAT))
        EST_SEC=$(((EST_MS + 999) / 1000))
        MARGIN=10
        CALC_TIMEOUT=$((EST_SEC + MARGIN))
        if [ $CALC_TIMEOUT -gt $ADJ_TIMEOUT ]; then ADJ_TIMEOUT=$CALC_TIMEOUT; fi
    fi

    # Run with timeout and monitor progress
    timeout "$ADJ_TIMEOUT" "$PHILO" $ARGS > "$LOG" 2>&1 &
    local pid=$!
    local start_time=$(date +%s)

    # Monitor the process
    while kill -0 $pid 2>/dev/null; do
        local elapsed=$(($(date +%s) - start_time))
        echo "$elapsed|$ADJ_TIMEOUT" > "$PROGRESS_FILE"
        sleep 0.1
    done

    # Wait for process and get exit code
    wait $pid
    EXIT_CODE=$?

    echo "$ADJ_TIMEOUT|$ADJ_TIMEOUT" > "$PROGRESS_FILE"

    FAILED=0
    RESULT=""

    # Check for crash or timeout
    if [ $EXIT_CODE -eq 124 ]; then
        if [ "$EXPECTED" = "NO_DEATH" ]; then
            EXIT_CODE=0
        else
            FAILED=1
            RESULT="Timeout (deadlock or infinite loop)"
        fi
    fi

    if [ $EXIT_CODE -ne 0 ] && [ $FAILED -eq 0 ]; then
        FAILED=1
        RESULT="Program crashed (exit code $EXIT_CODE)"
    else
        # Validate based on expected behavior
        case "$EXPECTED" in
            "SHOULD_DIE")
                if grep -q "died" "$LOG"; then
                    DEATH_LINE=$(grep "died" "$LOG" | head -1)

                    # Timestamp: prefer "123ms" (enhanced), fallback to first field (standard)
                    if echo "$DEATH_LINE" | grep -q "ms"; then
                        DEATH_TIME=$(echo "$DEATH_LINE" | grep -oE '[0-9]+ms' | head -1 | tr -d 'ms')
                    else
                        DEATH_TIME=$(echo "$DEATH_LINE" | awk '{print $1}')
                    fi
                    if [ -z "$DEATH_TIME" ]; then
                        # Last resort: first number anywhere
                        DEATH_TIME=$(echo "$DEATH_LINE" | grep -oE '[0-9]+' | head -1)
                    fi
                    DEATH_TIME=${DEATH_TIME:-0}

                    # Philosopher id: prefer "Philo   N" (enhanced), fallback to second field (standard)
                    PHILO_ID=$(echo "$DEATH_LINE" | grep -oE 'Philo[[:space:]]+[0-9]+' | grep -oE '[0-9]+' | head -1)
                    if [ -z "$PHILO_ID" ]; then
                        PHILO_ID=$(echo "$DEATH_LINE" | awk '{print $2}')
                    fi
                    if [ -z "$PHILO_ID" ]; then
                        # Last resort: second number anywhere
                        PHILO_ID=$(echo "$DEATH_LINE" | grep -oE '[0-9]+' | sed -n '2p')
                    fi
                    PHILO_ID=${PHILO_ID:-1}

                    # Find last eating line for this philosopher (supports both formats)
                    EATING_LINE=$(grep "is eating" "$LOG" | grep -E "Philo[[:space:]]+${PHILO_ID}[[:space:]]" | tail -1)
                    if [ -z "$EATING_LINE" ]; then
                        EATING_LINE=$(grep -E "^[[:space:]]*[0-9]+[[:space:]]+${PHILO_ID}[[:space:]]+is eating" "$LOG" | tail -1)
                    fi

                    if [ -n "$EATING_LINE" ]; then
                        if echo "$EATING_LINE" | grep -q "ms"; then
                            LAST_MEAL=$(echo "$EATING_LINE" | grep -oE '[0-9]+ms' | head -1 | tr -d 'ms')
                        else
                            LAST_MEAL=$(echo "$EATING_LINE" | awk '{print $1}')
                        fi
                        if [ -z "$LAST_MEAL" ]; then
                            LAST_MEAL=$(echo "$EATING_LINE" | grep -oE '[0-9]+' | head -1)
                        fi
                        LAST_MEAL=${LAST_MEAL:-0}
                        ACTUAL_DEATH_DELAY=$((DEATH_TIME - LAST_MEAL))
                    else
                        # Never ate: compare absolute death time to TT_DIE
                        ACTUAL_DEATH_DELAY=$DEATH_TIME
                    fi

                    EXPECTED_DEATH_TIME=$TT_DIE
                    DEATH_DIFF=$((ACTUAL_DEATH_DELAY - EXPECTED_DEATH_TIME))
                    if [ $DEATH_DIFF -lt 0 ]; then
                        DEATH_DIFF=$((DEATH_DIFF * -1))
                    fi

                    if [ $DEATH_DIFF -gt $DEATH_TOLERANCE_MS ]; then
                        FAILED=1
                        RESULT="Death timing incorrect (${ACTUAL_DEATH_DELAY}ms vs ${EXPECTED_DEATH_TIME}ms)"
                    fi
                else
                    FAILED=1
                    RESULT="No philosopher died (expected death)"
                fi
                ;;
            "NO_DEATH")
                if grep -q "died" "$LOG"; then
                    FAILED=1
                    RESULT="A philosopher died (unexpected)"
                fi
                ;;
            "COMPLETE_MEALS")
                if grep -q "died" "$LOG"; then
                    FAILED=1
                    RESULT="A philosopher died (unexpected)"
                else
                    TOTAL_MEALS=$(grep "is eating" "$LOG" | wc -l)
                    EXPECTED_TOTAL=$((N_PHILO * N_MEALS))

                    if [ $TOTAL_MEALS -lt $EXPECTED_TOTAL ]; then
                        FAILED=1
                        RESULT="Insufficient meals ($TOTAL_MEALS vs $EXPECTED_TOTAL)"
                    fi
                fi
                ;;
        esac
    fi

    # Write result to file
    if [ $FAILED -eq 0 ]; then
        echo "PASS|$RESULT" > "$STATUS_FILE"
    else
        echo "FAIL|$RESULT" > "$STATUS_FILE"
    fi
}

# Main parallel execution loop
echo -e "Running ${BLUE}$TOTAL_TESTS${NC} tests (max ${BLUE}$MAX_PARALLEL${NC} in parallel)...\n"

# Reserve fixed progress window
for i in $(seq 1 $MAX_PARALLEL); do
    echo ""
done
PROGRESS_WINDOW=$MAX_PARALLEL

PIDS=()
INDICES=()

CURRENT_TEST=0
while [ $CURRENT_TEST -lt $TOTAL_TESTS ] || [ ${#PIDS[@]} -gt 0 ]; do
    # Start new tests up to MAX_PARALLEL
    while [ ${#PIDS[@]} -lt $MAX_PARALLEL ] && [ $CURRENT_TEST -lt $TOTAL_TESTS ]; do
        run_single_test $CURRENT_TEST &
        PIDS+=($!)
        INDICES+=($CURRENT_TEST)
        ((CURRENT_TEST++))
    done

    # Move cursor up to the start of progress window
    printf "\033[%dA" "$PROGRESS_WINDOW"

    FINISHED_IDX=()

    # Render exactly PROGRESS_WINDOW lines
    for row in $(seq 0 $((PROGRESS_WINDOW - 1))); do
        if [ $row -lt ${#PIDS[@]} ]; then
            pid=${PIDS[$row]}
            test_idx=${INDICES[$row]}
            test_case="${TESTS[$test_idx]}"
            IFS='|' read -r ARGS EXPECTED TIMEOUT DESC <<< "$test_case"

            # Detect finished
            if ! kill -0 "$pid" 2>/dev/null; then
                FINISHED_IDX+=("$row")
                printf "\033[K\n"
                continue
            fi

            # Read progress
            progress_file="$TMPDIR/progress_$((test_idx + 1)).txt"
            elapsed=0
            total=$TIMEOUT
            if [ -f "$progress_file" ]; then
                IFS='|' read -r elapsed total < "$progress_file"
                if [ -z "$total" ] || [ "$total" -le 0 ]; then total=$TIMEOUT; fi
                if [ -z "$elapsed" ] || [ "$elapsed" -lt 0 ]; then elapsed=0; fi
            fi

            # Print progress line
            printf "${YELLOW}[${CYAN}Test $((test_idx + 1))${YELLOW}]${NC} ${DESC:0:45}... "
            show_progress "$elapsed" "$total"
            printf "\033[K\n"
        else
            # Empty slot
            printf "\033[K\n"
        fi
    done

    # Remove finished tests
    if [ ${#FINISHED_IDX[@]} -gt 0 ]; then
        for i in "${FINISHED_IDX[@]}"; do
            wait ${PIDS[$i]} 2>/dev/null
            unset 'PIDS[$i]'
            unset 'INDICES[$i]'
        done
        PIDS=("${PIDS[@]}")
        INDICES=("${INDICES[@]}")
    fi

    sleep 0.1
done

printf "\n"

# Collect and display results
echo -e "${BLUE}╔════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║              Test Results                  ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════╝${NC}"
echo

for i in $(seq 0 $((TOTAL_TESTS - 1))); do
    test_case="${TESTS[$i]}"
    IFS='|' read -r ARGS EXPECTED TIMEOUT DESC <<< "$test_case"

    result_file="$TMPDIR/status_$((i + 1)).txt"
    if [ -f "$result_file" ]; then
        IFS='|' read -r status message < "$result_file"

        if [ "$status" = "PASS" ]; then
            echo -e "${GREEN}✅ Test $((i + 1)):${NC} $DESC"
            ((PASSED_TESTS++))
        else
            echo -e "${RED}❌ Test $((i + 1)):${NC} $DESC"
            echo -e "   ${GRAY}$message${NC}"
            ((FAILED_TESTS++))
        fi
    fi
done

echo
echo -e "${BLUE}╔════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║              Summary                       ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════╝${NC}"
echo -e "Total:  ${BLUE}$TOTAL_TESTS${NC}"
echo -e "${GREEN}Passed: $PASSED_TESTS${NC}"
echo -e "${RED}Failed: $FAILED_TESTS${NC}"
echo

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}🎉 All tests passed! 🎉${NC}"
    exit 0
else
    echo -e "${RED}❌ Some tests failed${NC}"
    exit 1
fi
