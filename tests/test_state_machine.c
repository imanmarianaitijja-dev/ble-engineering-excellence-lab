#include "ble_ll_model.h"
#include <stdio.h>
#include <assert.h>

static void test_null_pointer_safety(void) {
    assert(ble_ll_init(NULL) == -1);
    assert(ble_ll_get_state(NULL) == BLE_LL_STATE_STANDBY);
    assert(ble_ll_process_event(NULL, BLE_LL_EVENT_START_ADV) == -1);
    printf("[PASS] Null pointer guardrails verified.\n");
}

static void test_valid_transitions(void) {
    ble_ll_controller_t ctrl;
    assert(ble_ll_init(&ctrl) == 0);

    // 1. Cycle Advertising : Standby -> Advertising -> Connection -> Standby
    assert(ble_ll_process_event(&ctrl, BLE_LL_EVENT_START_ADV) == 0);
    assert(ble_ll_get_state(&ctrl) == BLE_LL_STATE_ADVERTISING);

    assert(ble_ll_process_event(&ctrl, BLE_LL_EVENT_CONNECT_REQ_RECEIVED) == 0);
    assert(ble_ll_get_state(&ctrl) == BLE_LL_STATE_CONNECTION);

    assert(ble_ll_process_event(&ctrl, BLE_LL_EVENT_DISCONNECT) == 0);
    assert(ble_ll_get_state(&ctrl) == BLE_LL_STATE_STANDBY);

    // 2. Cycle Scanning : Standby -> Scanning -> Standby
    assert(ble_ll_process_event(&ctrl, BLE_LL_EVENT_START_SCAN) == 0);
    assert(ble_ll_get_state(&ctrl) == BLE_LL_STATE_SCANNING);

    assert(ble_ll_process_event(&ctrl, BLE_LL_EVENT_STOP) == 0);
    assert(ble_ll_get_state(&ctrl) == BLE_LL_STATE_STANDBY);

    // 3. Cycle Initiating : Standby -> Initiating -> Connection -> Standby
    assert(ble_ll_process_event(&ctrl, BLE_LL_EVENT_START_INITIATE) == 0);
    assert(ble_ll_get_state(&ctrl) == BLE_LL_STATE_INITIATING);

    assert(ble_ll_process_event(&ctrl, BLE_LL_EVENT_CONNECT_REQ_SENT) == 0);
    assert(ble_ll_get_state(&ctrl) == BLE_LL_STATE_CONNECTION);

    assert(ble_ll_process_event(&ctrl, BLE_LL_EVENT_DISCONNECT) == 0);
    assert(ble_ll_get_state(&ctrl) == BLE_LL_STATE_STANDBY);

    assert(ctrl.state_transition_count == 8);
    printf("[PASS] Nominal state transitions (Adv, Scan, Init, Conn) verified.\n");
}

static void test_invalid_transition_robustness(void) {
    ble_ll_controller_t ctrl;
    ble_ll_init(&ctrl);

    // Test A : Tenter une déconnexion pendant l'Advertising
    ble_ll_process_event(&ctrl, BLE_LL_EVENT_START_ADV);
    assert(ble_ll_process_event(&ctrl, BLE_LL_EVENT_DISCONNECT) == -1);
    assert(ble_ll_get_state(&ctrl) == BLE_LL_STATE_ADVERTISING);

    // Test B : Tenter d'envoyer un CONNECT_REQ pendant le Scanning (Il faut être en INITIATING)
    ble_ll_process_event(&ctrl, BLE_LL_EVENT_STOP);
    ble_ll_process_event(&ctrl, BLE_LL_EVENT_START_SCAN);
    assert(ble_ll_process_event(&ctrl, BLE_LL_EVENT_CONNECT_REQ_SENT) == -1);
    assert(ble_ll_get_state(&ctrl) == BLE_LL_STATE_SCANNING);

    assert(ctrl.invalid_transition_attempts == 2);
    printf("[PASS] Invalid transition attempts correctly rejected & counted.\n");
}

int main(void) {
    printf("--- Running BLE Link Layer Model Test Suite ---\n");
    test_null_pointer_safety();
    test_valid_transitions();
    test_invalid_transition_robustness();
    printf("--- All unit tests passed successfully! ---\n");
    return 0;
}