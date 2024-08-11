/* roulette.c
 *
 * Copyright (c) 2023 RandomGuyWithoutY
 *
 *
 * Compilation flags:
 *
 *   -DREAL_SYSTEM32
 *       Enables the real System32 folder as returned by
 *       the machine as deletion target. If this flag is
 *       not set, a dummy folder is used as target.
 *
 *   -DNO_PROGRESS_BAR
 *       Disables the Windows file explorer progress bar.
 *       If not present, the user has no way to abort the
 *       deletion aside from closing the program as a
 *       whole.
 *
 *   -DPRO_EDITION
 *       Enables Pro Edition. In Pro Edition, the target
 *       folder is actually deleted if the player looses.
 *       If not present, the program will instead display
 *       "Trial Version" in the copyright notice and no
 *       folder is deleted, independent of the -DREAL_SYSTEM32
 *       flag.
 *
 * Linker flags:
 *
 *   -lmsvcrt
 *   -Wl,-nodefaultlib:libcmt
 *       Link against the Microsoft C Runtime dynamically instead
 *       of statically.
 *
 *   -lShell32
 *       Required for the folder deletion function.
 *
 * Command line:
 *
 *  $ llvm-rc -DPRO_EDITION roulette.rc
 *
 *  $ clang -Wall
 *          -Wl,-nodefaultlib:libcmt -lmsvcrt -lShell32
 *          [-DREAL_SYSTEM32] [-DNO_PROGRESS_BAR] [-DPRO_EDITION]
 *          roulette.c roulette.res -o "roulette (pro|trial).exe"
 *
 */

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>

char *get_system32(void)
{
	char *SystemPath = malloc(4096);
	if (SystemPath == NULL) {
		fprintf(stderr,
			"Your system sucks so bad we can't even delete System32 due to \"Insufficient Memory\". "
			"Let that sink in for just a second.\n"
		);
		exit(1);
	}
	int StringEnd = GetSystemDirectory(SystemPath, 4094);

	if (StringEnd == 0) {
		exit(1);
	}

	printf(
		"Found System32 at \"%s\"...\n"
		"\n",
		SystemPath
	);

	SystemPath[StringEnd + 1] = 0;

#ifdef REAL_SYSTEM32
	return SystemPath;
#else
	return ".\\DummySystem32\0";
#endif
}

void del_system32(void)
{
	SHFILEOPSTRUCT DirectoryDelOp = (SHFILEOPSTRUCT){0};
	DirectoryDelOp.hwnd   = NULL;
	DirectoryDelOp.wFunc  = FO_DELETE;
	char *System32Path    = get_system32();
	DirectoryDelOp.pFrom  = System32Path;
	DirectoryDelOp.fFlags =
		FOF_NOCONFIRMATION
	#ifdef NO_PROGRESS_BAR
		| FOF_SILENT | FOF_NOERRORUI
	#endif
	;
	SHFileOperation(&DirectoryDelOp);
	RemoveDirectory(System32Path);
}

int main(void)
{
	srand(time(0));
	printf(
		"Russian Roulette: System32 Edition %s\n"
		"Copyright (c) 2000 RookieMistakeSoft LLC\n"
		"Press any key to play.\n"
		"\n",
	#ifdef PRO_EDITION
		"(Pro)"
	#else
		"(Trial Version)"
	#endif
	);

	int RemainingRevolverSlots = 6;

	Iliterallyonlyusegotostopisspeopleoff:
#ifndef SKIP_USERINTERACT
	_getch();
	if (rand() % RemainingRevolverSlots-- == 0) {
#else
	{
#endif
		printf(
			"You are dead motherfucker!\n"
		#ifdef PRO_EDITION
			"As a price, we will delete your System32 folder for you!\n"
		#else
			"Please purchase the Pro version on Steam to get the full death experience!\n"
		#endif
			"\n"
		);
	#ifdef PRO_EDITION
		del_system32();
	#endif
		printf("Press any key to end the game, now that we have ended you.\n");
		_getch();
		return 0;
	}

	printf(
		"You survive.\n"
		"It is your opponents turn. Press any key.\n"
		"\n"
	);

	_getch();

	if (rand() % RemainingRevolverSlots-- == 0) {
		printf(
			"Your motherfucking opponent is dead!\n"
			"Always remember to practice gun safety, kids! Or else you might have the same fate as this poor bastard!\n"
			"(You may keep your System32 folder. For now...)\n"
			"\n"
		);
		printf("Press any key to end the game.\n");
		_getch();
		return 0;
	}

	printf(
		"Your opponent is still alive.\n"
		"Time for you to proof your luck once again. Press any key.\n"
		"\n"
	);

	goto Iliterallyonlyusegotostopisspeopleoff;

	return 0;
}