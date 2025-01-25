#include "io.h"

int serial_empty() {
    return inb(COM1 + 5) &  LSR_EMPTY;
}

void serial_putc(const char c) {
    while(!serial_empty());
    outb(COM1, c);
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
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xc7);
    outb(COM1 + 4, 0x0b);
}


static char buffer[BUFFER_SIZE];

char *itoa(u32 input) {
    if (input == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return buffer;
    }
    int i = 0;

    while(input > 0 && i < BUFFER_SIZE - 1) {
        buffer[i++] = (input % 10) + '0';
        input /= 10;
    }

    buffer[i] = '\0';

    for(int j = 0, k = i - 1; j < k; ++j, --k) {
        char temp = buffer[j];
        buffer[j] = buffer[k];
        buffer[k] = temp;
    }

    return buffer;
}


char *itohex(u32 input) {
    if (input == 0) {
        buffer[0] = '0';
        buffer[1] = 'x';
        buffer[2] = '0';
        buffer[3] = '\0';
        return buffer;
    }

    const char *hex_digits = "0123456789ABCDEF";
    int i = 0;

    // Fill the buffer with hex digits (in reverse order)
    while (input > 0 && i < (int)(BUFFER_SIZE - 3)) { // Reserve 2 for "0x" and 1 for '\0'
        buffer[i++] = hex_digits[input % 16];
        input /= 16;
    }

    buffer[i++] = 'x';
    buffer[i++] = '0';

    buffer[i] = '\0'; // Null-terminate the string

    // Reverse the entire string (including the prefix)
    for (int j = 0, k = i - 1; j < k; ++j, --k) {
        char temp = buffer[j];
        buffer[j] = buffer[k];
        buffer[k] = temp;
    }

    return buffer;
}

void print(const char* s1, ...) {
    const char *cur = s1;
    const char **arg = &s1;

    while(cur) {
        serial_puts(cur);
        cur = *(++arg);
    }
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
  if(c == '\n') {
    serial_putc('\r');
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