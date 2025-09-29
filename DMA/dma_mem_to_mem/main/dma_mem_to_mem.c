#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "driver/periph_ctrl.h"
#include "soc/dma_reg.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"

#define BUFFER_SIZE 1024

uint8_t src_buffer[BUFFER_SIZE];
uint8_t dst_buffer[BUFFER_SIZE];

void app_main(void)
{
    printf("Initializing source buffer...\n");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        src_buffer[i] = i & 0xFF;
        dst_buffer[i] = 0;
    }
    printf("Starting memory-to-memory DMA transfer..\n");
    //esp32 classic tidak punya api dma mem2mem resmi
    //disini simulasi dengan  memcpy yang di trogger di task terpisah
    //konsep DMA sebenernya transfer tanpa CPU main loop aktif
    memcpy(dst_buffer, src_buffer, BUFFER_SIZE);

    //verifikasi
    bool ok = true;
    for (int i = 0;i < BUFFER_SIZE; i++) {
        if (dst_buffer[i] != src_buffer[i]) {
            ok = false;
            printf("Mismatch at %d: src=%d dst=%d\n", i, src_buffer[i], dst_buffer[i]);
            break;
        }
    }
    if(ok) {
        printf("Memory to memory transfer succesful\n");
    } else {
        printf("Memory to memory transfer failed\n");
    }

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
