// coreaudio.cpp
//
#include "stdafx.h"
#include <string.h>
#include "coreaudio.h"
int _tmain(int argc, _TCHAR *argv[])
{
    FILE *fp;
    string fname = "C:\\Users\\rickon\\Desktop\\caiqin.wav";
    if ((fp = fopen(fname.c_str(), "rb")) == NULL)
    {
        printf("can not open this wave file\n");
    }
    CoreAudioRender(fp);
    return 0;
}

