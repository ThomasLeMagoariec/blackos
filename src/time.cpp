#include "time.hpp"
#include "io.h"
#include "irq.h"
#include "memory.hpp"
#include "stdio.h"

time_ctx t_ctx;

void register_timeevent(time_event event) {
    int num = t_ctx.numHandlers == 0 ? 1 : t_ctx.numHandlers;

    t_ctx.handlers = (time_event*)realloc(t_ctx.handlers, sizeof(time_event) * num);
    t_ctx.handlers[t_ctx.numHandlers] = event;
    t_ctx.numHandlers++;
}

void	timer_main_event() {
	t_ctx.timer_ticks++;
	
	if (t_ctx.timer_ticks % 100 == 0) {
		t_ctx.time_s++;
	}

    for (int i = 0; i < t_ctx.numHandlers; i++) {
        t_ctx.handlers[i]();
    }
}

void	timer_phase(int hz) {
	// from http://www.osdever.net/bkerndev/Docs/pit.htm
	int divisor = 1193180 / hz;      	 			/* Calculate our divisor */
	i686_outb(0x43, 0x36);             /* Set our command byte 0x36 */
	i686_outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
	i686_outb(0x40, divisor >> 8);
}

void	timer_init() {
	timer_phase(100);
	register_timeevent(timer_main_event);

	t_ctx.handlers = NULL;
	t_ctx.numHandlers = 0;
	t_ctx.timer_ticks = 0;
}
