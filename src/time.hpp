#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#ifdef __cplusplus
    extern "C" {
#endif
	#include "io.h"
#ifdef __cplusplus
    }
#endif

typedef void (*time_event)();

typedef struct {
    uint8_t numHandlers;
    time_event* handlers;
	int	timer_ticks;
	int time_s;
} time_ctx;

void	timer_init();
void register_timeevent(time_event event);
void	timer_main_event();

#endif  // TIME_H
