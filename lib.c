#include"lib.h"

const char all_suits[4] = {'c', 'd', 'h', 's'};


// There are four suits in a deck of cards.
// There is 13 ranks in every suit.
// For simplicity and the ability to store the rank as an int: J => 11, Q => 12, K => 13, A => 14
void create_deck(Card **deck) {
    *deck = malloc(sizeof(Card) * SUIT_COUNT * RANK_COUNT);
    for (int rank = 0; rank < RANK_COUNT; rank++) {
        for (int suits = 0; suits < SUIT_COUNT; suits++) {
            (*deck)[suits * RANK_COUNT + rank] = (Card){.rank = rank + 2, .suit = all_suits[suits]};
        }
    }
}


void print_cards(Card *deck) {
    for (int i = 0; i < SUIT_COUNT * RANK_COUNT; i++) {
        printf("rank: %d, suit: %c\n", deck[i].rank, deck[i].suit);
    }
}
