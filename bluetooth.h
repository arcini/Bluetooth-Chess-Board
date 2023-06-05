#include "ble/BLE.h"
#include "ble/Gap.h"
#include "pretty_printer.h"
#include <cstdint>
#include <events/mbed_events.h>

#include "board_utils.h"

// ----- GLOBAL CONSTANTS -----
const static char DEVICE_NAME[] = "SPLICED CHESSBOARD";
const static UUID CHESSBOARD_SERVICE_UUID = UUID("9c9adab1-ed38-4fe1-862a-ced7576d91b8"); // DO NOT CHANGE

using namespace std::literals::chrono_literals;

// Global eventqueue for BLE module
static events::EventQueue event_queue(/* event count */ 16 * EVENTS_EVENT_SIZE);

void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context);

// BT Device Class
class ChessboardBLE : ble::Gap::EventHandler {
public:
    ChessboardBLE(BLE &ble, events::EventQueue &event_queue);

    void start();

    // main.cpp will call this if the board state has been changed
    void board_state_changed_callback();

private:
    /** Callback triggered when the ble initialization process has finished */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params);

    void start_advertising();

    void update_board_state();

    void process_board_state();

private:
    BLE &_ble;
    events::EventQueue &_event_queue;

    uint8_t _board_state[8];

    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
};

