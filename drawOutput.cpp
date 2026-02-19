#include "drawOutput.h"





void set_colors() {

    start_color();

    //(text,background)
    init_pair(C_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(C_STATUS_BAR, COLOR_BLACK, COLOR_WHITE);
    init_pair(C_ARENA_BORDERS, COLOR_WHITE, COLOR_BLACK);

    init_pair(C_SWALLOW_HIGH, COLOR_GREEN, COLOR_BLACK);
    init_pair(C_SWALLOW_MEDIUM, COLOR_YELLOW, COLOR_BLACK);
    init_pair(C_SWALLOW_LOW, COLOR_RED, COLOR_BLACK);

    init_pair(C_STAR, COLOR_YELLOW, COLOR_BLACK);

    init_pair(C_HUNTER_1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(C_HUNTER_2, COLOR_RED, COLOR_BLACK);
    init_pair(C_HUNTER_3, COLOR_ORANGE, COLOR_BLACK);

    init_pair(C_TAXI, COLOR_CYAN, COLOR_BLACK);
}



void draw_controls_guide(gameContext* context) {
    int x = context->screen_w / 2 - 18;
    int start_y = context->screen_h / 3 - 2;


    mvprintw(start_y, x, "  CONTROLS  ");



    mvprintw(start_y + 3, x, "w, s, a, d    - Movement");
    mvprintw(start_y + 5, x, "t             - Call albatros taxi");
    mvprintw(start_y + 7, x, "o / p         - Change speed level ( - / + )");
    mvprintw(start_y + 9, x, "q             - Quit game");


    mvprintw(start_y + 13, x, "Press any key to continue...");


    refresh();

    getch();
    clear();
}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


void draw_game(const swallow* player, const star stars[], const hunter hunters[], const gameContext* ctx, const gameConfig* cfg) {
    erase();

    draw_arena_borders(cfg);

    if (ctx->taxi.active) {
        draw_safe_zone_circle(ctx, cfg->safe_zone_size);
    }

    draw_actors(player, stars, hunters, ctx, cfg);

    draw_status_bar(player, ctx, cfg);

    refresh();
}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------




void draw_arena_borders(const gameConfig* cfg) {

    int width = cfg->map.width+1;
    int height = cfg->map.height;

    attron(COLOR_PAIR(C_ARENA_BORDERS));
    //poziome
    //mvhline(0, 0, ACS_HLINE, width);          // top
    mvhline(height , 0, ACS_HLINE, width); // bottom

    //pionowe
    //mvvline(0, 0, ACS_VLINE, height);           // left
    mvvline(0, width - 1, ACS_VLINE, height);   // right

    // narozniki
   // mvaddch(0, 0, ACS_ULCORNER);                    // Upper left
   //mvaddch(0, width-1, ACS_URCORNER);            // Upper right
   //mvaddch(height, 0, ACS_LLCORNER);           // Lower left
    mvaddch(height, width - 1, ACS_LRCORNER);   // Lower right


    attroff(COLOR_PAIR(C_ARENA_BORDERS));
}



void draw_status_bar(const swallow* player, const gameContext* ctx, const gameConfig* cfg) {
    
    
	// status bar - look


    int status_y = ctx->screen_h - STATUS_HEIGHT;

    // line
    mvhline(status_y, 0, ACS_HLINE, ctx->screen_w);

    // background
    attron(COLOR_PAIR(C_STATUS_BAR));
    for (int y = status_y + 1; y < ctx->screen_h; y++) {
        mvhline(y, 0, ' ', ctx->screen_w);
    }

    //status bar - game statistics
    mvprintw(status_y + 1, 2, "PLAYER: %s", player->name);
    mvprintw(status_y + 1, 30, "LIFE FORCE: %d%%", player->life);
    mvprintw(status_y + 1, 60, "SPEED LEVEL: %d/%d", player->speed_lvl, NUMBER_SPEED_LEVELS);
    mvprintw(status_y + 1, 90, "HUNTERS: %d", cfg->current_max_hunters);
    mvprintw(status_y + 2, 2, "LEVEL: %d/%d", ctx->level + 1, NUMBER_LEVELS);
    mvprintw(status_y + 2, 30, "STARS: %d/%d", player->stars_collected, cfg->star_win);
    mvprintw(status_y + 2, 60, "SCORE: %d", player->score);
    mvprintw(status_y + 2, 90, "TIME LEFT: %.1f s", ctx->time_left);
    attroff(COLOR_PAIR(C_STATUS_BAR));
}



void draw_actors(const swallow* player, const star stars[], const hunter hunters[], const gameContext* ctx, const gameConfig* cfg) {
   
    draw_stars(stars, cfg);
    draw_taxi(&ctx->taxi);
    draw_swallow(player);
    draw_hunters(hunters, cfg);
}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


void draw_stars(const star stars[], const gameConfig* cfg) {

    attron(COLOR_PAIR(C_STAR) | A_BOLD);

    for (int i = 0; i < cfg->max_stars; i++) {
        if (stars[i].active) {
            // int for cell
            mvaddch((int)stars[i].pos.y, (int)stars[i].pos.x, '*');
        }
    }

    attroff(COLOR_PAIR(C_STAR) | A_BOLD);
}



void draw_swallow(const swallow* player) {

    int current_swallow_color = C_SWALLOW_HIGH;

    if (player->life <= LOW_LIFE) current_swallow_color = C_SWALLOW_LOW;
    else if (player->life <= MEDIUM_LIFE) current_swallow_color = C_SWALLOW_MEDIUM;

    attron(COLOR_PAIR(current_swallow_color) | A_BOLD);
    for (int y = 0; y < player->size.height; y++) {
        for (int x = 0; x < player->size.width; x++) {

            if (player->shape[y][x] != '\0') {
                mvaddch((int)player->pos.y + y, (int)player->pos.x + x, player->shape[y][x]);
            }
        }
    }
    attroff(COLOR_PAIR(current_swallow_color) | A_BOLD);
}



void draw_hunters(const hunter hunters[], const gameConfig* cfg) {
    for (int i = 0; i < cfg->current_max_hunters; i++) {
        if (hunters[i].active) {

			attron(COLOR_PAIR(hunters[i].color_pair)); //| A_BOLD) - ale są jaśniejsze;

            for (int y = 0; y < hunters[i].size.height; y++) {
                for (int x = 0; x < hunters[i].size.width; x++) {
                    if (hunters[i].shape[y][x] != '\0') {
                        mvaddch((int)hunters[i].pos.y + y, (int)hunters[i].pos.x + x, hunters[i].shape[y][x]);
                    }
                }
            }
            mvprintw((int)hunters[i].pos.y + hunters[i].size.height, (int)hunters[i].pos.x, "%d", hunters[i].bounces_left); // show bounces below hunter

            attroff(COLOR_PAIR(hunters[i].color_pair) | A_BOLD);
        }
    }
}




//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


void draw_taxi(const taxi* t) {
    if (!t->active) return;

    attron(COLOR_PAIR(C_TAXI) | A_BOLD);
    for (int y = 0; y < t->size.height; y++) {
        mvprintw((int)t->pos.y + y, (int)t->pos.x, t->shape[y]);
    }
    attroff(COLOR_PAIR(C_TAXI) | A_BOLD);
}



void draw_safe_zone_circle(const gameContext* ctx, int radius) {

    int centerX = ctx->gameAreaSize.width / 2;
    int centerY = ctx->gameAreaSize.height / 2;

    attron(COLOR_PAIR(C_TAXI));

    for (int y = centerY - radius; y <= centerY + radius; y++) {


        for (int x = centerX - (radius * 2); x <= centerX + (radius * 2); x++) {  //*2 char proportion


            float dx = (float)(x - centerX) / 2.0f;
            float dy = (float)(y - centerY);

            float distance = sqrtf(dx * dx + dy * dy);

            if (distance > radius - 0.5f && distance < radius + 0.5f) { //tylko obwód

                if (y >= 0 && y < ctx->gameAreaSize.height && //mieści w planszy
                    x >= 0 && x < ctx->gameAreaSize.width) {
                    mvaddch(y, x, '.');
                }
            }
        }
    }
    attroff(COLOR_PAIR(C_TAXI));
}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------



void draw_final_screen(const swallow* player, const gameContext* ctx) {
    erase();

    int x = ctx->screen_w / 2;
    int y = ctx->screen_h / 2;

    attron(A_BOLD | COLOR_PAIR(C_DEFAULT));
 
    draw_result(player, ctx, y, x);
    
    draw_result_statistics(player, ctx, y, x);
   
    
    attroff(A_BOLD | COLOR_PAIR(C_DEFAULT));

    

  
    mvprintw(y + 3, x - 15, "Press '%c' to see ranking or '%c' to exit...", RANKING_BUTTON, QUIT_BUTTON);
  

    refresh();



}



void draw_result(const swallow* player, const gameContext* ctx, int y, int x) {

    

    if (ctx->win) {
        mvprintw(y - 6, x - 15, "CONGRATULATIONS! YOU WON!");
    }
    else if (player->life <= 0) {
        mvprintw(y - 6, x - 15, "GAME OVER - YOU LOST LIFE!");
    }
    else if (ctx->time_left <= 0) {
        mvprintw(y - 6, x - 15, "GAME OVER - END OF TIME!");
    }
    else {
        mvprintw(y - 6, x - 15, "GAME TERMINATED");
    }


}



void draw_result_statistics(const swallow* player, const gameContext* ctx, int y, int x) {
    

    mvprintw(y - 2, x - 15, "PLAYER: %s", player->name);
    mvprintw(y - 1, x - 15, "LAST LEVEL: %d", ctx->level + 1);
    mvprintw(y, x - 15, "FINAL SCORE: %d", player->score);

}



void draw_ranking_screen(rankingEntry entries[], int scores_to_display, const gameContext* ctx, int display_limit) {

    erase();

    attron(COLOR_PAIR(C_DEFAULT) | A_BOLD);
    mvprintw(2, ctx->screen_w / 2 - 10, "--- TOP %d RANKING ---", display_limit);
    


    if (scores_to_display == 0) {
        mvprintw(5, ctx->screen_w / 2 - 10, "No scores recorded yet.");
    }
    else {
        for (int i = 0; i < scores_to_display; i++) {
            int y = 5 + i;
            int x = ctx->screen_w / 2 - 15;
            mvprintw(y, x, "%d. %-15s %d", i + 1, entries[i].name, entries[i].score);

        }
    }

    mvprintw(ctx->screen_h - 2, ctx->screen_w / 2 - 12, "Press any key to exit...");

    attroff(COLOR_PAIR(C_DEFAULT) | A_BOLD);

    refresh();


    nodelay(stdscr, FALSE); // Czekaj na gracza
    getch();
    nodelay(stdscr, TRUE);
}


