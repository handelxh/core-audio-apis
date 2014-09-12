
#include <string.h>
#include "Mmdeviceapi.h"   //IMMDevice interface
#include  "Audioclient.h" //WASAPI
#include "Audiopolicy.h"  //WASAPI interface
#include <EndpointVolume.h>
#include "CWaveFile.h"
#define REFTIMES_PER_SEC  10000*1000*1
#define REFTIMES_PER_MILLISEC  10000

//     if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
    if ((punk) != NULL)  \
    { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

