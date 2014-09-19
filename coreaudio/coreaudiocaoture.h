#include <string.h>
#include "Mmdeviceapi.h"   //IMMDevice interface
#include  "Audioclient.h" //WASAPI
#include "Audiopolicy.h"  //WASAPI interface
#include <EndpointVolume.h>
#include "CWaveFile.h"
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

// #define EXIT_ON_ERROR(hres)  \
//               if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);