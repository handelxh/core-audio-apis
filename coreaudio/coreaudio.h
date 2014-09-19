#include "stdafx.h"
#include <string.h>
#include "Mmdeviceapi.h"   //IMMDevice interface
#include  "Audioclient.h" //WASAPI
#include "Audiopolicy.h"  //WASAPI interface
#include <EndpointVolume.h>
#include "CWaveFile.h"
#include <process.h>  
#include "windows.h"
extern unsigned int WINAPI CoreAudioRender(void *);
extern HRESULT CoreAudioCapture(FILE *fp);
FILE *frp;