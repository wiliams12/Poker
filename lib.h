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
#define PLAYER_COUNT 3
#define INPUT_SIZE 128

typedef struct {
    int rank;
    int suit;
} Card, *CardPtr;

typedef struct {
    int player_num;
    CardPtr hand;
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
    CardPtr cards;
    int card_num;
    // number of cards drawn over more rounds
    int drawn_cards;
    int num_folded;
    int num_all_in;
} GameState, *GameStatePtr;

typedef enum {
    CHECK, RAISE, CALL, FOLD, ALL_IN, INVALID
} Action;

void create_deck(CardPtr *deck);
void game_loop(CardPtr deck);
void init_random();
Action get_action(char *input, GameStatePtr gamestate);
Card *draw_cards(Card *deck, int *start, int num);

#include"helpers.h"
#endif
