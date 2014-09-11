// coreaudio.cpp
//

#include "stdafx.h"
#include <string.h>
#include "Mmdeviceapi.h"   //IMMDevice interface
#include  "Audioclient.h" //WASAPI
#include "Audiopolicy.h"  //WASAPI interface
#include <EndpointVolume.h>
#include "CWaveFile.h"
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

// #define EXIT_ON_ERROR(hres)  \
//     if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
    if ((punk) != NULL)  \
    { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
int _tmain(int argc, _TCHAR *argv[])
{
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioRenderClient *pRenderClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE *pData;
    DWORD flags = 0;
	CoInitialize(NULL);  //to tell system creat COM by single thread!!!!
    hr = CoCreateInstance(
             CLSID_MMDeviceEnumerator, NULL,
             CLSCTX_ALL, IID_IMMDeviceEnumerator,
             (void **)&pEnumerator);
    // EXIT_ON_ERROR(hr)

    hr = pEnumerator->GetDefaultAudioEndpoint(
             eRender, eConsole, &pDevice);
    // EXIT_ON_ERROR(hr)

    hr = pDevice->Activate(
             IID_IAudioClient, CLSCTX_ALL,
             NULL, (void **)&pAudioClient);
    // EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    // EXIT_ON_ERROR(hr)

    hr = pAudioClient->Initialize(
             AUDCLNT_SHAREMODE_SHARED,
             0,
             hnsRequestedDuration,
             0,
             pwfx,
             NULL);
    // EXIT_ON_ERROR(hr)

    // Tell the audio source which format to use.
    // hr = pMySource->SetFormat(pwfx);
    //EXIT_ON_ERROR(hr);

    // Get the actual size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    // EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService(
             IID_IAudioRenderClient,
             (void **)&pRenderClient);
    // EXIT_ON_ERROR(hr)

    // Grab the entire buffer for the initial fill operation.
    hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
    // EXIT_ON_ERROR(hr)

    // Load the initial data into the shared buffer.
    FILE *fp;
    string fname = "C:\\Users\\rickon\\Desktop\\3.wav";
    if ((fp = fopen(fname.c_str(), "rb")) == NULL)
    {
        printf("can not open this wave file\n");
    }
    fseek(fp, 48, SEEK_CUR);  // forget wave file wave
    fread(pData, sizeof(BYTE), bufferFrameCount, fp);
    fseek(fp, bufferFrameCount, SEEK_CUR);
	printf("%d \n",pwfx->wBitsPerSample);
    // printf("%x %x %x %x\n", pData[0], pData[1],pData[2],pData[3] );
    for (int i = 0; i < 0xcc44; i+=16)
    {
        printf("%x%x %x%x %x%x %x%x %x%x %x%x %x%x %x%x\n", 
            pData[i], pData[i+1],pData[i+2],pData[i+3],pData[i+4],
            pData[i+5],pData[i+6],pData[i+7],pData[i+8], pData[i+1+8],pData[i+2+8],pData[i+3+8],pData[i+4+8],
            pData[i+5+8],pData[i+6+8],pData[i+7+8]);
    }

    hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
    // EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                        bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start playing.
    // EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {
        // Sleep for half the buffer duration.
        Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

        // See how much buffer space is available.
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
        // EXIT_ON_ERROR(hr)

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
        // EXIT_ON_ERROR(hr)

        // Get next 1/2-second of data from the audio source.
        // hr = pMySource->LoadData(numFramesAvailable, pData, &flags);
       // EXIT_ON_ERROR(hr);
        if (fread(pData, sizeof(BYTE), bufferFrameCount, fp) == bufferFrameCount)
            fseek(fp, bufferFrameCount, SEEK_CUR);


        hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
        // EXIT_ON_ERROR(hr)
    }

    // Wait for last data in buffer to play before stopping.
    Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

    hr = pAudioClient->Stop();  // Stop playing.
    // EXIT_ON_ERROR(hr)

// Exit:
    // // CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pRenderClient)
fclose(fp);
    return hr;
}

