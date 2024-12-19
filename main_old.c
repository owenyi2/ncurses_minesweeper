#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#define CRESET "\e[0m"
#define COLOUR_1 "\e[0;31m"
#define COLOUR_2 "\e[0;32m"
#define COLOUR_3 "\e[0;33m"
#define COLOUR_4 "\e[0;34m"
#define COLOUR_5 "\e[0;35m"
#define COLOUR_6 "\e[0;36m"
#define COLOUR_7 "\e[0;37m"
#define COLOUR_8 "\e[0;38m"

// TODO: for input use `ncurses` for input
// Use hjkl and qw for left click right click

int moves_scaffolding_delete_later[10] = { 31, 14, 15, 29, 6, 35, 5, 42, 9, 40 };
int move_scaffolding_delete_later = 0;

// https://stackoverflow.com/questions/1608181/unique-random-number-generation-in-an-integer-array
void generate_M_unique_random_integers(int* integer_array, size_t M, int N) {
    // generates M unique random integers from range [0, N)
    printf("%zu, %u\n", M, N);
    assert(M < N);
    char is_used[N];
    int in, im;

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

int get_move() {
    // char move = getchar();
    // // printf("%c\n", move);
    // return 10;
    int move = moves_scaffolding_delete_later[move_scaffolding_delete_later]; 
    move_scaffolding_delete_later += 1;
    if (move_scaffolding_delete_later >= 10) {
        exit(1);
    }
    printf("get move line 49: %d\n", move);
    return move;
}

void first_move_shift_mine(int height, int width, int matrix[height][width], int first_move, int mine_location) { 
    int first_move_i = first_move / width, first_move_j = first_move % width;
    if (matrix[first_move_i][first_move_j] == 9) {
        matrix[first_move_i][first_move_j] = 0;
        matrix[mine_location / width][mine_location % width] = 9;
    } 
}

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

void count_adjacent(int height, int width, int matrix[height][width]) {
    // lol just count it normally
    int i_low, i_upp, j_low, j_upp, count; 

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            i_low = max(i - 1, 0);
            i_upp = min(i + 1, height - 1);
            j_low = max(j - 1, 0);
            j_upp = min(j + 1, width - 1);
           
            if (matrix[i][j] == 9) {
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
            
            matrix[i][j] = count;
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

void uncover_board(int move, int height, int width, int matrix[height][width]) {
    printf("uncover_board: %d\n", move);
    int move_i = move / width, move_j = move % width;
    // you know we might not even need to / % out the move into parts if arrays work how I think they work. If they are just a contiguous section of memory and we can use array[i] to access the i-th element even for a multdim array, then just do matrix[move];
    if (matrix[move_i][move_j] < 10) {
 
        printf("already uncovered this square\n");
        // probably handle this elsewhere
        return;
    }

    if (matrix[move_i][move_j] % 10 == 9) {
        printf("Game Over"); 
        exit(0);
        // make this function modify a global game end flag. Or just return an int indicating game over or not
        // create a separate function to handle gameover by revealing everything
    }
    else if (matrix[move_i][move_j] % 10 == 0) {
        int stack[width * height * 8];
        stack[0] = move_i;
        stack[1] = move_j;
        int len = 2;
    
        while (len >= 2) {
            move_i = stack[len - 2];
            move_j = stack[len - 1];
            // do they also reveal corners? Need to check that. TODO
            len -= 2;

            int up, down, left, right = 0;
            if (matrix[move_i][move_j] >= 10) {
                if (matrix[move_i][move_j] == 10) {
                    if (move_i > 0) { // up
                        stack[len] = move_i - 1;
                        stack[len+1] = move_j;
                        len += 2;
                        up = 1;
                    }
                    if (move_i < height - 1) { // down 
                        stack[len] = move_i + 1;
                        stack[len+1] = move_j;
                        len += 2;
                        down = 1;
                    }
                    if (move_j > 0) { // left 
                        stack[len+1] = move_j - 1;
                        stack[len] = move_i;
                        len += 2;
                        left = 1;
                    }
                    if (move_j < width - 1) { // right 
                        stack[len+1] = move_j + 1;
                        stack[len] = move_i;
                        len += 2;
                        right = 1;
                    }
                    // push adjacents
                    if (up && left) {
                        stack[len] = move_i - 1;
                        stack[len+1] = move_j - 1;
                        len += 2;
                    }
                    if (up && right) {
                        stack[len] = move_i - 1;
                        stack[len+1] = move_j + 1;
                        len += 2;
                    }
                    if (down && left) {
                        stack[len] = move_i + 1;
                        stack[len+1] = move_j - 1;
                        len += 2;
                    }
                    if (down && right) {
                        stack[len] = move_i + 1;
                        stack[len+1] = move_j + 1;
                        len += 2;
                    }
                } 
                matrix[move_i][move_j] -= 10;
            }

        }
    } else {
        matrix[move_i][move_j] -= 10;  
    }
}

void print_board(int height, int width, int matrix[height][width]) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (matrix[i][j] / 10 == 0) {
                if (matrix[i][j] % 10 == 0) {
                    printf(" , ");
                } else {
                    switch (matrix[i][j]) {
                    case 1:
                        printf(COLOUR_1 "%d, " CRESET, matrix[i][j]);
                        break;
                    case 2:
                        printf(COLOUR_2 "%d, " CRESET, matrix[i][j]);
                        break;
                    case 3:
                        printf(COLOUR_3 "%d, " CRESET, matrix[i][j]);
                        break;
                    case 4:
                        printf(COLOUR_4 "%d, " CRESET, matrix[i][j]);
                        break;
                    case 5:
                        printf(COLOUR_5 "%d, " CRESET, matrix[i][j]);
                        break;
                    case 6:
                        printf(COLOUR_6 "%d, " CRESET, matrix[i][j]);
                        break;
                    case 7:
                        printf(COLOUR_7 "%d, " CRESET, matrix[i][j]);
                        break;
                    case 8:
                        printf(COLOUR_8 "%d, " CRESET, matrix[i][j]);
                        break;
                    }
                }
            } else {
                printf("?, ");
            } 
        }
        printf("\n");
    }
}

void debug_board(int height, int width, int matrix[height][width]) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (matrix[i][j] == 9) {
                printf("#, ");
            } else {
                printf("%d, ", matrix[i][j]);
            }
        }
        printf("\n");
    }
}
// Yeah by the way, it also reveals numbers that are diagonally adjacent to 0's.

int main() {
    const int mines = 30;
    const int height = 12;
    const int width = 10;

    int game_ended = 0;
    int move;

    srand(0);

    int matrix[height][width] = { 0 }; 
    // 0-9 represent uncovered
    // 10-19 represent covered
    // 0-8 and 10-18 indicate number of adjacent mines
    // 9 and 19 represent is a mine 
    int mine_locations[mines + 1]; // generate one more location to move mine

    generate_M_unique_random_integers(mine_locations, sizeof(mine_locations) / sizeof(int), height * width);

    init_board(height, width, matrix, mine_locations, mines);
    move = get_move();
    first_move_shift_mine(height, width, matrix, move, mine_locations[mines]);
    count_adjacent(height, width, matrix); 
    mask_board(height, width, matrix);    
   
    printf("line 163: debug board\n");
    debug_board(height, width, matrix);

    while (!game_ended) {
        uncover_board(move, height, width, matrix);
        print_board(height, width, matrix);
        move = get_move();
    }
}
