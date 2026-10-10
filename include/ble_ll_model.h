#ifndef BLE_LL_MODEL_H
#define BLE_LL_MODEL_H

#include <stdbool.h>

/**
 * @brief Simplified 5-state subset model inspired by Bluetooth LE Link Layer (Core Spec v5.4).
 * NOTE: This is an educational architectural model for testing/validation workflows.
 * It excludes Synchronization and Isochronous Broadcasting states.
 */
typedef enum {
    BLE_LL_STATE_STANDBY = 0,
    BLE_LL_STATE_ADVERTISING,
    BLE_LL_STATE_SCANNING,
    BLE_LL_STATE_INITIATING,
    BLE_LL_STATE_CONNECTION
} ble_ll_state_t;

typedef enum {
    BLE_LL_EVENT_START_ADV,
    BLE_LL_EVENT_START_SCAN,
    BLE_LL_EVENT_START_INITIATE,
    BLE_LL_EVENT_CONNECT_REQ_RECEIVED,
    BLE_LL_EVENT_CONNECT_REQ_SENT,
    BLE_LL_EVENT_DISCONNECT,
    BLE_LL_EVENT_STOP
} ble_ll_event_t;

typedef struct {
    ble_ll_state_t current_state;
    unsigned int state_transition_count;
    unsigned int invalid_transition_attempts;
} ble_ll_controller_t;

int ble_ll_init(ble_ll_controller_t *ctrl);
ble_ll_state_t ble_ll_get_state(const ble_ll_controller_t *ctrl);
int ble_ll_process_event(ble_ll_controller_t *ctrl, ble_ll_event_t event);

#endif // BLE_LL_MODEL_H