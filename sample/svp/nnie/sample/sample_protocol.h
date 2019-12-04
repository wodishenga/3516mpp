/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef SAMPLE_PROTOCOL_H_
#define SAMPLE_PROTOCOL_H_

#include "sample_comm.h"

//pub主题
typedef enum  hiCAMERA_PUB_TP_E
{
    PUB_GetServerConfig       = 1,    //1，主动向服务器获取配置
    PUB_ReplyConfigComplete   = 2,    //2，回应服务器配置更新完成
    PUB_WarningMsg            = 4,    //4, 主动上报报警信息
} CAMERA_PUB_TP_E;


//设备状态
typedef enum hiCAMERA_STATU_E
{
	STATU_FIRECHANNELNORMAL    = 1,     //1.消防通道正常
	STATU_STAFFNORMAL          = 2,     //2.人员在岗正常
	STATU_FIRENORMAL           = 3,     //3.火焰正常
	STATU_SMOKENORMAL          = 4,     //4.烟雾正常
	STATU_CONNECTIONLOST       = 5,     //5.失联 
	STATU_FIRECHANNELWARNING   = 6,     //6.消防通道阻塞报警
	STATU_STAFFLEAVEWARNING    = 7,     //7.人员离岗报警
	STATU_FIREWARNING          = 8,     //8.火焰报警
	STATU_SMOKEWARNING         = 9,     //9.烟雾报警
	STATU_SLEEPPOSTNORMAL      = 10,    //10.睡岗正常
	STATU_SLEEPPOSTWARNING     = 11,    //11.睡岗报警
	STATU_SALUTENORMAL         = 12,    //12.敬礼正常 
	STATU_SALUTEWARNING        = 13,    //13.敬礼报警
	STATU_PLAYCELLPHONENORMAL  = 14,    //14.玩手机正常
	STATU_PLAYCELLPHONEWARNING = 15,    //15.玩手机报警
	STATU_DRESSNORMAL          = 16,    //16.着装正常
	STATU_DRESSWARNING         = 17,    //17.着装报警
	STATU_PARKINGNORMAL        = 18,    //18.停车正常
	STATU_PARKINGWARNING       = 19,    //19.违停报警
	STATU_OVERWALLNORMAL       = 20,    //20.翻墙正常
	STATU_OVERWALLWARNING      = 21,    //21.翻墙报警
	STATU_INVADENORMAL         = 22,    //22.入侵正常
	STATU_INVADEWARNING        = 23,    //23.入侵报警
}CAMERA_STATU_E;

typedef struct hiCAMERA_PUB_INFO
{
    CAMERA_PUB_TP_E pub_id;           //pub主题
	CAMERA_STATU_E statu;             //设备状态
	char *picture_path1;              //告警图片上传到文件服务器的路径1
	char *picture_path2;              //告警图片上传到文件服务器的路径1
}CAMERA_PUB_INFO_S;





extern HI_S32 get_mac_addr(char * macAddr);
extern HI_S32 get_mqtt_password(char *macAddr, char *passwordMd5);
extern HI_S32 get_mqtt_username(char    * macAddr, char *username);
extern HI_S32 get_mqtt_clientid(char    * macAddr, char *client);
extern HI_S32 get_mqtt_pubTopic(char    * macAddr, char *pubTopic);
extern HI_S32 get_mqtt_subTopic(char *macAddr, char  *subTopic);
extern HI_S32 CAMERA_GET_PubMsg(CAMERA_PUB_INFO_S *pubInfo, char *pubMsg);





#endif /* PROJECT_CONF_H_ */
