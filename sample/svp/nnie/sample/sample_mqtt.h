#ifndef SAMPLE_MQTT_H_
#define SAMPLE_MQTT_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "MQTTClient.h"

#include "sample_protocol.h"

typedef struct
{
	char act[10];               //摄像头账号
	int allow_play;          //允许玩手机时间（分钟）
	char attireLocs[40];        //着装坐标
	char brand[5];             //摄像头品牌
	int channel;             //通道号
	int httpPort;
	char ip[20];                //摄像头静态IP
	char mac[20];               //摄像头MAC地址
	int model;
	int motionless_time;     //人员静止多少分钟报警
	int moving_range;        //运动幅度大小阈值
	int parkingTime;         //违停时间超过时限报警
	char pwd[20];               //摄像头密码
	int to;                //功能,AL摄像头一次只跑一个算法
}SERVER_CONFIG;



extern int mqtt_publish_data(char *data, int len);
extern int mqtt_subscribing_process(void);

#endif 





