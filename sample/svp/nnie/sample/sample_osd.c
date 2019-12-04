#include "sample_osd.h"


char tmp[25];
TTF_Font *font;  
SDL_Surface *text, *temp;  
SDL_Rect bounds;
BITMAP_S stBitmap;

static BITMAP_S SDL_OSDtoBMP(char *tmp_sys_time)  
{  
	if (TTF_Init() < 0) {  
		fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());  
		SDL_Quit();
	}  
       
	font = TTF_OpenFont("./simhei.ttf", 48);
	if (font == NULL) {  
		fprintf(stderr, "Couldn't load %d pt font from %s: %s\n", 48,"ptsize",SDL_GetError());  
	}  
	SDL_Color forecol=   { 0xff, 0xff, 0xff, 0xff };  
	text = TTF_RenderUTF8_Solid(font, tmp_sys_time, forecol);
       
	SDL_PixelFormat *fmt;
	fmt = (SDL_PixelFormat*)malloc(sizeof(SDL_PixelFormat));
	memset(fmt,0,sizeof(SDL_PixelFormat));
	fmt->BitsPerPixel = 16;
	fmt->BytesPerPixel = 2;
	fmt->colorkey = 0xffffffff;
	fmt->alpha = 0xff;

	SDL_Surface *temp = SDL_ConvertSurface(text,fmt,0);
	SDL_SaveBMP(temp, "sys_time.bmp");
       
	stBitmap.u32Width = temp->pitch/2;                //do not use temp->w
	stBitmap.u32Height = temp->h;
	stBitmap.pData= temp->pixels;

	stBitmap.enPixelFormat= PIXEL_FORMAT_ARGB_1555;
       
	SDL_FreeSurface(text);  
	SDL_FreeSurface(temp);
	TTF_CloseFont(font);  
	TTF_Quit();  
	return stBitmap;
}


static HI_S32 SAMPLE_RGN_UpdateCanvas(const char* filename, BITMAP_S* pstBitmap, HI_BOOL bFil,
                               HI_U32 u16FilColor, SIZE_S* pstSize, HI_U32 u32Stride, PIXEL_FORMAT_E enPixelFmt)
{
    OSD_SURFACE_S Surface;
    OSD_BITMAPFILEHEADER bmpFileHeader;
    OSD_BITMAPINFO bmpInfo;

    if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
        printf("GetBmpInfo err!\n");
        return HI_FAILURE;
    }

    if (PIXEL_FORMAT_ARGB_1555 == enPixelFmt) {
        Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
    } else if (PIXEL_FORMAT_ARGB_4444 == enPixelFmt) {
        Surface.enColorFmt = OSD_COLOR_FMT_RGB4444;
    } else if (PIXEL_FORMAT_ARGB_8888 == enPixelFmt) {
        Surface.enColorFmt = OSD_COLOR_FMT_RGB8888;
    } else {
        printf("Pixel format is not support!\n");
        return HI_FAILURE;
    }
	
    if (NULL == pstBitmap->pData) {
        printf("malloc osd memroy err!\n");
        return HI_FAILURE;
    }
    CreateSurfaceByCanvas(filename, &Surface, (HI_U8*)(pstBitmap->pData), pstSize->u32Width, pstSize->u32Height, u32Stride);

    pstBitmap->u32Width  = Surface.u16Width;
    pstBitmap->u32Height = Surface.u16Height;

    if (PIXEL_FORMAT_ARGB_1555 == enPixelFmt) {
        pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_1555;
    } else if (PIXEL_FORMAT_ARGB_4444 == enPixelFmt) {
        pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_4444;
    } else if (PIXEL_FORMAT_ARGB_8888 == enPixelFmt) {
        pstBitmap->enPixelFormat = PIXEL_FORMAT_ARGB_8888;
    }
	
    int i, j;
    HI_U16* pu16Temp;
    pu16Temp = (HI_U16*)pstBitmap->pData;

    if (bFil)
    {
        for (i = 0; i < pstBitmap->u32Height; i++)
        {
            for (j = 0; j < pstBitmap->u32Width; j++)
            {
                if (u16FilColor == *pu16Temp) {
                    *pu16Temp &= 0x7FFF;
                }
                pu16Temp++;
            }
        }

    }

    return HI_SUCCESS;
}


