#include"lib.h"

int main(void) {
    init_random();
    int deck[RANK_COUNT * SUIT_COUNT];
    create_deck(deck);
    shuffle(deck, SUIT_COUNT * RANK_COUNT);
    game_loop(deck);
}