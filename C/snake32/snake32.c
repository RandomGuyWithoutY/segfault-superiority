/* snake32
 *
 *      . ..__ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ ___
 *   ~=-=-=___s__n__a__k__e__=__=__=__2__3__=__=__=__=__=__=__=__o )---<
 *
 *
 * [::::::::::::::] START OF MEMO [::::::::::::::]
 *
 *
 *  1. Have you ever played Metal Gear Solid 2?
 *    ( ) Yes   (x) No
 *
 *  2. What is Pink Lady?
 *    [ ] A brand of apples
 *    [ ] Your fat mother! (xD)
 *    [ ] Yet another synonym for a snatch
 *    [x] Rather not say
 *
 *  3. How do you spell out the number 32?
 *    [ ] thirty-two
 *    [ ] thirty two
 *    [ ] thirtytwo
 *    [ ] 32
 *    [ ] 30-2
 *    [x] I just want to see my wife and children again please this is not a joke I am actually trapped in a large factory building and being forced to answer humoristic forms all day and night without pause please send help ple... (999+ words not displayed)
 *
 *  ( SUBMIT )
 *
 *  Submitting form......
 *
 *  Done.
 *
 *  Our automatic form analysis system has concluded the following based on your
 *  selected answers:
 *
 *                   DIAGNOSED MORRON
 *         NOT FIT FOR THIS GAME IN THE SLIGHTEST
 *             SHOULD CONSIDER URGENT SUICIDE
 *
 *  To get your full results, please pay us a monthly subscription fee of 20.99 USD
 *  and give us your full address and social security number so we can send you everything
 *  by mail. For further questions about terminating your subscription, please write a
 *  message to no-reply@dont.give-a.fuck and wait for our answer.
 *
 *
 * [:::::::::::::::] END OF MEMO [:::::::::::::::]
 *
 * ==MemoDate: 2025-10-07
 * ==FormUrl: http://solid-snake.rocks/default_form.php
 * ==UserName: RGWY
 * ==FormIssuer: Hideo Koji ma balls
 * ==METDATA END
 *
 *
 */

/* .~==~==~==):)-<  <~o~> Headersss: <~o~>  >-(:(==~==~==~. */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <windows.h>

/* .~==~==~==):)-<  <~o~> Macrosss: <~o~>  >-(:(==~==~==~. */

#define S32_GRID_EMPTY    0x0
#define S32_GRID_APPLE    0xf
#define S32_GRID_SNAKE_W  0x1
#define S32_GRID_SNAKE_N  0x2
#define S32_GRID_SNAKE_E  0x3
#define S32_GRID_SNAKE_S  0x4
#define S32_GRID_SNAKE    0x5

#define S32_GRID_GET(grid, off) \
	( \
		( \
			grid[ \
				(off) * 4 / (sizeof(*grid) * CHAR_BIT) \
			] >> ( \
				(off) * 4 % (sizeof(*grid) * CHAR_BIT) \
			) \
		) & 0x0f \
	)
#define S32_GRID_SET(grid, off, val) \
	( \
		grid[ \
			(off) * 4 / (sizeof(*grid) * CHAR_BIT) \
		] = ((unsigned long long)val) << ( \
			(off) * 4 % (sizeof(*grid) * CHAR_BIT) \
		) | (~(0xfULL << ( \
			(off) * 4 % (sizeof(*grid) * CHAR_BIT) \
		)) & grid[ \
			(off) * 4 / (sizeof(*grid) * CHAR_BIT) \
		]) \
	)
#define S32_CELL_GET(cell, off)       ((cell >> ((off) * 4 % (sizeof(*grid) * CHAR_BIT))) & 0x0f)
#define S32_GRID_SKIP(grid, off)      (off = ((off) + (sizeof(*grid) * CHAR_BIT)) / 4)

/* .~==~==~==):)-<  <~o~> Constantsss: <~o~>  >-(:(==~==~==~. */

const COORD s32_directions[] = {
	{-1,  0},
	{ 0, -1},
	{+1,  0},
	{ 0, +1},
};

/* .~==~==~==):)-<  <~o~> Typesss: <~o~>  >-(:(==~==~==~. */

