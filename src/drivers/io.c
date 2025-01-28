#include "io.h"
#include "../drivers/video.h"

int serial_empty() {
    return inb(COM1 + 5) &  LSR_EMPTY;
}

void serial_putc(const char c) {
    while(!serial_empty());
    outb(COM1, c);
}

void init_fpu() {
	asm volatile("finit" ::: "memory");
}

u8 inb(u16 port) {
	u8 res;

	__asm__("in %%dx, %%al" : "=a" (res) : "d" (port));
	return res;
}

void outb(u16 port, u8 data) {
	__asm__ __volatile__("out %%al, %%dx" : : "a" (data), "d" (port));
}


void serial_puts(const char *str) {
    for(int i = 0; str[i] != 0; i++) {
        serial_putc(str[i]);
        if(str[i] == '\n') {
            serial_putc('\r');
        }
        
    }
}

void init_serial() {
    outb(COM1 + 1, 0x00); //no irqs
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03); // 38400 baud
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03); //8bits, no parity, 1 stop bit
    outb(COM1 + 2, 0xc7); //FIFO with 14 byte threshold
    outb(COM1 + 4, 0x0b); //IRQ enabled
}

int strcmp(const char *a, const char *b) {
    while(*a && (*a == *b)) {
        a++;
        b++;
    }
    return (u8)(*a) - (u8)(*b);
}

static void itoa2 (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
  
  /*  If %d is specified and D is minus, put ‘-’ in the head. */
  if (base == 'd' && d < 0)
    {
      *p++ = '-';
      buf++;
      ud = -d;
    }
  else if (base == 'x')
    divisor = 16;

  /*  Divide UD by DIVISOR until UD == 0. */
  do
    {
      int remainder = ud % divisor;
      
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
  while (ud /= divisor);

  /*  Terminate BUF. */
  *p = 0;
  
  /*  Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}

void putchar(int c) {
  
  serial_putc((char)c);
  kprint_c((char)c, 0x0f);
  if(c == '\n') {
    serial_putc('\r');
    kprint_c('\r', 0x0f);
  }
}

void printf (const char *format, ...)
{
  char **arg = (char **) &format;
  int c;
  char buf[20];

  arg++;
  
  while ((c = *format++) != 0)
    {
      if (c != '%')
        putchar (c);
      else
        {
          char *p, *p2;
          int pad0 = 0, pad = 0;
          
          c = *format++;
          if (c == '0')
            {
              pad0 = 1;
              c = *format++;
            }

          if (c >= '0' && c <= '9')
            {
              pad = c - '0';
              c = *format++;
            }

          switch (c)
            {
            case 'd':
            case 'u':
            case 'x':
              itoa2 (buf, c, *((int *) arg++));
              p = buf;
              goto string;
              break;

            case 's':
              p = *arg++;
              if (! p)
                p = "(null)";

            string:
              for (p2 = p; *p2; p2++);
              for (; p2 < p + pad; p2++)
                putchar (pad0 ? '0' : ' ');
              while (*p)
                putchar (*p++);
              break;

            default:
              putchar (*((int *) arg++));
              break;
            }
        }
    }
}

void putchar2(int c, u8 attr) {
  
  serial_putc((char)c);
  kprint_c((char)c, attr);
  if(c == '\n') {
    serial_putc('\r');
    kprint_c('\r', 0x0f);
  }
}

void printf2(const char *format, ...)
{
  char **arg = (char **) &format;
  int c;
  char buf[20];
  u8 attribute = 0x7;
  int colored = 0;
  arg++;
  
  while ((c = *format++) != 0)
    {
      if (c != '%')
        putchar2 (c, attribute);
      else
        {
          char *p, *p2;
          int pad0 = 0, pad = 0;
          
          c = *format++;
          if(c == 'a') { 

            if(!colored) {
            switch(*format) {
              case '0': 
                colored = 1;
                attribute = 0;
                break;
              case '1': 
                colored = 1;
                attribute = 1;
                break;
                case '2': 
                colored = 1;
                attribute = 2;
                break;
              case '3': 
                colored = 1;
                attribute = 3;
                break;
                case '4': 
                colored = 1;
                attribute = 4;
                break;
              case '5': 
                colored = 1;
                attribute = 5;
                break;
              case '6': 
                colored = 1;
                attribute = 6;
                break;
              case '7': 
                colored = 1;
                attribute = 7;
                break;
              case '8': 
                colored = 1;
                attribute = 8;
                break;
              case '9': 
                colored = 1;
                attribute = 9;
                break;
              case 'a': 
                colored = 1;
                attribute = 10;
                break;
              case 'b': 
                colored = 1;
                attribute = 11;
                break;
              case 'c': 
                colored = 1;
                attribute = 12;
                break;
              case 'd': 
                colored = 1;
                attribute = 13;
                break;
              case 'e': 
                colored = 1;
                attribute = 14;
                break;
              case 'f': 
                colored = 1;
                attribute = 15;
                break;

              default: 
                colored = 0;
                attribute = 0x7;
                break;
            }
          }

            if(*(format) == 's') {
              colored = 0;
              attribute = 0x7;
            }
            format++;
            continue;
          }



          if (c == '0')
            {
              pad0 = 1;
              c = *format++;
            }

          if (c >= '0' && c <= '9')
            {
              pad = c - '0';
              c = *format++;
            }

          switch (c)
            {
            case 'd':
            case 'u':
            case 'x':
              itoa2 (buf, c, *((int *) arg++));
              p = buf;
              goto string;
              break;
            case 's':
              p = *arg++;
              if (! p)
                p = "(null)";

            string:
              for (p2 = p; *p2; p2++);
              for (; p2 < p + pad; p2++)
                putchar2 (pad0 ? '0' : ' ', attribute);
              while (*p)
                putchar2 (*p++, attribute);
              break;

            default:
              putchar2 (*((int *) arg++), attribute);
              break;
            }
        }
    }
}

void print_float(float input) {
  int ipart = (int)input;
  float fpart = input - ipart;
  printf("%d", ipart);
  printf(".");
  fpart *= 1000000;
  int fint = (int)(fpart + 0.5);
  printf("%d", fint);
}

float get_time() {
  return (float)(get_tick() * 3) / 100.0;
}