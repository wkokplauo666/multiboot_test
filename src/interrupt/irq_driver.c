#include "irq_driver.h"
#include "interrupt.h"
#include "../drivers/io.h"
#include "../drivers/video.h"
#include "../memory/string.h"

void print_letter(u8 sc);

u32 tick = 0;

static void timer_callback(registers_t regs) {
    (void)regs;
    tick++;
    printf("Tick: %d\n", tick);
    if(tick == 100) stop_timer();
}

void start_timer(u32 freq) {
    u32 divisor = 1193182 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)( (divisor >> 8) & 0xFF);
    /* Send the command */
    outb(0x43, 0x36); /* Command port */
    outb(0x40, low);
    outb(0x40, high);
}

void stop_timer() {
    outb(0x43, 0x36);
    outb(0x40, 0);
    outb(0x40, 0);
    irq_set_mask(0);
    tick = 0;
    printf("\n");
    shell();
}

void init_timer() {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);
    irq_clear_mask(0);
}

