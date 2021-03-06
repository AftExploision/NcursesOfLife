#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes.h"

typedef struct cell {
	int state;
} Cell;

void init(Cell *grid);
void sim(Cell *grid);

int main() {
	initscr();
	erase();
	keypad(stdscr, TRUE);
	noecho();
	cbreak();
	curs_set(0);
	refresh();

	start_color();
	use_default_colors();
	init_pair(1, COLOR_YELLOW, -1);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	attron(COLOR_PAIR(1));

	int MAXX, MAXY;
	getmaxyx(stdscr, MAXY, MAXX);
	MAXY -= 1;	

	Cell *gameGrid = malloc(MAXY * MAXX * sizeof(Cell));
	
	init(gameGrid);
	sim(gameGrid);
		
	endwin();

	free(gameGrid);
	return 0;
}

void init(Cell *grid) {
	curs_set(1);
	int MAX_X, MAX_Y;
	getmaxyx(stdscr, MAX_Y, MAX_X);
	MAX_Y -=1;

	for (int i=0; i<MAX_Y; i++) {
		for (int j=0; j<MAX_X; j++) {
			grid[i*MAX_X + j].state = 0;
		}
	}

	erase();
}

void sim(Cell *grid) {
	int MAX_X, MAX_Y;
	getmaxyx(stdscr, MAX_Y, MAX_X);
	MAX_Y -= 1;
	int curr = 1;

	int ch;
	Cell *next = malloc(MAX_Y*MAX_X*sizeof(Cell));

	int speed = 100;

	int y = 0, x = 0;

	int iter = 0;
	
	attroff(COLOR_PAIR(1));
	attron(A_REVERSE | COLOR_PAIR(2));
	move(MAX_Y, 0);
	addstr("Iteration: ");
	attroff(A_REVERSE | COLOR_PAIR(2));
	attron(COLOR_PAIR(1));

	while (1) {
		attroff(COLOR_PAIR(1));
		attron(COLOR_PAIR(2) | A_REVERSE);
		char tmp[15];
		sprintf(tmp, "%d", iter);
		move(MAX_Y, 11);
		addstr(tmp);
		attroff(A_REVERSE | COLOR_PAIR(2));
		attron(COLOR_PAIR(1));
	
		if (ch == 'q') {
			free(next);
			return;
		}

		if (ch == 'p') 
			curr = curr == 1 ? 0 : 1;

		if (ch == '+') 
			speed -= 10;
		if (ch == '-') 
			speed += 10;

		if (ch == 'i') 
			init(grid);

		if (speed < 1) 
			speed = 1;

		if (!curr) 
			curs_set(0);

		memcpy(next, grid, sizeof(Cell)*MAX_Y*MAX_X);

		int count = 0;
		for (int i=0; i<MAX_Y; i++) {
			for (int j=0; j<MAX_X; j++) {
				if (j > 0) count += grid[i*MAX_X + j - 1].state;
				else count += grid[i*MAX_X + MAX_X-1].state;
				
				if (j < MAX_X-1)
					count += grid[i*MAX_X + j + 1].state;
				else 
					count += grid[i*MAX_X].state;

				if (i > 0) 
					count += grid[(i-1)*MAX_X + j].state;
				else 
					count+= grid[(MAX_Y-1)*MAX_X + j].state;

				if (i < MAX_Y-1) 
					count += grid[(i+1)*MAX_X + j].state;
				else 
					count += grid[j].state;

				if (j > 0 && i > 0) 
					count += grid[(i-1)*MAX_X + j - 1].state;
				else if (j>0 && !(i > 0)) 
					count += grid[(MAX_Y-1)*MAX_X + j-1].state;
				else if (!(j>0) && i > 0)
					count += grid[(i-1)*MAX_X + MAX_X-1].state;
				else if (!(j>0) && !(i>0))
					count += grid[(MAX_Y-1)*MAX_X + MAX_X-1].state;

				if (j > 0 && i < MAX_Y-1)
					count += grid[(i+1)*MAX_X + j - 1].state;
				else if (j > 0 && !(i < MAX_Y-1))
					count += grid[j-1].state;
				else if (!(j > 0) && i < MAX_Y-1)
					count += grid[(i+1)*MAX_X + MAX_X-1].state;
				else if (!(j > 0) && !(i < MAX_Y-1))
					count += grid[MAX_X-1].state;

				if (j < MAX_X-1 && i > 0)
					count += grid[(i-1)*MAX_X + j + 1].state;
				else if (j < MAX_X-1 && !(i > 0))
					count += grid[(MAX_Y-1)*MAX_X + j + 1].state;
				else if (!(j < MAX_X-1) && i > 0)
					count += grid[(i-1)*MAX_X].state;
				else if (!(j < MAX_X-1) && !(i > 0))
					count += grid[(MAX_Y-1)*MAX_X].state;

				if (j < MAX_X-1 && i < MAX_Y-1)
					count += grid[(i+1)*MAX_X + j + 1].state;
				else if (j < MAX_X-1 && !(i < MAX_Y-1))
					count += grid[j+1].state;
				else if (!(j < MAX_X-1) && i < MAX_Y-1)
					count += grid[(i+1)*MAX_X].state;
				else if (!(j < MAX_X-1) && !(i < MAX_Y-1))
					count += grid[0].state;
				
				if (count == 3)
					next[i*MAX_X + j].state = 1;
				else if (count < 2)
					next[i*MAX_X + j].state = 0;
				else if (count > 3)
					next[i*MAX_X + j].state = 0;			
				
				count = 0;
			}
		}

		memcpy(grid, next, sizeof(Cell)*MAX_Y*MAX_X);

		for (int i=0; i<MAX_Y; i++) {
			for (int j=0; j<MAX_X; j++) {
				move(i, j);
				addch(grid[i*MAX_X + j].state == 0 ? ' ' : '#');
			}
		}
 
		iter++;

		if (!curr) {
			timeout(speed);
			ch = getch();
		} else {
			timeout(10);
			while (1) {
				attron(A_REVERSE);
				curs_set(1);				

				ch = getch();
				
				if (ch == 'w' || ch == KEY_UP)
					y--;
				if (ch == 's' || ch == KEY_DOWN)
					y++;
				if (ch == 'a' || ch == KEY_LEFT)
					x--;
				if (ch == 'd' || ch == KEY_RIGHT)
					x++;

				if (ch == 'n')
					break;
				
				if (ch == 'p') {
					curr = 0;
					ch = ' ';
					break;
				}

				if (ch == 'q') {
					free(next);
					return;
				}

				if (ch == '+')
					speed -= 10;
				if (ch == '-')
					speed += 10;

				if (y > MAX_Y-1)
					y = MAX_Y-1;
				if (x > MAX_X-1)
					x = MAX_X-1;
				if (y < 0)
					y = 0;
				if (x < 0)
					x = 0;

				if (ch == ' ') {
					grid[y*MAX_X + x].state = grid[y*MAX_X + x].state ? 0 : 1;
					move(y, x);
					addch(grid[y*MAX_X + x].state ? '#' : ' ');
				}
		
				move(y, x);
			}
		}
	}
}
