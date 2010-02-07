/*
 * Modified for use with MPlayer, detailed changelog at
 * http://svn.mplayerhq.hu/mplayer/trunk/
 */

#include "config.h"
#include "DS_Filter.h"
#include "driver.h"
#include "com.h"
#include "wine/winerror.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "win32.h" // printf macro
#include "filterprop.h"

static HANDLE page_mutex;
typedef long STDCALL (*GETCLASS) (const GUID*, const GUID*, void**);

#ifndef WIN32_LOADER
const GUID IID_IUnknown =
{
    0x00000000, 0x0000, 0x0000,
    {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
};
const GUID IID_IClassFactory =
{
    0x00000001, 0x0000, 0x0000,
    {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
};

HRESULT STDCALL CoInitialize(LPVOID pvReserved); 
void STDCALL CoUninitialize(void); 
#endif

//void trapbug();

static void DS_Filter_Start(DS_Filter* This)
{
    HRESULT hr;

    //Debug printf("DS_Filter_Start(%p)\n", This);
    hr = This->m_pFilter->vt->Run(This->m_pFilter, (REFERENCE_TIME)0);
    if (hr != 0)
    {
	Debug printf("WARNING: m_Filter->Run() failed, error code %x\n", (int)hr);
    }
}

static void DS_Filter_Stop(DS_Filter* This)
{
    if (This->m_pAll)
    {
	//Debug	printf("DS_Filter_Stop(%p)\n", This);
	This->m_pFilter->vt->Stop(This->m_pFilter); // causes weird crash ??? FIXME
	This->m_pAll->vt->Release((IUnknown*)This->m_pAll);
	This->m_pAll = 0;
    }
}

void DS_Filter_Destroy(DS_Filter* This)
{
    This->Stop(This);

    if (This->m_pOurInput)
	This->m_pOurInput->vt->Release((IUnknown*)This->m_pOurInput);
    if (This->m_pInputPin)
	This->m_pInputPin->vt->Disconnect(This->m_pInputPin);
    if (This->m_pOutputPin)
	This->m_pOutputPin->vt->Disconnect(This->m_pOutputPin);
    if (This->m_pFilter)
	This->m_pFilter->vt->Release((IUnknown*)This->m_pFilter);
    if (This->m_pOutputPin)
	This->m_pOutputPin->vt->Release((IUnknown*)This->m_pOutputPin);
    if (This->m_pInputPin)
	This->m_pInputPin->vt->Release((IUnknown*)This->m_pInputPin);
    if (This->m_pImp)
	This->m_pImp->vt->Release((IUnknown*)This->m_pImp);

    if (This->m_pOurOutput)
	This->m_pOurOutput->vt->Release((IUnknown*)This->m_pOurOutput);
    if (This->m_pParentFilter)
	This->m_pParentFilter->vt->Release((IUnknown*)This->m_pParentFilter);
    if (This->m_pSrcFilter)
	This->m_pSrcFilter->vt->Release((IUnknown*)This->m_pSrcFilter);

    // FIXME - we are still leaving few things allocated!
    if (This->m_iHandle)
	FreeLibrary((unsigned)This->m_iHandle);

    free(This);

#ifdef WIN32_LOADER
    CodecRelease();
#else
    CoUninitialize();
#endif
}

static HRESULT STDCALL DS_Filter_CopySample(void* pUserData,IMediaSample* pSample){
    BYTE* pointer;
    int len;
    int i = 0;
    AM_MEDIA_TYPE *mt;
    REFERENCE_TIME stoptime;
 
    SampleProcUserData* pData=pUserData;
    SampleProcUserFrame* pFrame;
    Debug printf("CopySample called(%p,%p)\n",pSample,pUserData);
#ifdef USE_SHARED_MEM
    WaitForSingleObject(page_mutex, INFINITE);
    for(i = pData->lastFrame; 1;) {
      int j = (i + 1) % PD_MAX_FRAMES;
      if(! pData->frame[i].state) {
        break;
      }
      if(j == pData->lastFrame) {
        break;
      }
      i = j;
    }
    ReleaseMutex(page_mutex);
#endif
    pFrame = &pData->frame[i];
    if(pSample->vt->GetMediaType &&
       pSample->vt->GetMediaType(pSample, &mt) == S_OK) {
      if(memcmp(&mt->formattype, &FORMAT_VideoInfo2, sizeof(GUID)) == 0) {
	pFrame->interlace = ((VIDEOINFOHEADER2 *)(mt->pbFormat))->dwInterlaceFlags;
      }
    }

    if (pSample->vt->GetPointer(pSample, &pointer))
	return 1;
    len = pSample->vt->GetActualDataLength(pSample);
    if (len == 0)
	len = pSample->vt->GetSize(pSample);//for iv50

    pSample->vt->GetTime(pSample, &pFrame->pts_nsec, &stoptime);
    memstruct_setlock(pointer, 1);
    pFrame->frame_pointer = pointer;
    pFrame->frame_size = len;
    pFrame->state = PD_SET;
/*
    FILE* file=fopen("./uncompr.bmp", "wb");
    char head[14]={0x42, 0x4D, 0x36, 0x10, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00};
    *(int*)(&head[2])=len+0x36;
    fwrite(head, 14, 1, file);
    fwrite(&((VIDEOINFOHEADER*)me.type.pbFormat)->bmiHeader, sizeof(BITMAPINFOHEADER), 1, file);
    fwrite(pointer, len, 1, file);
    fclose(file);
*/
    return 0;
}

void GetProductVersion(HMODULE hMod)
{
    HANDLE r;
    char *res;
    int len, i;
    r = FindResourceA(hMod, (LPCSTR)1, (LPCSTR)16); //VS_VERSION_INFO, RT_VERSION
    res = (char *)LoadResource(hMod, r);
    len = SizeofResource(hMod, r);
    printf("len: %d\n", len);
    for(i = 0; i < len-50; i ++) {
      if(res[i]=='P'   && res[i+2]=='r' && res[i+4]=='o' && res[i+6]=='d' &&
        res[i+8]=='u'  && res[i+10]=='c' && res[i+12]=='t' && res[i+14]=='V' &&
        res[i+16]=='e' && res[i+18]=='r' && res[i+20]=='s' && res[i+22]=='i' &&
        res[i+24]=='o' && res[i+26]=='n' && res[i+28]==0) {
	char s[256];
	WideCharToMultiByte(0,0,(LPCWSTR)(res+i+30),-1,s,256,NULL,NULL);
	printf("ProductVersion: %s\n", s);
      }
    }
}

void DS_ShowPropertyPage(DS_Filter* This)
{
	HRESULT result;
	CAUUID cauuid;
	HWND topwindow = GetTopWindow(0);

	ISpecifyPropertyPages *pointer;
	result = This->m_pFilter->vt->QueryInterface((IUnknown*)This->m_pFilter, &IID_ISpecifyPropertyPages, (void*)&pointer);
	if(result || ! pointer)
	{
		printf("Filter does not provide ISpecifyPropertyPages\n");
		return;
	}
	printf("Filter does provide ISpecifyPropertyPages\n");
	result = pointer->vt->GetPages(pointer, &cauuid);
	result = OleCreatePropertyFrame(topwindow, 30, 30, NULL, 1,
                        (void *)&This->m_pFilter, cauuid.cElems,
                        (GUID *)cauuid.pElems, 0, 0, NULL);

}

DS_Filter* DS_FilterCreate(const char* dllname, const GUID* id,
			   AM_MEDIA_TYPE* in_fmt,
			   AM_MEDIA_TYPE* out_fmt,SampleProcUserData* pUserData)
{
    int init = 0;
//    char eb[250];
    const char* em = NULL;
    MemAllocator* tempAll;

    ALLOCATOR_PROPERTIES props,props1;
    DS_Filter* This = malloc(sizeof(DS_Filter));
    if (!This)
	return NULL;

    page_mutex = CreateMutexA( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

#ifdef WIN32_LOADER
    CodecAlloc();
#else
    CoInitialize(0L);
#endif

    /*
        tempAll is not used  anywhere. 
	MemAllocatorCreate() is called to ensure that RegisterComObject for IMemoryAllocator
	will be	called before possible call 
	to CoCreateInstance(...,&IID_IMemoryAllocator,...) from binary codec.
    */
    tempAll=MemAllocatorCreate();
    This->m_pFilter = NULL;
    This->m_pInputPin = NULL;
    This->m_pOutputPin = NULL;
    This->m_pSrcFilter = NULL;
    This->m_pParentFilter = NULL;
    This->m_pOurInput = NULL;
    This->m_pOurOutput = NULL;
    This->m_pAll = NULL;
    This->m_pImp = NULL;

    This->Start = DS_Filter_Start;
    This->Stop = DS_Filter_Stop;

    for (;;)
    {
	GETCLASS func;
	struct IClassFactory* factory = NULL;
	struct IUnknown* object = NULL;
	IEnumPins* enum_pins = 0;
	IPin* array[256];
	ULONG fetched;
        HRESULT result;
        unsigned int i;

	This->m_iHandle = LoadLibraryA(dllname);
	if (!This->m_iHandle)
	{
	    em = "could not open DirectShow DLL";
	    break;
	}
        GetProductVersion(This->m_iHandle);
	func = (GETCLASS)GetProcAddress((unsigned)This->m_iHandle, "DllGetClassObject");
	if (!func)
	{
	    em = "illegal or corrupt DirectShow DLL";
	    break;
	}
	result = func(id, &IID_IClassFactory, (void*)&factory);
	if (result || !factory)
	{
	    em = "no such class object";
	    break;
	}
	result = factory->vt->CreateInstance(factory, 0, &IID_IUnknown, (void*)&object);
	factory->vt->Release((IUnknown*)factory);
	if (result || !object)
	{
	    em = "class factory failure";
	    break;
	}
	result = object->vt->QueryInterface(object, &IID_IBaseFilter, (void*)&This->m_pFilter);
	object->vt->Release((IUnknown*)object);
	if (result || !This->m_pFilter)
	{
	    em = "object does not provide IBaseFilter interface";
            break;
	}
	// enumerate pins
	result = This->m_pFilter->vt->EnumPins(This->m_pFilter, &enum_pins);
	if (result || !enum_pins)
	{
	    em = "could not enumerate pins";
            break;
	}

	enum_pins->vt->Reset(enum_pins);
	result = enum_pins->vt->Next(enum_pins, (ULONG)256, (IPin**)array, &fetched);
	Debug printf("Pins enumeration returned %ld pins, error is %x\n", fetched, (int)result);

	for (i = 0; i < fetched; i++)
	{
	    PIN_DIRECTION direction = -1;
	    array[i]->vt->QueryDirection(array[i], &direction);
	    if (!This->m_pInputPin && direction == PINDIR_INPUT)
	    {
		This->m_pInputPin = array[i];
		This->m_pInputPin->vt->AddRef((IUnknown*)This->m_pInputPin);
	    }
	    if (!This->m_pOutputPin && direction == PINDIR_OUTPUT)
	    {
		This->m_pOutputPin = array[i];
		This->m_pOutputPin->vt->AddRef((IUnknown*)This->m_pOutputPin);
	    }
	    array[i]->vt->Release((IUnknown*)(array[i]));
	}
	if (!This->m_pInputPin)
	{
	    em = "could not find input pin";
            break;
	}
	if (!This->m_pOutputPin)
	{
	    em = "could not find output pin";
            break;
	}
	result = This->m_pInputPin->vt->QueryInterface((IUnknown*)This->m_pInputPin,
						       &IID_IMemInputPin,
						       (void*)&This->m_pImp);
	if (result)
	{
	    em = "could not get IMemInputPin interface";
	    break;
	}

	This->m_pOurType = in_fmt;
	This->m_pDestType = out_fmt;
        result = This->m_pInputPin->vt->QueryAccept(This->m_pInputPin, This->m_pOurType);
	if (result)
	{
	    em = "source format is not accepted";
            break;
	}
	This->m_pParentFilter = CBaseFilter2Create();
	This->m_pSrcFilter = CBaseFilterCreate(This->m_pOurType, This->m_pParentFilter);
	This->m_pOurInput = This->m_pSrcFilter->GetPin(This->m_pSrcFilter);
	This->m_pOurInput->vt->AddRef((IUnknown*)This->m_pOurInput);

	result = This->m_pInputPin->vt->ReceiveConnection(This->m_pInputPin,
							  This->m_pOurInput,
							  This->m_pOurType);
	if (result)
	{
	    em = "could not connect to input pin";
            break;
	}
	result = This->m_pImp->vt->GetAllocator(This->m_pImp, &This->m_pAll);
	if (result || !This->m_pAll)
	{
	    em="error getting IMemAllocator interface";
            break;
	}

        //Seting allocator property according to our media type
	props.cBuffers=1;
	props.cbBuffer=This->m_pOurType->lSampleSize;
	props.cbAlign=1;
	props.cbPrefix=0;
	This->m_pAll->vt->SetProperties(This->m_pAll, &props, &props1);

	//Notify remote pin about choosed allocator
	This->m_pImp->vt->NotifyAllocator(This->m_pImp, This->m_pAll, 0);

	This->m_pOurOutput = COutputPinCreate(This->m_pDestType,DS_Filter_CopySample,pUserData);

	result = This->m_pOutputPin->vt->QueryAccept(This->m_pOutputPin,
							   This->m_pDestType);
	// Only connect if we can.  Otherwise delay the connection until
	// DS_VideoDecoder_SetDestFmt is called
  	if (! result) {
	result = This->m_pOutputPin->vt->ReceiveConnection(This->m_pOutputPin,
							   (IPin*) This->m_pOurOutput,
							   This->m_pDestType);
	if (result)
	{
	    em = "could not connect to output pin";
            break;
	}
	}

	init++;
        break;
    }
    tempAll->vt->Release((IUnknown*)tempAll);

    if (!init)
    {
	DS_Filter_Destroy(This);
	printf("Warning: DS_Filter() %s.  (DLL=%.200s)\n", em, dllname);
        This = 0;
    }
    return This;
}
