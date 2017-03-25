#include <ogg.h>
#include <labyrinth.h>
#include <stdlib.h>
//#include <stdio.h>

extern void exit(int);

static ogg_grid_canvas* main_canvas;
static ogg_edit* edit_width,* edit_height;

static int width, height;

void exit_button_click(ogg_button* button)
{
    exit(0);
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
    map* labyrinth;
    get_maze_size();
    coordinate entrance_position = { 0, 0 }, exit_position = { width - 1, height - 1 };
    labyrinth = generate(width, height, entrance_position, exit_position);
    resize_canvas(main_canvas, coord(width, height));
    coordinate pos;
    for (pos.y = 0; pos.y != labyrinth->height; ++pos.y) {
        for (pos.x = 0; pos.x != labyrinth->width; ++pos.x) {
            switch (get_land_type(labyrinth, pos)) {
            case ROAD: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_WHITE); break;
            case WALL: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_BLUE); break;
            case LAND: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_GREEN); break;
            case ENTRANCE: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_RED); break;
            case EXIT: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_YELLOW); break;
            default: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_CYAN);
            }
        }
    }
    ogg_send_event(main_canvas, OGG_PAINT_EVENT);
    ogg_flush_screen();
    free(labyrinth->data);
    free(labyrinth);
}

void generate_union_button_click(ogg_button* button)
{
    map* labyrinth;
    get_maze_size();
    coordinate entrance_position = { 0, 0 }, exit_position = { width - 1, height - 1 };
    labyrinth = generate_union(width, height, entrance_position, exit_position);
    resize_canvas(main_canvas, coord(width, height));
    coordinate pos;
    for (pos.y = 0; pos.y != labyrinth->height; ++pos.y) {
        for (pos.x = 0; pos.x != labyrinth->width; ++pos.x) {
            switch (get_land_type(labyrinth, pos)) {
            case ROAD: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_WHITE); break;
            case WALL: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_BLUE); break;
            case LAND: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_GREEN); break;
            case ENTRANCE: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_RED); break;
            case EXIT: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_YELLOW); break;
            default: set_canvas_color(main_canvas, coord(pos.x, pos.y), OGG_CYAN);
            }
        }
    }
    ogg_send_event(main_canvas, OGG_PAINT_EVENT);
    ogg_flush_screen();
    free(labyrinth->data);
    free(labyrinth);
}

void ogg_main(ogg_startup_info* st)
{
    st->window.title = "Maze";
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
        .size = { 1, 1 }
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
            .content = "Exit",
            .color = ogg_make_color(255, 255, 255)
        },
        .callback = exit_button_click
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