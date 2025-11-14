#ifndef LIB_H

#include<stdlib.h>
#include<stdio.h>
#define SUIT_COUNT 4
#define RANK_COUNT 13

#endif


typedef struct {
    int rank;
    int suit;
} Card;

void create_deck(Card **deck);
void print_cards(Card *deck);