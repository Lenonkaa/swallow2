#include "IO.h"
#include "drawOutput.h"

#include "updateActors.h"


// Input / Output

void load_config(const char* filename, gameConfig* cfg) {
    FILE* file = fopen(filename, "r");
    if (!file) return;

    char key[50];
    float value;

    while (fscanf(file, "%s %f", key, &value) != EOF) {
        if (strcmp(key, "MAX_STARS") == 0) cfg->max_stars = (int)value;
        else if (strcmp(key, "GAME_SPEED_MS") == 0) cfg->game_speed = (int)value;

        /*
        else if (strcmp(key, "DAMAGE_VALUE") == 0) cfg->damage_value = (int)value;
        else if (strcmp(key, "DAMAGE_DIFFERENCE") == 0) cfg->damage_diff = (int)value;
        else if (strcmp(key, "HUNTERS_STANDARD_SPEED") == 0) cfg->hunter_speed = value;
        else if (strcmp(key, "HUNTERS_STANDARD_BOUNCES") == 0) cfg->hunter_bounces = (int)value;
        else if (strcmp(key, "HUNTERS_SPEED_DIFFERENCE") == 0) cfg->hunter_speed_diff = value;
        else if (strcmp(key, "HUNTERS_BOUNCES_DIFFERENCE") == 0) cfg->hunter_bounces_diff = (int)value;
        else if (strcmp(key, "HUNTERS_SPEED_LEVEL_DIFF") == 0) cfg->hunter_speed_lvl_diff = value;
        else if (strcmp(key, "STARS_TO_WIN") == 0) cfg->star_win = (int)value;
        else if (strcmp(key, "STARS_TO_WIN_LEVEL_DIFF") == 0) cfg->star_win_diff = (int)value;
        */


    }
    fclose(file);
}




void input(swallow* player, gameContext* ctx) {
    int ch = getch();

    if (ch == ERR) return; // no key pressed


    switch (ch) {
        // q for quit
    case QUIT_BUTTON: ctx->running = false; break;

    /*
    if (ctx->taxi.active) { //no movement
        if (ch == 'w' || ch == 's' || ch == 'a' || ch == 'd' || ch == VELOCITY_DECREASE_BUTTON || ch == VELOCITY_INCREASE_BUTTON) {
            return;
        }
    }
    */  
    case 't':
        if (!ctx->taxi.active && !player->is_carried_taxi) {
            start_taxi(player, ctx);
        }
        break;


    
       

        // change in velocity vector
    case 'w': player->pos.vy = UP; player->pos.vx = 0; break;
    case 's': player->pos.vy = DOWN; player->pos.vx = 0; break;
    case 'a': player->pos.vx = LEFT; player->pos.vy = 0; break;
    case 'd': player->pos.vx = RIGHT; player->pos.vy = 0; break;

        // o-p velocity change
    case VELOCITY_DECREASE_BUTTON: if (player->speed_lvl > 1) player->speed_lvl--; break;
    case VELOCITY_INCREASE_BUTTON: if (player->speed_lvl < NUMBER_SPEED_LEVELS) player->speed_lvl++; break;


    }


}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------




void screen_before_level(const swallow* player, gameContext* context) {

    clear();

    mvprintw(context->screen_h / 2 - 2, context->screen_w / 2 - 20, "LEVEL %d", context->level + 1);

    mvprintw(context->screen_h / 2, context->screen_w / 2 - 20, "SCORE: %d", player->score);

    mvprintw(context->screen_h / 2 + 4, context->screen_w / 2 - 20, "Press '%c' to continue...", CONTINUE_BUTTON);
    refresh();

    nodelay(stdscr, FALSE);

    int key;
    do {
        key = getch();
    } while (key != CONTINUE_BUTTON);
  
    flushinp(); // cleaning bufor
    nodelay(stdscr, TRUE);

    clear();

}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------





