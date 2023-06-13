#include "board_utils.h"
#include "bluetooth.h"

// main() runs in its own thread in the OS
int main()
{
    // Make sure all muxes are enabled
    for (int i = 0; i < 4; ++i) {
        mux_enables.write(0);
    }

    // bluetooth initialization
    BLE& ble = BLE::Instance();
    ble.onEventsToProcess(schedule_ble_events);

    ChessboardBLE chessboard_bl(ble, event_queue);

    chessboard_bl.start();

    while (true) {
        if ( load_board_check_update()) {
            // callback to bluetooth firmware to process the update
            chessboard_bl.board_state_changed_callback();
        }

        // Print the state of the board
        print_board();

        // Wait for a second
        ThisThread::sleep_for(800ms);
    }
}
