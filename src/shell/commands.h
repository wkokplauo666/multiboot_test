#ifndef COMMAND_H
#define COMMAND_H

#include "../drivers/video.h"
#include "../drivers/io.h"
#include "../memory/string.h"
#include "../interrupt/irq_driver.h"

void print_command(int argc, char **argv);
void info_command(int argc, char **argv);
void timer_command(int argc, char **argv);
void halt_command(int argc, char **argv);
void clear_command(int argc, char **argv);
void ebda_command(int argc, char **argv);

#endif