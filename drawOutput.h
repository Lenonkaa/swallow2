#pragma once

#include "dataTypes.h"
#include "IO.h"


#define LOW_LIFE 25
#define MEDIUM_LIFE 50


#define COLOR_ORANGE 208


#define STATUS_HEIGHT 3

/*
0	COLOR_BLACK
1	COLOR_RED
2	COLOR_GREEN
3	COLOR_YELLOW
4	COLOR_BLUE
5	COLOR_MAGENTA
6	COLOR_CYAN
7	COLOR_WHITE
*/

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


void set_colors();

void draw_controls_guide(gameContext* context);


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


void draw_game(const swallow* player, const star stars[], const hunter hunters[], const gameContext* ctx, const gameConfig* cfg);

void draw_arena_borders(const gameConfig* cfg);

void draw_status_bar(const swallow* player, const gameContext* ctx, const gameConfig* cfg);



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


void draw_actors(const swallow* player, const star stars[], const hunter hunters[], const gameContext* ctx, const gameConfig* cfg);

void draw_swallow(const swallow* player);
void draw_stars(const star stars[], const gameConfig* cfg);
void draw_hunters(const hunter hunters[], const gameConfig* cfg);

void draw_taxi(const taxi* t);

void draw_safe_zone_circle(const gameContext* ctx, int radius);


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


void draw_ranking_screen(rankingEntry entries[], int scores_to_display, const gameContext* ctx, int display_limit);
void draw_final_screen(const swallow* player, const gameContext* ctx);

void draw_result(const swallow* player, const gameContext* ctx, int y, int x);
void draw_result_statistics(const swallow* player, const gameContext* ctx, int y, int x);


