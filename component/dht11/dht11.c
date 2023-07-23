#include <stdio.h>
#include <string.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include "dht11.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DHT11_DATA_BIT_COUNT 40

static const char *TAG = "DHT11";

static uint8_t dht11_data[5];

static int dht11_read_data(gpio_num_t pin) {
    int i;

    memset(dht11_data, 0, sizeof(dht11_data));

    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    ets_delay_us(20000);
    //vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(pin, 1);
    ets_delay_us(40);
    //vTaskDelay(pdMS_TO_TICKS(40));
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    if (gpio_get_level(pin)) {
        ESP_LOGE(TAG, "DHT11 bus not pulled down");
        dht11_data[2]=0;
        dht11_data[0]=0;
        return -1;
    }


    while (!gpio_get_level(pin))
        ;

    while (gpio_get_level(pin))
        ;

    for (i = 0; i < DHT11_DATA_BIT_COUNT; i++) {
        while (!gpio_get_level(pin))
            ;

        ets_delay_us(30);

        if (gpio_get_level(pin)) {
            dht11_data[i / 8] |= (1 << (7 - (i % 8)));
        }

        while (gpio_get_level(pin))
           ;
    }

    return 0;
}

int dht11_read(gpio_num_t pin) {
    if (dht11_read_data(pin) != 0) {
        return -1;
    }

    if (dht11_data[4] != ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xFF)) {
        ESP_LOGE(TAG, "DHT11 checksum error");
        return -1;
    }

    return DHT11_OK;
}

int dht11_get_temperature() {
    return dht11_data[2];
}

int dht11_get_humidity() {
    return dht11_data[0];
}

// void dht11_readData(){
//      ESP_LOGI(__func__, "Temperature: %d°C\t Humidity: %d%%", dht11_data[2], dht11_data[0]);
// }

// void dht11_readData(int temp, int humi){
//    ESP_LOGI(__func__, "Temperature: %d°C\tHumidity: %d%%", dht11_get_temperature(), dht11_get_humidity());
// }

