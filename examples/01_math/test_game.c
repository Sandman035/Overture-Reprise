#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "core/ecs.h"
#include "core/serialize_deserialize.h"
#include "core/systems.h"

//typedef struct {
//    int wins;
//    int losses;
//    int display;
//} score_t;
CREATE_SERIALIZABLE_STRUCT(score_t, (int, wins), (int, losses), (int, display));

REGISTER_COMPONENT(score_t);

typedef struct {
    char name[30];
} player_name_t;

REGISTER_COMPONENT(player_name_t);

void setup_game() {
    while(1) {
        printf("Please enter player name.\n");

        player_name_t player_name;
        scanf("%s", player_name.name);

        entity_t* player = create_entity();

        add_player_name_t_cpy(player, &player_name);

        score_t score;
        score.wins = 0;
        score.losses = 0;
        score.display = 1;

        unsigned char buff[score_t_fmt.packed_size];
        serialize(&score_t_fmt, &score, buff);

        score_t score2;
        deserialize(&score_t_fmt, buff, &score2);

        add_score_t_cpy(player, &score2);

        printf("Would you like to add a new player? [y/n]\n");

        char add_player;
        scanf("%s", &add_player);

        if (add_player == 'y') {
            continue;
        }

        break;
    }

    srand(time(NULL));
}

REGISTER_SYSTEM(setup_game, SETUP);

extern int should_exit;

void update_game() {
    entity_t** list = FILTER_ENTITIES(score_t, player_name_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        score_t* score = get_comp(*ent_ptr, GET_ID(score_t));
        player_name_t* name = get_comp(*ent_ptr, GET_ID(player_name_t));

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

        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(update_game, UPDATE);

void print_scores() {
    entity_t** list = FILTER_ENTITIES(score_t);

    entity_t** ent_ptr = list;
    while (*ent_ptr != NULL) {
        score_t* score = get_comp(*ent_ptr, score_t_id);
        if (score->display) {
            printf("wins: %d, losses: %d\n", score->wins, score->losses);
        }
        ent_ptr++;
    }

    free(list);
}

REGISTER_SYSTEM(print_scores, POST_UPDATE);
