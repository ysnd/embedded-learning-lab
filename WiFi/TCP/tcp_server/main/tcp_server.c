#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi_types_generic.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/err.h"
#include <fcntl.h>
#include "nvs_flash.h"

#define PORT 3333
#define WIFI_SSID "ESP32_server"
#define WIFI_PASS "12344321"

static void wifi_init_soft_ap(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg=WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .password = WIFI_PASS,
            .max_connection = 2,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };
    if (strlen(WIFI_PASS)==0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    printf("Wifi AP started. SSID: %s Password: %s\n", WIFI_SSID, WIFI_PASS);
}

void tcp_server_task(void *pvParameters) {
    char rx_buffer[128];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int server_sock, client_sock;
    int err;

    //create socket tcp 
    server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (server_sock < 0) {
        printf("Unable to create socket: erno %d\n",errno);
        vTaskDelete(NULL);
        return;
    }
    printf("Socket Created\n");

    //set socket to nonblocking mode    
    int flags = fcntl(server_sock, F_GETFL, 0);
    fcntl(server_sock, F_SETFL, flags | O_NONBLOCK);

    //bind SOCKET to port   
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    err = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err != 0) {
        printf("Socket unable to bind: errno %d\n", errno);
        close(server_sock);
        vTaskDelete(NULL);
        return;
    }
    printf("Socket bound, port %d\n", PORT);

    //listen for connection in  
    err = listen(server_sock, 1);
    if (err != 0) {
        printf("Error during listen: erno %d\n", errno);
        close(server_sock);
        vTaskDelete(NULL);
        return;
    }
    printf("Socket listening... on 192.168.4.1 : %d\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                //there no connection in for now
                vTaskDelay(pdMS_TO_TICKS(100));
                continue;
            } else {
                printf("Accept failed: errno %d", errno);
                break;
            }
        }
        printf("Client connected\n");
        //set socket to nonblocking
        int client_flags = fcntl(client_sock, F_GETFL, 0);
        fcntl(client_sock, F_SETFL, client_flags | O_NONBLOCK);

        while (1) {
            int len = recv(client_sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len > 0) {
                rx_buffer[len] = 0;
                printf("Received %d bytes: %s\n", len, rx_buffer);

                //send back (echo)
                int to_write = len;
                while (to_write > 0) {
                    int written = send(client_sock, rx_buffer, to_write, 0);
                    if (written > 0) {
                        to_write -=written;
                    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        //socket busy wait a second
                        vTaskDelay(pdMS_TO_TICKS(50));
                    } else {
                        printf("Send failed: errno %d\n",errno);
                        break;
                    }
                }
            } else if (len == 0) {
                printf("Client disconected\n");
                close(client_sock);
                break;
            } else {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    //no data
                    vTaskDelay(pdMS_TO_TICKS(100));
                    continue;
                } else {
                    printf("Recv failed : errno %d\n", errno);
                    close(client_sock);
                    break;
                }
            }
        }
    }

    if (server_sock != -1) {
        printf("Shutting down server socket\n");
        shutdown(server_sock, 0);
        close(server_sock);
    }
    vTaskDelete(NULL);
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_soft_ap();
    printf("Starting TCP Server on port %d\n", PORT);
    xTaskCreate(tcp_server_task, "tcp_server_task", 4096, NULL, 5, NULL);
}
            
