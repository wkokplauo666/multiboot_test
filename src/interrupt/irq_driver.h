#ifndef IRQDRIVER_H
#define IRQDRIVER_H

#include "../common.h"

void init_timer();
void start_timer(u32 freq);
void stop_timer();
u32 get_tick();

#endif