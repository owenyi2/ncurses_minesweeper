#include <ncurses.h>			
#include <stdlib.h>
// https://www.paulgriffiths.net/program/c/srcs/curwormsrc.html

WINDOW* mainwin;
int row, col;

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

int main() {
    const int height = 12;
    const int width = 10;

    if ( (mainwin = initscr()) == NULL ) {
        perror("error initialising ncurses");
        exit(0);
    }
    curs_set(1);
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printw("?, ");
        }
        printw("\n");
    }
    move(0, 0);
    noecho();

    while (1) {
        int key = getch();
        switch (key) {
            case 'H':
            case 'h':
                getyx(stdscr, row, col);  
                move(row, max(col - 3, 0));
                break;
            case 'J':
            case 'j':
                getyx(stdscr, row, col);  
                move(min(row + 1, height-1), col);
                break;
            case 'K':
            case 'k':
                getyx(stdscr, row, col);  
                move(max(row - 1, 0), col);
                break;
            case 'L':
            case 'l':
                getyx(stdscr, row, col);  
                move(row, min(col + 3, (width-1) * 3));
                break;
            case 'Q':
            case 'q':
                getyx(stdscr, row, col);  
                mvaddch(row, col, 'Q'); 
                move(row, col); 
                break;
            case 'W':
            case 'w':
                getyx(stdscr, row, col);  
                mvaddch(row, col, 'W');
                move(row, col);
                break;
        }
    }
    endwin();

    return 0;
}
