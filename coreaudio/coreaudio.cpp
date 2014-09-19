// coreaudio.cpp
//
#include "stdafx.h"
#include <string.h>
#include "coreaudio.h"
#ifndef RENDER
#define RENDER 1
#define CAPTURE 0
#endif
int _tmain(int argc, _TCHAR *argv[])
{
    FILE *fp;

#if RENDER
    string fname = "F:\\github\\core-audio-apis\\wavsource\\caiqin.wav";
    if ((frp = fopen(fname.c_str(), "rb")) == NULL)
    {
        printf("can not open this wave file\n");
    }
    HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, CoreAudioRender, NULL, 0, NULL);   
    WaitForSingleObject(handle, INFINITE);  
#elif CAPTURE
    //
    // capture
    //
    string fname = "F:\\github\\core-audio-apis\\wavsource\\wavsourcecapture.pcm";
    if ((fp = fopen(fname.c_str(), "wb")) == NULL)
    {
        printf("can not open this wave file\n");
    }
    CoreAudioCapture(fp);
#endif
	 
    return 0;
}

