/* rps.c
 *
 * Copyright (c) 2024 RandomGuyWithoutY
 *
 * Description:
 *     A minimalistic implementation of the good old
 *     game "Rock, Paper, Scissors" in C.
 *
 * Should compile with about anything, including your toaster.
 *
 * This file is licensed under the Unlicense and is
 * thus dedicated to the public domain.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#define RPS_EOF     -2
#define RPS_INVALID -1

#define RPS_TIE    0
#define RPS_WIN_Y  1
#define RPS_WIN_Z  2

#define RPS_ROCK      0
#define RPS_PAPER     1
#define RPS_SCISSORS  2


const char *RPS_STRING[] = {
    "Rock",
    "Paper",
    "Scissors",
};


int rps_algo(unsigned int player_y, unsigned int player_z)
{
    if (player_y >= 3 || player_z >= 3) {
        return RPS_INVALID;
    }

    if (player_y == player_z) {
        return RPS_TIE;
    } else if (player_y == (player_z + 1) % 3) {
        return RPS_WIN_Y;
    } else {
        return RPS_WIN_Z;
    }
}

int rps_chr2int(char chr)
{
    switch (chr) {
        case 'r':
        case 'R': return RPS_ROCK;
        case 'p':
        case 'P': return RPS_PAPER;
        case 's':
        case 'S': return RPS_SCISSORS;
    }

    return RPS_INVALID;
}

int rps_bad_rand(void)
{
    return rand() % 3;
}

int rps_user_input(void)
{
    int input;
    int chr = getchar();

    input = rps_chr2int(chr);

    while (chr != '\n' && chr != EOF) {
        chr = getchar();
    }

    return chr == EOF ? RPS_EOF : input;
}

int main(void)
{
    int player_y;
    int player_z;

    srand(time(NULL));

    while (1) {
        printf("Rock, Paper, Scissors? ");

        player_y = rps_user_input();
        player_z = rps_bad_rand();

        if (player_y < 0) {
            printf("Illegal move. Interpreting input as quit instruction.\n");

            break;
        }

        printf(
            "You play %s.\n"
            "CPU plays %s.\n",
            RPS_STRING[player_y],
            RPS_STRING[player_z]
        );

        switch (rps_algo(player_y, player_z)) {
            case RPS_TIE:
                printf("Tie.\n");
                break;
            case RPS_WIN_Y:
                printf("You win.\n");
                break;
            case RPS_WIN_Z:
                printf("CPU wins.\n");
                break;
        }
    }

    printf("Goodbye!\n");

    return 0;
}
