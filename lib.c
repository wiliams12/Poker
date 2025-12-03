#include"lib.h"
#include"bit.h"

void create_deck(int *deck) {
    int n = 0, suit = CLUB;
    for (int i = 0; i < 4; i++, suit >>= 1) {
        for (int j = 0; j < 13; j++, n++) {
            deck[n] = primes[j] | ((2+j) << 8) | suit | (1 << (16+j));
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
                    amount = get_raise_amount();  
                    if (amount <= player->bank && amount > 0) {
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
                player->all_inned = true;
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
    printf("==========\nplayers tied\n==========\n");
    for (int i = 0; i < count; i++) {
        printf("Player %d\n", (*players)[i].player_num + 1);
    }
    printf("rewards split\n\n");
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

bool is_over(PlayerPtr *players, int num_of_players) {
    int found = 0;
    for (int i = 0; i < num_of_players; i++) {
         if ((*players)[i].bank == 0) {
            found++;
         }
    }
    return (found == num_of_players - 1);
}

int not_folded(Player *players, int num_of_players) {
    for (int i = 0; i < num_of_players; i++) {
        if (!players[i].folded) {
            return i;
        }
    }
    return -1;
}

void blind_bets(PlayerPtr *players, GameStatePtr game_state) {
    int sb_amount = game_state->turn + 1;
    int bb_amount = 2 * (game_state->turn + 1);
    
    // --- 1. SMALL BLIND LOGIC ---
    int *sb_bank = &(*players)[game_state->small_blind_index].bank;
    if (sb_amount <= *sb_bank) {
        (*players)[game_state->small_blind_index].bet = sb_amount;
        *sb_bank -= sb_amount;
    } else {
        (*players)[game_state->small_blind_index].bet = *sb_bank;
        *sb_bank = 0;
        (*players)[game_state->small_blind_index].all_inned = true;
        game_state->num_all_in++;
    }

    // --- 2. BIG BLIND LOGIC ---
    int *bb_bank = &(*players)[game_state->big_blind_index].bank;
    
    if (bb_amount <= *bb_bank) {
        // BB has enough chips to cover the full blind
        (*players)[game_state->big_blind_index].bet = bb_amount;
        *bb_bank -= bb_amount;
    } else {
        // BB is short-stacked and goes All-In
        (*players)[game_state->big_blind_index].bet = *bb_bank;
        *bb_bank = 0;
        (*players)[game_state->big_blind_index].all_inned = true;
        game_state->num_all_in++;
    }

    game_state->bet = (*players)[game_state->big_blind_index].bet;
    game_state->to_go = game_state->big_blind_index + 1; // UTG is next to act
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

    while (!is_over(&players, num_of_players)) {

        bool betting_round = true;
        int winner_by_folds = -1;

        blind_bets(&players, &game_state);

        for (int round = 0; round < 4; round++) {
            betting_round = true;
            int aggressor = 0;

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
                
                default:
                    fprintf(stderr, "invalid stage reached\n");
                    break;
            }

            while (betting_round) {
                for (int player = game_state.to_go; player < num_of_players + game_state.to_go; player++) {
                    if (game_state.num_folded == num_of_players - 1) {
                        betting_round = false;
                        round = 5;
                        winner_by_folds = not_folded(players, num_of_players);
                        break;
                    }

                    if (game_state.num_folded + game_state.num_all_in == num_of_players) {
                        betting_round = false;
                        break;
                    }
                    if (players[player % num_of_players].bank == 0) {
                        if ((player + 1) % num_of_players == aggressor && game_state.bet == players[(player + 1) % num_of_players].bet) {
                            betting_round = false;
                            game_state.to_go = player + 1;
                            break;
                        }
                        continue; 
                    }

                    if (players[player % num_of_players].folded) {
                        continue;
                    }
                    print_game_state(&game_state, &players[player % num_of_players]);

                    char input[INPUT_SIZE];
                    while (true) {
                        get_input(input);
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

            game_state.stage++;
        }

        int winners[PLAYER_COUNT];

        if (winner_by_folds != -1) {
            reward_winner(&players, num_of_players, winner_by_folds);
        } else {
            printf("%d\n", game_state.cards[0]);
            printf("%d\n", game_state.cards[1]);
            printf("%d\n", game_state.cards[2]);
            printf("%d\n", game_state.cards[3]);
            printf("%d\n", game_state.cards[4]);

            printf("%d\n", players[0].hand[0]);
            printf("%d\n", players[0].hand[1]);

            int winner_count = find_winners(&players, num_of_players, game_state.cards, winners);

            if (winner_count == 1) {
                reward_winner(&players, num_of_players, winners[0]);
            } else {
                reward_winners(&players, num_of_players, winners, winner_count);
            }
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
            players[player].all_inned = false;
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

static unsigned short eval_5cards(int c1, int c2, int c3, int c4, int c5) {
    int q = (c1 | c2 | c3 | c4 | c5) >> 16;
    short s;

    // This checks for Flushes and Straight Flushes.
    if (c1 & c2 & c3 & c4 & c5 & 0xf000)
        return flushes[q];

    // This checks for Straights and High Card hands.
    if ((s = unique5[q]))
        return s;

    // This performs a perfect-hash lookup for remaining hands.
    q = (c1 & 0xff) * (c2 & 0xff) * (c3 & 0xff) * (c4 & 0xff) * (c5 & 0xff);
    unsigned short res = hash_values[find_fast(q)];
    return res;
}

unsigned short eval_5hand(int *hand) {
    int c1 = *hand++;
    int c2 = *hand++;
    int c3 = *hand++;
    int c4 = *hand++;
    int c5 = *hand;

    return eval_5cards(c1, c2, c3, c4, c5);
}

unsigned short eval_7hand(int *hand) {
    int subhand[5];
    unsigned short best = 9999;

    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 5; j++)
            subhand[j] = hand[ perm7[i][j] ];
        unsigned short q = eval_5hand(subhand);
        if (q < best)
            best = q;
    }
    return best;
}

int hand_rank(unsigned short val) {
    if (val > 6185) {
        return HIGH_CARD;        // 1277 high card
    }
    if (val > 3325) {
        return ONE_PAIR;         // 2860 one pair
    }
    if (val > 2467) {
        return TWO_PAIR;         //  858 two pair
    }
    if (val > 1609) {
        return THREE_OF_A_KIND;  //  858 three-kind
    }
    if (val > 1599) {
        return STRAIGHT;         //   10 straights
    }
    if (val > 322) {
        return FLUSH;            // 1277 flushes
    }
    if (val > 166) {
        return FULL_HOUSE;       //  156 full house
    }
    if (val > 10) {
        return FOUR_OF_A_KIND;   //  156 four-kind
    }
    if (val > 1) {
        return STRAIGHT_FLUSH;                   //   9 straight-flushes
    }
    return ROYAL_FLUSH; // royal flush
}

static unsigned find_fast(unsigned u) {
    unsigned a, b, r;

    u += 0xe91aaa35;
    u ^= u >> 16;
    u += u << 8;
    u ^= u >> 4;
    b  = (u >> 8) & 0x1ff;
    a  = (u + (u << 2)) >> 19;
    r  = a ^ hash_adjust[b];
    return r;
}


int score_player(PlayerPtr player, int *community_cards) {
    int hand[7] = {
        player->hand[0], player->hand[1], 
        community_cards[0], community_cards[1], 
        community_cards[2], community_cards[3], 
        community_cards[4]
    };
    return eval_7hand(hand);
}


int find_winners(PlayerPtr *players, int num_of_players, int community_cards[5], int winners[]) {
    int max_score = 7463;
    int winner_count = 0;

    for (int i = 0; i < num_of_players; i++) {
        if ((*players)[i].folded) {
            continue;
        }
        int eval = score_player(&(*players)[i], community_cards);  
        printf("Player: %d    %s    Evaluation: %d\n", i + 1, value_str[hand_rank(eval)], eval);
        // If this player has a strictly better score → reset winners list
        if (eval < max_score) {
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