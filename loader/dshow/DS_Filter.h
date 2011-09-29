#ifndef MPLAYER_DS_FILTER_H
#define MPLAYER_DS_FILTER_H

#include "inputpin.h"
#include "outputpin.h"

#define PD_SET 0x01
#define PD_SENT 0x02

typedef struct {
    char* frame_pointer;
    long frame_size;
    int  state;
    unsigned long interlace;
    uint64_t pts_nsec;
} SampleProcUserData;

/**
   User will allocate and fill format structures, call Create(),
   and then set up m_pAll.
 **/

typedef struct DS_Filter DS_Filter;
struct DS_Filter
{
    int m_iHandle;
    IBaseFilter* m_pFilter;
    IPin* m_pInputPin;
    IPin* m_pOutputPin;

    CBaseFilter* m_pSrcFilter;
    CBaseFilter2* m_pParentFilter;
    IPin* m_pOurInput;
    COutputPin* m_pOurOutput;

    AM_MEDIA_TYPE *m_pOurType, *m_pDestType;
    IMemAllocator* m_pAll;
    IMemInputPin* m_pImp;

    void ( *Start )(DS_Filter*);
    void ( *Stop )(DS_Filter*);
};

DS_Filter* DS_FilterCreate(const char* dllname, const GUID* id,
			   AM_MEDIA_TYPE* in_fmt, AM_MEDIA_TYPE* out_fmt,SampleProcUserData* pUserData);
void DS_Filter_Destroy(DS_Filter* This);

#endif /* MPLAYER_DS_FILTER_H */
