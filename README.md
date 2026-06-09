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
./codexion 199 340 100 100 100 3 10 edf
```

## Blocking cases handled
- **Deadlock prevention**: with taking both dongles at once and also by using lower id to avoiding circular wait and satisfying Coffman’s deadlock-prevention strategy.
- **Starvation prevention**: each dongle send a request to the queue with scheduling (FIFO or EDF) so that waiting coders are served.
- **Cooldown handling**: both dongles cannot be taken until their cooldown expires.
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
  - Dongles uses ordered locking to prevent races between competing coders.
  - Queue operations are always done while holding the dongle mutex.

  **Example**
  ```C
	int	take_both_dongles(t_coder *coder)
	{
		if (coder->left_dongle->id < coder->right_dongle->id)
		{
			pthread_mutex_lock(&coder->left_dongle->mutex);
			pthread_mutex_lock(&coder->right_dongle->mutex);
		}
		else
		{
			pthread_mutex_lock(&coder->right_dongle->mutex);
			pthread_mutex_lock(&coder->left_dongle->mutex);
		}
		if (check_can_take_dongle(coder))
		{
			take_and_pop(coder);
			pthread_mutex_unlock(&coder->left_dongle->mutex);
			pthread_mutex_unlock(&coder->right_dongle->mutex);
			print_and_pop_dongles(coder);
			return (1);
		}
		pthread_mutex_unlock(&coder->left_dongle->mutex);
		pthread_mutex_unlock(&coder->right_dongle->mutex);
		return (0);
	}
  ```

## Resources
- [Threads 1](https://kuleuven-diepenbeek.github.io/osc-course/ch6-tasks/threads/)
- [Threads 2 (Youtube playlist)](https://www.youtube.com/watch?v=d9s_d28yJq0)
- [Concurrency with Pthread](https://dev.to/emanuelgustafzon/mastering-concurrency-in-c-with-pthreads-a-comprehensive-guide-56je)
- [Priority queue](https://www.slideshare.net/slideshow/priority-queue-using-heap-using-c-in-data-structure/269400140)
- [Heaps and Heap Sort](https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-fall-2011/resources/lecture-4-heaps-and-heap-sort/)

### AI usage
- Help me with creating this README file
- Explain more things in details of some concepts
- Helped me in code enhancement