void load_level_config(const char* filename, gameConfig* cfg) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Blad: Nie mozna otworzyc pliku %s\n", filename);
        return;
    }

    char key[50];
    float value;

    while (fscanf(file, "%s %f", key, &value) != EOF) {
        if (strcmp(key, "MAP_WIDTH") == 0) cfg->map.width = (int)value;
        else if (strcmp(key, "MAP_HEIGHT") == 0) cfg->map.height = (int)value;
        else if (strcmp(key, "STARS_TO_COLLECT") == 0) cfg->star_win = (int)value;

        else if (strcmp(key, "TIME_LIMIT") == 0) cfg->time_limit = (int)value;

        else if (strcmp(key, "COLOR_ARENA") == 0) {
            cfg->color_arena = value;
            init_pair(C_ARENA_BORDERS, cfg->color_arena, COLOR_BLACK);
        }

        else if (strcmp(key, "---SCORE---") == 0) {
            load_config_score(file, cfg, (int)value);
        }

        else if (strcmp(key, "---ACTORS---") == 0) {
            load_config_actors(file, cfg, (int)value);
        }




    }
    fclose(file);
}




void load_config_score(FILE* file, gameConfig* cfg, int n_parameters_toread) {
    char key[50];
    float value;
    if (!file) {
        printf("Blad: Nie mozna otworzyc pliku\n");
        return;
    }
    for (int i = 0; i < n_parameters_toread; i++) {
        if (fscanf(file, "%s %f", key, &value) != EOF) {
            if (strcmp(key, "SCORE_STAR") == 0) cfg->score.star = (int)value;
            else if (strcmp(key, "SCORE_LIFE") == 0) cfg->score.life = (int)value;
            else if (strcmp(key, "SCORE_TIME") == 0) cfg->score.time = (int)value;
        }
    }
}



void load_config_actors(FILE* file, gameConfig* cfg, int n_parameters_toread) {
    char key[50];
    float value;
    for (int i = 0; i < n_parameters_toread; i++) {
        if (fscanf(file, "%s %f", key, &value) != EOF) {
            if (strcmp(key, "~~~STARS~~~") == 0) {
                load_config_stars(file, cfg, (int)value);
            }
            else if (strcmp(key, "~~~SWALLOW~~~") == 0) {
                load_config_swallow(file, cfg, (int)value);
            } 
            else if (strcmp(key, "~~~HUNTERS~~~") == 0) {
                load_config_hunters(file, cfg, (int)value);
            }
        }
    }


}



void load_config_stars(FILE* file, gameConfig* cfg, int n) {
    char key[50];
    float value;
    for (int i = 0; i < n; i++) {
        if (fscanf(file, "%s %f", key, &value) != EOF) {
            if (strcmp(key, "SPAWN_RATE_STARS") == 0) cfg->spawn_probability_stars = static_cast<float>(value);
        }
    }
}



void load_config_swallow(FILE* file, gameConfig* cfg, int n) {
    char key[50];
    float value;
    for (int i = 0; i < n; i++) {
        if (fscanf(file, "%s %f", key, &value) != EOF) {
            if (strcmp(key, "MIN_SPEED") == 0) cfg->swallow_speed_min = static_cast<float>(value);
            else if (strcmp(key, "MAX_SPEED") == 0) cfg->swallow_speed_max = static_cast<float>(value);
        }
    }
    cfg->swallow_speed_diff = (cfg->swallow_speed_max - cfg->swallow_speed_min) / (NUMBER_SPEED_LEVELS-1);
}



