#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#ifndef FSIZE
	#define FSIZE 0x10000000
#endif
#ifndef FSIGNATURE
	#define FSIGNATURE "VALORANT.DAT---"
#endif

#define GIB 1073741824

int main(void)
{
	unsigned char *GameDataBuffer = malloc(FSIZE);
	int FileNumber = 0;
	// int RealFileNumber = 0;
	char FileNameBuf[255];
	char GameDatFileNameBuf[255] = "Valorant\\GAME.dat";

	if (GameDataBuffer == NULL) {
		fprintf(stderr,
			"We can't even allocate memory on your system.\n"
			"What devilish fucking Indian tech guru did you get such a crap PC from in the first place?\n"
			"Truly magnificent, brother, even for our standards...\n"
		);
		exit(1);
	}

	memset(GameDataBuffer, 0xF0, FSIZE);
	memcpy(GameDataBuffer, FSIGNATURE, sizeof(FSIGNATURE) % FSIZE);

	(void)CreateDirectory("Valorant", NULL);

	HANDLE GameDataFile = CreateFile(GameDatFileNameBuf, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	if (GameDataFile != INVALID_HANDLE_VALUE) {
		WriteFile(GameDataFile, GameDataBuffer, FSIZE, NULL, NULL);
	#ifdef DEBUG
		RealFileNumber++;
	#endif
		CloseHandle(GameDataFile);
	} else if (GetLastError() != ERROR_FILE_EXISTS) {
		fprintf(stderr, "Your OS is too shit to create a file, apparently. Install Gentoo.\n");
		exit(1);
	}

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
		">> (0%%) 0GiB"
	);

#ifdef FLINK_MAX
	for (; FileNumber < FLINK_MAX; FileNumber++) {
		sprintf(FileNameBuf, "Valorant\\VALORANT_%08X.dat", FileNumber);
		if (CreateHardLink(FileNameBuf, GameDatFileNameBuf, NULL) == FALSE && GetLastError() == ERROR_TOO_MANY_LINKS) {
			memcpy(GameDatFileNameBuf, FileNameBuf, 255);

			GameDataFile = CreateFile(GameDatFileNameBuf, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

			if (GameDataFile != INVALID_HANDLE_VALUE) {
				/*printf(
					"\n"
					"\n"
					">> Unpacking archive VALORANT_AR%03d:\n"
					"\n",
					++RealFileNumber
				);*/
				WriteFile(GameDataFile, GameDataBuffer, FSIZE, NULL, NULL);
				CloseHandle(GameDataFile);
			} else if (GetLastError() != ERROR_FILE_EXISTS) {
				fprintf(stderr, "Your OS is too shit to create a file, apparently. Install Gentoo.\n");
				exit(1);
			}
		}
		printf(
			"\r>> (%.2f%%) %.3fGiB"
		#ifdef DEBUG
			" [DEBUG: %d Real Files]"
		#endif
			, (double)FileNumber / FLINK_MAX * 100,
			(double)(FileNumber + 1) * FSIZE / GIB
		#ifdef DEBUG
			, RealFileNumber
		#endif
		);
	}
	printf(
		"\r>> (100%%) %.3fGiB       \n"
		"\n"
		">> Finished extracting game data.\n",
		(double)FLINK_MAX * FSIZE / GIB
	);
#else
	while (1) {
		sprintf(FileNameBuf, "Valorant\\VALORANT_%08X.dat", FileNumber++);
		if (CreateHardLink(FileNameBuf, GameDatFileNameBuf, NULL) == FALSE && GetLastError() == ERROR_TOO_MANY_LINKS) {
			memcpy(GameDatFileNameBuf, FileNameBuf, 255);

			GameDataFile = CreateFile(GameDatFileNameBuf, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

			if (GameDataFile != INVALID_HANDLE_VALUE) {
				/*printf(
					"\n"
					"\n"
					">> Unpacking Archive VALORANT_AR%03d:\n"
					"\n",
					++RealFileNumber
				);*/
				WriteFile(GameDataFile, GameDataBuffer, FSIZE, NULL, NULL);
				CloseHandle(GameDataFile);
			} else if (GetLastError() != ERROR_FILE_EXISTS) {
				fprintf(stderr, "Your OS is too shit to create a file, apparently. Install Gentoo.\n");
				exit(1);
			}
		}
		printf(
			"\r>> (%d%%) %.3fGiB"
		#ifdef DEBUG
			" [DEBUG: %d Real Files]"
		#endif
			, FileNumber,
			(double)(FileNumber + 1) * FSIZE / GIB
		#ifdef DEBUG
			, RealFileNumber
		#endif
		);
	}
#endif

	exit(0);
}
