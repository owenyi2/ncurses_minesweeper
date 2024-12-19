#include <ncurses.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

// helper.h
int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

void init_colorpairs() {
    if(has_colors() == FALSE) {	
        endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, 1, COLOR_BLACK);
    init_pair(2, 2, COLOR_BLACK);
    init_pair(3, 3, COLOR_BLACK);
    init_pair(4, 4, COLOR_BLACK);
    init_pair(5, 5, COLOR_BLACK);
    init_pair(6, 6, COLOR_BLACK);
    init_pair(7, 8, COLOR_BLACK);
    init_pair(8, 9, COLOR_BLACK);
}

typedef struct Coordinate {
    int i; 
    int j;
} Coordinate;

// minesweeper.h
// https://stackoverflow.com/questions/1608181/unique-random-number-generation-in-an-integer-array

typedef struct GameState {
    int mines;
    int game_ended;
    int is_first_move;
    int covered_free_squares;
    int flags_placed;
} GameState;

void generate_M_unique_random_integers(int* integer_array, size_t M, int N) {
    // generates M unique random integers from range [0, N)
    assert(M < N);
    char is_used[N];
    int in, im;

    for (int i = 0; i < N; i++) {
        is_used[i] = 0;
    }

    im = 0;

    for (in = N - M; in < N && im < M; ++in) {
        int r = rand() % (in + 1); /* generate a random number 'r' */
    
        if (is_used[r]) {

            /* we already have 'r' */
            r = in; /* use 'in' instead of the generated number */
        }
    
        integer_array[im++] = r;
        is_used[r] = 1;
    }

}

void init_board(int height, int width, int matrix[height][width], int* mine_locations, int mines) {
    for (int i = 0; i < mines; i++) {
        matrix[mine_locations[i] / width][mine_locations[i] % width] = 9;
    }
}

void first_move_shift_mine(int height, int width, int matrix[height][width], Coordinate first_move, int mine_location) { 
    if (matrix[first_move.i][first_move.j] % 10 == 9) {
        matrix[first_move.i][first_move.j] = 10;
        matrix[mine_location / width][mine_location % width] = 19;
    } 
}

void count_adjacent(int height, int width, int matrix[height][width]) {
    int i_low, i_upp, j_low, j_upp, count; 

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            i_low = max(i - 1, 0);
            i_upp = min(i + 1, height - 1);
            j_low = max(j - 1, 0);
            j_upp = min(j + 1, width - 1);
           
            if (matrix[i][j] % 10 == 9) {
                continue;
            }

            count = 0;
            for (int ii = i_low; ii <= i_upp; ii++) {
                for (int jj = j_low; jj <= j_upp; jj++) {
                    if (jj == j && ii == i) {
                        continue;
                    }
                    if (matrix[ii][jj] % 10 == 9) {
                        count += 1;
                    }
                }
            }
            
            matrix[i][j] = 10 * (matrix[i][j] / 10) + count;
        }
    }
}

void mask_board(int height, int width, int matrix[height][width]) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            matrix[i][j] += 10;  
        }
    }
}

void handle_right_click(Coordinate move, int height, int width, int matrix[height][width], GameState* game_state) {
    if (matrix[move.i][move.j] / 10 == 1) {
        matrix[move.i][move.j] += 10;
        game_state->flags_placed += 1;
    } else if (matrix[move.i][move.j] / 10 == 2) {
        matrix[move.i][move.j] -= 10;
        game_state->flags_placed -= 1;
    }
}


