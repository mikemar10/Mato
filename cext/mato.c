#ifdef NOT_DRAGONRUBY
#define DRB_FFI //This will be accessible from DRGTK
#define DRB_FFI_NAME(name) //This will be accessible from DRGTK as `name`
#else
#include <dragonruby.h>
#endif

#define FNL_IMPL

#include "lib/FastNoiseLite.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "src/drb_symbols.h"
#include "src/globals.h"
#include "src/render.h"
#include "src/terrain.h"
#include "src/player.h"
#include "src/utils.h"

DRB_FFI_NAME(blank_screen)
void matocore_blank_screen(void) {
    memset(SCREEN, 0, SCREEN_PIXELS * sizeof(Color));
}

DRB_FFI_NAME(draw_terrain)
void matocore_draw_terrain(int x_0, int y_0) {
    render_terrain(x_0 / ZOOM, TERRAIN_SIZE - (y_0 / ZOOM) - SCREEN_HEIGHT);
}

DRB_FFI_NAME(draw_player)
void matocore_draw_player(void *player, int x_0, int y_0) {
    render_player((Player *) player, x_0 / ZOOM, TERRAIN_SIZE - (y_0 / ZOOM) - SCREEN_HEIGHT);
}

DRB_FFI_NAME(generate_terrain)
void matocore_generate_terrain(int base_seed) {
    terrain_generate(base_seed);
}

DRB_FFI_NAME(update_terrain)
void matocore_update_terrain(void) {
    terrain_update();
}

DRB_FFI_NAME(destroy_terrain)
void matocore_destroy_terrain(int screen_x, int screen_y, int radius) {
    int      mx = screen_x / ZOOM;
    int      my = TERRAIN_SIZE - (screen_y / ZOOM);
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (sqrt(dx * dx + dy * dy) <= radius) {
                int x = mx + dx;
                int y = my + dy;
                terrain_set_pixel(x, y, TERRAIN_NONE, true);
            }
        }
    }
}

DRB_FFI_NAME(create_terrain)
void matocore_create_terrain(int screen_x, int screen_y, int radius, char *material) {
    TerrainPixel tp = TERRAIN_NONE;
    if (strcmp(material, "DIRT") == 0) {
        tp = TERRAIN_DIRT;
    } else if (strcmp(material, "SAND") == 0) {
        tp = TERRAIN_SAND;
    } else if (strcmp(material, "XHST") == 0) {
        tp = TERRAIN_XHST;
    } else if (strcmp(material, "SMKE") == 0) {
        tp = TERRAIN_SMKE;
    }
    int      mx = screen_x / ZOOM;
    int      my = TERRAIN_SIZE - (screen_y / ZOOM);
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (terrain_get_pixel(mx + dx, my + dy, TERRAIN_DIRT).type == TERRAIN_NONE_TYPE && sqrt(dx * dx + dy * dy) <= radius) {
                int x = mx + dx;
                int y = my + dy;
                terrain_set_pixel(x, y, tp, true);
            }
        }
    }
}

DRB_FFI_NAME(spawn_player)
void *matocore_spawn_player(double screen_x, double screen_y, double r, double g, double b) {
    double px      = screen_x / ZOOM;
    double py      = TERRAIN_SIZE - (screen_y / ZOOM);
    Player *player = player_initialize_player(px, py, fclamp(r, 0, 1), fclamp(g, 0, 1), fclamp(b, 0, 1));
    player->sprite         = load_image("/sprites/player.png", &player->w, &player->h);
    player->collision_mask = load_image("/sprites/player_collision_layers.png", &player->w, &player->h);
    player_do_surface_warp(player);
    return player;
}

DRB_FFI_NAME(despawn_player)
void matocore_despawn_player(void *player) {
    Player *cast_player = player;
    player_destroy_player(&cast_player);
}

DRB_FFI_NAME(player_input)
void matocore_player_input(void *player, double up_down, double left_right) {
    player_calc_input((Player *) player, up_down, left_right);
}

DRB_FFI_NAME(player_tick)
void matocore_player_tick(void *player) {
    player_do_input((Player *) player);
    player_do_movement((Player *) player);
    player_do_terrain_edit((Player *) player);
}

DRB_FFI_NAME(player_surface_warp)
void matocore_player_surface_warp(void *player) {
    player_do_surface_warp((Player *) player);
}

DRB_FFI_NAME(player_x_pos)
double matocore_player_x_pos(void *player) {
    return ((Player *) player)->px * ZOOM;
}

DRB_FFI_NAME(player_y_pos)
double matocore_player_y_pos(void *player) {
    return (TERRAIN_SIZE - ((Player *) player)->py) * ZOOM;
}

DRB_FFI_NAME(player_x_pos_anchored)
double matocore_player_x_pos_anchored(void *player, double x_0, double y_0) {
    return (((Player *) player)->px  - (x_0 / (double) ZOOM))* ZOOM;
}

DRB_FFI_NAME(player_y_pos_anchored)
double matocore_player_y_pos_anchored(void *player, double x_0, double y_0) {
    double out = TERRAIN_SIZE - (y_0 / ZOOM) - SCREEN_HEIGHT;
    out = ((Player *) player)->py - out;
    return (SCREEN_HEIGHT - out) * ZOOM;
}

DRB_FFI_NAME(terrain_size)
int matocore_level_size() {
    return TERRAIN_SIZE;
}

DRB_FFI_NAME(get_zoom)
int matocore_get_zoom() {
    return ZOOM;
}

DRB_FFI_NAME(debug_hook)
void matocore_debug_hook() {
    printf("Pixel count: %d\n", TERRAIN_PIXELS);
    printf("TerrainPixel size: %llu\n", sizeof(TerrainPixel));
    printf("Color size: %llu\n", sizeof(Color));
    printf("TERRAIN size: %llu\n", sizeof(TERRAIN));
    printf("SCREEN size: %llu\n", sizeof(SCREEN));
    fflush(stdout);
}
