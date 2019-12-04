#ifndef __SAMPLE_OSD_H__
#define __SAMPLE_OSD_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>

#include "SDL.h"  
#include "SDL_ttf.h"  
#include "loadbmp.h"

#include "sample_comm.h"
#include "mpi_snap.h"



extern HI_S32 SAMPLE_SVP_REGION_VPSS_OSDEX(HI_BOOL StartFlag);

#endif




