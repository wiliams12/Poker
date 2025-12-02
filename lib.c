#include"lib.h"
#include"bit.h"

void create_deck(int *deck) {
    int n = 0, suit = CLUB;
    for (int i = 0; i < 4; i++, suit >>= 1)
        for (int j = 0; j < 13; j++, n++)
            deck[n] = primes[j] | ((2+j) << 8) | suit | (1 << (16+j));
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
                    amount = get_raise_amount();  
                    if (game_state->stage == 0) {
                        if (player->player_num == game_state->small_blind_index) {
                            // doesn't need to take into account the current bet because it always will be 0
                            if (amount < game_state->turn + 1) {
                                continue;
                            }
                        } else if (player->player_num == game_state->big_blind_index) {
                            if (game_state->bet + amount < (game_state->turn + 1) * 2) {
                                continue;
                            }
                        }
                    }
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
                // only big blind, small blind will never be able to call
                if (game_state->stage == 0) {
                    if (player->player_num == game_state->big_blind_index) {
                        if (game_state->bet < (game_state->turn + 1) * 2) {
                            return false;
                        }
                    }
                }
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

void reward_winner(PlayerPtr *players, int num_of_players, int winner_index) {
    printf("==========\nwinner\n==========\n");
    printf("Player: %d\n\n", winner_index + 1);
    int pot_size = 0;
    for (int player = 0; player < num_of_players; player++) {
        pot_size += (*players)[player].bet;
    }
    (*players)[winner_index].bank += pot_size;
}

void reward_winners(PlayerPtr *players, int num_of_players, int *winners, int count) {
    int pot_size = 0;

    for (int i = 0; i < num_of_players; i++) {
        pot_size += (*players)[i].bet;
        (*players)[i].bet = 0;
    }

    // odd chips will disapper (let's make this capitalistic game more communistic)
    int share = pot_size / count;

    for (int i = 0; i < count; i++) {
        (*players)[winners[i]].bank += share;
    }
}


void game_loop(int *deck) {
    Player *players = malloc(sizeof(Player) * PLAYER_COUNT); // TODO make variable based of some settings in game
    int num_of_players = PLAYER_COUNT;
    for (int i = 0; i < num_of_players; i++) {
        players[i] = (Player){.player_num = i, .bank = 2400, .bet=0, .folded=false, .hand = {0}};
    }

    GameState game_state;
    game_state.turn = 0;
    game_state.dealer_index = 0;
    game_state.small_blind_index = 1;
    game_state.big_blind_index = 2;
    game_state.bet = 0;
    game_state.card_num = 0;
    memset(game_state.cards, 0, sizeof(game_state.cards));
    game_state.drawn_cards = 0;
    game_state.num_folded = 0;
    game_state.num_all_in = 0;
    game_state.stage = 0;
    game_state.to_go = game_state.small_blind_index;

    bool running = true;

    while (running) {

        bool betting_round = true;

        for (int round = 0; round < 5; round++) {
            betting_round = true;
            int aggressor = 0;
            while (betting_round) {
                for (int player = game_state.to_go; player < num_of_players + game_state.to_go; player++) {
                    if (players[player % num_of_players].folded) {
                        continue;
                    }
                    if (game_state.num_folded + game_state.num_all_in == num_of_players) {
                        betting_round = false;
                        continue;
                    }
                    print_game_state(&game_state, &players[player % num_of_players]);
                    if (game_state.num_folded == num_of_players - 1) {
                        reward_winner(&players, num_of_players, player % num_of_players);
                        round = 5;
                        betting_round = false;
                        break;
                    }
                    char input[INPUT_SIZE];
                    while (true) {
                        get_input(input);
                        Action action = get_action(input, &game_state);
                        if (game_state.stage == 0) {
                            if ((player % num_of_players == game_state.small_blind_index ||
                                player % num_of_players == game_state.big_blind_index) && action == CHECK) {
                                    printf("invalid move\n");
                                    continue;
                                }
                        }
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
                            break;
                        }
                    }
                    if ((player + 1) % num_of_players == aggressor && game_state.bet == players[(player + 1) % num_of_players].bet) {
                        betting_round = false;
                        game_state.to_go = player + 1;
                        break;
                    }
                }
            }
            switch (game_state.stage) {
                case 0:
                    for (int player = 0; player < num_of_players; player++) {
                        int *tmp = draw_cards(deck, &game_state.drawn_cards, 2);
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
                
                case 4:
                    break;

                default:
                    fprintf(stderr, "invalid stage reached\n");
                    break;
            }


            game_state.stage++;
        }

        int winners[PLAYER_COUNT];

        int winner_count = find_winners(&players, num_of_players, game_state.cards, winners);

        if (winner_count == 1) {
            reward_winner(&players, num_of_players, winners[0]);
        } else {
            reward_winners(&players, num_of_players, winners, winner_count);
        }


        printf("round ended\n");

        game_state.turn++;
        game_state.dealer_index = (++game_state.dealer_index) % num_of_players;
        game_state.small_blind_index = (game_state.dealer_index + 1) % num_of_players;
        game_state.big_blind_index = (game_state.dealer_index + 2) % num_of_players;
        game_state.bet = 0;
        game_state.card_num = 0;
        game_state.num_folded = 0;
        game_state.num_all_in = 0;
        game_state.stage = 0;
        game_state.to_go = game_state.small_blind_index;

        for (int player = 0; player < num_of_players; player++) {
            players[player].folded = false;
            players[player].bet = 0;
        }
    }
}

// ! the result needs to be freed
int *draw_cards(int *deck, int *start, int num) {
    int *drawn = malloc(sizeof(int) * num);
    if (!drawn) {
        fprintf(stderr, "UNable to allocate memory\n");
        return 0;
    }

    for (int i = 0; i < num; i++) {
        drawn[i] = deck[*start + i];
    }


    *start += num;

    return drawn;
}

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

int score_player(PlayerPtr player, int *community_cards) {
    return 0;
    // TODO: do eval
}


int find_winners(PlayerPtr *players, int num_of_players, int community_cards[5], int winners[]) {
    int max_score = -1;
    int winner_count = 0;

    printf("in scoring\n");
    for (int i = 0; i < num_of_players; i++) {
        if ((*players)[i].folded) {
            continue;
        }
        int eval = score_player(players[i], community_cards);  
        printf("Player: %d    Evaluation: %d\n", i + 1, eval);

        // If this player has a strictly better score → reset winners list
        if (eval > max_score) {
            max_score = eval;
            winner_count = 1;
            winners[0] = i;
        }
        // If this player ties with current best → add them
        else if (eval == max_score) {
            winners[winner_count++] = i;
        }
    }

    return winner_count;
}