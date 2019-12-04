
#ifndef __HTTP_POST__
#define __HTTP_POST__
 
#define SERVER_ADDR	"192.168.1.239"               //文件服务器地址        线上地址:119.145.35.146
#define SERVER_PORT	7070
#define SERVER_URL	"alarmfile.zds-t.com"    
#define SERVER_PATH	"/uploadFile"
 
#define HTTP_HEAD 	"POST %s HTTP/1.1\r\n"\
					"Host: %s\r\n"\
					"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:59.0) Gecko/20100101 Firefox/59.0\r\n"\
					"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"\
					"Accept-Language: en-US,en;q=0.5\r\n"\
					"Accept-Encoding: gzip, deflate\r\n"\
					"Content-Type: multipart/form-data; boundary=%s\r\n"\
					"Content-Length: %ld\r\n"\
					"Connection: close\r\n"\
					"Upgrade-Insecure-Requests: 1\r\n"\
					"DNT: 1\r\n\r\n"\
					
					
#define UPLOAD_REQUEST	"--%s\r\n"\
						"Content-Disposition: form-data; name=\"cameraMac\"\r\n"\
						"\r\n"\
						"22:11:11:11:11:11\r\n"\
						"--%s\r\n"\
						"Content-Disposition: form-data; name=\"boxMac\"\r\n"\
						"\r\n"\
						"fe:e6:ac:14:9e:1b\r\n"\
						"--%s\r\n"\
						"Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"\
						"Content-Type: image/jpeg\r\n\r\n"\

					
 

extern int CAMERA_Post_Picture(const char *picture_path, char *src);
 
#endif
