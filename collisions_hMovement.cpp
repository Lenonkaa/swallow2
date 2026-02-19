#include "collisions_hMovement.h"




// STAR - SWALLOW collisions



void check_star_collisions(swallow* player, star stars[], const gameConfig* cfg) {

    for (int i = 0; i < cfg->max_stars; i++) {
        if (stars[i].active) {
            
            if (is_star_collision(i,player,stars)) {

                player->stars_collected++;
                player->score += cfg->score.star;
                stars[i].active = false;
            }

        }
    }
}



bool is_star_collision(int i, swallow* player, star stars[]) {

    int sx = (int)stars[i].pos.x;
    int sy = (int)stars[i].pos.y;

    int px = (int)player->pos.x;
    int py = (int)player->pos.y;

    // star (s) inside swallow (p - player) - left/right and top/below
    if (sx >= px && sx <= px + player->size.width &&
        sy >= py && sy <= py + player->size.height) {

        return true;
    }

    return false;
 
}





// HUNTER - SWALLOW collisons


void check_hunter_collision(swallow* player, hunter hunters[], const gameConfig* cfg) {

    if (player->is_carried_taxi) return;

    for (int i = 0; i < cfg->current_max_hunters; i++) {

        if (hunters[i].active) {

            
            if (is_hunter_collision(i,player,hunters)) {


                player->life -= hunters[i].damage; // damage
                hunters[i].active = false; //disappear after collision

                if (player->life < 0) player->life = 0;
            }

        }
    }
}



bool is_hunter_collision(int i, swallow* player, hunter hunters[]) {

    // swallow (p - player) borders
    int pLeft = (int)player->pos.x;
    int pRight = pLeft + player->size.width;
    int pTop = (int)player->pos.y;
    int pBottom = pTop + player->size.height;

    // hunter borders
    int hLeft = (int)hunters[i].pos.x;
    int hRight = hLeft + hunters[i].size.width;
    int hTop = (int)hunters[i].pos.y;
    int hBottom = hTop + hunters[i].size.height;

    // collision - checking if there is distant between swallow and hunter borders - overlap
    if (pLeft < hRight &&
        pRight > hLeft &&
        pTop < hBottom &&
        pBottom > hTop) {

        return true;

    }
    
    return false;
}



// HUNTERS Movement


void hunter_movement(int i, hunter hunters[], const swallow* player, const gameContext* ctx, const gameConfig* cfg) {

    hunter_dash_logic(i, hunters, player, ctx, cfg);

    if (hunters[i].wait_before_dash > 0) { //no movement or bouncing when waiting for dash
        return;
    }


    // const vector
    hunters[i].pos.x += hunters[i].pos.vx;
    hunters[i].pos.y += hunters[i].pos.vy;

    hunter_bouncing(i, hunters, ctx);
    
}



void hunter_bouncing(int i, hunter hunters[], const gameContext* ctx) {


    int max_w = ctx->gameAreaSize.width;
    int max_h = ctx->gameAreaSize.height;
   
    bool bounced = false;


    //zamienic w funkcje z parametrem - bo to samo prawie dla x i y

    // left/right
    if (hunters[i].pos.x <= 0 || hunters[i].pos.x >= max_w - hunters[i].size.width) {

        hunters[i].pos.vx *= -1;

        //wall protection
        if (hunters[i].pos.x <= 0) hunters[i].pos.x = 1;
        if (hunters[i].pos.x >= max_w - hunters[i].size.width) hunters[i].pos.x = max_w - hunters[i].size.width - 1;
        bounced = true;
    }

    // top/bottom
    if (hunters[i].pos.y <= 0 || hunters[i].pos.y >= max_h - hunters[i].size.height) {

        hunters[i].pos.vy *= -1;

        //wall protection
        if (hunters[i].pos.y <= 0) hunters[i].pos.y = 1;
        if (hunters[i].pos.y >= max_h - hunters[i].size.height) hunters[i].pos.y = max_h - hunters[i].size.height;
        bounced = true;
    }


    if (bounced) {
        hunters[i].bounces_left--;

     

        if (hunters[i].has_dashed) {
            return_normal_speed_after_dash(&hunters[i]);
            
        }

        // disappear after bounces
        if (hunters[i].bounces_left <= 0) hunters[i].active = false;

    }



}