typedef struct {
	int   direction;
	int   size;
	COORD head;
	COORD tail;
} Snake;

typedef struct {
	COORD               frame;
	unsigned long long *grid;
	size_t              size;
} Grid;

typedef struct {
	CONSOLE_SCREEN_BUFFER_INFO info;
	HANDLE output;
	HANDLE input;
} ConsoleInfo;

/* .~==~==~==):)-<  <~o~> Functionsss: <~o~>  >-(:(==~==~==~. */

void s32_spawn_apple(Grid *grid)
{
	unsigned rn = rand();
	unsigned free_fields = 0;

	for (unsigned i = 0; i < grid->frame.X * grid->frame.Y; i++) {
		if (!S32_GRID_GET(grid->grid, i)) {
			free_fields++;
		}
	}

	if (!free_fields) {
		return;
	}

	rn %= free_fields;

	for (unsigned i = 0; i < grid->frame.X * grid->frame.Y; i++) {
		if (!S32_GRID_GET(grid->grid, i)) {
			if (!rn--) {
				S32_GRID_SET(grid->grid, i, S32_GRID_APPLE);
				return;
			}
		}
	}
}

void s32_render_grid(const Grid *grid, Grid *old_grid, ConsoleInfo *console)
{
	WORD  attr;
	DWORD len;

	if (grid->size > old_grid->size) {
		return;
	}

	for (unsigned i = 0; i < grid->frame.X * grid->frame.Y; i++) {
		if (S32_GRID_GET(grid->grid, i) ^ S32_GRID_GET(old_grid->grid, i)) {
			switch (S32_GRID_GET(grid->grid, i)) {
				case S32_GRID_APPLE: attr = BACKGROUND_RED; break;
				case S32_GRID_EMPTY: attr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; break;
				default:             attr = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
			}

			WriteConsoleOutputAttribute(console->output, &attr, 1, (COORD){(i % grid->frame.X) + 1, (i / grid->frame.X) + 1}, &len);
		}
	}

	memcpy(old_grid->grid, grid->grid, grid->size);
}

void s32_process_events(Snake *snake, Grid *grid, ConsoleInfo *console)
{
	DWORD event_count;
	INPUT_RECORD input_record;
	int last_direction = 0;

	while (GetNumberOfConsoleInputEvents(console->input, &event_count) && event_count) {
		ReadConsoleInputA(console->input, &input_record, 1, &event_count);

		switch (input_record.EventType) {
			case KEY_EVENT:
				if (!input_record.Event.KeyEvent.bKeyDown) {
					break;
				}
				if (
					input_record.Event.KeyEvent.wVirtualKeyCode >= VK_LEFT &&
					input_record.Event.KeyEvent.wVirtualKeyCode <= VK_DOWN
				) {
					last_direction = input_record.Event.KeyEvent.wVirtualKeyCode - VK_LEFT + 1;
				} else if (input_record.Event.KeyEvent.wVirtualKeyCode == 'R') {
					/* reset */
				} else if (input_record.Event.KeyEvent.wVirtualKeyCode == 'Q') {
					/* quit */
				}
				break;
			case WINDOW_BUFFER_SIZE_EVENT:
				// input_record.Event.WindowBufferSizeEvent.dwSize;
				break;
			default:
				break;
		}
	}

	snake->direction = last_direction % 2 == snake->direction % 2 || !last_direction
	                 ? snake->direction
	                 : last_direction;
}

