#ifndef _FILTER_PROP_H_
#define _FILTER_PROP_H_

typedef struct {
    ULONG cElems;
    GUID *pElems;
} CAUUID;
typedef struct ISpecifyPropertyPages ISpecifyPropertyPages;
struct ISpecifyPropertyPages_vt
{
    INHERIT_IUNKNOWN();

    LONG    STDCALL ( *GetPages )(ISpecifyPropertyPages* This, CAUUID *pPages);
};

struct ISpecifyPropertyPages
{
    struct ISpecifyPropertyPages_vt *vt;
};
HRESULT WINAPI OleCreatePropertyFrame(
        HWND hwndOwner, UINT x, UINT y,
        void *lpszCaption, ULONG cObjects, IUnknown** ppUnk,
        ULONG cPages, void *pPageClsID, LCID lcid, DWORD dwReserved,
        void *pvReserved );
HWND WINAPI GetTopWindow(HWND);
#endif
