#include "overture/core.h"
#include <overture/overture.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int wins;
    int losses;
    int display;
} score_t;

REGISTER_COMPONENT(score_t);

typedef struct {
    char name[30];
} player_name_t;

REGISTER_COMPONENT(player_name_t);

typedef struct {} player_t;

REGISTER_COMPONENT(player_t);

void setup_game() {
    while(1) {
        printf("Please enter player name.\n");

        player_name_t player_name;
        scanf("%s", player_name.name);

        entity_t player = create_entity();

        add_player_name_t(player, &player_name);

        score_t score;
        score.wins = 0;
        score.losses = 0;
        score.display = 1;

        add_score_t(player, &score);

        ADD_COMPONENT_EMPTY(player_t, player);

        printf("Would you like to add a new player? [y/n]\n");

        char add_player;
        scanf("%s", &add_player);

        if (add_player == 'y') {
            continue;
        }

        break;
    }

    player_name_t player_name;
    strcpy(player_name.name, "I have no score!!!");

    entity_t player = create_entity();

    add_player_name_t(player, &player_name);
    ADD_COMPONENT_EMPTY(player_t, player);

    srand(time(NULL));
}

REGISTER_SYSTEM(setup_game, SETUP);

extern int should_exit;

void update_game() {
    entity_t* list = FILTER_ENTITIES(score_t, player_name_t, player_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        score_t* score = get_comp(list[i], GET_ID(score_t));
        player_name_t* name = get_comp(list[i], GET_ID(player_name_t));

        int n1 = rand() % 100;
        int n2 = rand() % 100;

        printf("%s: %d + %d = ", name->name, n1, n2);
        
        int answer;
        scanf("%d", &answer);

        if (answer == n1 + n2) {
            printf("Correct!\n");
            score->wins++;
        } else {
            printf("wtf man... no\n");
            score->losses++;
        }

        if (score->wins < score->losses) {
            printf("dude this is simple, how tf are you so bad? I'm done with u...\n");
            score->display = 0;
            should_exit = 1;
        }
    }

    list = FILTER_ENTITIES_EXCLUDING((player_name_t, player_t), (score_t));

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        player_name_t* name = get_comp(list[i], GET_ID(player_name_t));

        printf("I AM NO SCORE: %s.\n", name->name);
    }

    free(list);
}

REGISTER_SYSTEM(update_game, UPDATE);

void print_scores() {
    entity_t* list = FILTER_ENTITIES(score_t);

    for (uint64_t i = 0; list[i] != ENTITY_INVALID; i++) {
        score_t* score = get_comp(list[i], score_t_id);
        if (score->display) {
            printf("wins: %d, losses: %d\n", score->wins, score->losses);
        }
    }

    free(list);
}

REGISTER_SYSTEM(print_scores, POST_UPDATE);
