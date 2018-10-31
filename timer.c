/* © Algirdas Palubinskas */
/* Fair Use */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "timer.h"

typedef enum timer_flags {
	TIMER_F_ZERO = 0,
	TIMER_F_START = 1,
	TIMER_F_PAUSE = 2
} timer_flags;

void* timer_init(){
	timer_type *timer = malloc(sizeof(timer_type));

	timer->flags = 0;
	timer->start_ticks = 0;
	timer->pause_ticks = 0;

	return timer;
}

void timer_start(timer_type *timer){
    timer->flags = TIMER_F_START;

    timer->start_ticks = SDL_GetTicks();
	timer->pause_ticks = 0;
}
void timer_stop(timer_type *timer){
    timer->flags = TIMER_F_ZERO;

	timer->start_ticks = 0;
	timer->pause_ticks = 0;
}
void timer_pause(timer_type *timer){
    if((timer->flags & TIMER_F_START) != 0 && (timer->flags & TIMER_F_PAUSE) == 0){
        timer->flags = timer->flags | TIMER_F_PAUSE;

        timer->pause_ticks = SDL_GetTicks() - timer->start_ticks;
		timer->start_ticks = 0;
    }
}
void timer_unpause(timer_type *timer){
    if((timer->flags & TIMER_F_START) != 0 && (timer->flags & TIMER_F_PAUSE) != 0){
        timer->flags = timer->flags & ~TIMER_F_PAUSE;

        timer->start_ticks = SDL_GetTicks() - timer->pause_ticks;
        timer->pause_ticks = 0;
    }
}

uint timer_ticks(timer_type *timer){
	uint time = 0;

    if(timer_started(timer)){
        if(timer_paused(timer)){
            time = timer->pause_ticks;
        } else {
            time = SDL_GetTicks() - timer->start_ticks;
        }
    }

    return time;
}
inline uint timer_ticks_running(timer_type *timer){
	return SDL_GetTicks() - timer->start_ticks;
}

inline int timer_started(timer_type *timer){
    return (timer->flags & TIMER_F_START) != 0;
}
inline int timer_paused(timer_type *timer){
	return (timer->flags & TIMER_F_PAUSE) != 0;
}
