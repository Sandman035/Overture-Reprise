#include "core/ecs.h"
#include "core/systems.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int wins;
    int losses;
    int display;
} score_t;

REGISTER_COMPONENT(score_t);

void setup_game() {
    unsigned long player_id = add_ent();

    score_t score;
    score.wins = 0;
    score.losses = 0;
    score.display = 1;

    void* score_ptr = &score;

    add_score_t(player_id, score_ptr);

    srand(time(NULL));
}

REGISTER_SYSTEM(setup_game, SETUP);

extern int should_exit;

void update_game() {
    signature_t filter = id_to_sig(score_t_id);
    entity_t* list = filter_entities(filter);

    entity_t* ent = list;
    while (*ent != NULL) {
        score_t* score = get_comp_from_ent(*ent, score_t_id);

        int n1 = rand() % 100;
        int n2 = rand() % 100;

        printf("Please answer the following:\n");
        printf("%d + %d = ", n1, n2);
        
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

        ent++;
    }

    free(list);
    list = NULL;
}

REGISTER_SYSTEM(update_game, UPDATE);

void print_scores() {
    signature_t filter = id_to_sig(score_t_id);
    entity_t* list = filter_entities(filter);

    entity_t* ent = list;
    while (*ent != NULL) {
        score_t* score = get_comp_from_ent(*ent, score_t_id);
        if (score->display) {
            printf("wins: %d, losses: %d\n", score->wins, score->losses);
        }
        ent++;
    }

    free(list);
    list = NULL;
}

REGISTER_SYSTEM(print_scores, POST_UPDATE);
