#include "mbed.h"
#include <vector>

DigitalIn mux_1_sig(PB_0);
DigitalIn mux_2_sig(PB_1);
DigitalIn mux_3_sig(PB_2);
DigitalIn mux_4_sig(PB_3);

BusOut mux_1_select(PA_0, PA_1, PA_2, PA_3);
BusOut mux_2_select(PA_4, PA_5, PA_6, PA_7);
BusOut mux_3_select(PA_8, PA_9, PA_10, PA_11);
BusOut mux_4_select(PA_12, PA_13, PA_14, PA_15);

bool board[8][8];
vector<BusOut> muxes;

// Load the board state into the board 2D array
int load_board() {
    for (int i = 0; i < 4; ++i) {
        for (int s = 0; s < 8; ++s) {

        }
    }

    return 0;
}

// main() runs in its own thread in the OS
int main()
{
    // Put the mux select variables into the muxes array 
    muxes.push_back(mux_1_select);
    muxes.push_back(mux_2_select);
    muxes.push_back(mux_3_select);
    muxes.push_back(mux_4_select);
    
    while (true) {

    }
}

