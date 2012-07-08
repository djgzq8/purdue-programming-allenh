#include <stdio.h>
#include <stdlib.h>
#include "hproj3.h"

int DotDotRemover(struct HTTP_Request *request)
{
  int j;
  int ctr;
  int place_holder = 0;
  int filename_place = 0;
  int lcv;

  if (!(strcmp(request->filename.str, ".."))){
    for (lcv = strlen(request->path.str) - 2; lcv >= 0; lcv--){
      if (request->path.str[lcv] == '/'){
        filename_place = lcv;
        break;
      }
    }
    request->filename.numUsed = strlen(request->path.str) - filename_place - 2;
    GrowStringSet(&(request->filename), request->filename.numUsed + 1);

    for (lcv = filename_place + 1; lcv < strlen(request->path.str) - 1; lcv++){
      request->filename.str[lcv - 1 - filename_place] = request->path.str[lcv];
    }

    request->path.numUsed = filename_place + 1;
    GrowStringSet(&(request->path), request->path.numUsed + 1);
  }
  if( strlen(request->path.str) > 5){
    for(ctr = 0; ctr < strlen(request->path.str) - 3; ctr++) {
      if(request->path.str[ctr] == '/'&& request->path.str[ctr + 1] == '.' && request->path.str[ctr + 2] == '.'&& request->path.str[ctr + 3] == '/'){
        place_holder = strlen(request->path.str) - ctr - 3;
        for(j = 0; j < place_holder; j++){
          request->path.str[ctr + 1 + j] = request->path.str[ctr + 4 + j];
        }  
      }
    }
  }
  return(0);
}
