#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#include <curses.h>
#include <stdlib.h> // rand, srand
#include <time.h>  
#include <stdbool.h>// bool, true, false
#include <math.h>   
#include <string.h>


#define NUMBER_LEVELS 5
#define NUMBER_SPEED_LEVELS 5




const int INITIAL_LEVEL = 0;


//direction vectors
const float UP = -1.0 / 2; //upward velocity vector
const float DOWN = 1.0f / 2; //downward velocity
const float LEFT = -2.0f / 2; //left velocity (x2 for char proportions)
const float RIGHT = 2.0f / 2; //right velocity



enum colorPairs {
    C_DEFAULT,
    C_SWALLOW_HIGH,
    C_SWALLOW_MEDIUM,
    C_SWALLOW_LOW,
    C_STAR,
    C_STATUS_BAR,
    C_ARENA_BORDERS,
    C_HUNTER_1,
    C_HUNTER_2,
    C_HUNTER_3,
    C_TAXI
};



#define TAXI_WIDTH 10
#define TAXI_HEIGHT 3

#define SAFE_ZONE_SIZE 12



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


//TYPE Definitions


typedef struct {
    int height;
    int width;
}areaDimensions;

typedef struct{
    float x, y;
    float vx, vy; // velocity
}positionVector;

typedef struct {
    char name[30];
    positionVector pos;
    int life;
	int stars_collected; //number of stars collected during a level, reset at new level
    int score; //cumulative score
    int speed_lvl;
    areaDimensions size;
	char shape[3][10];  //symbol - max 3x10

    bool is_carried_taxi;
} swallow;



typedef struct {
    positionVector pos;
    bool active;
}star;


typedef struct {
    bool is_carrying;
    bool active;
    positionVector pos;
    areaDimensions size;
    float speed;
    char shape[TAXI_HEIGHT+1][TAXI_WIDTH+1];
} taxi;

typedef struct{
    positionVector pos;
    int bounces_left;
    bool active;
    int type;
    int damage;
    float speed;
    int color_pair;
    areaDimensions size;
	char shape[3][10];  //symbol - max 3x10

    bool has_dashed;
    float dash_speed;
    int wait_before_dash;
}hunter;


typedef struct {
    int damage;
    float speed;

    float dash_speed;

    int hunter_bounces;
    int color_pair;

    int type_allowed; //0 not, 1 allowed at the level

    areaDimensions size;
    char shape[3][10];
}hunter_characteristics;


typedef struct{
    int screen_w, screen_h;
	bool running; // only for playing game state - main loop
    long frame_count;
    int level;
    bool win;
    areaDimensions gameAreaSize;
    //level_characteristics levels[NUMBER_LEVELS];
    float world_time;
    float time_left;
    taxi taxi;

    
} gameContext;

typedef struct {
    int star;
    int life;
    int time;

}score_weights;



typedef struct{
    areaDimensions map;

    int safe_zone_size;

    int max_stars;
    int max_hunters;

    int time_limit;
    int game_speed;

    int star_win;

    score_weights score;



    float spawn_probability_stars;
    float spawn_probability_hunters;

    float swallow_speed_min;
    float swallow_speed_max;

    float swallow_speed_diff;



    hunter_characteristics hunter_types[3];


    int escalation_interval_seconds; // Co ile sekund trudność rośnie
    int escalation_max_hunters;    // O ile zwiększyć max_hunters
    int escalation_hunters_bounces;

    int escalation_steps;
    int current_max_hunters;

    int color_arena;


    /* from config.txt
    int star_win_diff;

    float swallow_speed;

    int damage_value;
    int damage_diff;
    int hunter_bounces;
    int hunter_bounces_diff;
    float hunter_speed;
    float hunter_speed_diff;
    float hunter_speed_lvl_diff;//
    */
}gameConfig;






/*
typedef struct {
    int level_number;

    int screen_w, screen_h;
    int stars_to_collect;
    int time_limit;


    float spawn_probability_stars;
    float spawn_probability_hunters;

    float swallow_speed_min;
    float swallow_speed_max;
    float swallow_speed_diff;


    float hunter_speed;//

    int hunter_bounces;//


    int max_hunters;
    int allowed_hunters;
    hunter_characteristics hunter_types[3];
}level_characteristics;

*/