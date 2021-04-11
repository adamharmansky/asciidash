#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>

#define LEVEL_HEIGHT 20
#define SCREEN_LENGTH 80
#define CUBE_OFFSET 10
#define DIE { printf("?"); return 1; }

typedef struct {int x,y;} point;

point cube;
int yvel = 0; //used when falling

char* level[LEVEL_HEIGHT]; //first dimension are rows(a constant amount of them), second are columns
int level_length;

int
load_level(char* filename)
{
	FILE* file = fopen(filename, "r");
	if(file == NULL){printf("unable to load the level\n"); return 1;}

	level_length = 0;
	//scan for a newline to find the length of the level
	for(char c;(c=fgetc(file))!='\n';level_length++);
	rewind(file);
	//allocate the level by rows
	for(int i = 0; i < LEVEL_HEIGHT; i++) level[i] = malloc(level_length);

	for(int i = 0; i < LEVEL_HEIGHT; i++) {
		for(int j = 0; j < level_length; j++)
		{
			char c;
			read_char:
			c = fgetc(file);
			if (c == '\n') goto read_char;
			level[i][j] = c;
		}
	}
	fclose(file); //oh my god I almost forgot to do this
	return(0);
}

int
render_level()
{
	for(int i = 0; i < LEVEL_HEIGHT; i++) {
		for(int j = 0; j < SCREEN_LENGTH; j++) {
			move(i, j);
			if( level[i][j+cube.x] == '#' ) addch(ACS_BLOCK);
			else addch(level[i][j+cube.x] == '.' ? ' ' : level[i][j+cube.x]);
		}
		mvaddch(i, SCREEN_LENGTH, ' ');
	}
}

int main(int argc, char** argv)
{
	if(argc != 2) DIE
	if(load_level(argv[1])) DIE
	// initialize ncurses
	initscr(); cbreak(); noecho(); keypad(stdscr, TRUE); curs_set(0); timeout(80);

	restart:
	cube = (point){0, 15};
	yvel = 0;

	int input;
	for(int time = 0;;time++) { //main loop
		input = getch();

		//draws the level
		render_level();

		// draws the cube
		mvaddch(cube.y, CUBE_OFFSET, 'o');

		int falldistance = 0;
		for(;level[cube.y+falldistance+1][cube.x+CUBE_OFFSET] != '#' && level[cube.y+falldistance+1][cube.x+CUBE_OFFSET] != '/';falldistance++);

		int ceiling = 0;
		for(;level[cube.y-ceiling-1][cube.x+CUBE_OFFSET] != '#';ceiling++);

		if(falldistance == 0 || ceiling == 0) {
			if (input == ' ' && ceiling >= 3) yvel = -3;
			else              yvel = 0;
		} else if(falldistance > yvel && ceiling >= -yvel) {
			yvel += 1;
		} else {
			if(falldistance <= yvel) {
				yvel = 0;
				cube.y += falldistance;
			}
			if(ceiling < -yvel) {
				yvel = yvel<0?-yvel:1;
				cube.y -= ceiling;
			}
		}

		while(level[cube.y][cube.x+CUBE_OFFSET] == '/') {
			cube.y--;
		}

		if(level[cube.y][cube.x+CUBE_OFFSET+1] == '#' || level[cube.y][cube.x+CUBE_OFFSET] == '^') {
			mvprintw(LEVEL_HEIGHT/2,    SCREEN_LENGTH/2 - 10, "      You died!       ");
			mvprintw(LEVEL_HEIGHT/2 +1, SCREEN_LENGTH/2 - 10, "press SPACE to restart");
			while(getch() != ' ');
			goto restart;
		}
		if(cube.x >= level_length - 1 - CUBE_OFFSET) {
			mvprintw(LEVEL_HEIGHT/2,    SCREEN_LENGTH/2 - 10, "       You WIN!       ");
			mvprintw(LEVEL_HEIGHT/2 +1, SCREEN_LENGTH/2 - 10, "press SPACE to restart");
			while(getch() != ' ');
			goto restart;
		}
		
		//moves cube one character right
		cube.x++;
		//applies velocity
		cube.y+=yvel;
	}
}
