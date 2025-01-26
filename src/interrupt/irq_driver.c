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
}

void init_timer(u32 freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    u32 divisor = 1193180 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)( (divisor >> 8) & 0xFF);
    /* Send the command */
    outb(0x43, 0x36); /* Command port */
    outb(0x40, low);
    outb(0x40, high);
}
