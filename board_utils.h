#pragma once

#include "mbed.h"

// ----- PORT INITIALIZATION -----
extern BusOut mux_1_select;
extern BusOut mux_2_select;

// PA_9 and PA_10 are used for UART (serial monitor), so PB 6 and 7 used instead.
extern BusOut mux_3_select; 
extern BusOut mux_4_select;


// ----- GLOBAL VARS -----
// Port containers
extern DigitalOut mux_enables;
extern DigitalIn mux_outputs[];
extern BusOut* mux_selectors[];

// Data structures
extern bool board[8][8];

// ----- FUNCTION PROTOTYPES -----
bool set_and_read_mux(int mux_index, int channel);
void load_board();
bool load_board_check_update();
void print_board();