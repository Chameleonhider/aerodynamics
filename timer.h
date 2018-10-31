/* © Algirdas Palubinskas */
/* Fair Use */

#ifndef TIMER_TYPE_HEADER_
#define TIMER_TYPE_HEADER_

typedef struct timer_type {
	uint start_ticks;
	uint pause_ticks;
	char flags;
} timer_type;

void* timer_init();
// Start the timer (zeroes the timer)
void timer_start(timer_type *timer);
// Stop the timer (zeroes and stops the timer)
void timer_stop(timer_type *timer);
// Pauses the timer
void timer_pause(timer_type *timer);
// Un-pauses the timer
void timer_unpause(timer_type *timer);

// Returns time from start to now, excluding paused time
uint timer_ticks(timer_type *timer);
// Returns time from start to now
// Does not check timer's state! Call only if 100% sure that the timer is running
uint timer_ticks_running(timer_type *timer);

// Checks if timer is non-zero (non-zero)
int timer_started(timer_type *timer);
// Checks if timer is running (non-zero, running)
int timer_paused(timer_type *timer);

#endif
