#!/bin/bash

# Colors for readability
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
NC="\033[0m" # No Color

# Your executable
PHILO="./philo"

# Test cases: (num_philos time_to_die time_to_eat time_to_sleep [num_meals])
declare -a TESTS = (
    "2 410 200 200"
    "5 800 200 200"
    "5 800 200 200 7"
    "4 310 100 100"
    "200 800 200 200"
)

TIMEOUT = 5 # seconds

echo -e "${YELLOW}=== Philosophers Tester ===${NC}"
echo

for TEST in "${TESTS[@]}"; do
    echo -e "🧩 Running test: ${TEST}"

    # Run with timeout to catch infinite loops or deadlocks
    timeout $TIMEOUT $PHILO $TEST > tmp.log 2>&1
    EXIT_CODE=$?

    if [ $EXIT_CODE -eq 124 ]; then
        echo -e "${RED}❌ Timeout!${NC} (Likely deadlock or infinite loop)"
    elif [ $EXIT_CODE -ne 0 ]; then
        echo -e "${RED}❌ Crash (exit code $EXIT_CODE)!${NC}"
    else
        # Basic check for unexpected output
        if grep -q "died" tmp.log; then
            echo -e "${YELLOW}⚠️  A philosopher died (expected?)${NC}"
        else
            echo -e "${GREEN}✅ OK${NC}"
        fi
    fi
    echo
done

rm -f tmp.log
echo -e "${YELLOW}=== Done! ===${NC}"
