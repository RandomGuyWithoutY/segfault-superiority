/* fucking heftig
 *
 * LP: 100
 *
 * 1) Charge (+ 1 Charge) (Full Damage)
 * 2) Block (Normal Attack no Damage, Super Attack does half damage)
 * 3) Attack (1 Charge) (10 Damage)
 * 4) Super Attack (3 Charge) (30 Damage)
 *
 * Gegner: [2] [2] [2] (Anfang der Runde)
 * You: [1] [2] [3] (Anfang der Runde)
 *
 * May those who tried to implement this crapware previously rest in penis.
 * Pyome, I respect your grind. Aäfwaan.
 *
 *
 * [ EDITORS NOTE ]:
 *
 *   Due to extensive use of the N-word throughout the source code, it has been
 *   decided to replace some occurences with REDACTED. This decision was made in
 *   particular due to the racial nature of the insult which could be easily taken
 *   the wrong way. Aside from that, fuck you and kill yourself, I can still insult
 *   your fatass all I want regardless and you are not getting away any time soon.
 *   Fuck bullshit censorship, you are all cunts and I hate you :3
 *        Hearts and kisses,
 *        RGWY
 *
 *
 * Compilation Instructions:
 *
 *    Don't.
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>

#ifdef RACISM
	#error Don't be racist!!

	#define UNRELATED_SYLLABLE_2  "ga"
	#define UNRELATED_SYLLABLE_1  "Nig"
#else
	#define UNRELATED_SYLLABLE_2  "ski"
	#define UNRELATED_SYLLABLE_1  "bro"
#endif


#define getch  _getch


#define TURN_CHARGE  0x00
#define TURN_BLOCK   0x01
#define TURN_ATTCK   0x02
#define TURN_SATTCK  0x03
#define TURN_INVAL   0xFF

#define SET_TURN(t, n)  (t << (2 * n))
#define GET_TURN(t, n)  ((t >> (2 * n)) & 0x03)


typedef struct {
	int      life;
	unsigned charge;
	unsigned turns;
} Character;


static const char *STRING_TURNS[] = {
	[TURN_CHARGE] = "Charge",
	[TURN_BLOCK]  = "Block",
	[TURN_ATTCK]  = "Attack",
	[TURN_SATTCK] = "Super Attack",
};

void cbt_play_turns(Character **nigREDACTEDs)
{
	static const struct {
		unsigned charge_cost;
		unsigned dmg;
		unsigned blocked_dmg;
	} ATTCK_LEVELS[] = {
		{1, 10, 0},
		{3, 30, 15},
	};

	/* NOTE: play out the individual rounds */
	for (int i = 0; i < 3; i++) {
		/* NOTE: calculate the effects of the respective turns of both players */
		for (int j = 0; j < 2; j++) {
			unsigned turn = GET_TURN(nigREDACTEDs[j]->turns, i);

			if (turn == TURN_CHARGE) {
				nigREDACTEDs[j]->charge++;

				continue;
			}

			if (turn == TURN_BLOCK) {
				continue;
			}

			if (ATTCK_LEVELS[turn - TURN_ATTCK].charge_cost > nigREDACTEDs[j]->charge) {
				nigREDACTEDs[j]->charge++;
				nigREDACTEDs[j]->turns &= ~SET_TURN(0x03, i);
				nigREDACTEDs[j]->turns |=  SET_TURN(TURN_CHARGE, i);

				continue;
			}

			nigREDACTEDs[j]->charge -= ATTCK_LEVELS[turn - TURN_ATTCK].charge_cost;
			nigREDACTEDs[(j + 1) % 2]->life -=
				(GET_TURN(nigREDACTEDs[(j + 1) % 2]->turns, i) == TURN_BLOCK) ?
				ATTCK_LEVELS[turn - TURN_ATTCK].blocked_dmg :
				ATTCK_LEVELS[turn - TURN_ATTCK].dmg;
		}

		printf(
			"Chrg/%d  Life/%d  :  <%s>  <--->  <%s>  :  Chrg/%d  Life/%d\n",
			nigREDACTEDs[0]->charge,
			nigREDACTEDs[0]->life,
			STRING_TURNS[GET_TURN(nigREDACTEDs[0]->turns, i)],
			STRING_TURNS[GET_TURN(nigREDACTEDs[1]->turns, i)],
			nigREDACTEDs[1]->charge,
			nigREDACTEDs[1]->life
		);
	}
}

