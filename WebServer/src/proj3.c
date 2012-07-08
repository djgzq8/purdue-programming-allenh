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
#include "hproj3.h"

int main(int argc, char **argv)
{
  struct GrowString Request_String;/*resizable array to receive request*/
  int sockNum;/*socket to listen on*/
  int new_socket;/*accepting socket*/
  struct sockaddr_in connect_addr;/*address on incoming connection*/
  int connect_addr_len = sizeof(connect_addr);
  int max_requests;/*requests to take before terminating server*/
  int ctr;
  struct HTTP_Request request;/*method, filename, and path in request*/
  int http_err_code;/*the http code to reply with*/
  int port;/*port to listen on*/
  int recv_lcv = 0;/*recv flag that indicates a proper request was recvd*/
  char recvbuff[RECVBUFF_SIZE];
  int pid;/*if the fork succeds thepid determines which proc needs to do what*/
  int bytes_recvd;/*bytes from recv*/
  FILE *file;/*pointer to file to be sent to client*/
  int lcv = 0;/*generic counter*/

/*check main's arguments for correctness*/
  if (argc > 3){
    fprintf(stderr, "Invalid Arguments To Function\n");
    exit(-1);
  }

  if (argv[1] == NULL){
    port = DEFAULT_PORT;
  }
  else{
    port = atoi(argv[1]);

  }

  if (argv[2] == NULL || argv[1] == NULL){
    max_requests = 5;
  }
  else{
    max_requests = atoi(argv[2]);
  }

/*open the socket and listen on it*/
  OpenConnection(port, &sockNum);

/*accept requests for max_requests*/
  for (ctr = 0; ctr < max_requests; ctr++){
    new_socket = accept(sockNum, (struct sockaddr *)&connect_addr,
	 &connect_addr_len);
/*if the accept succeeds then fork*/
    if (new_socket != -1){
      pid = fork();
/*the child handles the requests and the parent closes the new socket*/
      if (pid == 0){
        close(sockNum);/*close the listening socket*/

/*initialize a resizeable array to recv the request into*/
        GrowStringInit(&(Request_String));
        GrowStringSet(&(Request_String), RECVBUFF_SIZE);
        while(!recv_lcv){
          bytes_recvd = 0;
          bytes_recvd = recv(new_socket, recvbuff, RECVBUFF_SIZE - 1, 0);
/*check the recv for success, if its 0 an infinite loop can occur*/
          if (bytes_recvd == -1){
            close(new_socket);
            exit(-1);
          }
          else if (bytes_recvd == 0){
            break;
          }
/*force a NUL character*/
          recvbuff[bytes_recvd] = '\0';
          if(Request_String.maxSize <=  Request_String.numUsed + bytes_recvd + 1){
            DoubleGrowString(&(Request_String));
          }
/*concantenate the recvbuff into the resizeable array*/
          strcat(Request_String.str, recvbuff);
          Request_String.numUsed += bytes_recvd;
          lcv = 0; 
          while (Request_String.str[lcv] != '\0'){
            if ((Request_String.str[lcv] == '\r') && (Request_String.str[lcv + 
		1] == '\n') && (Request_String.str[lcv + 2] == '\r') && 
		(Request_String.str[lcv + 3] == '\n')){

              recv_lcv = 1;
              break;
            }
            else{
              lcv++;
            }
          }
        }

/*if the recv_lcv is 1 then we can operate on the request*/
        if (recv_lcv){
          http_err_code = RequestParse(Request_String.str, &request);
          if (http_err_code == 400){
            Reply(&file, new_socket, 400);
          }
          else {
/*remove any /../'s open the file, and send the data requested*/
            DotDotRemover(&(request));
            http_err_code = FileOpen(&file, &request);
            Reply(&file, new_socket, http_err_code);
          }
          DestroyHTTP_Request(&request);
        }
        close(new_socket);
        exit(0);
      }
      else{
        close(new_socket);
      }
    }
    else{
      fprintf(stderr, "Failed To Accept Connection\n");
      exit(-1);
    }
  }
  close(sockNum);
  return(0);
}

