#include"lib.h"

int main(void) {
    Card *deck = NULL;
    create_deck(&deck);
    print_cards(deck);
}