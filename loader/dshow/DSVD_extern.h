#ifndef HKEY_CURRENT_USER
#define HKEY_CURRENT_USER       ((unsigned long) 0x80000001)
#endif
#ifndef HKEY_LOCAL_MACHINE
#define HKEY_LOCAL_MACHINE      ((unsigned long) 0x80000002)
#endif

#ifndef REG_SZ
#define REG_SZ                      1       /* string type (ASCII) */
#endif

typedef struct {
        uint32_t f1;
        uint16_t f2;
        uint16_t f3;
        uint8_t  f4[8];
} GUID;

#ifndef _BITMAPINFOHEADER_
#define _BITMAPINFOHEADER_
typedef struct __attribute__((__packed__))
{
    int32_t 	biSize;
    int32_t  	biWidth;
    int32_t  	biHeight;
    int16_t 	biPlanes;
    int16_t 	biBitCount;
    int32_t 	biCompression;
    int32_t 	biSizeImage;
    int32_t  	biXPelsPerMeter;
    int32_t  	biYPelsPerMeter;
    int32_t 	biClrUsed;
    int32_t 	biClrImportant;
} BITMAPINFOHEADER;
#endif

//SAFERTY_SIZE == SAFETY_ACEL from cmediasample.c
#ifndef SAFETY_SIZE
  #define SAFETY_SIZE 1024
#endif

typedef struct _DS_VideoDecoder DS_VideoDecoder;

DS_VideoDecoder *DS_VideoDecoder_Open(const char* dllname, GUID* guid, BITMAPINFOHEADER* format, int flip, int maxauto);

void DS_VideoDecoder_Destroy(DS_VideoDecoder *dsvd);

void DS_VideoDecoder_StartInternal(DS_VideoDecoder *dsvd);

void DS_VideoDecoder_StopInternal(DS_VideoDecoder *dsvd);
void DS_VideoDecoder_SeekInternal(DS_VideoDecoder *dsvd);

int DS_VideoDecoder_DecodeInternal(DS_VideoDecoder *dsvd, const void* src, int size, int is_keyframe, char* pImage);

void DS_VideoDecoder_SetPTS(DS_VideoDecoder *dsvd, uint64_t pts_nsec);
uint64_t DS_VideoDecoder_GetPTS(DS_VideoDecoder *dsvd);

void DS_VideoDecoder_SetInputType(DS_VideoDecoder *dsvd, BITMAPINFOHEADER* buffer);


/*
 * bits == 0   - leave unchanged
 */
//int SetDestFmt(DS_VideoDecoder * this, int bits = 24, fourcc_t csp = 0);
int DS_VideoDecoder_SetDestFmt(DS_VideoDecoder *dsvd, int bits, unsigned int csp);
int DS_VideoDecoder_SetDirection(DS_VideoDecoder *dsvd, int d);
int DS_VideoDecoder_GetValue(DS_VideoDecoder *dsvd, const char* name, int* value);
int DS_VideoDecoder_SetValue(DS_VideoDecoder *dsvd, const char* name, int value);
int DS_SetAttr_DivX(char* attribute, int value);

// registry functions
__attribute__((__stdcall__)) long RegCloseKey(long key);
__attribute__((__stdcall__)) long RegQueryValueExA(long key, const char* value, int* reserved,
                      int* type, int* data, int* count);
__attribute__((__stdcall__)) long RegCreateKeyExA(long key, const char* name, long reserved,
                     void* classs, long options, long security,
                     void* sec_attr, int* newkey, int* status);
__attribute__((__stdcall__)) long RegSetValueExA(long key, const char* name, long v1, long v2,
                    const void* data, long size);

// From cmediasample.c
void set_memstruct(void *base, int count, int size);

