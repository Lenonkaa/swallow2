#pragma once

#include "dataTypes.h"


enum hunterType {
    H_STANDARD = 0,
    H_BIG,
    H_FAST
};

enum walls {
    TOP_WALL = 0,
    BOTTOM_WALL,
    LEFT_WALL,
    RIGHT_WALL = 3
};


#define STARS_VELOCITY_FALLING 0.2f


#define TAXI_SPEED 0.8f
#define TAXI_SPEED_RESCUING 4.0f


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//OGOLNE

void actors_movement_and_collisions(swallow* player, hunter hunters[], star stars[], gameContext* context, gameConfig* cfg);


bool spawn_probability_check(float probability);



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


//SWALLOW

void update_swallow(swallow* player, const gameContext* ctx, const gameConfig* cfg);

void swallow_dont_go_beyond_screen(int max_w, int max_h, swallow* player);



//STARS

void star_spawn_mechanism(star* star, bool* spawn, const gameContext* ctx, const gameConfig* cfg);

void star_falling_movement(star* star, const gameContext* ctx, const gameConfig* cfg);

void update_stars(star stars[], const gameContext* ctx, const gameConfig* cfg);

void star_activate(star* star, bool* spawn, const gameContext* ctx);





//HUNTERS

void update_hunters(hunter hunters[], const swallow* player, const gameContext* ctx, gameConfig* cfg);



void hunters_escalation(const gameContext* ctx, gameConfig* cfg);



void hunter_activate(int i, hunter hunters[], const swallow* player, const gameContext* ctx, const gameConfig* cfg);

//Hunters start

void hunter_choose_start_wall(int i, hunter hunters[], const gameContext* ctx);

void choose_hunter_type(int i, hunter hunters[], const gameConfig* cfg);

void set_hunter_basedon_type(int i, hunter hunters[], const gameConfig* cfg);

bool any_hunter_type_allowed_check(const gameConfig* cfg);



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


//TAXI



void start_taxi(swallow* player, gameContext* ctx);



void update_taxi(taxi* t, swallow* p, const gameContext* ctx, hunter hunters[], gameConfig* cfg);




void taxi_go_to_swallow(taxi* t, swallow* p);

void taxi_take_swallow(taxi* t, swallow* p);

bool taxi_check_overlap(const taxi* t, const swallow* p);

void taxi_go_to_safe_zone(taxi* t, swallow* p, const gameContext* ctx, hunter hunters[], gameConfig* cfg);

void taxi_leave_swallow(taxi* t, swallow* p, hunter hunters[], gameConfig* cfg);

void taxi_create_safe_zone(swallow* p, hunter hunters[], gameConfig* cfg);