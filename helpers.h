#ifndef HELPERS_H
#define HELPERS_H
#include"lib.h"
void swap(CardPtr a, CardPtr b);
void clean_input(char *input);
void print_cards(CardPtr cards, int size);
void shuffle(CardPtr cards, int size);
void draw_community(GameStatePtr game_state, CardPtr deck);
void print_game_state(GameStatePtr game_state, PlayerPtr player);
#endif
