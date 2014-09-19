#include "stdafx.h"

#include "coreaudiorender.h"
unsigned int WINAPI CoreAudioRender(void *)
{
    DWORD flags = 0;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioRenderClient *pRenderClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;

    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    UINT32 readnum = 0;
    BYTE *pData;
    CoInitialize(NULL);  //to tell system creat COM by single thread!!!!
    hr = CoCreateInstance(
             CLSID_MMDeviceEnumerator, NULL,
             CLSCTX_ALL, IID_IMMDeviceEnumerator,
             (void **)&pEnumerator);

    hr = pEnumerator->GetDefaultAudioEndpoint(
             eRender, eConsole, &pDevice);


    // LPWSTR pstr;
    // hr = pDevice->GetId(&pstr);

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


    fseek(frp, DATA, SEEK_SET);

    fread(pData, sizeof(BYTE), bufferFrameCount * 8, frp);


    hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                        bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start playing.

    CMMNotificationClient *pNotify = NULL ;
    pNotify = new(CMMNotificationClient);
    hr = pEnumerator->RegisterEndpointNotificationCallback (pNotify);
    // Each loop fills about half of the shared buffer.
    while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {
        if (pNotify->DefauleChanege == 1)  // if defaule device change
        {
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
            hr = pAudioClient->Start();  // Start playing.
            pNotify->DefauleChanege = 0 ;
        }
        else
        {
            // Sleep for half the buffer duration.
            Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

            // See how much buffer space is available.
            hr = pAudioClient->GetCurrentPadding(&numFramesPadding);

            numFramesAvailable = bufferFrameCount - numFramesPadding;

            // Grab all the available space in the shared buffer.
            hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);

            // hr = pMySource->LoadData(numFramesAvailable, pData, &flags);
            //GetWaveData( pData ,frp,numFramesAvailable ,bitperSample);
            readnum = fread(pData, sizeof(BYTE), numFramesAvailable * 8, frp);
            if (readnum == 0)
                flags |= AUDCLNT_BUFFERFLAGS_SILENT;

            hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
        }
    }

    // Wait for last data in buffer to play before stopping.
    Sleep((DWORD)(hnsActualDuration / REFTIMES_PER_MILLISEC / 2));

    hr = pAudioClient->Stop();  // Stop playing.
    pEnumerator->UnregisterEndpointNotificationCallback(pNotify);
    delete(pEnumerator);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pRenderClient)
    CoUninitialize();
    fclose(frp);
    return hr;
}
