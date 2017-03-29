#ifndef __GENERATE_UNION_H
#define __GENERATE_UNION_H
#include "generate.h"

typedef int* union_find;

union_find create_union_find(int size);

void destroy_union_find(union_find this);

map *generate_union(int width, int height, coordinate entrance_position, coordinate exit_position);

#endif