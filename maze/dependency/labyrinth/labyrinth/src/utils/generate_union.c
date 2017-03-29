#include <stdlib.h>
#include <time.h>
#include "utils/generate_union.h"

union_find create_union_find(int size)
{
    union_find this;
    this = (union_find)malloc(size * sizeof(int));
    int i = 0, *p = this, *end = p + size;
    for (; p != end; ++i, ++p) {
        *p = i;
    }
    return this;
}

void destroy_union_find(union_find this)
{
    free(this);
}

static int find_elem(int *pre, int node)
{
    register int branch = node;
    while (pre[node] != node) {
        node = pre[node];
    }
    register int temp = branch;
    while ((branch = pre[branch]) != node) {
        pre[temp] = node; temp = branch;
    }
    return node;
}

# define is_joint(this, s, t)                                   \
    (!(find_elem((this), (s)) - find_elem((this), (t))))

# define join_elem(this, a, b)                                  \
    ((this)[find_elem((this), (a))] = (this)[find_elem((this), (b))])

typedef struct {
    int u, v, w;
} edge;

static int n, m;
static int *lookup;
static int *pos_lookup;
static edge *edges;
static union_find set;

# define EDGE_VERT (1)
# define EDGE_HORI (2)

static int cmp(const void* p, const void* q)
{
    return ((edge*)p)->w - ((edge*)q)->w;
}

static void init_labyrinth_walls(map *map)
{
    const int width = map->width, height = map->height,
        calc_width = (map->width + 1 & -2) - 1, calc_height = (map->height + 1 & -2) - 1;

    lookup = (int*)malloc(width * height * sizeof(int));
    pos_lookup = (int*)malloc(width * height * sizeof(int));

    int i, j; n = m = 0;
    for (j = 0; j < calc_height; j += 2) {
        int begin = j * width, end = begin + calc_width;
        for (i = begin + 1; i < end; i += 2) {      // breakable
            map->data[i].type = WALL;
            map->data[i].timestamp = EDGE_HORI; ++m;    // left-right
        }
        for (i = begin; i < end; i += 2) {       // road
            map->data[i].type = ROAD;
            lookup[i] = n; pos_lookup[n] = i; ++n;
        }
    }
    for (j = 1; j < calc_height; j += 2) {
        int begin = j * width, end = begin + calc_width;
        for (i = begin; i < end; i += 2) {       // breakable
            map->data[i].type = WALL;
            map->data[i].timestamp = EDGE_VERT; ++m;    // up-down
        }
        for (i = begin + 1; i < end; i += 2) {      // unbreakable
            map->data[i].type = WALL;
            map->data[i].timestamp = 0;
        }
    }
    if (calc_width != width) {
        int begin = calc_width, end = begin + calc_height * width;
        for (i = begin; i < end; i += width * 2) {
            map->data[i].type = ROAD;
        }
        for (i = begin + width; i < end; i += width * 2) {
            map->data[i].type = WALL;
        }
    }
    if (calc_height != height) {
        int begin = calc_height * width, end = begin + calc_width;
        for (i = begin; i < end; i += 2) {
            map->data[i].type = ROAD;
        }
        for (i = begin + 1; i < end; i += 2) {
            map->data[i].type = WALL;
        }
    }
    if (calc_width != width && calc_height != height) {
        map->data[width * height - 1].type = ROAD;
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
}

map *generate_union(int width, int height, coordinate entrance_position, coordinate exit_position)
{
    map *labyrinth = create_empty_map(width, height);
    init_labyrinth_walls(labyrinth);
    set_entrance(labyrinth, entrance_position);
    set_exit(labyrinth, exit_position);
    srand(clock());
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