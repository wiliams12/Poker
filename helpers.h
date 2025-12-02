#ifndef HELPERS_H
#define HELPERS_H
#include"lib.h"
void swap(int *a, int *b);
void clean_input(char *input);
void print_cards(int *cards, int size);
void shuffle(int *cards, int size);
void draw_community(GameStatePtr game_state, int *deck);
void print_game_state(GameStatePtr game_state, PlayerPtr player);
#endif
