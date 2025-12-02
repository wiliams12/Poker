#ifndef LIB_H
#define LIB_H

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<time.h>
#include<ctype.h>
#include<string.h>

#define SUIT_COUNT 4
#define RANK_COUNT 13

typedef struct {
    int player_num;
    int hand[2];
    int bet;
    int bank;
    bool folded;
} Player, *PlayerPtr;

typedef struct {
    int turn;
    int dealer_index;
    int small_blind_index;
    int big_blind_index;
    int bet;
    int cards[5];
    int card_num;
    // number of cards drawn over more rounds
    int drawn_cards;
    int num_folded;
    int num_all_in;
    int stage;
    int to_go;
} GameState, *GameStatePtr;

typedef enum {
    CHECK, RAISE, CALL, FOLD, ALL_IN, INVALID
} Action;

void create_deck(int *deck);
void game_loop(int *deck);
void init_random();
Action get_action(char *input, GameStatePtr gamestate);
int *draw_cards(int *deck, int *start, int num);
int find_winners(PlayerPtr *players, int num_of_winners, int community_cards[5], int winners[]);

#include"helpers.h"
#endif
