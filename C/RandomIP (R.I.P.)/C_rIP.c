#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char **argv)
{
	FILE    *file_out;
	char     string_ip_count[4096];
	char    *string_end;
	uint64_t ip_count;

	srand(time(0));

	file_out = fopen("C_rIP_out.txt", "wb");

	if (file_out == NULL) {
		return 1;
	}

	fputs("Number of IP addresses: ", stdout);

	if (argc < 2) {
		fgets(string_ip_count, 4096, stdin);  // NOTE: Lovely memory leak right here
	} else {
		strncpy(string_ip_count, argv[1], 4095);

		puts(string_ip_count);
	}

	ip_count = strtoull(string_ip_count, &string_end, 10);  // TODO: Validate the user input if you don't want your CPU to catch fire

	for (uint64_t i = 0; i < ip_count; i++) {
		fprintf(file_out, "%d.%d.%d.%d\n",
			rand() % 0xFF,
			rand() % 0xFF,
			rand() % 0xFF,
			rand() % 0xFF);
	}

	fclose(file_out);

	return 0;
}