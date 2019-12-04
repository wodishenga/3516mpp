#include "sample_application.h"
#include "sample_queue.h"
#include "sample_mqtt.h"
#include "sample_protocol.h"
#include "sample_http_post.h"

#include <sys/time.h>

Queue queue;	//创建队列变量,保存每次图片识别的告警状态，长度为5


HI_BOOL setAlarming = HI_FALSE;   //设置告警标志位

HI_BOOL ifAlarmReportToServer = HI_FALSE; //告警是否上报到平台

long beginAlarmTime = 0;   //开始告警时间

long nextAlarmTime = 0;    //下一次告警的时间

long timeInterval = 0;     //时间间隔


/*获取当前时间*/
static long getLocalTime(void)
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	
	return tv.tv_sec;
}


static int CAMERA_Publish_Msg(CAMERA_STATU_E statu)
{
	char pubMsg[500] = {0};
	CAMERA_PUB_INFO_S pubinfo;
	memset(&pubinfo, 0, sizeof(CAMERA_PUB_INFO_S));
	
	pubinfo.pub_id = PUB_WarningMsg;
	pubinfo.statu = statu;
	
	char *first_pic_path = NULL;
	char *last_pic_path  = NULL;
	int status = (int)statu;
	switch(status)
	{
		case STATU_STAFFNORMAL:
			pubinfo.picture_path1 = NULL;
			pubinfo.picture_path2 = NULL;
			break;
		case STATU_STAFFLEAVEWARNING:
			{
				first_pic_path = (char *)malloc(100);
				last_pic_path  = (char *)malloc(100);
				//获取图片路径
				if(CAMERA_Post_Picture("./begin.jpg", first_pic_path) != 0) {
					printf("Failed to get the first picture path\n");
					return -1;
				}  
				if(CAMERA_Post_Picture("./end.jpg", last_pic_path) != 0) {
					printf("Failed to get the end picture path\n");
					return -1;
				}
				pubinfo.picture_path1 = first_pic_path;
				pubinfo.picture_path2 = last_pic_path;
			}
			break;
		defalut:
			break;
	}
	
	if (CAMERA_GET_PubMsg(&pubinfo, pubMsg) != 0) {
		printf("Failed to get warning pubinfo\n");
		return -1;
	} 

	int msgLen = strlen(pubMsg);
	if (mqtt_publish_data(pubMsg, msgLen) != 0 ) {
		printf("Failed to publish warning data\n");
		return -1;
	}

	if(first_pic_path != NULL)
	{
		free(first_pic_path);
		first_pic_path = NULL;
	}

	if(last_pic_path != NULL)
	{
		free(last_pic_path);
		last_pic_path = NULL;
	}

	return 0;
}


/*人员监测告警事件处理*/
void SAMPLE_SVP_NNIE_Yolov3_ProcessAlarm(HI_BOOL isAlarm, VIDEO_FRAME_S *pVbuf)
{
	
	if(isAlarm && !setAlarming) {                                                    //1.上一帧没检测到告警，这一次检测到告警，告警状态位设置为TRUE
		setAlarming = HI_TRUE; 
		beginAlarmTime = getLocalTime();
		printf("发现人员离岗警情 time = %ld\n", beginAlarmTime);
		
		/*保存最开始告警的图片*/
		SAMPLE_SVP_NNIE_Yolov3_SaveBitMap(pVbuf, 0);
	} else if (isAlarm && setAlarming) {                                             //2.上一帧检测到告警，这一帧同样检测到告警，记录时间间隔
		nextAlarmTime = getLocalTime();
		timeInterval = nextAlarmTime - beginAlarmTime;
		printf("人员离岗警情持续 , timeInterval = %ld\n",  timeInterval);

		/*如果队列集合中检测到4个告警标志，并且时间间隔大于设定值，则上报警情*/
		if(isAvailableAlarm(&queue) && !ifAlarmReportToServer && timeInterval > 10) {
			printf("上报人员离岗警情\n");
			
			/*保存最后一张图片*/
			SAMPLE_SVP_NNIE_Yolov3_SaveBitMap(pVbuf, 1);
			
			/*推送告警消息*/
			CAMERA_STATU_E statu = STATU_STAFFLEAVEWARNING;
  			CAMERA_Publish_Msg(statu);
			
			ifAlarmReportToServer = HI_TRUE;
		}
	} else if (!isAlarm && setAlarming) {                                             //3.这一帧没告警，上一帧告警
		/*连续检测到4个非告警标志位False*/
		if(isClearingAlarm(&queue)) {
			 //如果已经报了警,则上报告警清除
			if(ifAlarmReportToServer) {    
				
				printf("人员在岗正常\n");
				/*告警清除*/
				CAMERA_STATU_E statu = STATU_STAFFNORMAL;
				CAMERA_Publish_Msg(statu);
				
				ifAlarmReportToServer = HI_FALSE;
			} else {
				//否则表示无效的警情
				printf("无效的警情\n");
			}

			/*告警状态置位False*/
			setAlarming = HI_FALSE;
		} else {
			printf("没有识别出???\n");
		}
	} else {                                       //4.什么都没发生                          
		setAlarming = HI_FALSE;
		printf("什么也没发生\n");
	}
	
}


/*把告警标志位加入队列*/
void SAMPLE_SVP_NNIE_Yolov3_AddDetectFlagToList(HI_BOOL ifDetect)
{
	if(ifDetect) {
		addWarningFlagToList(&queue, 1);
	} else {
		addWarningFlagToList(&queue, 0);
	}
}


