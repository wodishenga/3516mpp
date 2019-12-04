#ifndef __SAMPLE_QUEUE_H__
#define __SAMPLE_QUEUE_H__

#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include <stdbool.h>


typedef int Elementype;        //    定义数据类型
//    定义节点结构
typedef struct Node {
    Elementype Element;        //    数据域
    struct Node * Next;
}NODE, *PNODE;

//    定义队列结构体
typedef struct QNode {
    PNODE Front, Rear;        //    队列头，尾指针
} Queue, *PQueue;

//    声明函数体
void InitQueue(PQueue);    //    创建队列函数
bool IsEmptyQueue(PQueue);    //    判断队列是否为空函数
int InsertQueue(PQueue, int val);    //    入队函数
void DeleteQueue(PQueue,int * val);    //    出队函数
void DestroyQueue(PQueue);    //    摧毁队列函数
void TraverseQueue(PQueue);    //    遍历队列函数
void ClearQueue(PQueue);    //    清空队列函数
int LengthQueue(PQueue);    //    求队列长度函数
int GetQueueLength(PQueue queue);

int addWarningFlagToList(PQueue queue, int value);   /*添加告警标志至集合*/
/*判断告警集合里面有没有4个告警标志，如果有则返回True，否则返回False*/
bool isAvailableAlarm(PQueue queue);  
/*判断告警集合里面有没有4个非告警标志，如果有则返回True，否则返回False*/
bool isClearingAlarm(PQueue queue);

#endif



