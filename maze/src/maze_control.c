#include "maze_control.h"

extern handler(ogg_grid_canvas, OGG_PAINT_EVENT);

handler(maze_control, OGG_MOUSE_MOVE_EVENT);

handler(maze_control, OGG_MOUSE_EVENT);

def_vtable(maze_control)(
    [OGG_PAINT_EVENT] = ogg_handler(ogg_grid_canvas, OGG_PAINT_EVENT),
    [OGG_MOUSE_MOVE_EVENT] = ogg_handler(maze_control, OGG_MOUSE_MOVE_EVENT),
    [OGG_MOUSE_EVENT] = ogg_handler(maze_control, OGG_MOUSE_EVENT),
    [OGG_MOUSE_DRAG_EVENT] = ogg_handler(maze_control, OGG_MOUSE_MOVE_EVENT),
);

default_startup_inh(maze_control, ogg_grid_canvas)(
    .src_color = OGG_RED,
    .dest_color = OGG_BLUE,
)

def_constructor(maze_control, args)
{
    memcpy(&this->src_color, &args->src_color, sizeof(ogg_color));
    memcpy(&this->dest_color, &args->dest_color, sizeof(ogg_color));
    this->cursor.x = this->cursor.y = -1;
    this->labyrinth = 0;
    this->set_src = ogg_true;
}

def_destructor(maze_control)
{
    if (this->labyrinth) {
        free(this->labyrinth->data);
        free(this->labyrinth);
    }
}

static void update_maze(maze_control* this)
{
    coordinate pos;
    map* maze = this->labyrinth;
    if (maze) {
        for (pos.y = 0; pos.y != maze->height; ++pos.y) {
            for (pos.x = 0; pos.x != maze->width; ++pos.x) {
                switch (get_land_type(maze, pos)) {
                case ROAD: case BEEN_ROAD: 
                    set_canvas_color(this, coord(pos.x, pos.y), light_green); break;
                case WALL: 
                    set_canvas_color(this, coord(pos.x, pos.y), green); break;
                case LAND: 
                    set_canvas_color(this, coord(pos.x, pos.y), green/*OGG_GREEN*/); break;
                case ENTRANCE: 
                    set_canvas_color(this, coord(pos.x, pos.y), OGG_RED); break;
                case EXIT: 
                    set_canvas_color(this, coord(pos.x, pos.y), OGG_BLUE/*OGG_YELLOW*/); break;
                case SOLUTION_ROAD:
                    set_canvas_color(this, coord(pos.x, pos.y), OGG_YELLOW/*OGG_BLACK*//*ogg_make_color(227, 76, 5)*/); break;
                default: 
                    set_canvas_color(this, coord(pos.x, pos.y), OGG_CYAN);
                }
            }
        }
    }
    ogg_send_event(this, OGG_PAINT_EVENT);
    ogg_flush_screen();
}

def_handler(maze_control, OGG_MOUSE_MOVE_EVENT)
{
    if (this->labyrinth) {
        ogg_anchor anchor;
        get_component_real_coord_anchor(this, &anchor);
        const int width = anchor.coord.right - anchor.coord.left,
                height = anchor.coord.bottom - anchor.coord.top;
        const int xpos = x - anchor.coord.left,
                ypos = y - anchor.coord.top;
        coordinate pix = make_coordinate(
                (int)(xpos / ((float)width / this->super.size.x)),
                (int)(ypos / ((float)height / this->super.size.y))
            );
        if (is_in_map(this->labyrinth, pix)) {
            int land_type = get_land_type(this->labyrinth, pix);
            if (land_type == ROAD || land_type > 100) {
                if (this->cursor.x != pix.x || this->cursor.y != pix.y) {
                    if (~this->cursor.x && ~this->cursor.y) {
                        memcpy(&this->super.sub[this->super.size.x * this->cursor.y + this->cursor.x],
                            &this->temp_unit, 
                            sizeof(this->temp_unit));
                        paint_grid_canvas_elem(this, this->cursor);
                    }
                    memcpy(&this->temp_unit,
                        &this->super.sub[this->super.size.x * pix.y + pix.x],
                        sizeof(this->temp_unit));
                    this->cursor.x = pix.x; this->cursor.y = pix.y;
                    set_canvas_color(this, this->cursor, this->set_src ? 
                        this->src_color : this->dest_color);
                    paint_grid_canvas_elem(this, this->cursor);
                }
            }
        }
    }
}

