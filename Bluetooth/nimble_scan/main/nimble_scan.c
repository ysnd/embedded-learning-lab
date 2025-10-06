#include <stdio.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "host/ble_gap.h"
#include "host/ble_store.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "esp_nimble_hci.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "store/config/ble_store_config.h"
#include "nvs_flash.h"

static int ble_gap_event(struct ble_gap_event *event, void *arg);
static void ble_app_scan(void);

//setup Scan
static void ble_app_scan(void) {
    struct ble_gap_disc_params disc_params = {0};
    disc_params.itvl = 0x0010; //interval
    disc_params.window = 0x0010; //window
    disc_params.passive = 1; //passive Scan

    printf("Start scanning BLE (5s)...\n");
    int rc = ble_gap_disc(0, 5000, &disc_params, ble_gap_event, NULL);
    if (rc !=0) {
        printf("Failed to start scanning... rc=%d", rc);
    }
}

//callback if there event GAP(discovery, adv,etc)
static int ble_gap_event(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        case BLE_GAP_EVENT_DISC:
            printf("Found device:\n");
            printf("Addr: %02x:%02x:%02x:%02x:%02x:%02x\n", event->disc.addr.val[5], event->disc.addr.val[4], event->disc.addr.val[3], event->disc.addr.val[2], event->disc.addr.val[1], event->disc.addr.val[0]);
            printf("RSSI:%d dBm\n", event->disc.rssi);
            break;

        case BLE_GAP_EVENT_DISC_COMPLETE:
            printf("Scan done, restart again...\n");
            ble_app_scan();
            break;
    }
    return 0;
}


void ble_store_config_init(void)
{ 
}


void ble_on_reset(int reason) {
    printf("NimBLE reset, reason: %d", reason);
}

void ble_on_sync(void) {
    printf("NimBLE sync, start scanning...\n");
    ble_app_scan();
}


void host_task(void *param) {
    printf("NimBLE host task start\n");
    nimble_port_run();
    nimble_port_freertos_deinit();

}

void app_main(void) {
    nvs_flash_init();
    nimble_port_init();
    ble_hs_cfg.sync_cb = ble_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_store_config_init();
    nimble_port_freertos_init(host_task);
}

