// coreaudio.cpp
//

#include "stdafx.h"
#include <string.h>
#include "Mmdeviceapi.h"   //IMMDevice interface
#include  "Audioclient.h" //WASAPI
#include "Audiopolicy.h"  //WASAPI interface
#include <EndpointVolume.h>
#include "CWaveFile.h"
#ifndef BUFFRETIME
#define BUFFRETIME 1*1000*10  // 
#endif
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000
int _tmain(int argc, _TCHAR *argv[])
{
    const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
    const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
    const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    HRESULT hr;
    IMMDeviceEnumerator *pEnumerator;

    CoInitialize(NULL);  //to tell system creat COM by single thread!!!!
    hr = CoCreateInstance(
             CLSID_MMDeviceEnumerator, NULL,
             CLSCTX_ALL, IID_IMMDeviceEnumerator,
             (void **)&pEnumerator);   //creat COM instance
    //
    //Generates a collection of audio endpoint devices that meet the specified criteria
    //
    IMMDeviceCollection *pCDevices;
    pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCDevices);
    unsigned int i = 0;
    pCDevices->GetCount(&i);
    printf("device number is : %d\n", i);
    //
    //  Generate a Default IMMDevice interface -_-!!
    //  we also can generate it by pCDevice -> Iteam(0~n-1,&pDDevice)
    //
    IMMDevice *pDDevice;
    pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDDevice);

    //
    //
    //
    LPWSTR strid = NULL;
    pDDevice->GetId(&strid);
    // printf("%s \n",strid);
    DWORD pdwState;
    pDDevice->GetState(&pdwState);

    //
    //WASAPI
    //
    WAVEFORMATEX *pwfx = NULL;
    IAudioClient *Audioclient = NULL;
    LPCGUID AudioSessionGuid = NULL;
    pDDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void **)(&Audioclient));
    Audioclient->GetMixFormat(&pwfx);
    Audioclient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0,
                            hnsRequestedDuration, 0, pwfx, AudioSessionGuid);

    UINT32 bufferFrameCount;
    Audioclient->GetBufferSize(&bufferFrameCount);

    IAudioRenderClient *pRenderClient = NULL;
    Audioclient->GetService(
        IID_IAudioRenderClient,
        (void **)&pRenderClient);
    //
    // Grab the entire buffer for the initial fill operation.
    //
    BYTE *pData = NULL;
    pRenderClient->GetBuffer(bufferFrameCount, &pData);
    //
    //open wave file.
    //
    FILE *fp;
    string fname = "C:\\Users\\rickon\\Desktop\\1.wav";
    if((fp=fopen(fname.c_str(),"rb"))==NULL)
    {
        printf("can not open this wave file\n");
    }
    fseek(fp, 48, SEEK_CUR);  // forget wave file wave
    fread(pData,sizeof(BYTE),bufferFrameCount,fp);
    fseek(fp, bufferFrameCount, SEEK_CUR);
    printf("%x %x\n",pData[0],pData[1] );

    // WaveFile MySource ;
    // MySource.WavRead(fname);
    // printf("%x %x \n",MySource.Data[0],MySource.Data[1] );
   // memcpy(pData,MySource.Data,bufferFrameCount);//copy wav data to audio engine buff
   //  //
    //release buff ,i think this fun is sending data in buffer to audio engine.
    //
    DWORD flags = 0;
    pRenderClient->ReleaseBuffer(bufferFrameCount, flags);


    hnsActualDuration = (double)REFTIMES_PER_SEC *
                        bufferFrameCount / pwfx->nSamplesPerSec;
    Audioclient->Start();  // Start playing.
    UINT32 numFramesAvailable; 
    UINT32 numFramesPadding;
    while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {
        // Sleep for half the buffer duration.
        Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));
       
        // See how much buffer space is available.
        Audioclient->GetCurrentPadding(&numFramesPadding);

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        pRenderClient->GetBuffer(numFramesAvailable, &pData);

        // Get next 1/2-second of data from the audio source.
        // pMySource->LoadData(numFramesAvailable, pData, &flags);
        if (fread(pData,sizeof(BYTE),bufferFrameCount,fp) == bufferFrameCount)
            fseek(fp, bufferFrameCount, SEEK_CUR);

        pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
    }
    Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));

    Audioclient->Stop();  // Stop playing.
    fclose(fp);
    return 0;
}

