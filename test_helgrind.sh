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

# Check if helgrind is available
if ! command -v valgrind &> /dev/null; then
    echo -e "${RED}Valgrind not found. Please install valgrind.${NC}"
    exit 1
fi

# Test cases format: "n_philo time_to_die time_to_eat time_to_sleep [n_meals]|TIMEOUT|DESCRIPTION"
declare -a TESTS=(
    "2 410 200 200|3|Two philosophers - race check"
    "4 310 200 100|2|Four philosophers - race check"
    "5 800 200 200 7|5|Five philosophers - 7 meals race check"
    "4 410 200 200 5|5|Four philosophers - 5 meals race check"
)

MAX_PARALLEL=2

echo -e "${BLUE}╔════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║    Data Race Detection (Helgrind)         ║${NC}"
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

# Progress bar function
show_progress() {
    local current=$1
    local total=$2
    local width=28

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

# Run helgrind test
run_single_test() {
    local test_idx=$1
    local test_case="${TESTS[$test_idx]}"
    
    IFS='|' read -r ARGS TIMEOUT DESC <<< "$test_case"
    LOG="$TMPDIR/test_$((test_idx + 1)).log"
    STATUS_FILE="$TMPDIR/status_$((test_idx + 1)).txt"
    PROGRESS_FILE="$TMPDIR/progress_$((test_idx + 1)).txt"
    
    # Initialize progress file
    echo "0|$TIMEOUT" > "$PROGRESS_FILE"
    
    # Run with helgrind and timeout
    timeout "$TIMEOUT" valgrind --tool=helgrind --log-file="$LOG" "$PHILO" $ARGS > /dev/null 2>&1 &
    local pid=$!
    local start_time=$(date +%s)
    
    # Monitor the process
    while kill -0 $pid 2>/dev/null; do
        local elapsed=$(($(date +%s) - start_time))
        echo "$elapsed|$TIMEOUT" > "$PROGRESS_FILE"
        sleep 0.2
    done
    
    wait $pid
    EXIT_CODE=$?
    
    echo "$TIMEOUT|$TIMEOUT" > "$PROGRESS_FILE"
    
    FAILED=0
    RESULT=""
    
    # Check for timeout
    if [ $EXIT_CODE -eq 124 ]; then
        FAILED=1
        RESULT="Timeout"
    elif [ $EXIT_CODE -ne 0 ]; then
        FAILED=1
        RESULT="Valgrind error (exit code $EXIT_CODE)"
    else
        # Check for data races in helgrind output
        RACE_COUNT=$(grep -c "Possible data race" "$LOG" 2>/dev/null || echo 0)
        
        if [ "$RACE_COUNT" -gt 0 ]; then
            FAILED=1
            RESULT="Found $RACE_COUNT data race(s)"
        else
            RESULT="No data races detected"
        fi
    fi
    
    # Write result to file
    if [ $FAILED -eq 0 ]; then
        echo "PASS|$RESULT" > "$STATUS_FILE"
    else
        echo "FAIL|$RESULT" > "$STATUS_FILE"
    fi
}

# Main parallel execution loop
echo -e "Running ${BLUE}$TOTAL_TESTS${NC} helgrind tests (max ${BLUE}$MAX_PARALLEL${NC} in parallel)...\n"
echo -e "${YELLOW}Note: Helgrind tests are slower than normal execution${NC}\n"

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
            IFS='|' read -r ARGS TIMEOUT DESC <<< "$test_case"

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

    sleep 0.2
done

printf "\n"

# Collect and display results
echo -e "${BLUE}╔════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║              Test Results                  ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════╝${NC}"
echo

for i in $(seq 0 $((TOTAL_TESTS - 1))); do
    test_case="${TESTS[$i]}"
    IFS='|' read -r ARGS TIMEOUT DESC <<< "$test_case"
    
    result_file="$TMPDIR/status_$((i + 1)).txt"
    if [ -f "$result_file" ]; then
        IFS='|' read -r status message < "$result_file"
        
        if [ "$status" = "PASS" ]; then
            echo -e "${GREEN}✅ Test $((i + 1)):${NC} $DESC"
            echo -e "   ${GRAY}$message${NC}"
            ((PASSED_TESTS++))
        else
            echo -e "${RED}❌ Test $((i + 1)):${NC} $DESC"
            echo -e "   ${GRAY}$message${NC}"
            # Show some race details
            log_file="$TMPDIR/test_$((i + 1)).log"
            if [ -f "$log_file" ]; then
                echo -e "   ${YELLOW}First race location:${NC}"
                grep -A 5 "Possible data race" "$log_file" | head -10 | sed 's/^/   /'
            fi
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
    echo -e "${GREEN}🎉 No data races detected! 🎉${NC}"
    exit 0
else
    echo -e "${RED}❌ Data races detected${NC}"
    echo -e "${YELLOW}Review helgrind logs in $TMPDIR for details${NC}"
    exit 1
fi