def_handler(maze_control, OGG_MOUSE_EVENT)
{
    if (this->labyrinth) {
        if (this->set_src = !this->set_src) {
            set_land_type(this->labyrinth, make_coordinate(
                    this->dest.x, this->dest.y), ROAD);
            set_land_type(this->labyrinth, make_coordinate(
                    this->cursor.x, this->cursor.y), EXIT);
            set_exit(this->labyrinth, make_coordinate(
                    this->cursor.x, this->cursor.y));
            this->dest.x = this->cursor.x; this->dest.y = this->cursor.y;
        } else {
            set_land_type(this->labyrinth, make_coordinate(
                    this->src.x, this->src.y), ROAD);
            set_land_type(this->labyrinth, make_coordinate(
                    this->cursor.x, this->cursor.y), ENTRANCE);
            set_entrance(this->labyrinth, make_coordinate(
                    this->cursor.x, this->cursor.y));
            this->src.x = this->cursor.x; this->src.y = this->cursor.y;
        }
        this->cursor.x = this->cursor.y = -1;
        update_maze(this);
        ogg_send_event(this, OGG_PAINT_EVENT);
        ogg_flush_screen();
    }
}

static map* dfs_maze;

const int dx[] = { 1, -1, 0, 0 }, dy[] = { 0, 0, 1, -1 },
        dsize = sizeof(dx) / sizeof(int);

static ogg_bool dfs(coordinate pix)
{
    set_land_type(dfs_maze, pix, BEEN_ROAD);
    int i = 0;
    for (; i != dsize; ++i) {
        coordinate pos = make_coordinate(pix.x + dx[i], pix.y + dy[i]);
        if (is_in_map(dfs_maze, pos)) {
            switch (get_land_type(dfs_maze, pos)) {
            case ROAD:
                if (!dfs(pos)) break;
            case EXIT:
                set_land_type(dfs_maze, pos, SOLUTION_ROAD);
                return ogg_true;
            }
        }
    }
    return ogg_false;
}

void make_dir(ogg_com_ptr this)
{
    map* maze = dfs_maze = ((maze_control*)this)->labyrinth;
    if (!maze) return;
    int i = 0;
    for (; i != maze->width * maze->height; ++i) {
        switch (maze->data[i].type) {
        case BEEN_ROAD: case SOLUTION_ROAD: 
        case ENTRANCE: case EXIT:
            maze->data[i].type = ROAD;
        }
    }
    //set_land_type(labyrinth, src, ENTRANCE);
    set_land_type(maze, maze->exit_position, EXIT);
    dfs(maze->entrance_position);
    set_land_type(maze, maze->entrance_position, ENTRANCE);
    set_land_type(maze, maze->exit_position, EXIT);
    maze_control* p = ((maze_control*)this);
    p->cursor.x = p->cursor.y = -1;
    p->set_src = ogg_true;
    update_maze(p);
}

coordinate make_coordinate(int x, int y)
{
    coordinate r = { x, y };
    return r;
}

void generate_maze(ogg_com_ptr this, map* (*gen)(), int width, int height)
{
    maze_control* maze = (maze_control*)this;
    if (maze->labyrinth) {
        free(maze->labyrinth->data);
        free(maze->labyrinth);
    }
    maze->src.x = maze->src.y = 0;
    maze->dest.x = width - 1; maze->dest.y = height - 1;
    maze->labyrinth = 
        gen(width, height, maze->src, maze->dest);
    resize_canvas(this, coord(width, height));
    maze->cursor.x = maze->cursor.y = -1;
    maze->set_src = ogg_true;
    update_maze(this);
}
