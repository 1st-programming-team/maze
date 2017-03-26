#ifndef __GENERATE_UNION_H
#define __GENERATE_UNION_H
#include "generate.h"

typedef struct {
    int *pre;
} union_find;

union_find *create_union_find(int size);

void destroy_union_find(union_find *this);

int find_elem(union_find *this, int node);

void join_elem(union_find *this, int a, int b);

int is_joint(union_find *this, int s, int t);

map *generate_union(int width, int height, coordinate entrance_position, coordinate exit_position);

#endif