#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "sample_comm.h"
#include <sys/time.h>
#include "sample_md5.h"
#include "sample_cjson.h"
#include "sample_protocol.h"



extern char macAddr[30];

static HI_U8 
hex2char(HI_U8 hex, HI_U8 flag)
{
	if (hex <= 9)
		return hex + '0';
	if (hex >= 10 && hex <= 15) {
		if (flag)
			return (hex - 10 + 'A');
		else
			return (hex - 10 + 'a');
	}
	return '0';
}


static HI_VOID 
hex2string(char *string, char *hex, HI_U16 len, char flag)
{
	for (HI_U8 i = 0; i < len; i++) {
		string[i * 2] = hex2char(hex[i] >> 4 & 0x0f, flag);
		string[i * 2 + 1] = hex2char(hex[i] & 0x0f, flag);
	}
	string[len * 2] = '\0';
}


static HI_VOID
delete_char(char str[],char target)
{
	int i,j;
	for (i = j = 0; str[i] != '\0'; i++) {
		if(str[i] != target) {
			str[j++]=str[i];
		}
	}
	str[j]='\0';
}


HI_S32 
get_mac_addr(char * macAddr)
{
	FILE* f_mac = NULL;
	if((f_mac = fopen("/root/mac.txt","r")) == NULL) {
		printf("Failed to open mac.txt\n");
		return -1;
	}
	
	if (fseek(f_mac, 4, SEEK_SET) != 0) {
		printf("Failed to fseek\n");
		return -1;
	};

	if (fread(macAddr,17,1,f_mac) == 0) {
		printf("Failed to fread\n");
		return -1;
	};
	
	delete_char(macAddr,':');
	printf("macAddr=%s\n", macAddr);
	fclose(f_mac);
	
	return 0;
}


HI_S32
get_mqtt_password(char *macAddr, char *passwordMd5)
{
	
	char  password[30] = "0";
	char md51[16];
	if (strlen((char *)macAddr) > 0) {
		strcpy(password, macAddr);
		strcat((char *)password, "_zdst666");
		md5((const char *)password, strlen((const char *)password), md51);
		hex2string((char *)passwordMd5, md51, 16, 0);
		return 0;
	}
	return -1;
}


HI_S32
get_mqtt_username(char *macAddr, char *username)
{
	if (strlen(macAddr) > 0) {
		strcpy(username, macAddr);
		strcat((char *)username, "_account");
		printf("%s\n", username);
		return 0;
	}
	return -1;
}


HI_S32
get_mqtt_clientid(char    * macAddr, char *client)
{
	if (strlen(macAddr) > 0) {
		strcpy(client, macAddr);
		strcat((char *)client, "_client");
		return 0;
	}
	return -1;
}


HI_S32
get_mqtt_pubTopic(char    * macAddr, char *pubTopic)
{
	if (strlen(macAddr)> 0) {
		strcpy((char *)pubTopic, "/BOX/");
		strcat((char *)pubTopic, macAddr);
		strcat((char *)pubTopic, "/pub");
		return 0;
	}
	return -1;
}


HI_S32 
get_mqtt_subTopic(char *macAddr, char  *subTopic)	
{
	if (strlen(macAddr)> 0) {
		strcpy((char *)subTopic, "/BOX/");
		strcat((char *)subTopic, macAddr);
		strcat((char *)subTopic, "/sub");
		return 0;
	}
	return -1;
}


HI_VOID
get_time(char *time_now)
{
 struct tm nowtime;
 struct timeval tv;
 gettimeofday(&tv, NULL);
 localtime_r(&tv.tv_sec,&nowtime);

 sprintf(time_now,"%d-%d-%d %d:%d:%d",
    nowtime.tm_year+1900,
    nowtime.tm_mon+1,
    nowtime.tm_mday,
    nowtime.tm_hour,
    nowtime.tm_min,
    nowtime.tm_sec
 );
}


static void
CAMERA_GET_PubStr(char *pubContentStr, CAMERA_PUB_INFO_S *pubInfo)
{
	char *msg;
	cJSON *ContentDTO,*alarmDTO; 
	char *picture_path1 = pubInfo->picture_path1;
	char *picture_path2 = pubInfo->picture_path2;
	CAMERA_STATU_E  cameraStatu  = pubInfo->statu;

	//组件cjson对象
	ContentDTO=cJSON_CreateObject();
	cJSON_AddItemToObject(ContentDTO, "alarm", alarmDTO=cJSON_CreateObject());
	if(picture_path1 != NULL && picture_path2 != NULL) {
				cJSON *AlarmPicArray = cJSON_CreateArray();
				cJSON * cJsonpath1 = cJSON_CreateString(picture_path1);
				cJSON * cJsonpath2 = cJSON_CreateString(picture_path2);

				cJSON_AddItemToArray(AlarmPicArray, cJsonpath1);
				cJSON_AddItemToArray(AlarmPicArray, cJsonpath2);
				cJSON_AddItemToObject(alarmDTO,"alarm_pic", 	AlarmPicArray);
			}
	//cJSON_AddStringToObject(alarmDTO,"alarm_time", 	date);
	cJSON_AddNumberToObject(alarmDTO,"channel", 	0);
	cJSON_AddStringToObject(alarmDTO,"mac", 	"111111111111");
	//cJSON_AddNumberToObject(alarmDTO,"oneself", 1);       //是否带分析功能
	cJSON_AddNumberToObject(alarmDTO,"statu",	cameraStatu);
	
	msg=cJSON_Print(ContentDTO);	cJSON_Delete(ContentDTO); 
	//拷贝至字符数组里面
	strcpy(pubContentStr, msg);
	free(msg);
}