HI_S32 SAMPLE_RGN_AddOsdToVpss(HI_BOOL StartFlag)
{
    RGN_HANDLE Handle;  //定义区域句柄。
    HI_S32 s32Ret = HI_SUCCESS;
    RGN_ATTR_S stRgnAttrSet;  //定义区域属性结构体。
    RGN_CANVAS_INFO_S stCanvasInfo; //定义画布信息结构体。
    BITMAP_S stBitmap;  //定义位图图像信息结构。
    SIZE_S stSize;
   
   
    /*************************************************
      step 1: create region and attach to vpss group
    *************************************************/
    Handle    = 20;
   
	char *wday[]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	time_t timep;
	struct tm *p;
	char time_buf[30]="\0";
            
	while(StartFlag)
	{
		time(&timep);
		p=localtime(&timep);
		sprintf(time_buf, "%04d/%02d/%02d-%02d:%02d:%02d-%s", (1900+p->tm_year), (1+p->tm_mon), p->tm_mday,\
                                p->tm_hour, p->tm_min, p->tm_sec, wday[p->tm_wday]);
       
		//printf("%s\n",time_buf);
		stBitmap = SDL_OSDtoBMP(time_buf);
               
		sleep(1);       
          
		/*************************************************
          step 7: load bitmap to region
		 *************************************************/
		s32Ret = HI_MPI_RGN_GetAttr(Handle, &stRgnAttrSet);  //获取区域属性。
		if (HI_SUCCESS != s32Ret) {
            printf("HI_MPI_RGN_GetAttr failed! s32Ret: 0x%x.\n", s32Ret);
            return s32Ret;
        }
       
        s32Ret = HI_MPI_RGN_GetCanvasInfo(Handle, &stCanvasInfo); //获取区域的显示画布信息。
        if (HI_SUCCESS != s32Ret) {
            printf("HI_MPI_RGN_GetCanvasInfo failed! s32Ret: 0x%x.\n", s32Ret);
            return s32Ret;
        }
       
        stBitmap.pData   = (HI_VOID*)(HI_UL)stCanvasInfo.u64VirtAddr;  //位图数据。
        stSize.u32Width  = stCanvasInfo.stSize.u32Width;                          //位图宽度。
        stSize.u32Height = stCanvasInfo.stSize.u32Height;  
        s32Ret = SAMPLE_RGN_UpdateCanvas("sys_time.bmp", &stBitmap, HI_FALSE, 0, &stSize, stCanvasInfo.u32Stride,
                                         stRgnAttrSet.unAttr.stOverlayEx.enPixelFmt);
        if (HI_SUCCESS != s32Ret) {
            printf("SAMPLE_RGN_UpdateCanvas failed! s32Ret: 0x%x.\n", s32Ret);
            return s32Ret;
         }
         
        s32Ret = HI_MPI_RGN_UpdateCanvas(Handle);  //更新显示画布。
        if (HI_SUCCESS != s32Ret) {
            printf("HI_MPI_RGN_UpdateCanvas failed! s32Ret: 0x%x.\n", s32Ret);
           return s32Ret;
        }
       
	}
	
	if(NULL != stBitmap.pData) {
		free(stBitmap.pData);
		stBitmap.pData = NULL;
	}
	
	
	
}


static HI_S32 SAMPLE_SVP_REGION_VI_VPSS_VO(HI_S32 HandleNum,RGN_TYPE_E  enType,MPP_CHN_S *pstChn,HI_BOOL StartFlag)
{
    HI_S32         i;
    HI_S32         s32Ret;
    HI_S32         MinHandle;
	
    s32Ret = SAMPLE_COMM_REGION_Create(HandleNum,enType);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_Create failed!\n");
        return HI_FALSE;
    }
    s32Ret = SAMPLE_COMM_REGION_AttachToChn(HandleNum,enType,pstChn);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
        return HI_FALSE;
    }
    MinHandle = SAMPLE_COMM_REGION_GetMinHandle(enType);
	
	SAMPLE_RGN_AddOsdToVpss(StartFlag);  

    s32Ret = SAMPLE_COMM_REGION_DetachFrmChn(HandleNum,enType,pstChn);
    if(HI_SUCCESS!= s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
    }

    s32Ret = SAMPLE_COMM_REGION_Destroy(HandleNum,enType);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_REGION_AttachToChn failed!\n");
    }

    return s32Ret;
}


HI_S32 SAMPLE_SVP_REGION_VPSS_OSDEX(HI_BOOL StartFlag)
{
    HI_S32             s32Ret;
    HI_S32             HandleNum;
    RGN_TYPE_E         enType;
    MPP_CHN_S          stChn;

    HandleNum =1;
    enType = OVERLAYEX_RGN;
    stChn.enModId = HI_ID_VPSS;
    stChn.s32DevId = 0;
    stChn.s32ChnId = 0;
    s32Ret = SAMPLE_SVP_REGION_VI_VPSS_VO(HandleNum,enType,&stChn,StartFlag);
    return s32Ret;
}

