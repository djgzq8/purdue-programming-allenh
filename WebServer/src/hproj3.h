/*************************************************************************
*
* file: proj3.c
*
* author: Allen E Humphreys (aehumphr)
*         Sean H King (shking)
*
*   This program accepts a request for data accross a TCP socket which it
*   creates. After the HTTP request is sent it is parsed, the file to be
*   sent is determined and sent to the requesting client. 
* 
*
**************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
/*forking*/
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
/*sockets*/
#include<netinet/in.h>
#include<arpa/inet.h>

#define DEFAULT_PORT 8080 /*if the port isn't specified*/
#define INITIAL_GROWSTRING 4 
#define RECVBUFF_SIZE 1024
#define SENDBUFF_SIZE 16384

struct GrowString {
  char *str;
  int numUsed;
  int maxSize;
};

struct HTTP_Request {
  struct GrowString method;
  struct GrowString filename;
  struct GrowString path;
};

int OpenConnection(int port, int *sockNum);
void GrowStringInit(struct GrowString *s);
void DoubleGrowString(struct GrowString *s);
void GrowStringSet(struct GrowString *s, int newsize);
void GrowStringDestroy(struct GrowString *s);
int RequestParse(char *requeststring, struct HTTP_Request *request);
int isvalidfilename(char ch);
int DotDotRemover(struct HTTP_Request *);
int FileOpen(FILE **, struct HTTP_Request *);
int Reply(FILE **, int socket, int http_err_code);
void DestroyHTTP_Request(struct HTTP_Request *);
