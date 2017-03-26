#define OGG_EXIT_STATUS
#include <ogg.h>
#include <labyrinth.h>
#include <stdlib.h>
//#include <stdio.h>

extern void exit(int);

# define green          ogg_make_color(6, 128, 67)
# define light_green    ogg_make_color(174, 221, 129)
# define screen_color   green

static map* labyrinth;
static ogg_grid_canvas* main_canvas;
static ogg_edit* edit_width,* edit_height;

static int width, height;

coordinate make_coordinate(int x, int y)
{
    coordinate r = { x, y };
    return r;
}

# define BEEN_ROAD          (127)
# define SOLUTION_ROAD      (126)

const int dx[] = { 1, -1, 0, 0 }, dy[] = { 0, 0, 1, -1 },
        dsize = sizeof(dx) / sizeof(int);
ogg_bool dfs(coordinate pix)
{
    set_land_type(labyrinth, pix, BEEN_ROAD);
    int i = 0;
    for (; i != dsize; ++i) {
        coordinate pos = make_coordinate(pix.x + dx[i], pix.y + dy[i]);
        if (is_in_map(labyrinth, pos)) {
            switch (get_land_type(labyrinth, pos)) {
            case ROAD:
                if (!dfs(pos)) break;
            case EXIT:
                set_land_type(labyrinth, pos, SOLUTION_ROAD);
                return ogg_true;
            }
        }
    }
    return ogg_false;
}

void make_dir(coordinate src, coordinate dest)
{
    if (!labyrinth) return;
    int i = 0;
    for (; i != labyrinth->width * labyrinth->height; ++i) {
        switch (labyrinth->data[i].type) {
        case BEEN_ROAD: case SOLUTION_ROAD: 
        case ENTRANCE: case EXIT:
            labyrinth->data[i].type = ROAD;
        }
    }
    //set_land_type(labyrinth, src, ENTRANCE);
    set_land_type(labyrinth, dest, EXIT);
    dfs(src);
    set_land_type(labyrinth, src, ENTRANCE);
    set_land_type(labyrinth, dest, EXIT);
}

void paint_maze()
{
    coordinate pos;
    for (pos.y = 0; pos.y != labyrinth->height; ++pos.y) {
        for (pos.x = 0; pos.x != labyrinth->width; ++pos.x) {
            switch (get_land_type(labyrinth, pos)) {
            case ROAD: case BEEN_ROAD: 
                set_canvas_color(main_canvas, coord(pos.x, pos.y), light_green); break;
            case WALL: 
                set_canvas_color(main_canvas, coord(pos.x, pos.y), green); break;
            case LAND: 
                set_canvas_color(main_canvas, coord(pos.x, pos.y), green/*OGG_GREEN*/); break;
            case ENTRANCE: 
                set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_RED); break;
            case EXIT: 
                set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_BLUE/*OGG_YELLOW*/); break;
            case SOLUTION_ROAD:
                set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_YELLOW/*OGG_BLACK*//*ogg_make_color(227, 76, 5)*/); break;
            default: 
                set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_CYAN);
            }
        }
    }
    ogg_send_event(main_canvas, OGG_PAINT_EVENT);
    ogg_flush_screen();
}

void go_button_click(ogg_button* button)
{
    make_dir(make_coordinate(0, 0), make_coordinate(width - 1, height - 1));
    paint_maze();
}

void get_maze_size()
{
    sscanf(ogg_edit_get_text(edit_width), "%d", &width);
    sscanf(ogg_edit_get_text(edit_height), "%d", &height);
    if (width <= 0) {
        width = 151;
    }
    if (height <= 0) {
        height = 101;
    }
}

void generate_button_click(ogg_button* button)
{
    if (labyrinth) {
        free(labyrinth->data);
        free(labyrinth);
    }
    get_maze_size();
    coordinate entrance_position = { 0, 0 }, exit_position = { width - 1, height - 1 };
    labyrinth = generate(width, height, entrance_position, exit_position);
    resize_canvas(main_canvas, coord(width, height));
    paint_maze();
}

