#include "bluetooth.h"

ChessboardBLE::ChessboardBLE(BLE &ble, events::EventQueue &event_queue) :
    _ble(ble),
    _event_queue(event_queue),
    _adv_data_builder(_adv_buffer),
    _board_state()
{}

void ChessboardBLE::start() {
    // mbed will call on_init_complete when when ble is ready
    _ble.init(this, &ChessboardBLE::on_init_complete);

    // initialize board state
    update_board_state();

    _event_queue.dispatch_forever();
}

// main.cpp will call this if the board state has been changed
void ChessboardBLE::board_state_changed_callback() {
    update_board_state(); // update the state var
    process_board_state(); // immediately broadcast the change
}

/** Callback triggered when the ble initialization process has finished */
void ChessboardBLE::on_init_complete(BLE::InitializationCompleteCallbackContext *params) {
    if (params->error != BLE_ERROR_NONE) {
        print_error(params->error, "Ble initialization failed.");
        return;
    }

    print_mac_address();

    start_advertising();
}

void ChessboardBLE::start_advertising() {
    // create advertising parameters and payload
    ble::AdvertisingParameters adv_parameters(
        ble::advertising_type_t::NON_CONNECTABLE_UNDIRECTED,
        ble::adv_interval_t(ble::millisecond_t(1000))
    );

    const uint8_t _vendor_specific_data[7] = { 0x53, 0x50, 0x4C, 0x49, 0x43, 0x45, 0x44 }; // ASCII FOR "SPLICED"
    _adv_data_builder.setManufacturerSpecificData(_vendor_specific_data);

    // the advertising payload that gets sent during advertising
    _adv_data_builder.clear();
    _adv_data_builder.setFlags();
    _adv_data_builder.setName(DEVICE_NAME);

    // independently updateable payload containing the board state (sent with each advertising packet)
    _adv_data_builder.setServiceData(CHESSBOARD_SERVICE_UUID, make_Span(_board_state));

    // ----- ADVERTISING SETUP -----

    ble_error_t error = _ble.gap().setAdvertisingParameters(
        ble::LEGACY_ADVERTISING_HANDLE,
        adv_parameters
    );

    if (error) {
        print_error(error, "_ble.gap().setAdvertisingParameters() failed");
        return;
    }

    error = _ble.gap().setAdvertisingPayload(
        ble::LEGACY_ADVERTISING_HANDLE,
        _adv_data_builder.getAdvertisingData()
    );

    if (error) {
        print_error(error, "_ble.gap().setAdvertisingPayload() failed");
        return;
    }

    // ----- ADVERTISING START -----

    error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

    if (error) {
        print_error(error, "_ble.gap().startAdvertising() failed");
        return;
    }

    // No matter what, 2x a second the board state will be broadcast
    _event_queue.call_every(
        500ms,
        [this]() {
            process_board_state();
        }
    );
}

void ChessboardBLE::update_board_state() {
    // Take board[8][8] and put it into _board_state
    for ( int row = 0; row < 8; ++row ) {
        uint8_t row_int = 0;
        for ( const bool& square : board[row] ) {
            if ( square == true ) {
                row_int += 1;
            }
            row_int = row_int << 1;
        }
        // put resulting int into its row of state var
        _board_state[row] = row_int;
    }
}

void ChessboardBLE::process_board_state() {   
    // update payload with the board state (whether it has changed or not)
    ble_error_t error = _adv_data_builder.setServiceData(CHESSBOARD_SERVICE_UUID, make_Span(_board_state));

    if (error) {
        print_error(error, "_adv_data_builder.setServiceData() failed");
        return;
    }

    /* set the new payload, we don't need to stop advertising */
    error = _ble.gap().setAdvertisingPayload(
        ble::LEGACY_ADVERTISING_HANDLE,
        _adv_data_builder.getAdvertisingData()
    );

    if (error) {
        print_error(error, "_ble.gap().setAdvertisingPayload() failed");
        return;
    }
}

/* Schedule processing of events from the BLE middleware in the event queue. */
void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}
