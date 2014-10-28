#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <curses.h>
#include <locale.h>

void draw(bool **state)
{
	for (int i = 0; i < LINES; ++i) {
		for (int j = 0; j < COLS; ++j) {
			if (state[i][j])
				mvaddch(i, j, '*');
			else
				mvaddch(i, j, ' ');
		}
	}
}

bool lives(int y, int x, bool **state)
{
	int live_neighbors = 0;
	if (y != 0 && x != 0 && state[y-1][x-1])
		++live_neighbors;
	if (y != 0 && state[y-1][x])
		++live_neighbors;
	if (y != 0 && x < COLS - 1 && state[y-1][x+1])
		++live_neighbors;
	if (x != 0 && state[y][x-1])
		++live_neighbors;
	if (x < COLS - 1 && state[y][x+1])
		++live_neighbors;
	if (y < LINES - 1 && x != 0 && state[y+1][x-1])
		++live_neighbors;
	if (y < LINES - 1 && state[y+1][x])
		++live_neighbors;
	if (y < LINES - 1 && x < COLS - 1 && state[y+1][x+1])
		++live_neighbors;

	if (state[y][x]) {
		if (live_neighbors < 2 || live_neighbors > 3)
			return false;
	} else if (live_neighbors == 3) {
		return true;
	} else {
		return false;
	}

	return true;
}

enum { PAUSE, QUIT };

int game(bool **state)
{
	int ch;
	bool pause = false;
	struct timespec tim;
	tim.tv_sec = 1;
	tim.tv_nsec = 0;

	bool *oldstate[LINES];
	for (int i = 0; i < LINES; ++i)
		oldstate[i] = calloc(COLS, sizeof(bool));

	while (true) {
		if ((ch = getch()) != ERR) {
			double doubtime = tim.tv_sec + (double) (tim.tv_nsec) / 1000000000L;
			switch (ch) {
			case 'p':
				pause = true;
				goto end;
				break;
			case 'q':
				goto end;
				break;
			case '+':
			case '=':
				doubtime /= 1.5;
				break;
			case '-':
				doubtime *= 1.5;
				break;
			}

			// Technically there could be time drift here from all
			// the conversions if you were to keep pressing keys
			// that weren't in the case statement (or just keep
			// speeding up and slowing down), but I'd be impressed
			// if you could get something noticeable out of it.
			tim.tv_sec = doubtime;
			tim.tv_nsec = fmod(doubtime, 1) * 1000000000L;
		}


		for (int i = 0; i < LINES; ++i)
			for (int j = 0; j < COLS; ++j)
				oldstate[i][j] = state[i][j];

		for (int i = 0; i < LINES; ++i) {
			for (int j = 0; j < COLS; ++j) {
				if (lives(i, j, oldstate))
					state[i][j] = true;
				else
					state[i][j] = false;
			}
		}

		draw(state);
		refresh();
		nanosleep(&tim, NULL);
	}

end:
	for (int i = 0; i < LINES; ++i)
		free(oldstate[i]);

	if (pause)
		return PAUSE;
	else
		return QUIT;
}

void toggle(int y, int x, bool **state)
{
	if (state[y][x]) {
		state[y][x] = false;
		mvaddch(y, x, ' ');
	} else {
		state[y][x] = true;
		mvaddch(y, x, '*');
	}

	move(y, x);
}

int main()
{
	int ch, y, x;
	bool **state;

	// Various options everyone uses with curses
	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);

	state = malloc(LINES * sizeof(bool *));
	for (int i = 0; i < LINES; ++i)
		state[i] = calloc(COLS, sizeof(bool));

	y = LINES / 2;
	x = COLS / 2;
	move(y, x);
	refresh();

	while (true) {
		int stat;

		getyx(stdscr, y, x);
		ch = getch();
		switch (ch) {
		case KEY_LEFT:
		case 'h':
			move(y, --x);
			break;
		case KEY_DOWN:
		case 'j':
			move(++y, x);
			break;
		case KEY_UP:
		case 'k':
			move(--y, x);
			break;
		case KEY_RIGHT:
		case 'l':
			move(y, ++x);
			break;
		case 'g':
			move(0, x);
			break;
		case 'G':
			move(LINES - 1, x);
			break;
		case '0':
			move(y, 0);
			break;
		case '$':
			move(y, COLS - 1);
			break;
		case ' ':
			toggle(y, x, state);
			break;
		case 'r':
			stat = game(state);
			if (stat == QUIT)
				goto end;
			move(y, x);
			break;
		case 'q':
			goto end;
			break;
		}

		refresh();
	}

end:
	for (int i = 0; i < LINES; ++i)
		free(state[i]);

	free(state);
	endwin();
	return 0;
}
