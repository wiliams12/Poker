#ifndef BIT_H
#define BIT_H

#define	STRAIGHT_FLUSH  1
#define	FOUR_OF_A_KIND  2
#define	FULL_HOUSE      3
#define	FLUSH           4
#define	STRAIGHT        5
#define	THREE_OF_A_KIND 6
#define	TWO_PAIR        7
#define	ONE_PAIR        8
#define	HIGH_CARD       9

#define	RANK(x)  ((x >> 8) & 0xF)

static char *value_str[] = {
    "",
    "Straight Flush",
    "Four of a Kind",
    "Full House",
    "Flush",
    "Straight",
    "Three of a Kind",
    "Two Pair",
    "One Pair",
    "High Card"
};

#define CLUB    0x8000
#define DIAMOND 0x4000
#define HEART   0x2000
#define SPADE   0x1000

#define Deuce   2
#define Trey    3
#define Four    4
#define Five    5
#define Six     6
#define Seven   7
#define Eight   8
#define Nine    9
#define Ten     10
#define Jack    11
#define Queen   12
#define King    13
#define Ace     14

#define SUIT_COUNT 4
#define RANK_COUNT 13
#define PLAYER_COUNT 3
#define INPUT_SIZE 128

extern int primes[];

#endif
