#include "stdafx.h"
#include "coreaudiorender.h"
int CoreAudioRender(FILE *fp)
{
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioRenderClient *pRenderClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    WAVEFORMATEX wavefx ;
    WAVEFORMATEX wavefx1 ;
    WAVEFORMATEX *devicefx = &wavefx1 ;;

    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    UINT32 readnum = 0;
    BYTE *pData;
    DWORD flags = 0;
    CoInitialize(NULL);  //to tell system creat COM by single thread!!!!
    hr = CoCreateInstance(
             CLSID_MMDeviceEnumerator, NULL,
             CLSCTX_ALL, IID_IMMDeviceEnumerator,
             (void **)&pEnumerator);

    hr = pEnumerator->GetDefaultAudioEndpoint(
             eRender, eConsole, &pDevice);

    hr = pDevice->Activate(
             IID_IAudioClient, CLSCTX_ALL,
             NULL, (void **)&pAudioClient);

    hr = pAudioClient->GetMixFormat(&pwfx);


    hr = pAudioClient->Initialize(
             AUDCLNT_SHAREMODE_SHARED,
             0,
             hnsRequestedDuration,
             0,
             pwfx,
             NULL);

    hr = pAudioClient->GetBufferSize(&bufferFrameCount);

    hr = pAudioClient->GetService(
             IID_IAudioRenderClient,
             (void **)&pRenderClient);

    // Grab the entire buffer for the initial fill operation.
    hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);


    fseek(fp, DATA, SEEK_SET);

    fread(pData, sizeof(BYTE), bufferFrameCount * 8, fp);


    hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                        bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start playing.

    // Each loop fills about half of the shared buffer.
    while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {
        // Sleep for half the buffer duration.
        Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

        // See how much buffer space is available.
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);

        // hr = pMySource->LoadData(numFramesAvailable, pData, &flags);
        //GetWaveData( pData ,fp,numFramesAvailable ,bitperSample);
        readnum = fread(pData, sizeof(BYTE), numFramesAvailable * 8, fp);
        if (readnum == 0)
            flags |= AUDCLNT_BUFFERFLAGS_SILENT;

        hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
    }

    // Wait for last data in buffer to play before stopping.
    Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

    hr = pAudioClient->Stop();  // Stop playing.


    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pRenderClient)
    fclose(fp);
    return hr;
}