#include"lib.h"

const char all_suits[4] = {'c', 'd', 'h', 's'};

// There are four suits in a deck of cards.
// There is 13 ranks in every suit.
// For simplicity and the ability to store the rank as an int: J => 11, Q => 12, K => 13, A => 14
void create_deck(CardPtr *deck) {
    *deck = malloc(sizeof(Card) * SUIT_COUNT * RANK_COUNT);
    for (int rank = 0; rank < RANK_COUNT; rank++) {
        for (int suits = 0; suits < SUIT_COUNT; suits++) {
            (*deck)[suits * RANK_COUNT + rank] = (Card){.rank = rank + 2, .suit = all_suits[suits]};
        }
    }
}

void init_random() {
    srand(time(NULL));
}

int get_raise_amount(void) {
    printf("Raise amount: \n");
    int amount;
    scanf("%d", &amount);

    // discards the rest of the characters
    int c;
    while ((c = getchar()) != '\n');
    return amount;
}

// Checks if the selected action is legal, if it is, it performs it
bool perform_action(Action action, PlayerPtr player, GameStatePtr game_state) {
    if (player->bank != 0) {
        switch (action)
            {
            case CHECK:
                if (player->bet != game_state->bet) {
                    return false;
                }
                break;
            case RAISE:
                int amount = 0;
                while (true) {
                    printf("%d\n", player->bank);
                    amount = get_raise_amount();  
                    if (amount <= player->bank) {
                        break;
                    } 
                }
                if (player->bet < game_state->bet) {
                    int diff = game_state->bet + amount - player->bet;
                    player->bet += diff;
                    player->bank -= diff;
                } else {
                    player->bet += amount;
                    player->bank -= amount;
                }
                game_state->bet += amount;
                break;
            case CALL:
                if (game_state->bet < player->bet) {
                    fprintf(stderr, "internal error: incorrect management of legal actions\n");
                    exit(1);
                }
                if (game_state->bet == player->bet) {
                    return false;
                }
                if (player->bank < game_state->bet - player->bet) {
                    return false;
                }  
                int diff = game_state->bet - player->bet;
                player->bet = game_state->bet;
                player->bank -= diff;
                if (player->bank == 0) {
                    game_state->num_all_in++;
                }
                break;
            case FOLD:
                player->folded = true;  
                game_state->num_folded++;
                break;
            case ALL_IN:
                player->bet += player->bank;
                player->bank = 0;
                if (game_state->bet < player->bet) {
                    game_state->bet = player->bet;  
                }
                game_state->num_all_in++;
                break;
            default:
                printf("invalid\n");
                break;
            }
    }
    return true;
}

void get_input(char *input) {
    // TODO: handle input (text base, later make a UI based variation)
    input = fgets(input, sizeof(char) * INPUT_SIZE, stdin);
}

void game_loop(CardPtr deck) {
    Player *players = malloc(sizeof(Player) * PLAYER_COUNT); // TODO make variable based of some settings in game
    int num_of_players = PLAYER_COUNT;
    for (int i = 0; i < num_of_players; i++) {
        players[i] = (Player){.player_num = i, .bank = 2400, .hand=malloc(sizeof(Card) * 2), .bet=0, .folded=false};
    }

    GameState game_state;
    game_state.turn = 0;
    game_state.dealer_index = 0;
    game_state.small_blind_index = 1;
    game_state.big_blind_index = 2;
    game_state.bet = 0;
    game_state.card_num = 0;
    game_state.cards = malloc(sizeof(Card) * 5);
    game_state.drawn_cards = 0;
    game_state.num_folded = 0;
    game_state.num_all_in = 0;

    bool running = true;

    while (running) {

        bool betting_round = true;
        int stage = 0;

        for (int round = 0; round < 5; round++) {
            betting_round = true;
            int aggressor = 0;
            while (betting_round) {
                for (int player = game_state.small_blind_index; player < num_of_players + game_state.small_blind_index; player++) {
                    if (players[player % num_of_players].folded) {
                        continue;
                    }
                    print_game_state(&game_state, &players[player % num_of_players]);
                    if (game_state.num_folded == num_of_players - 1) {
                        // TODO: declare a winner
                        printf("==========\nwinner\n============\n");
                    }
                    if (game_state.num_folded + game_state.num_all_in == num_of_players) {
                        // ! showdown
                        printf("showdown starts\n");
                    }
                    char input[INPUT_SIZE];
                    while (true) {
                        get_input(input);
                        // ! Implement blinds and money
                        Action action = get_action(input, &game_state);
                        if (action == INVALID) {
                            printf("invalid input\n");
                            continue;
                        } else {
                            bool successful = perform_action(action, &players[player % num_of_players], &game_state);
                            if (!successful) {
                                printf("invalid move\n");
                                continue;
                            }
                            if (action == RAISE) {
                                aggressor = player % num_of_players;
                            }
                            if (player % num_of_players == aggressor && game_state.bet == players[player % num_of_players].bet) {
                                betting_round = false;
                            }
                            break;
                        }
                    }
                }
            }
            switch (stage) {
                case 0:
                    for (int player = 0; player < num_of_players; player++) {
                        Card *tmp = draw_cards(deck, &game_state.drawn_cards, 2);
                        players[player].hand[0] = tmp[0];
                        players[player].hand[1] = tmp[1];
                        free(tmp);
                    }
                    break;

                case 1: {
                    for (int i = 0; i < 3; i++) {
                        draw_community(&game_state, deck);
                    }
                    break;
                }

                case 2: {
                    draw_community(&game_state, deck);
                    break;
                }

                case 3: {
                    draw_community(&game_state, deck);
                    break;
                }

                default:
                    fprintf(stderr, "invalid stage reached\n");
                    break;
            }


            stage++;
        }
        // ! showdown
        printf("round ended\n");

        game_state.turn++;
        game_state.dealer_index = (++game_state.dealer_index) % num_of_players;
        game_state.small_blind_index = (game_state.dealer_index + 1) % num_of_players;
        game_state.big_blind_index = (game_state.dealer_index + 2) % num_of_players;
        game_state.bet = 0;
        game_state.card_num = 0;
        game_state.num_folded = 0;
        game_state.num_all_in = 0;

        for (int player = 0; player < num_of_players; player++) {
            players[player].folded = false;
        }
    }
}

// action behaviour
// proper loop
// scoring system


// ! the result needs to be freed
Card *draw_cards(Card *deck, int *start, int num) {
    Card *drawn = malloc(sizeof(Card) * num);

    for (int i = 0; i < num; i++) {
        drawn[i] = deck[*start + i];
    }


    *start += num;

    return drawn;
}


/*
c - check
k - call
r - raise
*/
Action get_action(char *input, GameStatePtr gamestate) {

    clean_input(input);

    if (strcmp(input, "check") == 0) {
        return CHECK;
    } else if (strcmp(input, "call") == 0) {
        return CALL;
    } else if (strcmp(input, "raise") == 0) {
        return RAISE;
    } else if (strcmp(input, "all in") == 0) {
        return ALL_IN;
    } else if (strcmp(input, "fold") == 0) {
        return FOLD;
    } else {
        return INVALID;
    }
}


void check(GameStatePtr gamestate) {

}

void raise(GameStatePtr gamestate, PlayerPtr player) {

}

void call(GameStatePtr gamestate, PlayerPtr player) {

}

void fold(GameStatePtr gamestate, PlayerPtr player) {

}

Player find_winner(Player *players, Card *community_cards) {

}