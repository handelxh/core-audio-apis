// coreaudio.cpp : ¶¨Òå¿ØÖÆÌ¨Ó¦ÓÃ³ÌÐòµÄÈë¿Úµã¡£
//

#include "stdafx.h"
#include "Mmdeviceapi.h"

int _tmain(int argc, _TCHAR *argv[])
{
    const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
    const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
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
    return 0;
}

