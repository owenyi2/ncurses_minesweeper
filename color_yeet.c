#include <curses.h>

int main(void) {
    initscr();
    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);

    attron(COLOR_PAIR(1));
    printw("red\n");

    attroff(COLOR_PAIR(1));
    printw("normal\n");

    refresh();

    getch();

    endwin();
}

