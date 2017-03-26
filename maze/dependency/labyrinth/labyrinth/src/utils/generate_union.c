#include <stdlib.h>
#include <time.h>
#include "utils/generate_union.h"

union_find *create_union_find(int size)
{
    union_find *this = (union_find*)malloc(sizeof(union_find));
    this->pre = (int*)malloc(size * sizeof(union_find));
    int i = 0, *p = this->pre, *end = p + size;
    for (; p != end; ++i, ++p) {
        *p = i;
    }
    return this;
}

void destroy_union_find(union_find *this)
{
    free(this->pre); free(this);
}

int find_elem(union_find *this, int node)
{
    while (this->pre[node] != node) {
        node = this->pre[node];
    }
    return node;
}

int is_joint(union_find *this, int s, int t)
{
    return find_elem(this, s) == find_elem(this, t);
}

void join_elem(union_find *this, int a, int b)
{
    this->pre[find_elem(this, a)] = find_elem(this, b);
}

typedef struct {
    int u, v, w;
} edge;

static int n, m;
static int *lookup;
static int *pos_lookup;
static edge *edges;
static union_find *set;

# define EDGE_VERT (1)
# define EDGE_HORI (2)

static int cmp(const void* p, const void* q)
{
    return ((edge*)p)->w - ((edge*)q)->w;
}

static void init_labyrinth_walls(map *map, coordinate entrance_position, coordinate exit_position)
{
    const int width = map->width, height = map->height,
        lowx = !(min(entrance_position.x, exit_position.x) + 1 & 1),
        lowy = !(min(entrance_position.y, exit_position.y) + 1 & 1);

    lookup = (int*)malloc(width * height * sizeof(int));
    pos_lookup = (int*)malloc(width * height * sizeof(int));

    int i, j; n = m = 0;
    for (j = lowy; j < height; j += 2) {
        int begin = j * width, end = (j + 1) * width;
        for (i = !lowx + begin; i < end; i += 2) {      // breakable
            map->data[i].type = WALL;
            map->data[i].timestamp = EDGE_HORI; ++m;    // left-right
        }
        for (i = lowx + begin; i < end; i += 2) {       // road
            map->data[i].type = ROAD;
            lookup[i] = n; pos_lookup[n] = i; ++n;
        }
    }
    for (j = !lowy; j < height; j += 2) {
        int begin = j * width, end = (j + 1) * width;
        for (i = lowx + begin; i < end; i += 2) {       // breakable
            map->data[i].type = WALL;
            map->data[i].timestamp = EDGE_VERT; ++m;    // up-down
        }
        for (i = !lowx + begin; i < end; i += 2) {      // unbreakable
            map->data[i].type = WALL;
            map->data[i].timestamp = 0;
        }
    }

    const int rand_size = 0x3fffffff / n;
    edges = (edge*)malloc(m * sizeof(edge));
    int top = 0;

    for (i = 0; i != width * height; ++i) {
        if (map->data[i].type == WALL) {
            switch (map->data[i].timestamp) {
            case EDGE_HORI: {
                edges[top].u = lookup[i - 1];
                edges[top].v = lookup[i + 1];
                edges[top].w = rand() % rand_size;      // todo
            } ++top; break;
            case EDGE_VERT: {
                edges[top].u = lookup[i - width];
                edges[top].v = lookup[i + width];
                edges[top].w = rand() % rand_size;      // todo
            } ++top; break;
            }
        }
    }
    qsort(edges, m, sizeof(edge), cmp);
    set = create_union_find(n);
    set_entrance(map, entrance_position);
    set_exit(map, exit_position);
}

map *generate_union(int width, int height, coordinate entrance_position, coordinate exit_position)
{
    map *labyrinth = create_empty_map(width, height);
    init_labyrinth_walls(labyrinth, entrance_position, exit_position);

    #ifdef __DEBUG_LESS
    {
        clock_t seed = clock();
        seed = 187;
        printf("WATCH HERE: %ld!!!!!!!!!!!!!!!!!!!\n", seed);
        srand(seed);
    }
    #else
    srand(clock());
    #endif
    int i = 0, cnt = 1;
    for (; cnt != n; ++i, ++cnt) {
        while (is_joint(set, edges[i].u, edges[i].v)) {
            ++i;
        }
        join_elem(set, edges[i].u, edges[i].v);
        labyrinth->data[(pos_lookup[edges[i].u] + pos_lookup[edges[i].v]) / 2].type = ROAD;
    }
    destroy_union_find(set);
    free(edges);
    free(lookup);
    free(pos_lookup);

    return labyrinth;
}