/*去掉字符串中的回车以及制表符*/
static void
process(char *str)
{
	int len = strlen(str);
	char buff[len+1];
	int count = 0;
	char *p = str;
	while(*p != '\0')
	{
        if(*p == '\r' || *p == '\t' || *p == '\n' || *p == ' ') {
            p++;
            continue;
        } else {
            buff[count] = *p;
            count++;
            p++;
        }
    }
    buff[count] = '\0';
	strcpy(str,buff);
}


static void 
CAMERA_GET_CjsonInfo(cJSON *PubDTO, char *md5Buf, long long timestamp, CAMERA_PUB_INFO_S *pubInfo)
{
	char pubContentStr[300] = "0";
	char *picture_path1 = pubInfo->picture_path1;
	char *picture_path2 = pubInfo->picture_path2;
	
	CAMERA_PUB_TP_E pubID = pubInfo->pub_id;
	CAMERA_STATU_E  cameraStatu  = pubInfo->statu;
	
	cJSON *PubContentDTO,*AlarmDTO;
	//组建cjson消息
	cJSON_AddItemToObject(PubDTO, "pub_content", PubContentDTO=cJSON_CreateObject());
	cJSON_AddItemToObject(PubContentDTO, "alarm", AlarmDTO=cJSON_CreateObject());
	if(picture_path1 != NULL && picture_path2 != NULL) {
		cJSON *AlarmPicArray = cJSON_CreateArray();
		cJSON * cJsonpath1 = cJSON_CreateString(picture_path1);
		cJSON * cJsonpath2 = cJSON_CreateString(picture_path2);
	
		cJSON_AddItemToArray(AlarmPicArray, cJsonpath1);
		cJSON_AddItemToArray(AlarmPicArray, cJsonpath2);
		cJSON_AddItemToObject(AlarmDTO,"alarm_pic", 	AlarmPicArray);
	}
	cJSON_AddNumberToObject(AlarmDTO,"channel", 	0);
	cJSON_AddStringToObject(AlarmDTO,"mac", 	"111111111111");
	cJSON_AddNumberToObject(AlarmDTO,"statu",	cameraStatu);
	
	/*获取pubcontent 字符串,为了加密*/
	CAMERA_GET_PubStr(pubContentStr,pubInfo);
	/*去掉字符串中的回车以及换行符*/
	process(pubContentStr);
	printf("pubContentDTO = %s\n", pubContentStr);
	
	sprintf(md5Buf, "%d%s%lld%s",pubID ,pubContentStr, timestamp, "_zdst666");
	printf("md5buf = %s\n", md5Buf);

}


HI_S32
CAMERA_GET_PubMsg(CAMERA_PUB_INFO_S *pubInfo, char *pubMsg)
{
	struct timeval tv;
	cJSON *PubDTO; 
	char *out;
	char md51[16] = "0";
	char sign[32] = "0";
	char md5buf[400] = "0";
	
	/*获取时间戳*/
	gettimeofday(&tv,NULL);
	long long timestamp = tv.tv_sec;
	
	/*获取pub主题以及设备当前状态*/
	CAMERA_PUB_TP_E pubID = pubInfo->pub_id;
	CAMERA_STATU_E  cameraStatu  = pubInfo->statu;


	PubDTO=cJSON_CreateObject();
	cJSON_AddNumberToObject(PubDTO,"pub_tp",		pubID);

	switch (pubID) {
		/*获取服务器配置*/
		case PUB_GetServerConfig:
			sprintf(md5buf, "%d%lld%s", pubID, timestamp, "_zdst666");
			break;
		/*主动上报告警消息*/
		case PUB_WarningMsg:
			{
			 switch(cameraStatu) {
			 	case STATU_STAFFNORMAL:
			 	case STATU_STAFFLEAVEWARNING:
					CAMERA_GET_CjsonInfo(PubDTO, md5buf, timestamp, pubInfo);
					break;
				default:
					break;
			 	}
			
			}break;
		default:
			printf("not support yet\n");
			return -1;
			break;
		}
	cJSON_AddNumberToObject(PubDTO,"timestamp", 	timestamp);
	
	md5(md5buf, strlen((const char *)md5buf), md51);
	hex2string(sign, md51, 16, 0);
	cJSON_AddStringToObject(PubDTO,"sign",		sign);
	
	out=cJSON_Print(PubDTO);	cJSON_Delete(PubDTO); 
	
	strcpy(pubMsg, out);
	free(out);
	
	return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */



