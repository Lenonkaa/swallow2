#include "updateActors.h"


#include "initSetCleanup.h"
#include "collisions_hMovement.h"





void actors_movement_and_collisions(swallow* player, hunter hunters[], star stars[], gameContext* context, gameConfig* cfg) {


    update_swallow(player, context, cfg);
    update_stars(stars, context, cfg);
    update_taxi(&context->taxi, player, context, hunters, cfg);

    check_hunter_collision(player, hunters, cfg);
    update_hunters(hunters, player, context, cfg);

    check_star_collisions(player, stars, cfg);
    
}





bool spawn_probability_check(float probability_percent) {
    int roll = rand() % 100 + 1; // 1 to 100
    if (roll <= probability_percent * 100) return true;
    else return false;
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//STARS


void update_stars(star stars[], const gameContext* ctx, const gameConfig* cfg) {

    bool spawn = false; // only one star per frame

    for (int i = 0; i < cfg->max_stars; i++) {

        if (stars[i].active) {
			star_falling_movement(&stars[i], ctx, cfg);
        }
        else {
			star_spawn_mechanism(&stars[i], &spawn, ctx, cfg);
        }
    }
}



void star_falling_movement(star* star, const gameContext* ctx, const gameConfig* cfg) {
    // falling
    star->pos.y += STARS_VELOCITY_FALLING;
    // disappear when below game area
    if (star->pos.y >= ctx->gameAreaSize.height) {
        star->active = false;
    }
}



void star_spawn_mechanism(star * star, bool* spawn, const gameContext* ctx, const gameConfig* cfg) {
    float probability = cfg->spawn_probability_stars;
    if (spawn_probability_check(probability) and !*spawn) {
        star_activate(star, spawn, ctx);
    }
}


void star_activate(star* star, bool* spawn, const gameContext* ctx) {
    star->active = true;
    star->pos.x = (float)(rand() % ctx->gameAreaSize.width);
    star->pos.y = 0; // start from top
    *spawn = true; // only one star per frame
}





//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//SWALLOW


void update_swallow(swallow* player, const gameContext* ctx, const gameConfig* cfg) {

    //no movement in taxi

    if (player->is_carried_taxi) return;

    // movement

    float speed_factor = cfg->swallow_speed_min + ((player->speed_lvl - 1) * cfg->swallow_speed_diff); //calculating speed at current speed level

    player->pos.x += player->pos.vx * speed_factor;
    player->pos.y += player->pos.vy * speed_factor;


    swallow_dont_go_beyond_screen(ctx->gameAreaSize.width, ctx->gameAreaSize.height, player);

}



void swallow_dont_go_beyond_screen(int max_w, int max_h, swallow* player) {
    if (player->pos.x < 0) player->pos.x = 0;
    if (player->pos.x >= max_w - player->size.width) player->pos.x = max_w - player->size.width;
    if (player->pos.y < 0) player->pos.y = 0;
    if (player->pos.y >= max_h - player->size.height) player->pos.y = max_h - player->size.height;
}




//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//HUNTERS



void update_hunters(hunter hunters[], const swallow* player, const gameContext* ctx, gameConfig* cfg) {
    
    hunters_escalation(ctx, cfg);

    
    for (int i = 0; i < cfg->current_max_hunters; i++) {

        if (!hunters[i].active) {
            float probability = cfg->spawn_probability_hunters;
            if (spawn_probability_check(probability)) {
                hunter_activate(i, hunters, player, ctx, cfg);
            }
        }

        if (hunters[i].active) {
            hunter_movement(i, hunters, player, ctx, cfg);

        }
    }
}



void hunters_escalation(const gameContext* ctx, gameConfig* cfg) {

    if (cfg->escalation_interval_seconds > 0) {
        cfg->escalation_steps = (int)(ctx->world_time / cfg->escalation_interval_seconds);
    }


    cfg->current_max_hunters = cfg->max_hunters + (cfg->escalation_steps * cfg->escalation_max_hunters);

    // LIMIT_HUNTERS (initSetCleanup.h)
    if (cfg->current_max_hunters > LIMIT_HUNTERS) cfg->current_max_hunters = LIMIT_HUNTERS;
}



//HUNTER START

void hunter_activate(int i, hunter hunters[], const swallow* player, const gameContext* ctx, const gameConfig* cfg) {


    hunters[i].active = true;
    reset_hunter(i, hunters, cfg);

    choose_hunter_type(i, hunters, cfg);

    hunters[i].bounces_left += (cfg->escalation_steps * cfg->escalation_hunters_bounces); //escalating difficulty - new get more bounces

    hunter_choose_start_wall(i, hunters, ctx);

    // vector towards swallow
    float dx = player->pos.x - hunters[i].pos.x;
    float dy = player->pos.y - hunters[i].pos.y;
    float length = sqrtf(dx * dx + dy * dy);

    // constant speed
    if (length > 0) {
        hunters[i].pos.vx = (dx / length) * hunters[i].speed;
        hunters[i].pos.vy = (dy / length) * hunters[i].speed;
    }
}




void hunter_choose_start_wall(int i, hunter hunters[], const gameContext* ctx) {

    int max_w = ctx->gameAreaSize.width;
    int max_h = ctx->gameAreaSize.height;

    // choose 1 of 4 walls where hunter starts
    int side = rand() % 4;

    switch (side) {
    case TOP_WALL:
        hunters[i].pos.x = (float)(rand() % (max_w - hunters[i].size.width));
        hunters[i].pos.y = 0;
        break;
    case BOTTOM_WALL:
        hunters[i].pos.x = (float)(rand() % (max_w - hunters[i].size.width));
        hunters[i].pos.y = (float)(max_h - hunters[i].size.height);
        break;
    case LEFT_WALL:
        hunters[i].pos.x = 0;
        hunters[i].pos.y = (float)(rand() % (max_h - hunters[i].size.height));
        break;
    case RIGHT_WALL:
        hunters[i].pos.x = (float)(max_w - hunters[i].size.width);
        hunters[i].pos.y = (float)(rand() % (max_h - hunters[i].size.height));
        break;
    }
}


void choose_hunter_type(int i, hunter hunters[], const gameConfig* cfg) {
    //hunter type choice


    if (!any_hunter_type_allowed_check(cfg)) {
        hunters[i].active = false;
        return;
    }
   
    int type_roll;

    do {
        type_roll = rand() % 3; // 0, 1 or 2
    } while (cfg->hunter_types[type_roll].type_allowed!=1);


    hunters[i].type = type_roll;


    switch (type_roll) {
    case H_STANDARD:
        set_hunter_basedon_type(i, hunters, cfg);
        strcpy(hunters[i].shape[0], "/\\");
        strcpy(hunters[i].shape[1], "\\/");
        break;

    case H_BIG:
        set_hunter_basedon_type(i, hunters, cfg);
        strcpy(hunters[i].shape[0], "###");
        strcpy(hunters[i].shape[1], "###");
        break;

    case H_FAST:
        set_hunter_basedon_type(i, hunters, cfg);
        strcpy(hunters[i].shape[0], ">>>");
        break;
    }
}


bool any_hunter_type_allowed_check(const gameConfig* cfg) {
    bool any_allowed = false;
    for (int t = 0; t < 3; t++) { // Mamy 3 typy: H_STANDARD, H_BIG, H_FAST
        if (cfg->hunter_types[t].type_allowed == 1) {
            any_allowed = true;
            break;
        }
    }
    return any_allowed;

    

}


void set_hunter_basedon_type(int i, hunter hunters[], const gameConfig* cfg) {

    int type = hunters[i].type;

    hunters[i].size.width = cfg->hunter_types[type].size.width;
    hunters[i].size.height = cfg->hunter_types[type].size.height;
   // hunters[i].color_pair = cfg->hunter_types[type].color_pair;
    hunters[i].color_pair = cfg->hunter_types[type].color_pair;
    hunters[i].damage = cfg->hunter_types[type].damage;
    hunters[i].speed = cfg->hunter_types[type].speed; 
    hunters[i].dash_speed = cfg->hunter_types[type].dash_speed;
    hunters[i].bounces_left = cfg->hunter_types[type].hunter_bounces;

   // strcpy(hunters[i].shape[0], cfg->hunter_types[type].shape[0]);
    //co z jednolinijkowymi?
    //strcpy(hunters[i].shape[1], cfg->hunter_types[type].shape[1]);

}





//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//TAXI




void start_taxi(swallow* player, gameContext* ctx) {
    ctx->taxi.active = true;
    ctx->taxi.pos.x = ctx->gameAreaSize.width;
    ctx->taxi.pos.y = player->pos.y - 1;
}




void update_taxi(taxi* t, swallow* p, const gameContext* ctx, hunter hunters[], gameConfig* cfg) {

    if (!t->active) return;

    if (!t->is_carrying) {
        taxi_go_to_swallow(t, p);
    }
    else {

        taxi_go_to_safe_zone(t, p, ctx, hunters, cfg);
    }
}




void taxi_go_to_swallow(taxi* t, swallow* p) {

    if (taxi_check_overlap(t, p))  taxi_take_swallow(t, p);

    float vector_position_difference_x = p->pos.x - t->pos.x;
    float vector_position_difference_y = p->pos.y - t->pos.y;
    float distance = sqrtf(vector_position_difference_x * vector_position_difference_x + vector_position_difference_y * vector_position_difference_y);
    
    if (distance > 1.0f) {

        float direction_vector_x = vector_position_difference_x / distance;
        float direction_vector_y = vector_position_difference_y / distance;

        t->pos.x += direction_vector_x * TAXI_SPEED_RESCUING;
        t->pos.y += direction_vector_y * TAXI_SPEED_RESCUING;
    }

}



bool taxi_check_overlap(const taxi* t, const swallow* p) {

    return (t->pos.x   <   p->pos.x + p->size.width &&  // L t < P s
        t->pos.x + t->size.width   >   p->pos.x &&  // P t > L s
        t->pos.y   <   p->pos.y + p->size.height && // Top t < Dol s
        t->pos.y   +   t->size.height > p->pos.y);  // Dol t > Top s
}



void taxi_take_swallow(taxi* t, swallow* p) {
    t->is_carrying = true;
    p->is_carried_taxi = true;
}



void taxi_go_to_safe_zone(taxi* t, swallow* p, const gameContext* ctx, hunter hunters[], gameConfig* cfg){

    // middle of game arena
        float target_x = ctx->gameAreaSize.width / 2.0f;
        float target_y = ctx->gameAreaSize.height / 2.0f;

        float vector_position_difference_x = target_x - t->pos.x;
        float vector_position_difference_y = target_y - t->pos.y;

        float distance = sqrtf(vector_position_difference_x * vector_position_difference_x + vector_position_difference_y * vector_position_difference_y);



        if (distance > 1.0f) {

            float direction_vector_x = vector_position_difference_x / distance;
            float direction_vector_y = vector_position_difference_y / distance;

            t->pos.x += direction_vector_x * (t->speed);
            t->pos.y += direction_vector_y * (t->speed);

            //swallow in taxi
            p->pos.x = t->pos.x + 1;
            p->pos.y = t->pos.y + 1;

        }

        else {
            taxi_leave_swallow(t, p, hunters, cfg);
        }

}



void taxi_leave_swallow(taxi* t, swallow* p, hunter hunters[], gameConfig* cfg) {

    t->active = false;
    t->is_carrying = false;
    p->is_carried_taxi = false;

    p->pos.vx = 0;
    p->pos.vy = 0;


    taxi_create_safe_zone(p, hunters, cfg);
}



void taxi_create_safe_zone(swallow* p, hunter hunters[], gameConfig* cfg)
{
    for (int i = 0; i < cfg->current_max_hunters; i++) {

        //hunters[i].wait_before_dash += WAIT_TIME_BEFORE_DASH; - rozwiązane w hunter_dash_logic

        float dx = (hunters[i].pos.x - p->pos.x) / 2.0f; //char proportion
        float dy = hunters[i].pos.y - p->pos.y;
        float dist = sqrtf(dx * dx + dy * dy);
        
        if (dist < cfg->safe_zone_size) hunters[i].active = false; //unicestwienie hunterów
    }

   
}




