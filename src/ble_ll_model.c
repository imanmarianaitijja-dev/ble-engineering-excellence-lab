#include "ble_ll_model.h"
#include <stddef.h>

int ble_ll_init(ble_ll_controller_t *ctrl) {
    if (!ctrl) return -1;
    ctrl->current_state = BLE_LL_STATE_STANDBY;
    ctrl->state_transition_count = 0;
    ctrl->invalid_transition_attempts = 0;
    return 0;
}

ble_ll_state_t ble_ll_get_state(const ble_ll_controller_t *ctrl) {
    if (!ctrl) return BLE_LL_STATE_STANDBY;
    return ctrl->current_state;
}

int ble_ll_process_event(ble_ll_controller_t *ctrl, ble_ll_event_t event) {
    if (!ctrl) return -1; // Robustness check against null pointers

    ble_ll_state_t next_state = ctrl->current_state;
    bool valid_transition = false;

    switch (ctrl->current_state) {
        case BLE_LL_STATE_STANDBY:
            if (event == BLE_LL_EVENT_START_ADV) {
                next_state = BLE_LL_STATE_ADVERTISING;
                valid_transition = true;
            } else if (event == BLE_LL_EVENT_START_SCAN) {
                next_state = BLE_LL_STATE_SCANNING;
                valid_transition = true;
            } else if (event == BLE_LL_EVENT_START_INITIATE) {
                next_state = BLE_LL_STATE_INITIATING;
                valid_transition = true;
            }
            break;

        case BLE_LL_STATE_ADVERTISING:
            if (event == BLE_LL_EVENT_CONNECT_REQ_RECEIVED) {
                next_state = BLE_LL_STATE_CONNECTION;
                valid_transition = true;
            } else if (event == BLE_LL_EVENT_STOP) {
                next_state = BLE_LL_STATE_STANDBY;
                valid_transition = true;
            }
            break;

        case BLE_LL_STATE_SCANNING:
            if (event == BLE_LL_EVENT_STOP) {
                next_state = BLE_LL_STATE_STANDBY;
                valid_transition = true;
            }
            break;

        case BLE_LL_STATE_INITIATING:
            if (event == BLE_LL_EVENT_CONNECT_REQ_SENT) {
                next_state = BLE_LL_STATE_CONNECTION;
                valid_transition = true;
            } else if (event == BLE_LL_EVENT_STOP) {
                next_state = BLE_LL_STATE_STANDBY;
                valid_transition = true;
            }
            break;

        case BLE_LL_STATE_CONNECTION:
            if (event == BLE_LL_EVENT_DISCONNECT) {
                next_state = BLE_LL_STATE_STANDBY;
                valid_transition = true;
            }
            break;

        default:
            break;
    }

    if (valid_transition) {
        ctrl->current_state = next_state;
        ctrl->state_transition_count++;
        return 0;
    } else {
        ctrl->invalid_transition_attempts++;
        return -1; // Invalid transition rejected
    }
}