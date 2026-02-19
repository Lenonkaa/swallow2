#pragma once


#include "dataTypes.h"


#define LIMIT_STARS 100
#define LIMIT_HUNTERS 20

#define LEVEL_BONUS_MULTIPLIER 100

#define SWALLOW_INITIAL_MOMENTUM_X 0.5f
#define SWALLOW_INITIAL_MOMENTUM_Y 0
#define DEFAULT_SWALLOW_SPEED_LEVEL 3


#define SWALLOW_SHAPE "/\\/\\/\\"  //3x10max  - strlen
#define SWALLOW_SHAPE_WIDTH 6 // width of array of "/\/\/\"
#define SWALLOW_SHAPE_LENGTH 1



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


void game_running(swallow* player, hunter hunters[], star stars[], gameContext* context, gameConfig* cfg);



void init_game(swallow* player, star stars[], hunter hunters[], gameContext* context, gameConfig* cfg);

void get_player_initial_setup(swallow* player, gameContext* context);
void get_player_name(swallow* player, gameContext* context);
void choose_initial_level(gameContext* context);


// Start game
void init_screen_pdcurses();

void init_game_data(swallow* player, gameContext* context, gameConfig* cfg);

bool next_level_check(swallow* player, gameContext* context, const gameConfig* cfg);

void set_level(swallow* player, hunter hunters[], star stars[], gameContext* context, gameConfig* cfg);

void cleanup_game();


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


// Actors' initialization

void init_swallow(swallow* player, const gameContext* context);

void init_swallow_look(swallow* player);



void init_stars(star* stars);



void init_hunters(hunter* hunters, const gameConfig* cfg);

void reset_hunter(int i, hunter hunters[], const gameConfig* cfg); 



void init_taxi(taxi* t);



