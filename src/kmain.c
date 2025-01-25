#include "boot/multiboot.h"
#include "drivers/io.h"
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
  (void)size;

  init_serial();

  u32 faddr;
  u32 fwidth;
  u32 fheight;
  u32 fbpp; // bytes per pixel

  u32 iwidth = 800;
  u32 iheight = 600;
  u32 iaddr;
  u32 iend;

  for (tag = (struct multiboot_tag *)(addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag +
                                      ((tag->size + 7) & ~7))) {

    serial_puts("Recieved Tag type: ");
    serial_puts(itoa(tag->type));
    serial_puts("\n");
    switch (tag->type) {
    case MULTIBOOT_TAG_TYPE_CMDLINE:
      printf ("Command line = %s\n",
                  ((struct multiboot_tag_string *) tag)->string);
      break;
      
    case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
      printf ("Boot loader name = %s\n",
                  ((struct multiboot_tag_string *) tag)->string);
      break;
    case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
      serial_puts("frame buffer tag recieved.\n");
      struct multiboot_tag_framebuffer *frame =
          (struct multiboot_tag_framebuffer *)tag;
      serial_puts(itohex(frame->common.framebuffer_type));
      serial_puts("\n");

      faddr = (u32)frame->common.framebuffer_addr;
      fwidth = frame->common.framebuffer_width;
      fheight = frame->common.framebuffer_height;
      fbpp = frame->common.framebuffer_bpp / 8;

      serial_puts("Address: ");
      serial_puts(itohex(faddr));

      serial_puts(" Width: ");
      serial_puts(itoa(fwidth));

      serial_puts(" Height: ");
      serial_puts(itoa(fheight));

      serial_puts(" Bytes per pixel: ");
      serial_puts(itoa(fbpp));
      serial_puts("\n");

      printf("Calculated pitch: %d, recieved pitch: %d\n", fwidth*fbpp, frame->common.framebuffer_pitch);

      break;
    case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
      serial_puts("Basic meminfo tag recieved.\n");
      struct multiboot_tag_basic_meminfo *meminfo =
          (struct multiboot_tag_basic_meminfo *)tag;
      serial_puts("Total memory size: ");
      u32 low = meminfo->mem_lower;
      u32 high = meminfo->mem_upper;
      serial_puts(itoa((high - low) / 1000));
      serial_puts("MB.\n");
      break;

    case MULTIBOOT_TAG_TYPE_MODULE:
      serial_puts("Found an image module.\n");
      struct multiboot_tag_module *module = (struct multiboot_tag_module *)tag;
      iaddr = module->mod_start;
      iend = module->mod_end;

      serial_puts("Address: ");
      serial_puts(itohex(module->mod_start));

      serial_puts(" Cmdline: ");
      serial_puts(module->cmdline);
      serial_puts("\n");
      break;

      case MULTIBOOT_TAG_TYPE_MMAP:
          {
            multiboot_memory_map_t *mmap;

            printf("mmap\n");
      
            for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
                 (multiboot_uint8_t *) mmap 
                   < (multiboot_uint8_t *) tag + tag->size;
                 mmap = (multiboot_memory_map_t *) 
                   ((unsigned long) mmap
                    + ((struct multiboot_tag_mmap *) tag)->entry_size))
              printf (" base_addr = 0x%x%x,"
                      " length = 0x%x%x, type = 0x%x\n",
                      (unsigned) (mmap->addr >> 32),
                      (unsigned) (mmap->addr & 0xffffffff),
                      (unsigned) (mmap->len >> 32),
                      (unsigned) (mmap->len & 0xffffffff),
                      (unsigned) mmap->type);
          }
          break;
    }

    
  }
  (void)iend;
  if (fbpp == 3) {
    for (unsigned int i = 0; i < fheight; i++) {
      for (unsigned int j = 0; j < fwidth; j++) {
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 2] = 0xff;
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 1] = 0xaa; // BGR color mode
      }
    }

    for (unsigned int i = 0; i < iheight; i++) {
      for (unsigned int j = 0; j < iwidth; j++) {
        u32 index = (j + i * iwidth) * 3;
        // if(index >= (iend - iaddr)) break;
        // u32 color = ((u32 *)iaddr)[index];
        u8 red = ((u8 *)iaddr)[index];
        u8 green = ((u8 *)iaddr)[index + 1];
        u8 blue = ((u8 *)iaddr)[index + 2];

        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 2] = red;

        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 1] = blue;
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 0] = green;
      }
    }
  } else if(fbpp == 4) {
    for (unsigned int i = 0; i < fheight; i++) {
      for (unsigned int j = 0; j < fwidth; j++) {
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 2] = 0xff;
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 1] = 0xaa; // BGR color mode
      }
    }

    for (unsigned int i = 0; i < iheight; i++) {
      for (unsigned int j = 0; j < iwidth; j++) {
        u32 index = (j + i * iwidth) * 3;
        // if(index >= (iend - iaddr)) break;
        // u32 color = ((u32 *)iaddr)[index];
        u8 red = ((u8 *)iaddr)[index];
        u8 green = ((u8 *)iaddr)[index + 1];
        u8 blue = ((u8 *)iaddr)[index + 2];

        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 3] = 0;
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 2] = red;
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 1] = blue;
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 0] = green;
      }
    }
  }

  // print("S1", itohex(43), " S3", " S4\n");

  // addr = (void *)0xc000;
  // parse_all(addr);
  return;
}