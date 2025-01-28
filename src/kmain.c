#include "boot/multiboot.h"
#include "drivers/io.h"
#include "init.h"
#include "memory/memory.h"
#include "drivers/video.h"
#include "interrupt/interrupt.h"
#include "interrupt/irq_driver.h"
#include "drivers/keyboard.h"
#include "memory/string.h"

extern void kmain();

void kmain() {
  struct multiboot_tag *tag;
  unsigned size = 0;
  u32 addr = 0;
  // struct multiboot2_header init = {0};
  // void *addr = &init;

  asm volatile("movl %%ebx, %0" : "=r"(addr)::"memory", "ebx");
  size = *(unsigned *)addr;
  (void)size;

  init_serial();

  u32 faddr;
  u32 fwidth;
  u32 fheight;
  u32 fbpp; // bytes per pixel
  u32 fpitch;

  int isVideo = 0;
  for (tag = (struct multiboot_tag *)(addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag +
                                      ((tag->size + 7) & ~7))) {

    printf2("Recieved tag (%a3%d%as): %a6%s%as\n", tag->type, get_message(tag->type));
    switch (tag->type) {
    case MULTIBOOT_TAG_TYPE_CMDLINE:
      printf2("Command line = %s\n",
             ((struct multiboot_tag_string *)tag)->string);
      break;

    case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
      printf2("Boot loader name = %s\n",
             ((struct multiboot_tag_string *)tag)->string);
      break;
    case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
      printf2("frame buffer tag recieved.\n");
      struct multiboot_tag_framebuffer *frame =
          (struct multiboot_tag_framebuffer *)tag;

      printf2("Framebuffer type: 0x%x\n", frame->common.framebuffer_type);

      if(frame->common.framebuffer_type != 2) {
        printf2("Framebuffer is not in text mode!\n");
        isVideo = 1;
      } else {
        printf2("Framebuffer is in text mode!\n");
        isVideo = 0;
      }
      

      faddr = (u32)frame->common.framebuffer_addr;
      fwidth = frame->common.framebuffer_width;
      fheight = frame->common.framebuffer_height;
      fbpp = frame->common.framebuffer_bpp / 8;
      fpitch = frame->common.framebuffer_pitch;
      
      printf2("Address: 0x%x Resolution: %dx%d Bytes per pixel: %d\n", faddr, fwidth, fheight, fbpp);

      break;
    case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
      printf2("Basic meminfo tag recieved.\n");
      struct multiboot_tag_basic_meminfo *meminfo =
          (struct multiboot_tag_basic_meminfo *)tag;
      u32 low = meminfo->mem_lower;
      u32 high = meminfo->mem_upper;
      printf2("Low mem: 0x%x, High mem: 0x%x\n", low, high);
      printf2("  ->Total memory size is %dMB.\n", (high - low)/ 1024);

      break;

    case MULTIBOOT_TAG_TYPE_MODULE:
      printf2("Found a module.\n");
      struct multiboot_tag_module *module = (struct multiboot_tag_module *)tag;

      u32 mod_start = module->mod_start;
      u32 mod_end = module->mod_end;

      printf2("Module cmdline: %s, size: %dKB\n", module->cmdline,
             (mod_end - mod_start) / 1024);
      printf2("Module address: 0x%x\n", mod_start);

      break;

    case MULTIBOOT_TAG_TYPE_MMAP: {
      multiboot_memory_map_t *mmap;

      printf2("mmap\n");

      for (mmap = ((struct multiboot_tag_mmap *)tag)->entries;
           (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
           mmap = (multiboot_memory_map_t *)((unsigned long)mmap +
                                             ((struct multiboot_tag_mmap *)tag)
                                                 ->entry_size))
        printf2(" base_addr = 0x%x%x,"
               " length = 0x%x%x, type = 0x%x\n",
               (unsigned)(mmap->addr >> 32),
               (unsigned)(mmap->addr & 0xffffffff), (unsigned)(mmap->len >> 32),
               (unsigned)(mmap->len & 0xffffffff), (unsigned)mmap->type);
    } break;
    }
  }
  (void)fpitch;
  if(isVideo) return;

  printf2("\nInstalling %aaInterrupts...%as\n");
  isr_install();

  asm volatile("sti");
  init_keyboard();

	printf2("%a8Welcome to RandomOS. Contributors: %as\n");
  printf2("%a8  1.%as %acNarada Fox%as\n");
  //printf2("%a8  2.%as %acHawaryu Warsono%as\n");

  //printf("%08x\n", 0xff);
  init_timer();
  start_timer(4); //100 ticks = 1 second

  init_fpu();

	printf("\n");
  shell();

  return;
}