void generate_union_button_click(ogg_button* button)
{
    if (labyrinth) {
        free(labyrinth->data);
        free(labyrinth);
    }
    get_maze_size();
    coordinate entrance_position = { 0, 0 }, exit_position = { width - 1, height - 1 };
    labyrinth = generate_union(width, height, entrance_position, exit_position);
    resize_canvas(main_canvas, coord(width, height));
    paint_maze();
}

void ogg_main(ogg_startup_info* st)
{
    st->window.title = "Maze";
    set_screen_color(screen_color);
    ogg_create(ogg_grid_canvas)(
        .ogg_component = {
            .parent = ogg_main_window(),
            .anchor = {
                .type = ogg_anchor_pec,
                .pec = {
                    .top_left = { 3, 3 },
                    .bottom_right = { 97, 85 }
                }
            }
        },
        .size = { 0, 0 }
    )(main_canvas);
    ogg_create_def(ogg_button)(
        .ogg_rect = {
            .ogg_shape = {
                .ogg_component = {
                    .parent = ogg_main_window(),
                    .anchor = {
                        .type = ogg_anchor_pec,
                        .pec = {
                            .top_left = { 80, 90 },
                            .bottom_right = { 95, 95 }
                        }
                    }
                },
                //get_grid_coord(maze_grid, coord(2, 0))),
                .color = ogg_make_color(207, 107, 73)
            }
        },
        .focused_color = ogg_make_color(148, 113, 102),
        .down_color = ogg_make_color(159, 127, 117),
        .text = {
            .content = "Go",
            .color = ogg_make_color(255, 255, 255)
        },
        .callback = go_button_click
    );

    ogg_create_def(ogg_button)(
        .ogg_rect = {
            .ogg_shape = {
                .ogg_component = {
                    .parent = ogg_main_window(),
                    .anchor = {
                        .type = ogg_anchor_pec,
                        .pec = {
                            .top_left = { 40, 90 },
                            .bottom_right = { 55, 95 }
                        }
                    }
                },
                //get_grid_coord(maze_grid, coord(2, 0))),
                .color = ogg_make_color(207, 107, 73)
            }
        },
        .focused_color = ogg_make_color(148, 113, 102),
        .down_color = ogg_make_color(159, 127, 117),
        .text = {
            .content = "Generate1",
            .color = ogg_make_color(255, 255, 255)
        },
        .callback = generate_button_click
    );

    ogg_create_def(ogg_button)(
        .ogg_rect = {
            .ogg_shape = {
                .ogg_component = {
                    .parent = ogg_main_window(),
                    .anchor = {
                        .type = ogg_anchor_pec,
                        .pec = {
                            .top_left = { 60, 90 },
                            .bottom_right = { 75, 95 }
                        }
                    }
                },
                //get_grid_coord(maze_grid, coord(2, 0))),
                .color = ogg_make_color(207, 107, 73)
            }
        },
        .focused_color = ogg_make_color(148, 113, 102),
        .down_color = ogg_make_color(159, 127, 117),
        .text = {
            .content = "Generate2",
            .color = ogg_make_color(255, 255, 255)
        },
        .callback = generate_union_button_click
    );

    ogg_create(ogg_edit)(
        .ogg_rect = {
            .ogg_shape = {
                .ogg_component = {
                    .parent = ogg_main_window(),
                    .anchor = {
                        .type = ogg_anchor_pec,
                        .pec = {
                            .top_left = { 5, 90 },
                            .bottom_right = { 17.5, 95 }
                        }
                    }
                }
            }
        }
    )(edit_width);

    ogg_create(ogg_edit)(
        .ogg_rect = {
            .ogg_shape = {
                .ogg_component = {
                    .parent = ogg_main_window(),
                    .anchor = {
                        .type = ogg_anchor_pec,
                        .pec = {
                            .top_left = { 22.5, 90 },
                            .bottom_right = { 35, 95 }
                        }
                    }
                }
            }
        }
    )(edit_height);
}

int ogg_exit_status()
{
    if (labyrinth) {
        free(labyrinth->data);
        free(labyrinth);
    }
    return 0;
}