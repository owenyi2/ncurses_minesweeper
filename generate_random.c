#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

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

const int height = 12;
const int width = 10;
const int mines = 30;

int main() { 
    srand(time(NULL));
    int mine_locations[mines + 1] = { 0 }; // generate one more location to in case move mine later
    generate_M_unique_random_integers(mine_locations, sizeof(mine_locations) / sizeof(int), height * width);
    for (int i = 0; i < mines + 1; i++) {
        printf("%d, ", mine_locations[i]);
    }
}
