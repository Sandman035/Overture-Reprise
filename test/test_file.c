#include "core/ecs.h"
#include "core/systems.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int wins;
    int losses;
    int display;
} score_t;

typedef struct {
    char name[30];
} player_name_t;

void calculate_scores() {
    entity_t* list = FILTER_ENTITIES(score_t, player_name_t);

    entity_t* ent = list;
    while (*ent != NULL) {
        score_t* score = get_comp_from_ent(*ent, GET_ID(score_t));
        player_name_t* name = get_comp_from_ent(*ent, GET_ID(player_name_t));

        printf("%s score: %d\n", name->name, score->wins - score->losses);

        ent++;
    }

    free(list);
}

REGISTER_SYSTEM(calculate_scores, POST_UPDATE);