void load_config_hunters(FILE* file, gameConfig* cfg, int n) {
    char key[50];
    float value;
    for (int i = 0; i < n; i++) {
        if (fscanf(file, "%s %f", key, &value) != EOF) {
            if (strcmp(key, "SPAWN_RATE_HUNTERS") == 0) {
                cfg->spawn_probability_hunters = static_cast<float>(value);
            }
            else if (strcmp(key, "MAX_HUNTERS") == 0) {
                cfg->max_hunters = (int)value;
                cfg->current_max_hunters = cfg->max_hunters;
            }
            else if (strcmp(key, "ESCALATION_TIME") == 0) cfg->escalation_interval_seconds = (int)value;
            else if (strcmp(key, "ESCALATION_HUNTERS") == 0) cfg->escalation_max_hunters = (int)value;
            else if (strcmp(key, "ESCALATION_BOUNCES") == 0) cfg->escalation_hunters_bounces = (int)value;

            else if (strcmp(key, "TYPE") == 0) {
                int type = (int)value; // Typ 0, 1 lub 2 (H_STANDARD, H_BIG, H_FAST)

                for (int j = 0; j < 8; j++) {
                    if (fscanf(file, "%s %f", key, &value) != EOF) {
                        if (strcmp(key, "ALLOWED") == 0)
                            cfg->hunter_types[type].type_allowed = (int)value;
                        else if (strcmp(key, "SPEED") == 0)
                            cfg->hunter_types[type].speed = static_cast<float>(value);
                        else if (strcmp(key, "INITIAL_BOUNCES") == 0)
                            cfg->hunter_types[type].hunter_bounces = (int)value;
                        else if (strcmp(key, "DAMAGE") == 0) 
                            cfg->hunter_types[type].damage = (int)value;
                        else if (strcmp(key, "DASH_SPEED") == 0) 
                            cfg->hunter_types[type].dash_speed = value;
                        else if (strcmp(key, "WIDTH") == 0)
                            cfg->hunter_types[type].size.width = (int)value;
                        else if (strcmp(key, "HEIGHT") == 0)
                            cfg->hunter_types[type].size.height = (int)value;
                        else if (strcmp(key, "COLOR") == 0)
                            cfg->hunter_types[type].color_pair = (int)value;
                    }
                }
            }
        }
    }
}




//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//RANKING writing

void ranking_save_score_right_place(const char name[], int score) {
    rankingEntry entries[MAX_ENTRIES];
    int count = 0;

    ranking_read_scores(entries, &count);

    ranking_find_place_for_score(entries, count, name, score);

    count++;

    ranking_put_score_in_file(entries, count);
    
}



void ranking_read_scores(rankingEntry entries[], int* count) {
    FILE* file = fopen(RANKING_FILE, "r");

    if (file) {
        while (*count < MAX_ENTRIES - 1
            && fscanf(file, "%s %d", entries[*count].name, &entries[*count].score) != EOF) {
            (*count)++;
        }
        fclose(file);

    }

}



void ranking_find_place_for_score(rankingEntry entries[], int count, const char name[], int score) {

    int position = 0;

    while (position < count && entries[position].score >= score) {
        position++;
    }

    
    for (int i = count; i > position; i--) { //all lower scores shifted
        entries[i] = entries[i - 1];
    }

    
    strncpy(entries[position].name, name, 29);

    entries[position].score = score;

}



void ranking_put_score_in_file (rankingEntry entries[], int count) {
    FILE* file = fopen(RANKING_FILE, "w");
    if (file) {
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s %d\n", entries[i].name, entries[i].score);
        }
        fclose(file);
    }

}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//RANKING reading

void ranking_display_top_scores(const gameContext* ctx, int display_limit) {
    rankingEntry entries[MAX_ENTRIES];

    int scores_in_ranking_count = 0; 
    int scores_to_display;

    ranking_read_scores(entries, &scores_in_ranking_count);

    
    if (scores_in_ranking_count < display_limit) scores_to_display = scores_in_ranking_count;

    else scores_to_display = display_limit;

    draw_ranking_screen(entries, scores_to_display, ctx, display_limit);

    
}



void display_final_screen(const swallow* player, const gameContext* ctx) {
    
    draw_final_screen(player, ctx);

 
    nodelay(stdscr, FALSE);
   
    
    
    int key;
    do {
        key = getch();
    } while (key != QUIT_BUTTON && key != RANKING_BUTTON);

    if (key == RANKING_BUTTON) {
        ranking_display_top_scores(ctx, TOP_N);
    }
    else {
        return;
    }

    flushinp(); // cleaning bufor

    nodelay(stdscr, TRUE);
    
}