void uncover_board(Coordinate move, int height, int width, int matrix[height][width], GameState* game_state) {
    // we assume this is the specific case of a covered square
    // more general stuff is handled by handle_right_click
    assert(matrix[move.i][move.j] / 10 == 1); 

    if (matrix[move.i][move.j] % 10 == 0) {
        int stack[width * height * 8];
        stack[0] = move.i;
        stack[1] = move.j;
        int len = 2;
    
        while (len >= 2) {
            move.i = stack[len - 2];
            move.j = stack[len - 1];
            len -= 2;

            int up, down, left, right;
            up = down = left = right = 0;

            if (matrix[move.i][move.j] >= 10) {
                if (matrix[move.i][move.j] % 10 == 0) {
                    if (move.i > 0) { // up
                        stack[len] = move.i - 1;
                        stack[len+1] = move.j;
                        len += 2;
                        up = 1;
                    }
                    if (move.i < height - 1) { // down 
                        stack[len] = move.i + 1;
                        stack[len+1] = move.j;
                        len += 2;
                        down = 1;
                    }
                    if (move.j > 0) { // left 
                        stack[len] = move.i;
                        stack[len+1] = move.j - 1;
                        len += 2;
                        left = 1;
                    }
                    if (move.j < width - 1) { // right 
                        stack[len] = move.i;
                        stack[len+1] = move.j + 1;
                        len += 2;
                        right = 1;
                    }
                    // push adjacents
                    if (up && left) {
                        stack[len] = move.i - 1;
                        stack[len+1] = move.j - 1;
                        len += 2;
                    }
                    if (up && right) {
                        stack[len] = move.i - 1;
                        stack[len+1] = move.j + 1;
                        len += 2;
                    }
                    if (down && left) {
                        stack[len] = move.i + 1;
                        stack[len+1] = move.j - 1;
                        len += 2;
                    }
                    if (down && right) {
                        stack[len] = move.i + 1;
                        stack[len+1] = move.j + 1;
                        len += 2;
                    }
                } 
                matrix[move.i][move.j] -= 10;
                game_state->covered_free_squares -= 1;
            }
        }
    } else if (matrix[move.i][move.j] % 10 == 9) { 
        game_state->game_ended = 1;
    } else {
        matrix[move.i][move.j] -= 10;  
        game_state->covered_free_squares -= 1;
    }
}

void handle_left_click(Coordinate move, int height, int width, int matrix[height][width], GameState* game_state) {
    extern FILE* file_ptr;
    // Cases: 
    if (matrix[move.i][move.j] / 10 == 0) { 
        // - uncovered number [0, 9) :: chording if possible
        // - uncovered mine [9] :: unreachable

        int i_low, i_upp, j_low, j_upp, count; 
        i_low = max(move.i - 1, 0);
        i_upp = min(move.i + 1, height - 1);
        j_low = max(move.j - 1, 0);
        j_upp = min(move.j + 1, width - 1);
        
        count = 0;
        for (int ii = i_low; ii <= i_upp; ii++) {
            for (int jj = j_low; jj <= j_upp; jj++) {
                if (jj == move.j && ii == move.i) {
                    continue;
                }
                if (matrix[ii][jj] / 10 == 2) {
                    count += 1;
                }
            }
        }
        if (count == matrix[move.i][move.j] % 10) {    
            for (int ii = i_low; ii <= i_upp; ii++) {
                for (int jj = j_low; jj <= j_upp; jj++) {
                    if (jj == move.j && ii == move.i) {
                        continue;
                    }
                    if (matrix[ii][jj] / 10 == 1) {
                        uncover_board((Coordinate){ ii, jj }, height, width, matrix, game_state);
                    }
                }
            }
        }    
    } else if (matrix[move.i][move.j] / 10 == 1) {
        // - covered blank [10] :: DFS uncover all adjacent blanks
        // - covered number [11-19) :: uncover the number
        // - covered mine [19] :: end game
        uncover_board(move, height, width, matrix, game_state); 
    } else {
        // - flagged square [20-29] :: do nothing
    }
}

void print_board(int height, int width, int matrix[height][width], GameState game_state) {
    printw("Mines: %d\nCovered Free Squares Left: %d\n", game_state.mines - game_state.flags_placed, game_state.covered_free_squares);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (matrix[i][j] / 10 == 0) {
                if (matrix[i][j] % 10 == 0) {
                    printw("_  ");
                } else if (matrix[i][j] == 9) {
                    printw("@  ");
                } else {
                    attron(COLOR_PAIR(matrix[i][j]));
                    printw("%d  ", matrix[i][j]);
                    attroff(COLOR_PAIR(matrix[i][j]));
                }
            } else if (matrix[i][j] / 10 == 1){
                printw("?  ");
            } else {
                printw("F  ");
            }
        }
        printw("\n");
    }
}

