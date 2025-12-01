#include"lib.h"

int main(void) {
    Card *deck = NULL;
    init_random();
    create_deck(&deck);
    shuffle(deck, SUIT_COUNT * RANK_COUNT);
    game_loop(deck);
}