/*************************************************************************
*
* OpentConnection(port, *sockNum)
*
* port: integer describing port to listen on
* *sockNum: the socket to be created and listened on
*
* This funciton opens a connection on the port described in the first argument
* to main then listens on it.
*
**************************************************************************/
int OpenConnection(int port, int *sockNum){
  struct sockaddr_in addr;

/*check to see if the port is in a valid range*/
  if (port < 1024 || port > 65535){
    fprintf(stderr, "Incorrect port value.\n");
    exit(-1);
  }

/*create the socket to listen on */
  *sockNum = socket(AF_INET, SOCK_STREAM, 0);
  if (*sockNum == -1){
    fprintf(stderr,"Failed To Create Listening Socket on Port %d\n", port);
    exit(-1);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);    /* a specific port number that we want */
  addr.sin_addr.s_addr = htonl(INADDR_ANY); /* any IP address on this machine */

/*bind it to the struct sockaddr_in*/
  if ((bind(*sockNum, (struct sockaddr *)&addr, sizeof(addr))) == -1){
    fprintf(stderr, "Binding Error\n");
    exit(-1);
  }

/*listen on the socket*/
  if ((listen(*sockNum, 1024)) == -1){
    fprintf(stderr, "Failed To Listen On Port %d\n", port);
    exit (-1);
  }
  return (0);
}

/*************************************************************************
*
* RequestParse(*requeststring, struct *request)
*
* requeststring: a pointer to a string containing the request information
* request: a structure that is to be filled in by this funciton
*
* This function separates the information in requeststring into the struct
* HTTP_Reqeust *request. The goal is to get the method, filename, and path.
* It will also return error codes if the request method is incorrect.
*
**************************************************************************/
int 
RequestParse(char *requeststring, struct HTTP_Request *request){
  int ctr = 0;/*ctr used several times*/
  char *walker;/*for examining the request string*/
  int final_slash = -1;/*flag to determine the location of the last slash*/

  walker = requeststring;

/*initialize all the structure elements*/
  GrowStringInit(&(request->path));
  GrowStringInit(&(request->method));
  GrowStringInit(&(request->filename));

/*walk through the requeststring looking for a white space to get the method*/
  while (isvalidfilename(*walker)){
    if (request->method.maxSize == request->method.numUsed){
      DoubleGrowString(&(request->method));
    }
    request->method.str[request->method.numUsed] = *walker;
    request->method.numUsed++;
    walker++;
  }
/*minimize the memory used and set the \0 character*/
  GrowStringSet(&(request->method), request->method.numUsed + 1);

/*determine if its a valid request*/
  if (strcmp(request->method.str, "GET")){
    return(400);
  }

/*walk past the white space*/
  walker++;

/*add the . to indicate the current directory*/
  request->path.str[0] = '.';
  request->path.numUsed = 1;

/*walker through the filename until a whitepsace or ?*/
  while(isvalidfilename(*walker) && *walker != '?'){
    if (request->path.maxSize == request->path.numUsed){
      DoubleGrowString(&(request->path));
    }
    if (*walker == '/'){
      final_slash = request->path.numUsed;
    }
    request->path.str[request->path.numUsed] = *walker;
    request->path.numUsed++;
    walker++;
  }

/*minimize the memory and set the \0*/
  GrowStringSet(&(request->path), request->path.numUsed + 1);

/*if there wasn't a slash then its a bad request*/
  if (final_slash == -1){
    return(400);
  }

/*copy the filename out of the path*/
/*if the filename isn't given then the default is index.html*/
  request->filename.numUsed = strlen(request->path.str) - final_slash - 1;
  if (request->filename.numUsed == 0){
    GrowStringSet(&(request->filename), 11);
    request->filename.str = "index.html";
    request->filename.numUsed = 11;
  }
  else {
    GrowStringSet(&(request->filename), request->filename.numUsed + 1);
    for (ctr = (final_slash + 1); ctr < strlen(request->path.str); ctr++){
      request->filename.str[ctr - final_slash - 1] = request->path.str[ctr];
    }
  }

/*minimize the memory and set the \0*/
  request->path.numUsed = final_slash + 1;
  GrowStringSet(&(request->path), final_slash + 2);

  return(200);
}

