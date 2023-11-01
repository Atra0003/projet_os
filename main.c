#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

#include "image-search.h"



int main(int argc, char* argv[]){
   image_search();
   return 0;
}
