#ifndef MAZE_CONTROL__HEADER_FILE_____
# define MAZE_CONTROL__HEADER_FILE_____
# include <components/ogg_components.h>
# include <components/ogg_grid_canvas.h>
# include <labyrinth.h>

# define green          ogg_make_color(6, 128, 67)
# define light_green    ogg_make_color(174, 221, 129)
# define screen_color   green

# define BEEN_ROAD          (127)
# define SOLUTION_ROAD      (126)

    def_startup_inh(maze_control, ogg_grid_canvas)(
        ogg_color src_color;
        ogg_color dest_color;
    );

    def_component_inh(maze_control, ogg_grid_canvas)(
        ogg_color src_color;
        ogg_color dest_color;
        ogg_coord cursor;
        coordinate src;
        coordinate dest;
        ogg_paint_unit temp_unit;
        ogg_bool set_src;
        map* labyrinth;
    );

    coordinate make_coordinate(int x, int y);

    void make_dir(ogg_com_ptr this);

    void generate_maze(ogg_com_ptr this, map* (*)(), int width, int height);

#endif //MAZE_CONTROL__HEADER_FILE_____