/*************************************************************************
*
* DotDotRemover(struct HTTP_Request *request)
*
* request: A structure containing the path, method and filename requested
*      by the client
*
*  This function checks to see if ".." is the current file name. 
*   If it is the file name it changes it so that its no longer the file
*   name and removes the "..". If the file nam is not ".." then this 
*   function looks through the path to see if ".." is in it.  If ".." is
*   found between "/" and not next to another character it takes the ".."
*   out of the path. 
**************************************************************************/
//int DotDotRemover(struct HTTP_Request *request)
//{
//  int j;/*generic counter*/
//  int ctr;/*counter 2*/
//  int place_holder = 0;/*determines the remaining length in the path*/
//  int filename_place;/*holds the place of the filename
//                      *in the case the filename is ".."*/
//  int lcv;/*another counter*/
//
///* string compare the file name with ".." to see if it is the same */
//  if (!(strcmp(request->filename.str, ".."))){
//    for (lcv = strlen(request->path.str) - 2; lcv >= 0; lcv--){
//      if (request->path.str[lcv] == '/'){
//        filename_place = lcv;
//        break;
//      }
//    }
///* the ".." is removed as the file name and a new file name is found*/
///* using the back slash before as the reference to the file name.*/
//    request->filename.numUsed = strlen(request->path.str) - filename_place - 2;
//    GrowStringSet(&(request->filename), request->filename.numUsed + 1);
//
//    for (lcv = filename_place + 1; lcv < strlen(request->path.str) - 1; lcv++){
//      request->filename.str[lcv - 1 - filename_place] = request->path.str[lcv];
//    }
//
//    request->path.numUsed = filename_place + 1;
//    GrowStringSet(&(request->path), request->path.numUsed + 1);
//  }
//
///* if the string path isn't long enough to have ".." in it then it doesn't do anything */
//  if( strlen(request->path.str) > 5){
///* incriments looking through the string looking for ".." */
//    for(ctr = 0; ctr < strlen(request->path.str) - 3; ctr++) {
//      if(request->path.str[ctr] == '/'&& request->path.str[ctr + 1] == '.'
//	&& request->path.str[ctr + 2] == '.'
//	&& request->path.str[ctr + 3] == '/'){
//
//        place_holder = strlen(request->path.str) - ctr - 3;
///* after finding ".." it over writes the ".." with the characters that procede the ".." */
//        for(j = 0; j < place_holder; j++){
//          request->path.str[ctr + 1 + j] = request->path.str[ctr + 4 + j];
//        }
//      }
//    }
//  }
//  return(0);
//}

/*************************************************************************
*
* Reply(FILE **file, int socket, int http_err_code)
*
* **file:if the file was opened this points to the file to be sent on socket
* socket: socket over which data needs to be sent
* http_err_code: used to determine which header to send and which error file
*	to open
*
* This function sends the appropriate header information and opens the correct
* error file to be sent back in the case of anything other than 200.
*
**************************************************************************/

/*************************************************************************
*
* DoubleGrowString(struct GrowString *s)
*
* s: pointer to struct GrowString
*
* Doubles the memory size of s, and sets the NUL termination character.
* 
*
**************************************************************************/
void
DoubleGrowString(struct GrowString *s){
  s->maxSize = s->maxSize * 2;
  s->str = realloc(s->str, s->maxSize * sizeof(s->str[0]));
  if (s->str == NULL){
    fprintf(stderr, "Out of memory!\n");
    exit(-1);
  }
  
  s->str[s->numUsed] = '\0';
}

/*************************************************************************
*
* GrowStringSet(struct GrowString *s, newsize)
*
* s: pointer to struct GrowString
*
* Sets the memory of s to newsize and locates the NUL terminator.
* 
*
**************************************************************************/
void 
GrowStringSet(struct GrowString *s, int newsize)
{
  s->maxSize = newsize;
  s->str = realloc(s->str, s->maxSize * sizeof(s->str[0]));
  if (s->str == NULL){
    fprintf(stderr, "Out of memory!\n");
    exit(-1);
  }

  s->str[s->numUsed] = '\0';
}
  
/*************************************************************************
*
* GrowStringInit(struct GrowString *s)
*
* s: pointer to struct GrowString
*
* Initializes the structure s to INITIAL_GROWSTRING bytes.
* 
*
**************************************************************************/
void
GrowStringInit(struct GrowString *s)
{
  s->maxSize = INITIAL_GROWSTRING;
  s->numUsed = 0;
  s->str = malloc(s->maxSize * sizeof(s->str[0]));  
  if (s->str == NULL) {
    fprintf(stderr, "Out of memory!\n");
    exit(-1);      
  }
  s->str[s->numUsed] = '\0';
}

/*************************************************************************
*
* GrowStringDestroy(struct GrowString *s)
*
* s: pointer to struct GrowString
*
* Frees all memory associated with s 
* 
*
**************************************************************************/
void
GrowStringDestroy(struct GrowString *s)
{
  free(s->str);
}

/*************************************************************************
*
* DestroyHTTP_Request(struct HTTP_Request *request)
*
* *request: pointer to struct HTTP_Request
* 
*
* Frees all memory associated with the structure *request
* 
*
**************************************************************************/
void
DestroyHTTP_Request(struct HTTP_Request *request)
{
  GrowStringDestroy(&(request->path));
  GrowStringDestroy(&(request->method));
  GrowStringDestroy(&(request->filename));
}

/*************************************************************************
*
* isvalidfilename(char ch)
*
* ch: character to be investigated
*
* Returns 1 if the character ch is a valid filename and 0 if it is not.
*
**************************************************************************/
int
isvalidfilename(char ch)
{
  if (ch > 32 && ch < 127) {
    return (1);
  }
  else {
    return (0);
  }
}
