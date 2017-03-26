#include <ogg.h>
#include <labyrinth.h>
#include <stdlib.h>

extern void exit(int);

ogg_grid* main_grid;

void exit_button_click(ogg_button* button)
{
    exit(0);
}

void set_maze_grid(coordinate pos, ogg_color color)
{
    ogg_create_def(ogg_rect)(
        .ogg_shape = {
            .ogg_component = ogg_default(ogg_component)(
                get_grid_coord(main_grid, coord(pos.x, pos.y))),
            .color = color
        }
    );
}

void generate_button_click(ogg_button* button)
{
    map* labyrinth;
    coordinate entrance_position = { 0, 0 }, exit_position = { 5, 6 };
    labyrinth = generate(6, 7, entrance_position, exit_position);
    set_grid_size(main_grid, coord(6, 7));
    coordinate pos;
    for (pos.y = 0; pos.y != labyrinth->height; ++pos.y) {
        for (pos.x = 0; pos.x != labyrinth->width; ++pos.x) {
            switch (get_land_type(labyrinth, pos)) {
            case ROAD: {
                set_maze_grid(pos, OGG_WHITE);
            } break;
            case WALL: {
                set_maze_grid(pos, OGG_BLUE);
            } break;
            case LAND: {
                set_maze_grid(pos, OGG_GREEN);
            } break;
            case ENTRANCE: {
                set_maze_grid(pos, OGG_RED);
            } break;
            case EXIT: {
                set_maze_grid(pos, OGG_YELLOW);
            } break;
            default: {
                set_maze_grid(pos, OGG_CYAN);
            }
            }
        }
    }
    ogg_send_event(ogg_main_window(), OGG_PAINT_EVENT);
    ogg_flush_screen();
    free(labyrinth->data);
    free(labyrinth);
}

void ogg_main(ogg_startup_info* st)
{
    st->window.title = "Maze";
    ogg_create(ogg_grid)(
        .ogg_component = {
            .parent = ogg_main_window(),
            .anchor = {
                .type = ogg_anchor_pec,
                .pec = {
                    .top_left = { 5, 5 },
                    .bottom_right = { 95, 80 }
                }
            }
        },
        .size = { 0, 0 }
    )(main_grid);
    ogg_create_def(ogg_button)(
        .ogg_rect = {
            .ogg_shape = {
                .ogg_component = {
                    .parent = ogg_main_window(),
                    .anchor = {
                        .type = ogg_anchor_pec,
                        .pec = {
                            .top_left = { 75, 85 },
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
                            .top_left = { 45, 85 },
                            .bottom_right = { 65, 95 }
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
            .content = "Generate",
            .color = ogg_make_color(255, 255, 255)
        },
        .callback = generate_button_click
    );
}