void cbt_exit(void)
{
	printf("\nGoodbye!\n");

	exit(0);
}

unsigned cbt_translate_key(char key)
{
	switch (key) {
		case 'c':
		case 'C': return TURN_CHARGE;
		case 'b':
		case 'B': return TURN_BLOCK;
		case 'a':
		case 'A': return TURN_ATTCK;
		case 's':
		case 'S': return TURN_SATTCK;
		case 'q':
		case 'Q': cbt_exit();
		default: return TURN_INVAL;
	}
}

int main(int argc, const char **argv)
{
	Character nigREDACTED_one;
	Character nigREDACTED_two;

	srand(time(NULL));

	fputs(
		"Welcome to Attack-Block-Charge!\n"
		"-------------------------------\n"
		"In this game, you and your opponent select 3 Actions, where each Action can be one of:\n"
		"  [A] : Attack\n"
		"  [B] : Block\n"
		"  [C] : Charge\n"
		"  [S] : Super Attack\n"
		"  [Q] : Quit\n"
		"\n"
		"After selecting 3 Actions, you compare each of your Actions to ones selected by the opponent\n"
		"Depending on the type of the collision, following scenarios may occur:\n"
		"If a player attacks by spending 1 Charge, and the other player does not block, they get 10 damage\n"
		"If a player super attacks by spending 3 Charge, and the other player does not block, they get 30 damage, otherwise only 15\n"
		"If a player charges, they obtain 1 Charge\n"
		"If a player tries to Attack despite having too little Charge, the Action is automatically converted to a Charge\n"  /* NOTE: I am too lazy to implement proper error recovery so I added this extra clause to the rules */
		"Blocking does not exhaust Charge\n"
		"The player who reaches 0 health first looses\n",
		stdout
	);

	NewGame:

	nigREDACTED_one = nigREDACTED_two = (Character){
		.life = 100,
		.charge = 0,
		.turns = 0,
	};

	while (nigREDACTED_one.life > 0 && nigREDACTED_two.life > 0) {
		nigREDACTED_one.turns = 0;

		for (int i = 0; i < 3; i++) {
			unsigned in_chr;

			printf("Turn %d/3: ", i + 1);

			do {
				in_chr = cbt_translate_key(getch());
			} while (in_chr == TURN_INVAL);

			printf("<%s>\n", STRING_TURNS[in_chr]);

			nigREDACTED_one.turns |= SET_TURN(in_chr, i);
		}

		nigREDACTED_two.turns = rand();  /* actual genius (this is why we use bits for everything, gentlemen) */

		cbt_play_turns((Character *[]){&nigREDACTED_one, &nigREDACTED_two});
	}

	if (nigREDACTED_one.life > nigREDACTED_two.life) {
		puts("You win ma " UNRELATED_SYLLABLE_1 UNRELATED_SYLLABLE_2 "!");
	} else if (nigREDACTED_one.life == nigREDACTED_two.life) {
		puts("You got a tie ma " UNRELATED_SYLLABLE_1 UNRELATED_SYLLABLE_2 "!");
	} else {
		puts("You lost ma " UNRELATED_SYLLABLE_1 UNRELATED_SYLLABLE_2 ". Damn.");
	}

	printf("Type [C] to continue or any other key to end the game: ");

	switch (getch()) {
		case 'c':
		case 'C': {
			printf("<Continue>\n");
			goto NewGame;
		}
	}

	printf("<Exit>\n");

	cbt_exit();
}

