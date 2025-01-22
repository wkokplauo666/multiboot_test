#include "boot/multiboot.h"
#include "init.h"

extern void kmain();

void kmain() {
  struct multiboot_tag *tag;
  unsigned size = 0;
  u32 addr = 0;
  // struct multiboot2_header init = {0};
  // void *addr = &init;

  asm volatile("movl %%ebx, %0" : "=r"(addr)::"memory", "ebx");
  size = *(unsigned *)addr;

  for (tag = (struct multiboot_tag *)(addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag +
                                      ((tag->size + 7) & ~7))) {

    switch (tag->type) {
        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
            break;
    }
  }
  // addr = (void *)0xc000;
  // parse_all(addr);
  return;
}