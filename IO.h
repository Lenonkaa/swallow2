#pragma once

#include "dataTypes.h"




#define QUIT_BUTTON 'm'
#define RANKING_BUTTON 'r'
#define CONTINUE_BUTTON 'n'
#define VELOCITY_INCREASE_BUTTON 'p'
#define VELOCITY_DECREASE_BUTTON 'o'


#define RANKING_FILE "ranking.txt"
#define TOP_N 10
#define MAX_ENTRIES 100

typedef struct {
    char name[30];
    int score;
} rankingEntry;


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------



// Input 
void load_config(const char* filename, gameConfig* cfg);

void input(swallow* player, gameContext* ctx); //klawisze


void screen_before_level(const swallow* player, gameContext* context);

// level config

void load_level_config(const char* filename, gameConfig* cfg);
void load_config_score(FILE* file, gameConfig* cfg, int n_parameters_toread);
void load_config_actors(FILE* file, gameConfig* cfg, int n_parameters_toread);
void load_config_stars(FILE* file, gameConfig* cfg, int n);
void load_config_swallow(FILE* file, gameConfig* cfg, int n);
void load_config_hunters(FILE* file, gameConfig* cfg, int n);

// Output

void display_final_screen(const swallow* player, const gameContext* ctx);



//Ranking

void ranking_save_score_right_place(const char name[], int score);
void ranking_read_scores(rankingEntry entries[], int* count);
void ranking_find_place_for_score(rankingEntry entries[], int count,const char name[], int score);
void ranking_put_score_in_file(rankingEntry entries[], int count);

void ranking_display_top_scores(const gameContext* ctx, int display_limit);
