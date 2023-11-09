#ifndef img_search_H__
#define img_search_H__

#include <stdint.h>
#include <stdbool.h>
#include "img-search.c"

int appel(int argc, char* argv[]);

void *memoire_partage(size_t size);

int img_dist(char *image1, char *image2);

char *read_Pipe(int pipe, char *buffer, size_t sizePath);

void write_pipe(int pipe, const void *path, size_t sizePath);

#endif 



