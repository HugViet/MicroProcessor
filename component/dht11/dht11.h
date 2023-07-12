#include "esp_err.h"
#include "hal/gpio_types.h"

#ifndef DHT11_H
#define DHT11_H

#define DHT11_OK 0

int dht11_read(gpio_num_t pin);
int dht11_get_temperature();
int dht11_get_humidity();
//void dht11_readData();
int temp();

#endif
