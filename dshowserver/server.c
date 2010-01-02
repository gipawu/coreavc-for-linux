#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#ifndef __MINGW32__
#include <sys/mman.h>
#else
#include <windows.h>
#endif

#include "dshow/DSVD_extern.h"
#include "timeout_sem.h"

struct vd_struct {
  union {
    uint32_t ret;
    uint32_t cmd;
  };
  uint32_t buflen;
  uint64_t pts;
  uint32_t unused[8];
};

enum {
  VD_END = 1,
  VD_DECODE = 2,
  VD_SEEK = 3,
  VD_RELOAD_BIH = 4,
  VD_HAS_BIH = 0x10000,
  VD_VERSION_MASK = 0xFFFF,
};
#ifdef SAVE_FRAMES
  #include "dump_frames.c"
#endif

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

void *get_shared_mem(char *id, int memsize)
{
  void *mem;
  char shm[80];

#ifndef __MINGW32__
  int fd;

  snprintf(shm, 80, "/dshow_shm.%s", id);
  fd = shm_open(shm, O_RDWR, S_IRUSR | S_IWUSR);
  shm_unlink(shm);
  if(fd < 0) {
    perror("shm_open failed");
    exit(1);
  }
  mem = mmap(NULL, memsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(mem == MAP_FAILED) {
    printf("failed to mmap shared memory\n");
    exit(1);
  }
#else
  HANDLE hMapFile;      // handle for the file's memory-mapped region
  HANDLE hFile;

  snprintf(shm, 80, "Z:\\dev\\shm\\dshow_shm.%s", id);
  hFile = CreateFile(shm, 
                     GENERIC_READ | GENERIC_WRITE,
                     0, 
                     NULL,
                     OPEN_EXISTING, 
                     FILE_ATTRIBUTE_NORMAL, 
                     NULL);

  if(! hFile == -1) {
    fprintf(stderr, "failed to open file: %s\n", shm);
    exit(1);
  }
  hMapFile = CreateFileMapping( hFile,          // current file handle
                NULL,           // default security
                PAGE_READWRITE, // read/write permission
                0,              // size of mapping object, high
                memsize,        // size of mapping object, low
                NULL);          // name of mapping object

  if (hMapFile == NULL) 
  {
    fprintf(stderr, "hMapFile is NULL: last error: %ld\n", GetLastError() );
    exit(1);
  }

  // Map the view and test the results.

  mem = MapViewOfFile(hMapFile,            // handle to 
                      FILE_MAP_ALL_ACCESS, // read/write 
                      0,                   // high-order 32 
                      0,                   // low-order 32
                      memsize);
  if (mem == NULL) 
  {
    fprintf(stderr, "lpMapAddress is NULL: last error: %ld\n", GetLastError());
    exit(1);
  }
#endif /*__MINGW32__*/
  return mem;
}

int main(int argc, char *argv[])
{
  int width, height, memsize, ret = 0, port = 0;
  int pagecount = 0, pagesize;
  void *mem;
  char *buffer = NULL;
  char *picture = NULL;
  struct vd_struct *vd = NULL;
  DS_VideoDecoder *dshowdec;
  int opt;
  char c;
  void *sem;
  static struct option Long_Options[] = {
    {"bits", 1, 0, 'b'},
    {"codec", 1, 0, 'c'},
    {"debug", 0, 0, 'd'},
    {"fourcc", 1, 0, 'f'},
    {"guid", 1, 0, 'g'},
    {"id", 1, 0, 'i'},
    {"numpages", 1, 0, 'n'},
    {"outfmt", 1, 0, 'o'},
    {"pid", 1, 0, 'p'},
    {"size", 1, 0, 's'},
    {"port", 1, 0, 't'},
    {0, 0, 0, 0},
  };

  char *id = NULL, *codec = NULL;
  DS_GUID guid;
  BITMAPINFOHEADER bih, *bih_ptr = &bih;
  uint32_t fourcc = 0, fmt = 0;
  int bits = 0, ppid = 0;
  void *base = NULL;

  while(1) {
    c = getopt_long(argc, argv, "b:c:df:g:i:n:o:p:s:t:", Long_Options, &opt);
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
        print_verbose_messages = 8;
        break;
      case 's':
        sscanf(optarg,"%dx%d", &width, &height);
        break;
      case 'g':
        sscanf(optarg,"%8x-%4hx-%4hx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
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
      case 'n':
        pagecount = strtol(optarg, NULL, 0);
        break;
      case 'p':
        ppid = strtol(optarg, NULL, 0);
        break;
      case 't':
        port = strtol(optarg, NULL, 0);
        break;
    }
  }
  if(width*height == 0) {
    printf("Must specify size\n");
    exit(1);
  }
  if(!id) {
    printf("No id specified, assuming test mode\n");
    make_bih(&bih, width, height, fourcc);
  } else {
    pagesize = (width * height * bits / 8 + SAFETY_SIZE);
    memsize = sizeof(struct vd_struct) + width * height + 
              width * height * bits / 8 + pagesize * pagecount;
    //when using shared pages, must allocate before starting the codec
    mem = get_shared_mem(id, memsize);
    if(pagecount != 0) {
      base = ((char *)mem) + sizeof(struct vd_struct) + width * height
             + width * height * bits / 8;
      set_memstruct(base, pagecount, pagesize);
    } else {
      set_memstruct(NULL, 0, 0);
    }
    vd = (struct vd_struct *)mem;
    buffer = ((char *)mem) + sizeof(struct vd_struct);
    picture = ((char *)mem) + sizeof(struct vd_struct) + width * height;
    if(vd->cmd & VD_HAS_BIH)
      bih_ptr = (BITMAPINFOHEADER *)buffer;
    else
      make_bih(&bih, width, height, fourcc);
  }
  printf("Opening device (port is %d)\n", port);
  dshowdec = DS_VideoDecoder_Open(codec, &guid, bih_ptr, 0, 0);
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
  if(! id)
    exit(0);
  if(port) {
    printf("Using socket based mutex\n");
    sem = timed_seminit(DS_SOCKET, &port, 0);
  } else {
    printf("Using semaphore based mutex\n");
    sem = timed_seminit(DS_SEMAPHORE, id, 0);
  }
  printf("Dshowserver Connected to host\n");
#ifdef SAVE_FRAMES
  printf("Initializing Dump\n");
  init_dump(codec, &guid, bih_ptr, fourcc, fmt, bits, width, height);
#endif
  while(1) {
    while(1) {
      int ok = timed_semwait(sem, 10);
      if (ok == 1)
        break;
      if(ok != DS_TIMEOUT) {
        //We didn't successfully lock
        perror("sem_timedwait");
        exit(1);
      }
#ifndef __MINGW32__
      else if(kill(ppid, 0)) {
        fprintf(stderr, "Parent process %d died unexpectedly\n", ppid);
        exit(1);
      }
#endif
    }
#ifdef SAVE_FRAMES
    dump_frame(vd, buffer);
#endif
    switch(vd->cmd) {
      case VD_END:
        //We are done
        DS_VideoDecoder_Destroy(dshowdec);
        exit(0);
      case VD_DECODE:
        DS_VideoDecoder_FreeFrame(dshowdec);
        DS_VideoDecoder_SetPTS(dshowdec, vd->pts);
        vd->ret = DS_VideoDecoder_DecodeInternal(dshowdec, buffer, vd->buflen, 0, picture);
        vd->pts = DS_VideoDecoder_GetPTS(dshowdec);
        ret = vd->ret;
        break;
      case VD_SEEK:
        DS_VideoDecoder_SeekInternal(dshowdec);
        ret = 0;
        vd->ret = 0;
        break;
      case VD_RELOAD_BIH:
        bih_ptr = (BITMAPINFOHEADER *)buffer;
        if(bih_ptr->biWidth != width || bih_ptr->biHeight != height) {
          printf("Cannot reload BITMAPINFOHEADER because new size (%dx%d) != old size (%dx%d)\n",
                 (int)bih_ptr->biWidth, (int)bih_ptr->biHeight, width, height);
          vd->ret = -1;
        } else {
	  printf("Starting reload\n");
          DS_VideoDecoder_SetInputType(dshowdec, (BITMAPINFOHEADER *)buffer);
	  printf("Starting reload 2\n");
          DS_VideoDecoder_SetDestFmt(dshowdec,bits,fmt);
	  printf("Starting reload 3\n");
          vd->ret = 0;
        }
        break;
      default:
        fprintf(stderr, "Got illegal command %d\n", vd->cmd);
        ret = -1;
    }
    timed_sempost(sem);
    if(ret == -1) {
      exit(1);
    }
  }
  return 0;
}
