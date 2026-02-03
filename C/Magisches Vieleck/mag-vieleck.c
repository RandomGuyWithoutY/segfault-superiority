/* https://www.teacheconomy.de/media/unterrichtsmaterial/magische-vieleck-interaktiv/interaktiv/index.html */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <locale.h>

#define FIELD_COUNT  16
#define MATRIX_COUNT 6
#define MAX_PATH     10

#ifdef DEBUG
	#define PRINT_DEBUG printf("Passed by %d, matrix_ptr = %d\n", __LINE__, matrix_ptr)
#else
	#define PRINT_DEBUG
#endif

typedef int mxint_t;

const char *field_names[FIELD_COUNT] = {
	"Erhöhung der Exportzölle",
	"Aufhebung der Exportzölle",
	"Kombilohn",
	"Kaufprämie für Autos",
	"Meereszugang",
	"Zusatzsteuer auf fossile Energieträger",
	"Senkung des Leitzinses",
	"Erhöhung des Leitzinses",
	"Erhöhung des Spitzensteuersatzes",
	"Flugverbote",
	"Senkung von Einfuhrzöllen",
	"Lohnuntergrenze",
	"Preisobergrenzen",
	"Abschaffung der Unternehmenssteuern",
	"Energetische Gebäudesanierung",
	"Breitbandausbau",
};

const mxint_t field_matrixes[FIELD_COUNT][MATRIX_COUNT] = {
	{-1, -1, +1,  0,  0,  0},
	{+1, +1, -1,  0,  0,  0},
	{+1, +1,  0, -1,  0, +1},
	{+1, +1,  0,  0, -1,  0},
	{+1, +1,  0,  0, -1,  0},
	{-1, -1, +1,  0, +1,  0},
	{+1, +1,  0, -1,  0,  0},
	{-1, -1,  0, +1,  0,  0},
	{ 0,  0,  0, -1,  0, +1},
	{-1, -1,  0,  0, +1,  0},
	{-1, -1, +1,  0,  0,  0},
	{-1, -1,  0, -1,  0, +1},
	{-1, -1,  0, +1, +1,  0},
	{+1, +1,  0,  0,  0, -1},
	{+1, +1,  0,  0, +1,  0},
	{+1, +1,  0,  0,  0,  0},
};

const mxint_t initial_matrix[MATRIX_COUNT] = {2, 2, 1, 2, 1, 2};

typedef struct {
	mxint_t matrix_ptr;
	mxint_t matrix_path[MAX_PATH];
	mxint_t matrix_path_len;
} StackItem;

mxint_t *add_matrix(mxint_t *matrix_1, const mxint_t *matrix_2)
{
	int sum;

	for (int i = 0; i < MATRIX_COUNT; i++) {
		sum = matrix_1[i] + matrix_2[i];

		matrix_1[i] = (sum < 0) ? 0 :
		              (sum > 4) ? 4 :
		              sum;
	}

	return matrix_1;
}

mxint_t *get_matrix(mxint_t *matrix_path, mxint_t *first_matrix)
{
	for (int i = 0; i < MAX_PATH; i++) {
		add_matrix(first_matrix, field_matrixes[matrix_path[i]]);
	}

	return first_matrix;
}

bool matrix_gt(mxint_t *matrix_1, mxint_t *matrix_2)
{
	int matrix_1_sum = 0;
	int matrix_2_sum = 0;

	for (int i = 0; i < MATRIX_COUNT; i++) {
		matrix_1_sum += matrix_1[i];
		matrix_2_sum += matrix_2[i];
	}

	return matrix_1_sum > matrix_2_sum;
}

bool path_has(mxint_t *matrix_path, int matrix_path_len, int val)
{
	for (int i = 0; i < matrix_path_len; i++) {
		if (matrix_path[i] == val) {
			return true;
		}
	}

	return false;
}

void print_matrix(mxint_t *matrix_path, mxint_t *matrix)
{
	printf("[");

	for (int i = 0; i < MAX_PATH; i++) {
		printf(
		#ifdef NUMERIC_PATH
			"%d%s",
			matrix_path[i],
		#else
			"%s%s",
			field_names[matrix_path[i]],
		#endif
			(i != MAX_PATH - 1) ? " >> " : "] -x- ("
		);
	}

	for (int i = 0; i < MATRIX_COUNT; i++) {
		printf("%d%s", matrix[i], (i != MATRIX_COUNT - 1) ? ", " : ")\n");
	}
}

int main(void)
{
	mxint_t matrix_ptr = 0;
	mxint_t matrix_path[MAX_PATH] = {0};
	int     matrix_path_len = 0;

	mxint_t matrix_current[MATRIX_COUNT];
	mxint_t matrix_found[MATRIX_COUNT] = {0, 0, 0, 0, 0, 0};
	mxint_t matrix_found_path[MAX_PATH] = {0};

	StackItem current_stck_item;

	struct {
		int       stack_ptr;
		StackItem stack_items[MAX_PATH];
	} result_stack = {0};

	setlocale(LC_ALL, ".UTF-8");

	printf(
		"Magisches Vieleck: High Efficiency Bruteforce Solver v1.0\n"
		"Copyright (c) 2023, DIE WELTWIRTSCHAFT Ltd.\n"
		"Alle Rechte vorbehalten im Namen des Staates Deutschland. Nutzung Gebührenpflichtig (1€ pro Minute).\n"
		"\n"
	);

	do {
		PRINT_DEBUG;

		if (path_has(matrix_path, matrix_path_len, matrix_ptr)) {
			matrix_ptr++;
		} else if (matrix_ptr >= FIELD_COUNT) {
			current_stck_item = result_stack.stack_items[--result_stack.stack_ptr];

			matrix_ptr = current_stck_item.matrix_ptr;
			memcpy(matrix_path, current_stck_item.matrix_path, current_stck_item.matrix_path_len);
			matrix_path_len = current_stck_item.matrix_path_len;
		} else if (matrix_path_len < MAX_PATH) {
			current_stck_item.matrix_ptr = matrix_ptr + 1;
			memcpy(current_stck_item.matrix_path, matrix_path, matrix_path_len);
			current_stck_item.matrix_path_len = matrix_path_len;

			result_stack.stack_items[result_stack.stack_ptr++] = current_stck_item;

			matrix_path[matrix_path_len++] = matrix_ptr;

			matrix_ptr = 0;
		} else {
			memcpy(matrix_current, initial_matrix, sizeof(*matrix_current) * MATRIX_COUNT);

			get_matrix(matrix_path, matrix_current);

			if (matrix_gt(matrix_current, matrix_found)) {
				PRINT_DEBUG;

				memcpy(matrix_found, matrix_current, sizeof(*matrix_found) * MATRIX_COUNT);
				memcpy(matrix_found_path, matrix_path, MAX_PATH);

				print_matrix(matrix_path, matrix_current);
			}

			current_stck_item = result_stack.stack_items[--result_stack.stack_ptr];

			matrix_ptr = current_stck_item.matrix_ptr;
			memcpy(matrix_path, current_stck_item.matrix_path, current_stck_item.matrix_path_len);
			matrix_path_len = current_stck_item.matrix_path_len;
		}
	} while (
		result_stack.stack_ptr > 0 ||
		matrix_ptr < FIELD_COUNT
	);
}

