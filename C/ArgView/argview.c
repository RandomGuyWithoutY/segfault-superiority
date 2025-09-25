#include <stdio.h>
#ifdef _WIN32
#include <conio.h>
#endif

int main(int argc, const char **argv)
{
	for (int cur = 0; cur < argc; cur++) {
		printf("argv[%d] = \"%s\"\n", cur, argv[cur]);
	}

	#ifdef _WIN32
	_getch();
	#endif

	return 0;
}

