/* num-guess.c
 *
 * Copyright (c) 2024 RandomGuyWithoutY
 *
 * Description:
 *     I was bored.
 *
 * Should compile with about anything, including your dishwasher.
 *
 * This file is licensed under the Unlicense and is
 * thus dedicated to the public domain.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>


#define ull  unsigned long long


ull nguess_bad_rand(ull min, ull max)
{
	ull raw_num;
	unsigned *num_bytes = (unsigned *)&raw_num;
	ull delta;

	for (int i = 0; i < sizeof(raw_num); i++) {
		num_bytes[i] = rand();
	}

	if (min > max) {
		ull old = min;

		min = max;
		max = old;
	}

	delta = max - min;

	return min + (raw_num % delta);  /* NOTE: Lovely segfault because division by zero bitches */
}

ull nguess_user_num(const char *prompt)
{
	ull num;
	char     usr_buff[4096];

	printf("%s ", prompt);
	
	fgets(usr_buff, sizeof(usr_buff), stdin);  /* NOTE: Lovely memory leak right here */

	num = strtoull(usr_buff, NULL, 10);  /* TODO: Validate the user input if you don't want your CPU to catch fire */
	
	return num;
}

/* NOTE: idfk what this function was supposed to be for */
/*int nguess_user_question(const char *prompt, const char *choices)
{
	int input;
	int chr = getchar();

	printf("%s ", prompt);

	input = 0;

	while (chr != '\n' && chr != EOF) {
		chr = getchar();
	}

	return chr == EOF ? RPS_EOF : input;
}*/

int main(void)
{
	ull min = nguess_user_num("Lower bound: ");
	ull max = nguess_user_num("Upper bound: ");
	ull secret;
	ull guess;
	int false_guesses = 0;

	srand(time(NULL));

	printf("The CPU is coming up with a new number...\n");

	secret = nguess_bad_rand(min, max);

	printf(
		"The CPU found a number. As it happens, it is the same as your Social Security number.\n"
		"Start guessing! No pressure!\n"
	);

	while ((guess = nguess_user_num("Your guess: ")) != secret) {
		if (guess < secret) {
			printf("Too low. Unlucky.\n");
		} else {
			printf("Bro are you high? Your guess sure as shit is...\n");
		}
		false_guesses++;
	}

	if (false_guesses) {
		printf(
			"Would be kinda weird had you actually forgotten your social security number...\n"
			"Took you a good %d guesses. Go check yourself for dementia, maybe?\n",
			false_guesses
		);
	} else {
		printf(
			"Thanks for confirming your Social Security number. You are now signed up to https://hot.milfs.com/in-your-area/\n"
			"Enjoy your stay, you ain't getting out of this shit anytime soon!\n"
		);
	}

	return 0;
}

