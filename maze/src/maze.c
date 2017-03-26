#include <ogg.h>
#include <labyrinth.h>
#include "maze_control.h"
#include <stdio.h>

static maze_control* main_maze;
static ogg_edit* edit_width,* edit_height;

static int width, height;

void go_button_click(ogg_button* button)
{
    make_dir(main_maze);
}

void get_maze_size()
{
    if (sscanf(ogg_edit_get_text(edit_width), "%d", &width) != 1 || width <= 0) {
        width = 151;
    }
    if (sscanf(ogg_edit_get_text(edit_height), "%d", &height) != 1 || height <= 0) {
        height = 101;
    }
}

void generate_button_click(ogg_button* button)
{
    get_maze_size();
    generate_maze(main_maze, generate, width, height);
}

void generate_union_button_click(ogg_button* button)
{
    get_maze_size();
    generate_maze(main_maze, generate_union, width, height);
}

void ogg_main(ogg_startup_info* st)
{
    st->window.title = "Maze";
    set_screen_color(screen_color);
    ogg_create(maze_control)(
        .ogg_grid_canvas = {
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
        },
        .src_color = OGG_RED,
        .dest_color = OGG_BLUE
    )(main_maze);
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

