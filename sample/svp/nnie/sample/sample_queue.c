
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "sample_queue.h"

/*添加告警标志至集合*/
int addWarningFlagToList(PQueue queue, int value) 
{
	int ret = -1;
	int out = 0;
	/*如果队列长度大于等于5，则把第一个元素删除，再添加一个元素，否则直接添加*/
	if(GetQueueLength(queue) >= 5) {
		DeleteQueue(queue, &out);
		ret = InsertQueue(queue, value);
	} else {
		ret = InsertQueue(queue, value);
	}
	return ret;
}


/*判断告警集合里面有没有4个告警标志，如果有则返回True，否则返回False*/
bool isAvailableAlarm(PQueue queue) 
{
    
    if (IsEmptyQueue(queue)) {
        return false;
    }  
	
	int count = 0;
	
    PNODE P = queue->Front->Next;    //从队列首节点开始遍历（非头节点，注意区分）
    while (P != NULL) {
        if(P->Element == 1) {	
        	count++;
        }
        P = P->Next;
    }
	
    if(count >= 4) {
    	return true;
    } else {
		return false;
	}
}


/*判断告警集合里面有没有4个非告警标志，如果有则返回True，否则返回False*/
bool isClearingAlarm(PQueue queue)
{
    
    if (IsEmptyQueue(queue)) {
        exit(-1);
    }  
	int count = 0;
	
    PNODE P = queue->Front->Next;    //从队列首节点开始遍历（非头节点，注意区分）
    while (P != NULL) {
        if(P->Element == 0) {	
        	count++;
        }
        P = P->Next;
    }
	
    if(count >= 4) {
    	return true;
    } else {
		return false;
	}
}


/*定义队列初始化函数*/
void InitQueue(PQueue queue) {
    queue->Front = queue->Rear = (PNODE)malloc(sizeof(NODE));    //    动态创建头节点，使队头，队尾指向该节点
    //头节点相当于哨兵节点的作用，不存储数据（区别于首节点）
    if (queue->Front == NULL) {        //    判断内存是否分配成功
        printf("创建队列，无法分配所需内存...");
    }
    queue->Front->Next = NULL;    //    初始队列头节点指针指向为空
}


/*定义判断队列是否为空函数*/
bool IsEmptyQueue(PQueue queue) {
    if (queue->Front == queue->Rear) {
        return true;
    }
    else {
        return false;
    }
}


/*  定义入队函数
    从队列尾部插入数据val */
int InsertQueue(PQueue queue,int val) {
    PNODE P = (PNODE)malloc(sizeof(NODE));    //    创建一个新节点用于存放插入的元素
    if (P == NULL) {
        return -1;
    }
    P->Element = val;    //    把要插入的数据放到节点数据域
    P->Next = NULL;        //    新节点指针指向为空
    queue->Rear->Next = P;    //    使上一个队列尾部的节点指针指向新建的节点
    queue->Rear = P;    //    更新队尾指针，使其指向队列最后的节点
    return 0;
}


/*  定义出队函数
    从队列的首节点开始出队
    若出队成功，用val返回其值*/
void DeleteQueue(PQueue queue,int* val) {
    if (IsEmptyQueue(queue)) {
        printf("队列已经空，无法出队...\n");
        return;
    }
    PNODE  P= queue->Front->Next;    //    临时指针
    *val = P->Element;    //    保存其值
    queue->Front->Next = P->Next;        //    更新头节点
    if (queue->Rear==P)
        queue->Rear = queue->Front;
    free(P);    //    释放头队列
    P = NULL;    //    防止产生野指针
}


/*定义队列遍历函数*/
void TraverseQueue(PQueue queue) {
    
    if (IsEmptyQueue(queue)) {
        exit(-1);
    }        
    PNODE P = queue->Front->Next;    //从队列首节点开始遍历（非头节点，注意区分）
    printf("遍历队列结果为：");
    while (P != NULL) {
        printf("%d ", P->Element);
        P = P->Next;
    }
    printf("\n");
}


/*  定义队列的摧毁函数
    删除整个队列，包括头节点*/
void DestroyQueue(PQueue queue) {
    //从头节点开始删除
    while (queue->Front != NULL) {
        queue->Rear = queue->Front->Next;
        free(queue->Front);
        queue->Front = queue->Rear;
    }
}


/*定义清空队列函数*/
void ClearQueue(PQueue queue) {
    PNODE P = queue->Front->Next;    //    临时指针
    PNODE Q = NULL;        //    临时指针
    queue->Rear = queue->Front;        //    使队尾指针指向头节点
    queue->Front->Next = NULL;
    //    从首节点开始清空
    while (P != NULL) {
        Q = P;
        P = P->Next;
        free(Q);
    }
}


/*获取队列长度*/
int GetQueueLength(PQueue queue)
{
    if (IsEmptyQueue(queue)) {
        return -1;
    }
    int len = 0;
    PNODE P = queue->Front->Next;    //从队列首节点开始遍历（非头节点，注意区分）
    while (P != NULL) {
		len++;
        P = P->Next;
    }
    return len;
}


