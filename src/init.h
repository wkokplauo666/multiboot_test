#ifndef INIT_H
#define INIT_H

#include "common.h"
#include "boot/multiboot.h"

struct video_info {
    u32 addr;
    u32 width;
    u32 height;
    u32 bytes_pp;
};

void parse_header(u32 addr);
void get_video_info(struct video_info *in, struct multiboot_tag_framebuffer_common tag);

#endif