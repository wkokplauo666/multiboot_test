#include "init.h"

void get_video_info(struct video_info *in, struct multiboot_tag_framebuffer_common tag) {
    in->addr = tag.framebuffer_addr;
    in->width = tag.framebuffer_width;
    in->height = tag.framebuffer_height;
    in->bytes_pp = tag.framebuffer_bpp / 8;
    return;
}