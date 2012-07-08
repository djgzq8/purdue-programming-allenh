/*************************************************************************
*
* FileOpen(FILE **file, struct HTTP_Request *request)
*
* **file:pointer to a file to be sent to the client
* *request: pointer to the structure containing the filename, and path
*
* This function checks whether access is to be allowed to the file, if the
* file exists and if it contains any data. If everything is ok the file
* is opened and pointed to by the pointer.
*
**************************************************************************/
#include "hproj3.h"

int FileOpen(FILE **file, struct HTTP_Request *request)
{
  FILE *accessfile;/*file pointer to analyze the access file*/
  char access[] = "access";/*for opening the access file*/
  char rootdir[] = "./";/*for seeing if the current dir is the root dir*/
  char *accesspath;/*the concanentation of the path and access*/
  char *filepath;/*the concantenation of the path and filename*/
  char deny[5];/*for analyzing the first 4 characters of the access file*/
  int ctr;/*generic counter*/

/*check to see if the file being requested is in the current directory
* if it is the access file can be opened directly, if it is not then the 
* access has to be appened to the path before it can be opened*/
  if (strcmp(request->path.str, rootdir)){
    accesspath = malloc(sizeof(char) * (strlen(request->path.str) + 7));
    if (accesspath == NULL){
      fprintf(stderr, "Out of memory!\n");
      exit(-1);
    }
    strcpy(accesspath, request->path.str);
    for (ctr = 0; ctr < 6; ctr++){
      accesspath[strlen(request->path.str) + ctr] = access[ctr];
    }
    accesspath[strlen(request->path.str) + ctr] = '\0';
    accessfile = fopen(accesspath, "r");
    free(accesspath);
  }
  else {
    accessfile = fopen(access, "r");
  }

/*if the accessfile is successfully opened, look for the word deny.
* if deny is not found or the file is not present the code continues
* if its not then 403 is denied to indicate forbidden.*/
  if (accessfile != NULL){
    for (ctr = 0; ctr < 4; ctr++) {
      fscanf(accessfile, "%c", &deny[ctr]);
      deny[ctr] = toupper(deny[ctr]);
    }
    deny[4] = '\0';

    fclose(accessfile);

    if (strcmp("DENY", deny) == 0){
      return (403);
    }
  }

/*same as before, if the file being requested is in the current directory
* then the file can be opened straight away. if its in a subdir then the 
* filename has to be put back onto the path*/
  if (strcmp(request->path.str, rootdir)) {
    filepath = malloc(sizeof(char) * (strlen(request->path.str) + 
	strlen(request->filename.str) + 1));
    strcpy(filepath, request->path.str);
    for (ctr = 0; ctr < strlen(request->filename.str); ctr++){
      filepath[strlen(request->path.str) + ctr] = request->filename.str[ctr];
    }
    filepath[strlen(request->path.str) + ctr] = '\0';
    *file = fopen(filepath, "r");
    free(filepath);
  }
  else {
    *file = fopen(request->filename.str, "r");
  }

/*if the file is not opened then return 404 for not found,
* seek to the end and if the end is the beginning then return 204
* no content. If all else is well then return 200 ok*/
  if (*file == NULL){
    return(404);
  }
  else {
    fseek(*file, 0, SEEK_END);
    if (ftell(*file) == 0){
      fclose(*file);
      return (204);
    }
    else{
      fseek(*file, 0, SEEK_SET);
    }
  }

  return(200);
}
