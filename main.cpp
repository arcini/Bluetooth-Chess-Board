#include "mbed.h"
#include <cstdio>
#include <vector>


BusOut mux_1_select(PA_0, PA_1, PA_2, PA_3);
BusOut mux_2_select(PA_4, PA_5, PA_6, PA_7);
BusOut mux_3_select(PA_8, PA_9, PA_10, PA_11);
BusOut mux_4_select(PA_12, PA_13, PA_14, PA_15);


bool board[8][8];
DigitalIn mux_outputs[] = {PB_0, PB_1, PB_2, PB_3};
BusOut* mux_selectors[] = {&mux_1_select, &mux_2_select, &mux_3_select, &mux_4_select};

void load_board();
void print_board();

// main() runs in its own thread in the OS
int main()
{
    while (true) {
        // Load the state of the board
        load_board();

        // Print the state of the board
        print_board();

        // Wait for a second
        ThisThread::sleep_for(1s);
    }
}

// Load the board state into the board 2D array
void load_board() {
    // For each multiplexer
    int col;
    int row;
    for (int i = 0; i < 4; ++i) {
        // For each possible square in the mux
        for (int s = 0; s < 16; ++s) {
            // Set the current mux (i) to select the current square (s)
            // NOTE: Using an array of pointers is probably slow, but BusOut is nonCopyable
            (*mux_selectors[i]).write(s);
            
            // Board representation math
            col = s >= 8? i + 1: i;
            row = s % 8;

            // Read the value for the square and store in board array
            // 0 (LL low): piece present
            // 1 (LL high): piece not present
            // If piece is present....
            board[col][row] = mux_outputs[i].read() == 0 ? true : false;
        }
    }
}

void print_board() {
    puts("--------");
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (board[row][col] == true) {
                printf("▣");
            } else {
                printf("□");
            }
        }
        printf("\n");
    }
    puts("--------");
}