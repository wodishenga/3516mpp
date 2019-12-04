#ifndef __SAMPLE_APPLICATION_H__
#define __SAMPLE_APPLICATION_H__

#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include <stdbool.h>
#include "sample_comm.h"
#include <time.h>


extern void SAMPLE_SVP_NNIE_Yolov3_ProcessAlarm(HI_BOOL isAlarm, VIDEO_FRAME_S *pVbuf);

extern void SAMPLE_SVP_NNIE_Yolov3_AddDetectFlagToList(HI_BOOL ifDetect);


#endif



