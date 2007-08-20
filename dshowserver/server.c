#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

#include "dshow/DSVD_extern.h"

struct vd_struct {
  union {
    uint32_t ret;
    uint32_t cmd;
  };
  uint32_t buflen;
};
enum {
  VD_END = 1,
  VD_DECODE = 2,
};

unsigned int print_verbose_messages = 0;

void make_bih(BITMAPINFOHEADER *bih, uint32_t w, uint32_t h, uint32_t tag) {
  bih->biSize = sizeof(BITMAPINFOHEADER);
  bih->biWidth = w;
  bih->biHeight = h;
  bih->biPlanes = 1;
  bih->biBitCount = 24;
  bih->biCompression = tag;
  bih->biSizeImage = 0;
  bih->biXPelsPerMeter=0;
  bih->biYPelsPerMeter=0;
  bih->biClrUsed=0;
  bih->biClrImportant=0;
}
  
int main(int argc, char *argv[])
{
  char sem1[80], sem2[80], shm[80];
  sem_t *sem_rd, *sem_wr;
  int width, height, memsize, fd, ret;
  void *mem;
  char *buffer, *picture;
  struct vd_struct *vd;
  struct timespec ts;
  DS_VideoDecoder *dshowdec;
  struct option *LongOpts;
  int opt;
  char c;
  static struct option Long_Options[] = {
    {"bits", 1, 0, 'b'},
    {"codec", 1, 0, 'c'},
    {"debug", 0, 0, 'd'},
    {"fourcc", 1, 0, 'f'},
    {"guid", 1, 0, 'g'},
    {"id", 1, 0, 'i'},
    {"outfmt", 1, 0, 'o'},
    {"pid", 1, 0, 'p'},
    {"size", 1, 0, 's'},
    {0, 0, 0, 0},
  };
  char *id, *codec;
  GUID guid;
  BITMAPINFOHEADER bih;
  uint32_t fourcc, fmt;
  int bits, ppid;

  while(1) {
    c = getopt_long(argc, argv, "b:c:df:g:i:o:p:s:", Long_Options, &opt);
    if (c == EOF)
      break;
    switch (c) {
      case 'i':
        id = optarg;
        break;
      case 'c':
        codec = optarg;
        break;
      case 'd':
        //print_verbose_messages = 8;
        break;
      case 's':
        sscanf(optarg,"%dx%d", &width, &height);
        break;
      case 'g':
        sscanf(optarg,"%8x-%4x-%4x-%2x%2x%2x%2x%2x%2x%2x%2x",
               &guid.f1, &guid.f2, &guid.f3,
               &guid.f4[0], &guid.f4[1], &guid.f4[2], &guid.f4[3],
               &guid.f4[4], &guid.f4[5], &guid.f4[6], &guid.f4[7]);
        break;
      case 'f':
        fourcc = strtol(optarg, NULL, 0);
        break;
      case 'o':
        fmt = strtol(optarg, NULL, 0);
        break;
      case 'b':
        bits = strtol(optarg, NULL, 0);
        break;
      case 'p':
        ppid = strtol(optarg, NULL, 0);
    }
  }
  snprintf(shm, 80, "/dshow_shm.%s", id);
  snprintf(sem1, 80, "/dshow_sem1.%s", id);
  snprintf(sem2, 80, "/dshow_sem2.%s", id);
  printf("shm:%s\nsem1:%s\nsem2:%s\n", shm, sem1, sem2);
  if(width*height == 0) {
    printf("Must specify size\n");
    exit(1);
  }

  make_bih(&bih, width, height, fourcc);
  printf("Opening device\n");
  dshowdec = DS_VideoDecoder_Open(codec, &guid, &bih, 0, 0);
  if(! dshowdec) {
    fprintf(stderr, "Failed to open win32 codec %s\n", codec);
    exit(1);
  }
  printf("Setting fmt\n");
  DS_VideoDecoder_SetDestFmt(dshowdec,bits,fmt);
  printf("Starting\n");
  DS_VideoDecoder_StartInternal(dshowdec);

  printf("Initialization is complete\n");
  //Codec is now initialized
  fd = shm_open(shm, O_RDWR, S_IRUSR | S_IWUSR);
  shm_unlink(shm);
  if(fd < 0) {
    perror("shm_open failed");
    exit(1);
  }
  memsize = sizeof(struct vd_struct) + width * height + 
                                       width * height * bits / 8;
  mem = mmap(NULL, memsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(mem == MAP_FAILED) {
    printf("failed to mmap shared memory\n");
    exit(1);
  }
  vd = (struct vd_struct *)mem;
  buffer = ((char *)mem) + sizeof(struct vd_struct);
  picture = ((char *)mem) + sizeof(struct vd_struct) + width * height;
  sem_rd = sem_open(sem1, 0);
  sem_unlink(sem1);
  if(sem_rd == SEM_FAILED) {
    perror("sem_open(1)");
    exit(1);
  }
  sem_wr = sem_open(sem2, 0);
  sem_unlink(sem2);
  if(sem_wr == SEM_FAILED) {
    perror("sem_open(2)");
    exit(1);
  }
  //tell calling procedure that we are awake;
  sem_post(sem_wr);
  while(1) {
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
       perror("clock_gettime");
       exit(1);
    }
    ts.tv_sec += 10; //Wait 10 seconds before we give up hope
    while(1) {
      ret = sem_timedwait(sem_rd, &ts);
      if (ret == 0)
        break;
      if(errno != ETIMEDOUT) {
        //We didn't successfully lock
        perror("sem_timedwait");
        exit(1);
      } else if(kill(ppid, 0)) {
        fprintf(stderr, "Parent process %d died unexpectedly\n");
        exit(1);
      }
    }
    switch(vd->cmd) {
      case VD_END:
        //We are done
        DS_VideoDecoder_Destroy(dshowdec);
        exit(0);
      case VD_DECODE:
        vd->ret = DS_VideoDecoder_DecodeInternal(dshowdec, buffer, vd->buflen, 0, picture);
        ret = vd->ret;
        break;
      default:
        fprintf(stderr, "Got illegal command %d\n", vd->cmd);
        ret = -1;
    }
    sem_post(sem_wr);
    if(ret == -1) {
      exit(1);
    }
  }
}
