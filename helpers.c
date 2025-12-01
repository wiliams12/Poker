#include"helpers.h"

void swap(CardPtr a, CardPtr b) { 
    Card temp = *a; 
    *a = *b; 
    *b = temp; 
}

// SUIT_COUNT * RANK_COUNT for ful deck
void print_cards(CardPtr cards, int size) {
    for (int i = 0; i < size; i++) {
        printf("rank: %d, suit: %c\n", cards[i].rank, cards[i].suit);
    }
}

// SUIT_COUNT * RANK_COUNT for ful deck
void shuffle(CardPtr cards, int size) {
    // size - 2 because when reaching this index, there will be only one index left, thus nothing random to do
    for (int i = 0; i < size - 2; i++) {
        int j = rand() % (size - i) + i;
        swap(&cards[i], &cards[j]);
    }
}

void print_game_state(GameStatePtr game_state, PlayerPtr player) {
    printf("===================================================================\n\n");
    printf("Player to move                       %d\n", player->player_num + 1);
    printf("Chips: %d\n", player->bank);
    if (player->player_num == game_state->dealer_index) {
        printf("Dealer\n");
    } else if (player->player_num == game_state->small_blind_index) {
        printf("Small blind\n");
    } else if (player->player_num == game_state->big_blind_index) {
        printf("Big blind\n");
    }
    printf("Current bet: %d                       player's bet: %d\n\n", game_state->bet, player->bet );
    if (game_state->card_num > 0) {
        printf("Community cards:\n");
        print_cards(game_state->cards, game_state->card_num);
        printf("\n");
    }
    if (player->hand != NULL) {
        printf("Player's hand:\n");
        print_cards(player->hand, 2);
        printf("\n");
    }
    printf("make a move: \n");
}

void clean_input(char *input) {
    if (!input) {
        return;
    }

    input[strcspn(input, "\n")] = '\0';

    char *start = input;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }

    *end = '\0';

    memmove(input, start, end - start + 1);

    for (char *p = input; *p; p++) {
        *p = tolower((unsigned char)*p);
    }
}
void draw_community(GameStatePtr game_state, CardPtr deck) {
    Card *tmp = draw_cards(deck, &game_state->drawn_cards, 1);
    game_state->cards[game_state->card_num++] = *tmp;
    free(tmp);
}