int main(int argc, const char **argv)
{
	Snake snake;
	Grid  grid;
	Grid  old_grid;
	DWORD len;

	ConsoleInfo console;

	wchar_t snake_len_display[32];

	console.input  = GetStdHandle(STD_INPUT_HANDLE);
	console.output = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	/* Setup Console */
	GetConsoleScreenBufferInfo(console.output, &console.info);
	SetConsoleMode(console.input, ENABLE_WINDOW_INPUT);
	SetConsoleActiveScreenBuffer(console.output);

	/* Setup Grid */
	grid.frame     = console.info.dwSize;
	grid.frame.X  -= 4;
	grid.frame.Y  -= 2;
	old_grid.frame = grid.frame;

	grid.size     = ((grid.frame.X * grid.frame.Y * 4) / CHAR_BIT);
	grid.size    += sizeof(*grid.grid) * 2 - (grid.size % (sizeof(*grid.grid)));
	old_grid.size = grid.size;

	grid.grid     = malloc(grid.size);
	old_grid.grid = malloc(old_grid.size);

	memset(grid.grid,     0, grid.size);
	memset(old_grid.grid, 0, old_grid.size);

	/* Setup Game Objects */
	snake.direction = S32_GRID_SNAKE_E;

	snake.head = (COORD){grid.frame.X / 2, grid.frame.Y / 2};
	snake.tail = (COORD){snake.head.X, snake.head.Y};
	snake.size = 1;

	swprintf(snake_len_display, sizeof(snake_len_display) / sizeof(*snake_len_display), L"╡ Length: %d ╞", snake.size);

	srand(time(NULL));
	S32_GRID_SET(grid.grid, (snake.head.X) + (snake.head.Y) * grid.frame.X, S32_GRID_SNAKE);
	s32_spawn_apple(&grid);

	WriteConsoleOutputCharacterA(console.output, "╔", strlen("╔"), (COORD){0, 0}, &len);
	WriteConsoleOutputCharacterA(console.output, "╚", strlen("╚"), (COORD){0, grid.frame.Y + 1}, &len);
	WriteConsoleOutputCharacterA(console.output, "╗", strlen("╗"), (COORD){console.info.dwSize.X - 3, 0}, &len);
	WriteConsoleOutputCharacterA(console.output, "╝", strlen("╝"), (COORD){console.info.dwSize.X - 3, grid.frame.Y + 1}, &len);
	FillConsoleOutputCharacterW(console.output, L'═', grid.frame.X, (COORD){1, 0}, &len);
	FillConsoleOutputCharacterW(console.output, L'═', grid.frame.X, (COORD){1, grid.frame.Y + 1}, &len);
	WriteConsoleOutputCharacterW(console.output, snake_len_display, wcslen(snake_len_display), (COORD){grid.frame.X - wcslen(snake_len_display) - 2, 0}, &len);

	while (1) {
		int tail_direct;

		S32_GRID_SET(grid.grid, (snake.head.X) + (snake.head.Y) * grid.frame.X, snake.direction);

		snake.head.X = (snake.head.X + s32_directions[snake.direction - 1].X + grid.frame.X) % grid.frame.X;
		snake.head.Y = (snake.head.Y + s32_directions[snake.direction - 1].Y + grid.frame.Y) % grid.frame.Y;

		if (S32_GRID_GET(grid.grid, (snake.head.X) + (snake.head.Y) * grid.frame.X) == S32_GRID_APPLE) {
			S32_GRID_SET(grid.grid, (snake.head.X) + (snake.head.Y) * grid.frame.X, S32_GRID_SNAKE);

			s32_spawn_apple(&grid);

			swprintf(snake_len_display, sizeof(snake_len_display) / sizeof(*snake_len_display), L"╡ Length: %d ╞", ++snake.size);

			WriteConsoleOutputCharacterW(console.output, snake_len_display, wcslen(snake_len_display), (COORD){grid.frame.X - wcslen(snake_len_display) - 2, 0}, &len);
		} else {
			S32_GRID_SET(grid.grid, (snake.head.X) + (snake.head.Y) * grid.frame.X, S32_GRID_SNAKE);

			tail_direct = S32_GRID_GET(grid.grid, (snake.tail.X) + (snake.tail.Y) * grid.frame.X);

			S32_GRID_SET(grid.grid, (snake.tail.X) + (snake.tail.Y) * grid.frame.X, S32_GRID_EMPTY);

			snake.tail.X = (snake.tail.X + s32_directions[tail_direct - 1].X + grid.frame.X) % grid.frame.X;
			snake.tail.Y = (snake.tail.Y + s32_directions[tail_direct - 1].Y + grid.frame.Y) % grid.frame.Y;
		}

		s32_render_grid(&grid, &old_grid, &console);

		if (snake.direction % 2) {
			Sleep(50);
		} else {
			Sleep(75);
		}

		s32_process_events(&snake, &grid, &console);
	}

	Sleep(5000);
}