/* zamiana

void hunter_bouncing(int i, hunter hunters[], const gameContext* ctx) {


    int max_w = ctx->gameAreaSize.width;
    int max_h = ctx->gameAreaSize.height;

    bool bounced = bounce_wall(i, hunters, ctx);


    if (bounced) {
        hunters[i].bounces_left--;



        if (hunters[i].has_dashed) {
            return_normal_speed_after_dash(&hunters[i]);

        }

        // disappear after bounces
        if (hunters[i].bounces_left <= 0) hunters[i].active = false;

    }



}


bool bounce_wall(int i, hunter hunters[], const gameContext* ctx) {


    int max_w = ctx->gameAreaSize.width;
    int max_h = ctx->gameAreaSize.height;

    bool bounced = false;

    if (hunters[i].pos.x <= 0 || hunters[i].pos.x >= max_w - hunters[i].size.width) {
        hunters[i].pos.vx *= -1;
        bounced = true;
    }

    if (hunters[i].pos.y <= 0 || hunters[i].pos.y >= max_h - hunters[i].size.height) {
        hunters[i].pos.vy *= -1;
        bounced = true;

    }

    if (hunters[i].pos.y <= 0) hunters[i].pos.y = 1;
    if (hunters[i].pos.y >= max_h - hunters[i].size.height) hunters[i].pos.y = max_h - hunters[i].size.height;

   
    return bounced;

}


*/


// HUNTER - DASH

void hunter_dash_logic(int i, hunter hunters[], const swallow* player, const gameContext* ctx, const gameConfig* cfg) {


    if (hunters[i].wait_before_dash > 0) {

        if (ctx->taxi.is_carrying) {
            hunters[i].wait_before_dash = WAIT_TIME_BEFORE_DASH * 1000 / cfg->game_speed;
            return;
        }

        hunters[i].wait_before_dash--;

        if (hunters[i].wait_before_dash == 0){
            hunter_perform_dash(&hunters[i], player);
        }

        return;
    }


    if (!hunters[i].has_dashed) {

        if (hunter_check_if_miss_swallow(&hunters[i], player)) {

            hunters[i].wait_before_dash = WAIT_TIME_BEFORE_DASH * 1000 / cfg->game_speed;
            hunters[i].pos.vx = 0;
            hunters[i].pos.vy = 0;
            return;
        }
    }

    return;
}


bool hunter_check_if_miss_swallow(const hunter* h, const swallow* p) {
    
    //from hunter to swallow
    float vector_position_difference_x = p->pos.x - h->pos.x;
    float vector_position_difference_y = p->pos.y - h->pos.y;

    float hunter_speed_vector_magnitude = sqrtf(h->pos.vx * h->pos.vx + h->pos.vy * h->pos.vy);
    if (hunter_speed_vector_magnitude <= 0) return false;


    float area_parallelogram_on_vectors = fabsf(vector_position_difference_x * h->pos.vy - vector_position_difference_y * h->pos.vx); //cross product
    float swallow_shortest_distance_to_hunter_path = area_parallelogram_on_vectors / hunter_speed_vector_magnitude; //perpendicular distance = area / base




    if (swallow_shortest_distance_to_hunter_path > (float)p->size.width) { //do czego przyrównywać? width czy height?
        return true; // miss swallow
    }

    return false;

}



void hunter_perform_dash(hunter* h, const swallow* p) {


    //player and hunter center
    float p_center_x = p->pos.x + p->size.width / 2.0f;
    float p_center_y = p->pos.y + p->size.height / 2.0f;


    float h_center_x = h->pos.x + h->size.width / 2.0f;
    float h_center_y = h->pos.y + h->size.height / 2.0f;


    float vector_position_difference_x = p_center_x - h_center_x;
    float vector_position_difference_y = p_center_y - h_center_y;

    float distance = sqrtf(vector_position_difference_x * vector_position_difference_x + vector_position_difference_y * vector_position_difference_y);

    if (distance > 0) {

        float direction_vector_x = vector_position_difference_x / distance;
        float direction_vector_y = vector_position_difference_y / distance;

        h->pos.vx = direction_vector_x * h->dash_speed;
        h->pos.vy = direction_vector_y * h->dash_speed;

        h->has_dashed = true; //can dash later after return_to_normal_speed after wall bounce
    }

}




void return_normal_speed_after_dash(hunter* h) {

    float speed_vector_magnitude = sqrtf(h->pos.vx * h->pos.vx + h->pos.vy * h->pos.vy);


    if (speed_vector_magnitude > 0) {

        float direction_vector_x = h->pos.vx / speed_vector_magnitude;
        float direction_vector_y = h->pos.vy / speed_vector_magnitude;

        h->pos.vx = direction_vector_x * h->speed;
        h->pos.vy = direction_vector_y * h->speed;
    }

   // h->has_dashed = false; //can dash again later
}
