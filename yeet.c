#include <stdlib.h>
#include <stdio.h>

// int get_move() {
//     char* buffer = malloc(100);
// 
// 
//     char c = getchar();
//     int i = 0;
//     while (c != '\n') {
//         buffer[i] = c
//         c = getchar();
//     }
//     return 0;
// }
// 
// int main() {
//     get_move();
// }

void get_move() {
    int move = getchar();
    printf("%d\n", move);
}

int main() {
    int game_ended = 0;
    
    // ok i get it now. When you press a\n. the first get_move receives a. the next on receives \n. so if you do ab\n, then 3 get_moves are triggered.
    get_move();     
    get_move();      
    get_move();     
    get_move();      
    get_move();     
    // while (!game_ended) {
    //     printf("move: %d\n", move);
    //     move = get_move();     
    // }
}

// move: 0
// Called
// a
// move: 10 
// Called 
// move: 10
// Called
