// swallow.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "dataTypes.h"
#include "drawOutput.h"
#include "initSetCleanup.h"
#include "IO.h"
#include "updateActors.h"
#include "collisions_hMovement.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------


// MAIN

int main() {

    srand((unsigned int)time(NULL));

   // gameConfig cfg;
    gameConfig cfg = { 0 };
    gameContext context;
    swallow player;
    star stars[LIMIT_STARS];
    hunter hunters[LIMIT_HUNTERS];



    init_game(&player, stars, hunters, &context, &cfg);
    
    
    game_running(&player, hunters, stars, &context, &cfg);


    ranking_save_score_right_place(player.name, player.score);


    display_final_screen(&player, &context);


    cleanup_game();

	
        

    return 0;


}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------






