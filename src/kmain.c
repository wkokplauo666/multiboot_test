#include "boot/multiboot.h"
#include "drivers/io.h"
#include "init.h"
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

  u32 iwidth = 400;
  u32 iheight = 400;
  u32 iaddr;
  u32 iend;

  int isVideo = 0;

  for (tag = (struct multiboot_tag *)(addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag +
                                      ((tag->size + 7) & ~7))) {

    printf("Recieved tag (%d): %s\n", tag->type, get_message(tag->type));
    switch (tag->type) {
    case MULTIBOOT_TAG_TYPE_CMDLINE:
      printf("Command line = %s\n",
             ((struct multiboot_tag_string *)tag)->string);
      break;

    case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
      printf("Boot loader name = %s\n",
             ((struct multiboot_tag_string *)tag)->string);
      break;
    case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
      serial_puts("frame buffer tag recieved.\n");
      struct multiboot_tag_framebuffer *frame =
          (struct multiboot_tag_framebuffer *)tag;
      serial_puts("Framebuffer type: ");
      serial_puts(itohex(frame->common.framebuffer_type));
      serial_puts("\n");
      if(frame->common.framebuffer_type != 2) {
        printf("Framebuffer is not in text mode!\n");
        isVideo = 1;
      } else {
        printf("Framebuffer is in text mode!\n");
        isVideo = 0;
      }
      

      faddr = (u32)frame->common.framebuffer_addr;
      fwidth = frame->common.framebuffer_width;
      fheight = frame->common.framebuffer_height;
      fbpp = frame->common.framebuffer_bpp / 8;
      fpitch = frame->common.framebuffer_pitch;
      
      printf("Address: 0x%x Resolution: %dx%d Bytes per pixel: %d\n", faddr, fwidth, fheight, fbpp);

      printf("Calculated pitch: %d, recieved pitch: %d\n", fwidth * fbpp,
             frame->common.framebuffer_pitch);

      break;
    case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
      serial_puts("Basic meminfo tag recieved.\n");
      struct multiboot_tag_basic_meminfo *meminfo =
          (struct multiboot_tag_basic_meminfo *)tag;
      serial_puts("Total memory size: ");
      u32 low = meminfo->mem_lower;
      u32 high = meminfo->mem_upper;
      serial_puts(itoa((high - low) / 1024));
      serial_puts("MB.\n");
      break;

    case MULTIBOOT_TAG_TYPE_MODULE:
      serial_puts("Found a module.\n");
      struct multiboot_tag_module *module = (struct multiboot_tag_module *)tag;

      u32 mod_start = module->mod_start;
      u32 mod_end = module->mod_end;

      printf("Module cmdline: %s, size: %dKB\n", module->cmdline,
             (mod_end - mod_start) / 1024);
      printf("Module address: 0x%x\n", mod_start);

      if (strcmp(module->cmdline, "IMAGE") == 0) {
        printf("Module is an image file!\n");
        iaddr = mod_start;
        iend = mod_end;
      }

      break;

    case MULTIBOOT_TAG_TYPE_MMAP: {
      multiboot_memory_map_t *mmap;

      printf("mmap\n");

      for (mmap = ((struct multiboot_tag_mmap *)tag)->entries;
           (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
           mmap = (multiboot_memory_map_t *)((unsigned long)mmap +
                                             ((struct multiboot_tag_mmap *)tag)
                                                 ->entry_size))
        printf(" base_addr = 0x%x%x,"
               " length = 0x%x%x, type = 0x%x\n",
               (unsigned)(mmap->addr >> 32),
               (unsigned)(mmap->addr & 0xffffffff), (unsigned)(mmap->len >> 32),
               (unsigned)(mmap->len & 0xffffffff), (unsigned)mmap->type);
    } break;
    }
  }
  (void)iend;
  if (fbpp == 3 && isVideo) {
    for (unsigned int i = 0; i < fheight; i++) {
      for (unsigned int j = 0; j < fwidth; j++) {
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 2] = (i * 255)/fheight;
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 1] = 0; // BGR color mode
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
  } else if (fbpp == 4 && isVideo) {
    int row = 0;
    for (unsigned int i = 0; i < fheight; i++) {
      for (unsigned int j = 0; j < fwidth; j++) {
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 2] = (i * 255) / fheight;
        ((u8 *)faddr)[(j + i * fwidth) * fbpp + 1] = 0; // BGR color mode

        int ij = ((iwidth * j)/fwidth);
        int ii = ((iheight * i)/fheight);

        int index = (ii * fwidth + ij) * 5;

        ((u8 *)faddr)[index] = 255;
        ((u8 *)faddr)[index + 1] = (ii * 255) / iheight;
        ((u8 *)faddr)[index + 2] = (ij * 255) / iwidth;
        ((u8 *)faddr)[index + 3] = 0xff; // BGR color mode
        //printf("Frame address: %dx%d, Image address: %dx%d\n", j, i, ij, ii);
        //printf("we are at index %d with position: %dx%d\n", ind, ij, ii);
      }
      row++;
    }

    for(unsigned int i = 0; i < fwidth && i < fheight; i++) {
      u32 *pixel = (u32 *)(faddr + fpitch * i + 4 * i);
      *pixel = 255;
    }

    for (unsigned int i = 0; i < iheight; i++) {
      for (unsigned int j = 0; j < iwidth; j++) {
        //u32 index = (j + i * iwidth) * 3;
        // if(index >= (iend - iaddr)) break;
        // u32 color = ((u32 *)iaddr)[index];
        //u8 red = ((u8 *)iaddr)[index];
        //u8 green = ((u8 *)iaddr)[index + 1];
        //u8 blue = ((u8 *)iaddr)[index + 2];

        /*((u8 *)faddr)[(j + i * iwidth) * fbpp + 3] = 0;
        ((u8 *)faddr)[(j + i * iwidth) * fbpp + 2] = 255;//red;
        ((u8 *)faddr)[(j + i * iwidth) * fbpp + 1] = 255;//blue;
        ((u8 *)faddr)[(j + i * iwidth) * fbpp + 0] = 255;//green;*/
      }
    }
  }

  //this should be in the efi-dev branch!

  return;
}