# Philosophers - Dining Philosophers Problem

A multithreaded simulation of the classic dining philosophers problem using mutexes.

## Features

- **Thread-safe simulation** with mutex protection
- **Precise timing** to avoid philosopher deaths
- **Debug mode** with enhanced visual logging
- **Automated testing** suite with parallel execution
- **Race condition detection** using Helgrind

## Building

```bash
make                # Build release version (enhanced output)
make DEBUG=1        # Build with debug mode + statistics
make SANITIZE=1     # Build with thread sanitizer
make clean          # Remove object files
make fclean         # Remove all build artifacts
make re             # Rebuild from scratch
```

## Usage

```bash
./bin/philo <n_philos> <time_to_die> <time_to_eat> <time_to_sleep> [n_meals]
```

### Arguments
- `n_philos`: Number of philosophers (1-200)
- `time_to_die`: Time in ms before a philosopher dies without eating
- `time_to_eat`: Time in ms a philosopher spends eating
- `time_to_sleep`: Time in ms a philosopher spends sleeping
- `n_meals`: (optional) Number of meals before simulation ends (-1 for infinite)

### Examples

```bash
# 5 philosophers, 800ms to die, 200ms eating, 200ms sleeping
./bin/philo 5 800 200 200

# 4 philosophers, must eat 5 times each
./bin/philo 4 410 200 200 5

# Single philosopher (will die - has only one fork)
./bin/philo 1 800 200 200
```

## Debug Mode

Enable debug mode by building with `make DEBUG=1`:

```bash
make fclean
make DEBUG=1
./bin/philo 5 800 200 200
```

### Debug Features

**Enhanced Logging:**
- Emoji indicators with aligned columns
- Millisecond-precision timestamps
- Color-coded actions
- Internal state transitions

**Comprehensive Statistics Summary:**
At the end of execution, debug mode displays:

```
╔══════════════════════════════════════════════════════╗
║           DEBUG SUMMARY - SIMULATION STATS           ║
╚══════════════════════════════════════════════════════╝

⏱️  TIMING INFORMATION:
   Total duration:       12450 ms
   First action:         0 ms
   Last action:          12450 ms

📊 ACTION STATISTICS:
   Total actions:        245
   Fork acquisitions:    120
   Meals eaten:          60
   Sleep cycles:         60
   Think cycles:         60

🍽️  PER-PHILOSOPHER BREAKDOWN:
   Philo   1:  12 meals eaten
   Philo   2:  12 meals eaten
   Philo   3:  12 meals eaten
   Philo   4:  12 meals eaten
   Philo   5:  12 meals eaten

⚙️  CONFIGURATION:
   Philosophers:         5
   Time to die:          800 ms
   Time to eat:          200 ms
   Time to sleep:        200 ms
   Required meals:       ∞ (unlimited)

✓ Simulation completed successfully
```

**Death Detection:**
When a philosopher dies, debug mode shows:
```
⏱️  TIMING INFORMATION:
   Death occurred:       801 ms
   Philosopher died:     #2
   Detection precision:  < 10ms ✓
```

### Output Modes

The program supports 3 output modes:

#### 1. Standard Output (ENHANCED_OUTPUT=0, DEBUG_MODE=0)
Required by 42 School, plain text format:
```
0 1 has taken a fork
0 2 has taken a fork
0 1 is eating
200 1 is sleeping
200 2 is eating
400 1 is thinking
400 2 is sleeping
```

#### 2. Enhanced Output (ENHANCED_OUTPUT=1, DEBUG_MODE=0) - DEFAULT
Colorful and visually appealing while maintaining timing accuracy:
```
🍴      0ms │ Philo   1 │ has taken a fork
🍴      0ms │ Philo   2 │ has taken a fork
🍝      0ms │ Philo   1 │ is eating
😴    200ms │ Philo   1 │ is sleeping
🍝    200ms │ Philo   2 │ is eating
🤔    400ms │ Philo   1 │ is thinking
😴    400ms │ Philo   2 │ is sleeping
```

Features:
- **Color-coded actions:**
  - 🍴 Fork (Yellow)
  - 🍝 Eating (Green)
  - � Sleeping (Cyan)
  - 🤔 Thinking (Magenta)
  - 💀 Death (Red)
- **Aligned columns** for better readability
- **Precise millisecond timing**
- **Philosopher ID** clearly visible

#### 3. Debug Mode (DEBUG_MODE=1)
Full debugging information with state tracking:
```
[ℹ️  INFO] [    0ms] Philo   1: Acquired left fork
🍴 [    0ms] Philo   1: has taken a fork
[ℹ️  INFO] [    1ms] Philo   1: Acquired right fork
🍴 [    1ms] Philo   1: has taken a fork
🍝 [    2ms] Philo   1: is eating
[� DEATH] [  800ms] Philo   2: Death detected by monitor
💀 [  800ms] Philo   2: died
```

### State System

The simulation uses an advanced state machine:

```c
typedef enum e_sim_state
{
    SIM_STATE_OK = 0,          // Initial state
    SIM_STATE_RUNNING = 1,     // Simulation in progress
    SIM_STATE_COMPLETED = 2,   // All meals completed
    SIM_STATE_DEATH = 3,       // Philosopher died
    SIM_STATE_INFO = 100,      // Info messages
    SIM_STATE_WARNING = 200,   // Warning conditions
    SIM_STATE_ERROR = 300      // Error conditions
}   t_sim_state;
```

**State transitions:**
- `SIM_STATE_OK` → `SIM_STATE_RUNNING` (on init)
- `SIM_STATE_RUNNING` → `SIM_STATE_COMPLETED` (all meals eaten)
- `SIM_STATE_RUNNING` → `SIM_STATE_DEATH` (philosopher dies)

**Logging levels:**
- `LOG_STATE` - Philosopher state changes
- `LOG_INFO` - Internal operations (fork acquisition)
- `LOG_WARNING` - Warning conditions
- `LOG_ERROR` - Error conditions
- `LOG_DEATH` - Death detection

## Testing

### Run Automated Tests
```bash
make run            # Run full test suite
./test.sh           # Manual test execution
```

### Test with Helgrind (Race Detection)
```bash
chmod +x test_helgrind.sh
./test_helgrind.sh
```

## Architecture

### Main Components

- **main.c**: Entry point, initialization, thread management
- **parse.c**: Argument parsing and validation
- **init.c**: Simulation setup and mutex initialization
- **routine.c**: Philosopher routine and monitoring threads
- **actions.c**: Fork management, eating, sleeping, thinking
- **logs.c**: State logging with debug support
- **sim_state.c**: Thread-safe simulation state management
- **helpers.c**: Death detection and meal counting

### Thread Safety

- **Fork mutexes**: One mutex per fork
- **Meal lock**: Per-philosopher mutex for meal data
- **Print lock**: Serializes output
- **State locks**: Protect simulation state flags

## License

This is a 42 School project. Educational use only.
