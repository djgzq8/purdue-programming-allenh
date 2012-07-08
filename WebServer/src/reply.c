#include "hproj3.h"

int Reply(FILE **file, int socket, int http_err_code)
{
  char sendbuffer[SENDBUFF_SIZE];
  int bytesread;/*for knowing how many bytes were read from fread*/
  int fileend;/*the end of the file*/
  int head_size = 0;/*bytes printed to the sendbuffer*/

/*Prints the correct first header line based on http_err_code to the
* sendbuffer*/
  if (http_err_code == 200){
    head_size = sprintf(sendbuffer, "HTTP/1.0 200 OK\r\n");
  }
  else if (http_err_code == 404){
    head_size = sprintf(sendbuffer, "HTTP/1.0 404 Not Found\r\n");
    *file = fopen("404.html", "r");
  }
  else if (http_err_code == 204){
    head_size = sprintf(sendbuffer, "HTTP/1.0 204 No Content\r\n");
    *file = fopen("204.html", "r");
  }
  else if (http_err_code == 400){
    head_size = sprintf(sendbuffer, "HTTP/1.0 400 Bad Request\r\n");
    *file = fopen("400.html", "r");
  }
  else if (http_err_code == 403){
    head_size = sprintf(sendbuffer, "HTTP/1.0 403 Forbidden\r\n");
    *file = fopen("403.html", "r");
  }
/*send the first header line*/
  bytesread = send(socket, sendbuffer, head_size, 0); 

/*    head_size = sprintf(sendbuffer, "Content-Type: application/octet-stream\r\n");*/
/*  bytesread = send(socket, sendbuffer, head_size, 0); */

/*send the second header line*/
  head_size = sprintf(sendbuffer, "Connection: close\r\n");
  send(socket, sendbuffer, head_size, 0); 
  
/*if the file was successfully opened, whether it is the file requested
* or the error file, the file contents are sent on the socket*/
  if (*file != NULL){
    fseek(*file, 0, SEEK_END);
    fileend = ftell(*file);
    fseek(*file, 0, SEEK_SET);

    head_size = sprintf(sendbuffer, "Content-Length: %d\r\n\r\n", fileend);
    send(socket, sendbuffer, head_size, 0); 

    while (ftell(*file) < fileend){
      bytesread = fread(sendbuffer, sizeof(char), SENDBUFF_SIZE, *file);
      send(socket, sendbuffer, bytesread, 0);
    } 
    fclose(*file);
  }
  else {
    head_size = sprintf(sendbuffer, "Content-Length: 0\r\n\r\n");
    send(socket, sendbuffer, head_size, 0); 
    return (-1);
  }

  return(0);
}
