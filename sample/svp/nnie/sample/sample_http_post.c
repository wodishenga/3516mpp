#include <sys/time.h>
#include <arpa/inet.h>
#include <stdlib.h>  
#include <sys/types.h>  
#include <unistd.h>
#include <stdio.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <string.h>  
#include <sys/stat.h>  
#include "sample_http_post.h"
#include "sample_cjson.h"

 
 char header[1024]={0}; 
 char send_request[1024]={0};
 char send_end[1024]={0};
 char http_boundary[64]={0};


static unsigned long 
get_file_size(const char *path)  //获取文件大小
{    
    unsigned long filesize = -1;        
    struct stat statbuff;    
    if(stat(path, &statbuff) < 0){    
        return filesize;    
    }else{    
        filesize = statbuff.st_size;    
    }    
    return filesize;    
}   
 
static int SAMPLE_HTTP_PostUploadPic(const char *IP, const unsigned int port,char *URL, const char *filepath,
									char *ack_json, int ack_len) //Post方式上传图片
{
	int cfd = -1;
    int recbytes = -1;
    //int sin_size = -1;
	char buffer[1024*10]={0};     
    struct sockaddr_in s_add;
	
	cfd = socket(AF_INET, SOCK_STREAM, 0);  //创建socket套接字
    if(-1 == cfd)  
    {  
        printf("socket fail ! \r\n");  
        return -1;  
    }
	
	bzero(&s_add,sizeof(struct sockaddr_in));  
    s_add.sin_family=AF_INET; //IPV4
    s_add.sin_addr.s_addr= inet_addr((const char*)IP);  
    s_add.sin_port=htons(port);
    printf("s_addr = %#x ,port : %#x\r\n",s_add.sin_addr.s_addr,s_add.sin_port);
 
	if(-1 == connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))  //建立TCP连接
    {  
        printf("connect fail !\r\n");  
        return -1;  
    }
 
	//获取毫秒级的时间戳用于boundary的值
	long long int timestamp;
	struct timeval tv;
	gettimeofday(&tv,NULL);
	timestamp = (long long int)tv.tv_sec * 1000 + tv.tv_usec;
	snprintf(http_boundary,64,"---------------------------%lld",timestamp);
	
	unsigned long totalsize = 0;
	unsigned long filesize = get_file_size(filepath); //文件大小
	unsigned long request_len = snprintf(send_request,1024,UPLOAD_REQUEST,http_boundary,http_boundary,http_boundary,filepath); //请求信息
	unsigned long end_len = snprintf(send_end,1024,"\r\n--%s--\r\n",http_boundary); //结束信息
	totalsize = filesize + request_len + end_len;
 
	unsigned long head_len = snprintf(header,1024,HTTP_HEAD,SERVER_PATH,URL,http_boundary,totalsize); //头信息
	totalsize += head_len;
 
    char* request = (char*)malloc(totalsize);	//申请内存用于存放要发送的数据
    if (request == NULL){  
        printf("malloc request fail !\r\n");  
        return -1;  
    }  
    request[0] = '\0';
 
	/******* 拼接http字节流信息 *********/	
    strcat(request,header);  									//http头信息
    strcat(request,send_request);    							//文件图片请求信息
    printf("request = %s\n", request);
	FILE* fp = fopen(filepath, "rb+");							//打开要上传的图片
    if (fp == NULL){  
        printf("open file fail!\r\n");  
        return -1;  
    }
	int readbyte = fread(request+head_len+request_len, 1, filesize, fp);//读取上传的图片信息
    if(readbyte < 1024) //小于1024个字节 则认为图片有问题
	{
		printf("Read picture data fail!\r\n");  
        return -1;
	}
    memcpy(request+head_len+request_len+filesize,send_end,end_len);  //http结束信息
 
	/*********  发送http 请求 ***********/
	if(-1 == write(cfd,request,totalsize)) 					
    {  
        printf("send http package fail!\r\n");  
        return -1;  
    }
 
	/*********  接受http post 回复的json信息 ***********/
	if(-1 == (recbytes = read(cfd,buffer,10240)))  
    {  
        printf("read http ACK fail !\r\n");  
        return -1;  
    }  
   	buffer[recbytes]='\0';
	int index = 0,start_flag = 0;
	int ack_json_len = 0;
	for(index = 0; index<recbytes; index++)
	{
		if(buffer[index] == '{')
		{
			start_flag = 1;
		}
		if(start_flag)
			ack_json[ack_json_len++] = buffer[index];  //遇到左大括号则开始拷贝
 
		if(buffer[index] == '\0')		//遇到右大括号则停止拷贝
		{
			ack_json[ack_json_len] = '\0';
			break;
		}
	}
	if(ack_json_len > 0 && ack_json[ack_json_len-1] == '}')  //遇到花括号且有json字符串
	{
		printf("Receive:%s len = %d\n",ack_json,ack_json_len);
	}
	else
	{
		ack_json[0] = '\0';
		printf("Receive http ACK fail!!\n");
		printf("--- ack_json_len = %d\n",ack_json_len);
	}
	
    free(request);  
    fclose(fp);  
    close(cfd);
	return 0;
}


static int getPicPath(char *text, char *path)
{
	cJSON * root = NULL;
	cJSON * item = NULL;
	cJSON * array = NULL;
	root = cJSON_Parse(text);
	if (!root) {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    } else {
    	item = cJSON_GetObjectItem(root, "data");
		if(item != NULL) {
			array = cJSON_GetArrayItem(item, 0);
			if(array) {
				item = cJSON_GetObjectItem(array, "src");
				if(item) {
					strcpy(path, item->valuestring);
					return 0;
				}
			}
		}
	}
	return -1;
}


int CAMERA_Post_Picture(const char *picture_path, char *src)  
{  
	int ack_len = 256;
	char ack_json[256]={0};
	int ret = SAMPLE_HTTP_PostUploadPic(SERVER_ADDR, SERVER_PORT,SERVER_URL,picture_path,ack_json,ack_len); //Post方式上传图片
	if(ret == -1) {
		printf("\n\n----------- Post picture Fail!!\n");
		return -1;
	}
	ret = getPicPath(ack_json, src);   //解析cjson数据，获取文件服务器保存图片的地址
	if(ret == -1) {
		printf("\n\n----------- Get picture path Fail!!\n");
		return -1;
	}
	return ret;  
}

