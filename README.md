*This project has been created as part of the 42 curriculum by Acherifi.*

# Codexion

## Description
Codexion is a multithreaded simulation project. It models a set of coders competing for shared USB dongles and performing a compile–debug–refactor cycle while respecting burnout constraints and scheduling rules. The goal is to coordinate threads safely, avoid deadlocks or starvation, and stop the simulation when all coders have completed the required number of compiles or when a burnout occurs.

## Instructions

### Compilation
```bash
make
```

### Execution
```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

- `scheduler` must be either `fifo` or `edf`.
- All time values are in milliseconds.

### Example
```bash
./codexion 5 800 200 200 200 3 50 fifo
```

## Blocking cases handled
- **Deadlock prevention**: with taking both dongles at once and also by using lower id to avoiding circular wait and satisfying Coffman’s deadlock-prevention strategy.
- **Starvation prevention**: each dongle send a request to the queue with scheduling (FIFO or EDF) so that waiting coders are eventually served.
- **Cooldown handling**: dongles cannot be taken until their cooldown expires.
- **Precise burnout detection**: the monitor continuously checks the time since each coder’s last compile start to detect burnout.
- **Log serialization**: logging is guarded to avoid interleaved output during concurrent prints.

## Thread synchronization mechanisms
- **pthread_mutex_t** is used to protect all shared state:
  - `table->mutex` guards global flags (`stop`, `start_simulation`, `start_time`) and log serialization.
  - Each `coder->mutex` protects per-coder data such as `compile_count` and `last_compile_start`.
  - Each `dongle->mutex` protects dongle state (availability, queue operations, release time).
- **Thread-safe coordination between coders and monitor**:
  - Coders update their last compile timestamps under their own mutexes.
  - The monitor reads those timestamps under the same locks to avoid data race.
  - The global `stop` flag is checked and updated with `table->mutex` to ensure safety across threads.
- **Race condition prevention**:
  - Dongle acquisition uses ordered locking to prevent inconsistent states and races between competing coders.
  - Queue operations are always done while holding the dongle mutex.

## Resources
- POSIX Threads Programming (pthreads): official documentation and man pages (`pthread_mutex_*`, `pthread_create`, `pthread_join`)
- Classic concurrency references: dining philosophers problem, deadlock and Coffman’s conditions
- EDF and FIFO scheduling basics: introductory OS scheduling material

### AI usage
AI was used only to draft this README text based on the project requirements. It was not used to write or modify the project’s code, logic, or concurrency mechanisms.