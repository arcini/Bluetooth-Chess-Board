#include "board_utils.h"
#include <string>
#include <cstdio>

// ----- PORT INITIALIZATION -----
BusOut mux_1_select(PB_8, PA_1, PA_2, PA_3);
BusOut mux_2_select(PA_4, PA_5, PA_6, PA_7);

// PA_9 and PA_10 are used for UART (serial monitor), so PB 6 and 7 used instead.
BusOut mux_3_select(PA_8, PB_6, PB_7, PA_11); 
BusOut mux_4_select(PA_12, PA_13, PA_14, PA_15);


// ----- GLOBAL VARS -----
// Port containers
DigitalOut mux_enables = PB_9;
DigitalIn mux_outputs[] = {PB_0, PB_1, PB_2, PB_3};
BusOut* mux_selectors[] = {&mux_1_select, &mux_2_select, &mux_3_select, &mux_4_select};

// Data structures
bool board[8][8];

bool set_and_read_mux(int mux_index, int channel) {
    // Disable the mux temporarily
    mux_enables.write(1);

    // Select the channel
    // NOTE: Using an array of pointers is probably slow, but BusOut is nonCopyable
    (*mux_selectors[mux_index]).write(channel);

    // Re-enable the mux
    mux_enables.write(0);

    // Read the mux and return a boolean for the channel's state
        // 0 (LL low): piece present
        // 1 (LL high): piece not present
        // If piece is present....
    return mux_outputs[mux_index].read() == 0 ? true : false;
}

// Load the board state into the board 2D array
void load_board() {
    // For each multiplexer
    int col;
    int row;
    for (int i = 0; i < 4; ++i) {
        // For each possible square in the mux
        for (int s = 0; s < 16; ++s) {
            // Board representation math
            col = 7 - (s % 8);
            row = s >= 8 ? 2 * i + 1 : 2 * i;
            
            board[row][col] = set_and_read_mux(i, s);
        }
    }
}

// Same as load_board() except will return a boolean representing 
// if the board state has changed since last check
bool load_board_check_update() {
    bool has_changed = false;

    int col;
    int row;
    bool square;
    for (int i = 0; i < 4; ++i) {
        for (int s = 0; s < 16; ++s) {
            col = 7 - (s % 8);
            row = s >= 8 ? 2 * i + 1 : 2 * i;
            
            square = set_and_read_mux(i, s);
            if ( square != board[row][col]) {
                has_changed = true;
            }
            
            board[row][col] = square;
        }
    }
    return has_changed;
}

void print_board() {
    std::string out = "--------\n";
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (board[row][col] == true) {
                out += "▣ ";
            } else {
                out += "□ ";
            }
        }
        out += "\n";
    }
    printf("%s", out.c_str());
}