// void debug_board(int height, int width, int matrix[height][width]) {
//     extern FILE* file_ptr;
//     for (int i = 0; i < height; i++) {
//         for (int j = 0; j < width; j++) {
//             fprintf(file_ptr, "%d, ", matrix[i][j] % 10);
//         }
//         fprintf(file_ptr, "\n");
//     }
//     fprintf(file_ptr, "\n");
// }

/* io.h */

const int lines_preceding = 2;
const int cell_width = 3;

WINDOW* mainwin;

Coordinate user_move;
int row, col;

// FILE* file_ptr;

int main(int argc, char** argv) {
    /* parse args */ 
    if (argc != 4) {
        printf("Usage:\n./a.out height width mines\n");
        exit(1); 
    }

    int height = atoi(argv[1]);
    int width = atoi(argv[2]); 
    int mines = atoi(argv[3]);

    if (height <= 0 || width <= 0 || mines <= 0) {
        printf("bad input\n");
        exit(1);
    }
    /* init board */

    int matrix[height][width]; 
    
    // file_ptr = fopen("out.log", "w");

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            matrix[i][j] = 0;
        }
    }
    GameState game_state = { .covered_free_squares = width * height - mines, .flags_placed = 0, .game_ended = 0, .is_first_move = 1, .mines = mines }; 

    srand(time(NULL));
    int mine_locations[mines + 1]; // generate one more location to in case move mine later
    for (int i = 0; i < mines + 1; i++) {
        mine_locations[i] = 0;
    }

    generate_M_unique_random_integers(mine_locations, sizeof(mine_locations) / sizeof(int), height * width);
    init_board(height, width, matrix, mine_locations, mines);

    /* init window */

    if ((mainwin = initscr()) == NULL) {
        perror("error initialising ncurses");
        exit(0);
    }
    curs_set(1);
    noecho();
    start_color();
    init_colorpairs();
    mask_board(height, width, matrix);
    print_board(height, width, matrix, game_state);
    move(lines_preceding, 0);
    // debug_board(height, width, matrix); 

    /* game_loop */
    while (!game_state.game_ended) {
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
                move(min(row + 1, height-1 + lines_preceding), col);
                break;
            case 'K':
            case 'k':
                getyx(stdscr, row, col);  
                move(max(row - 1, lines_preceding), col);
                break;
            case 'L':
            case 'l':
                getyx(stdscr, row, col);  
                move(row, min(col + 3, (width-1) * 3));
                break;
            case 'Q': // left click
            case 'q':
                getyx(stdscr, row, col);  
                user_move = (Coordinate) { .i = row - lines_preceding, .j = col / cell_width }; 
                if (game_state.is_first_move) {
                    first_move_shift_mine(height, width, matrix, user_move, mine_locations[mines]);
                    // debug_board(height, width, matrix); 
                    count_adjacent(height, width, matrix); 
                    // debug_board(height, width, matrix);
                    game_state.is_first_move = 0;
                } 
                handle_left_click(user_move, height, width, matrix, &game_state);

                clear();
                // printw("Mines: %d\n", mines);
                print_board(height, width, matrix, game_state);
                move(row, col);
                break;
            case 'W': // Flag 
            case 'w':
                getyx(stdscr, row, col);
                user_move = (Coordinate) { .i = row - lines_preceding, .j = col / cell_width }; 
                handle_right_click(user_move, height, width, matrix, &game_state);

                clear();
                print_board(height, width, matrix, game_state);
                move(row, col);
                break;
        }
        if (game_state.covered_free_squares == 0) {
            game_state.game_ended = 1;
        }
    }

    /* game over */
    if (game_state.covered_free_squares == 0) {
        clear();
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (matrix[i][j] == 19) {
                    matrix[i][j] = 29;
                    game_state.flags_placed += 1;
                }
            }
        }
        print_board(height, width, matrix, game_state);
        printw("a WINNER is you\n");
    } else {
        clear();
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                matrix[i][j] = matrix[i][j] % 10; 
            }
        }
        // printw("Mines: %d\n", mines);
        print_board(height, width, matrix, game_state);
        printw("GAME OVER. You lose\n");
    }
    getch();
    endwin();
    // fclose(file_ptr);
}

// TODO: take in CLI args for specifying board size and number of mines
