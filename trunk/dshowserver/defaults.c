#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifdef __MINGW32__
#include <windows.h>
#endif

#include "dshow/DSVD_extern.h"
#include "defaults.h"

struct codecs {
    char     name[64];
    uint32_t width;
    uint32_t height;
    uint32_t fourcc;
    uint32_t outfmt;
    uint32_t outbits;
    DS_GUID  guid;
};

struct codecs known_codecs[] = {
    {"CoreAVCDecoder.ax", 1280, 720, 0x34363248, 0x30323449, 12,
        {0x09571a4b, 0xf1fe, 0x4c60, {0x97, 0x60, 0xde, 0x6d, 0x31, 0x0c, 0x7c, 0x31}}
    },
};

void get_default(const char *str, int param_num, void *param)
{
    struct codecs *codec = &known_codecs[0];

    while(codec->name[0]) {
        if(strstr(str, codec->name))
            break;
        codec++;
    }
    if(! codec->name[0]) {
        printf("Defaults requested, but codec %s is unknown\n", str);
        exit(1);
    }
    switch(param_num) {
        case PARAM_WIDTH:
            printf("Using default width  for %s: %d\n", codec->name, codec->width);
            *(uint32_t *)param = codec->width;
            break;
        case PARAM_HEIGHT:
            printf("Using default height for %s: %d\n", codec->name, codec->height);
            *(uint32_t *)param = codec->height;
            break;
        case PARAM_FOURCC:
            printf("Using default fourcc for %s: 0x%08x\n", codec->name, codec->fourcc);
            *(uint32_t *)param = codec->fourcc;
            break;
        case PARAM_OUTFMT:
            printf("Using default outfmt for %s: 0x%08x\n", codec->name, codec->outfmt);
            *(uint32_t *)param = codec->fourcc;
            break;
        case PARAM_OUTBITS:
            printf("Using default outbit for %s: %d\n", codec->name, codec->outbits);
            *(uint32_t *)param = codec->outbits;
            break;
        case PARAM_GUID:
            printf("Using default GUID   for %s: %08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x\n", codec->name,
                   codec->guid.f1, codec->guid.f2, codec->guid.f3,
                   codec->guid.f4[0], codec->guid.f4[1], codec->guid.f4[2], codec->guid.f4[3],
                   codec->guid.f4[4], codec->guid.f4[5], codec->guid.f4[6], codec->guid.f4[7]);
            *(DS_GUID *)param = codec->guid;
            break;
        default:
            printf("Unknown default param %d requested\n", param_num);
            exit(1);
    }
}
