
#include "initSetCleanup.h"
#include "IO.h"
#include "drawOutput.h"
#include "updateActors.h"



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------




void game_running(swallow* player, hunter hunters[], star stars[], gameContext* context, gameConfig* cfg) {

   

    clock_t level_start_time = clock();


    screen_before_level(player, context);

    while (context->running) {

        clock_t frame_start = clock();

        input(player, context);

        actors_movement_and_collisions(player, hunters, stars, context, cfg);

        /*
        context->frame_count++;
        context->world_time = context->frame_count * (cfg->game_speed / 1000.0f); //game speed in ms : 1000 = seconds
        context->time_left = cfg->time_limit - context->world_time;
        */

        context->world_time = (float)(clock() - level_start_time) / CLOCKS_PER_SEC;
        context->time_left = (float)cfg->time_limit - context->world_time;



        // checking life, time and score - when to end the game

        if (next_level_check(player, context, cfg)) {
            context->level++;
            if (context->level < NUMBER_LEVELS) {
                screen_before_level(player, context);
                set_level(player, hunters, stars, context, cfg);
                level_start_time = clock();
                frame_start = clock();
            }
            else {
                context->win = true;
                context->running = false;
            }
        }


        if (player->life <= 0 || context->time_left <= 0) {
            context->running = false;

        }

        


        draw_game(player, stars, hunters, context, cfg);


        int elapsed_ms = (int)((clock() - frame_start) * 1000 / CLOCKS_PER_SEC);
        int wait = cfg->game_speed - elapsed_ms;

        // time control
        napms(wait); // wait x miliseconds 
    }

}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------



// START GAME


void init_game(swallow* player, star stars[], hunter hunters[], gameContext* context, gameConfig* cfg) {
    init_screen_pdcurses();
    init_game_data(player, context, cfg);

    load_config("config.txt", cfg);

    get_player_initial_setup(player, context);



    //context->level = INITIAL_LEVEL;
    set_level(player, hunters, stars, context, cfg); //level 0, init actors

    //init_levels(context, cfg);
    init_swallow_look(player);
}





void init_screen_pdcurses() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    set_colors();
}



void init_game_data(swallow* player, gameContext* context, gameConfig* cfg) {
    getmaxyx(stdscr, context->screen_h, context->screen_w);
    context->running = true;
    context->frame_count = 0;
    context->win = false;
    player->speed_lvl = DEFAULT_SWALLOW_SPEED_LEVEL;
    player->score = 0;

}



void get_player_initial_setup(swallow* player, gameContext* context){

    nodelay(stdscr, FALSE);
    draw_controls_guide(context);
    nodelay(stdscr, TRUE);
    

    echo();      
    curs_set(1); 

    get_player_name(player, context);

    choose_initial_level(context);

    noecho();   
    curs_set(0); 
}


void get_player_name(swallow* player, gameContext* context) {
    mvprintw(context->screen_h / 2 - 2, context->screen_w / 2 - 20, "Player's nickname:  ");
    refresh();
    getnstr(player->name, 29);

}


void choose_initial_level(gameContext* context) {
    int initial_level;
    mvprintw(context->screen_h / 2, context->screen_w / 2 - 30, "Choose initial level (1-%d): ", NUMBER_LEVELS);
    refresh();
    scanw("%d", &initial_level);

    if (initial_level < 1)  initial_level = 1; //if smb gives 0

    context->level = initial_level - 1;
}





void cleanup_game() {
    flushinp();
    curs_set(1);
    endwin(); //terminal back
}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


// LEVELS


bool next_level_check(swallow* player, gameContext* context, const gameConfig* cfg) {
    if (player->stars_collected >= cfg->star_win){
        player->score += (context->level) * LEVEL_BONUS_MULTIPLIER; //level bonus, level alredy increased, but levels start from 0
        player->score += player->life * cfg->score.life; //bonus for life left
        player->score += context->time_left * cfg->score.time;
        return true;
    }
    else
        return false;
}

void set_level(swallow* player, hunter hunters[], star stars[], gameContext* context, gameConfig* cfg) {

    int level = context->level + 1;
    char filename[50];

    sprintf(filename, "level%d.txt", level);

    player->stars_collected = 0; //reset for next level
	
    context->frame_count = 0;
    
    load_level_config(filename, cfg);

    context->gameAreaSize.height = cfg->map.height;
    context->gameAreaSize.width = cfg->map.width;

    if (context->gameAreaSize.height > (SAFE_ZONE_SIZE * 2) && context->gameAreaSize.width > (SAFE_ZONE_SIZE * 2)) {
        cfg->safe_zone_size = SAFE_ZONE_SIZE;
    }
    else {
        cfg->safe_zone_size = (context->gameAreaSize.height)/2;
    }
   

    init_hunters(hunters, cfg); //reset hunters
    init_stars(stars);
	init_swallow(player, context); // now life=100, stars_collected=0, initial position
    init_taxi(&context->taxi);


}





//----------------------------------------------------------------------------------------------------------------------------------------------------------------------



// Actors' initialization

void init_swallow(swallow* player, const gameContext* context) {

    player->pos.x = 10.0f;
    player->pos.y = context->screen_h / 2.0f;
    player->pos.vx = SWALLOW_INITIAL_MOMENTUM_X;
    player->pos.vy = SWALLOW_INITIAL_MOMENTUM_Y;
    player->life = 100; //100%
    player->stars_collected = 0;


    player->is_carried_taxi = false;


}


void init_swallow_look(swallow* player) {
    // symbol
    player->size.width = SWALLOW_SHAPE_WIDTH;
    player->size.height = SWALLOW_SHAPE_LENGTH;

    // symbol into array
    //strcpy(player->shape[0], "SWALLOW_SHAPE");
    strcpy(player->shape[0], SWALLOW_SHAPE);


}




void init_stars(star* stars) {
    for (int i = 0; i < LIMIT_STARS; i++) {
        stars[i].active = false;
    }
}




void init_hunters(hunter* hunters, const gameConfig* cfg) {
    for (int i = 0; i < LIMIT_HUNTERS; i++) {
        hunters[i].active = false;
        reset_hunter(i, hunters, cfg);
    }
}



void reset_hunter(int i, hunter hunters[], const gameConfig* cfg) {

    /*
    hunters[i].bounces_left = cfg->hunter_bounces;
    hunters[i].damage = cfg->damage_value;
    hunters[i].speed = cfg->hunter_speed;
    */

    hunters[i].has_dashed = false;
    hunters[i].wait_before_dash = 0;
}



void init_taxi(taxi* t) {
    t->active = false;
    t->is_carrying = false;
    t->speed = TAXI_SPEED; 
    t->size.width = TAXI_WIDTH;
    t->size.height = TAXI_HEIGHT; 
    strcpy(t->shape[0], "+------+");
    strcpy(t->shape[1], "|      |");
    strcpy(t->shape[2], "+------+");
}

