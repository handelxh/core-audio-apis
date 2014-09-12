
#include "stdafx.h"
#include <string.h>
#include "Mmdeviceapi.h"   //IMMDevice interface
#include  "Audioclient.h" //WASAPI
#include "Audiopolicy.h"  //WASAPI interface
#include <EndpointVolume.h>
#include "CWaveFile.h"

int GetWaveData(UINT8 *data , FILE *fp, UINT32 framelen , int bitperSample)
{
    UINT32 temp32[60000];
    UINT16 temp16[60000];
    UINT8  temp8[60000];
    UINT32 data1[60000];
    if (bitperSample == 8)
    {
        fread(temp8, sizeof(UINT8), framelen, fp);
        for (int i = 0; i < framelen; ++i)
        {
            data1[i] = temp8[i];
        }
        memcpy(data, data1, framelen * 8);
        return 0;
    }
    else if (bitperSample == 16)
    {
        fread(temp16, sizeof(UINT16), framelen, fp);
        for (int i = 0; i < framelen; ++i)
        {
            data1[i] = temp16[i];
        }
        memcpy(data, data1, framelen * 8);
        return 0;
    }
    else if (bitperSample == 32)
    {
        fread(temp32, sizeof(UINT32), framelen, fp);
        for (int i = 0; i < framelen; ++i)
        {
            data1[i] = temp32[i];
        }
        memcpy(data, data1, framelen * 8);
        return 0;
    }
    else
        return 0;
}


// 5249 4646 a099 0000 5741 5645 666d 7420
// 1000 0000 0100 0200 007d 0000 00fa 0000
// 0200 0800 6461 7461 7c99 0000

// 5249 4646 a4e3 0203 5741 5645 666d 7420
// 1000 0000 0100 0200 44ac 0000 10b1 0200
// 0400 1000 6461 7461 80e3 0203
//
// 5249 4646 a4aa 6b00 5741 5645 666d 7420
// 1000 0000 0300 0200 44ac 0000 2062 0500
// 0800 2000 6461 7461 80aa 6b00
// typedef struct tWAVEFORMATEX
// {
//     WORD        wFormatTag;         /* format type */
//     WORD        nChannels;          /* number of channels (i.e. mono, stereo...) */
//     DWORD       nSamplesPerSec;     /* sample rate */
//     DWORD       nAvgBytesPerSec;    /* for buffer estimation */
//     WORD        nBlockAlign;        /* block size of data */
//     WORD        wBitsPerSample;     /* number of bits per sample of mono data */
//     WORD        cbSize;             /* the count in bytes of the size of */
//     /* extra information (after cbSize) */
// } WAVEFORMATEX

void GetWaveFormate(FILE *fp, WAVEFORMATEX *waveformat)
{
    DWORD temp;
    waveformat->wFormatTag = 0xfffe;
    fseek(fp, NUMCHANNELS, SEEK_SET);
    fread(&temp, sizeof(BYTE), 2, fp);
    waveformat->nChannels = temp;
    temp = 0;
    fseek(fp, SAMPLERATE, SEEK_SET);
    fread(&temp, sizeof(BYTE), 4, fp);
    waveformat->nSamplesPerSec = temp;
    temp = 0;
    fseek(fp, BYTERATE, SEEK_SET);
    fread(&temp, sizeof(BYTE), 4, fp);
    waveformat->nAvgBytesPerSec = temp;
    temp = 0;
    fseek(fp, BLOCKALIGN, SEEK_SET);
    fread(&temp, sizeof(BYTE), 2, fp);
    waveformat->nBlockAlign = temp;
    temp = 0;
    fseek(fp, BITSPERSAMPLE, SEEK_SET);
    fread(&temp, sizeof(BYTE), 2, fp);
    waveformat->wBitsPerSample = temp;
    temp = 0;
    waveformat->cbSize = 22;
}