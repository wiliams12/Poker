#include"lib.h"

int main(int argc, char *argv[]) {
    init_random();
    int deck[RANK_COUNT * SUIT_COUNT];
    create_deck(deck);
    int num_of_players = handle_arguments(argc, argv);
    game_loop(deck, num_of_players);
    return 0;
}