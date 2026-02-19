#pragma once

#include "dataTypes.h"



#define POINTS_FOR_STAR 5
#define WAIT_TIME_BEFORE_DASH 2



// Star/hunter-swallow 


void check_star_collisions(swallow* player, star stars[], const gameConfig* cfg);
bool is_star_collision(int i, swallow* player, star stars[]);


void check_hunter_collision(swallow* player, hunter hunters[], const gameConfig* cfg);
bool is_hunter_collision(int i, swallow* player, hunter hunters[]);



// Hunters movement

void hunter_movement(int i, hunter hunters[], const swallow* player, const gameContext* ctx, const gameConfig* cfg);

void hunter_bouncing(int i, hunter hunters[], const gameContext* ctx);


bool bounce_wall(int i, hunter hunters[], const gameContext* ctx);


// Hunters dash


void hunter_dash_logic(int i, hunter hunters[], const swallow* player, const gameContext* ctx, const gameConfig* cfg);


bool hunter_check_if_miss_swallow(const hunter* h, const swallow* p);

void hunter_perform_dash(hunter* h, const swallow* p);


void return_normal_speed_after_dash(hunter* h);
