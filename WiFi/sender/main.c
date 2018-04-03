
#include <stdio.h>
#include<string.h>    //strlen
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "nvs_flash.h"

#define SSID "ESP_32"
#define PASSPHARSE "12345678"
#define MESSAGE "HelloTCPServer"
#define TCPServerIP "192.168.1.1"

static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
static const char *TAG="tcp_client";
void wifi_connect(){
    wifi_config_t cfg = {
        .sta = {
            .ssid = SSID,
            .password = PASSPHARSE,
        },
    };
    ESP_ERROR_CHECK( esp_wifi_disconnect() );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg) );
    ESP_ERROR_CHECK( esp_wifi_connect() );
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    esp_log_level_set("wifi", ESP_LOG_NONE); // disable wifi driver logging
    tcpip_adapter_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

void tcp_client(void *pvParam){
    ESP_LOGI(TAG,"tcp_client task started \n");
    struct sockaddr_in tcpServerAddr;
    tcpServerAddr.sin_addr.s_addr = inet_addr(TCPServerIP);
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons( 3000 );
    int sc;
    int rs;
    uint8_t data_send[1000];
    int global_cnt = 0, count_send = 0;

    int sum = 0;

    // Fill array with data that makes senses.
    for (int i = 0; i < 1000; i++) {
        data_send[i] = 5;
        sum += 5;
    }

    printf("Expected sum: %d", sum);

    while(1){
        xEventGroupWaitBits(wifi_event_group,CONNECTED_BIT,false,true,portMAX_DELAY);
        sc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Try to allocate an socket on internal memory.
        if(sc < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.\n");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket\n");
         if(connect(sc, (struct sockaddr *)&tcpServerAddr, sizeof(tcpServerAddr)) != 0) { // Connect to the TCP server.
            ESP_LOGE(TAG, "... socket connect failed errno=%d \n", errno);
            close(sc);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... connected \n"); // Connected to the TCP server.
        /**if( write(s , MESSAGE , strlen(MESSAGE)) < 0) // If the amount of bytes send is smaller than zero, something went wrong
        {
            ESP_LOGE(TAG, "... Send failed \n");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success"); // Something has successfully been send.
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }
        } while(r > 0);
        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
        close(s);
        ESP_LOGI(TAG, "... new request in 5 seconds");
        vTaskDelay(5000 / portTICK_PERIOD_MS);**/
        while (1) {
                    //rs = write(sc,data_send,1000); // 0.5kbyte sending.
            rs = send(sc, data_send, sizeof(data_send), 0);
            if(rs <0){
                 //printf("Send retv < 0");
              }
              else if(rs == 0){
                 //printf("Send retv = 0");
              }
              else if(rs>0)
              {
                 //count_send++;
                count_send += rs;
                 if(count_send > 10000000){ // 10Mbytes((1kbyte * 10000)) sending 
                    //global_cnt++;
                    //printf("Sended bytes = 10 Mbytes, count=%d \n",global_cnt);
                    count_send = 0;
                    //data_send[0] = (uint8_t)global_cnt;
                 }
              }
        }

    }
    ESP_LOGI(TAG, "...tcp_client task closed\n");
}

void app_main()
{   
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_event_group = xEventGroupCreate();
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    initialise_wifi();
    xTaskCreate(&tcp_client,"tcp_client",4048,NULL,5,NULL);
}