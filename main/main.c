#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_adc_cal.h"
#include "esp_system.h"

#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/i2c.h"
#include "driver/spi_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_freertos_hooks.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "pms7003.h"
#include "dht11.h"
#include "MQ135.h"

#include "../component/TFT_DISPLAY/tft.h"

//UART Config
uart_config_t pms_uart_config = UART_CONFIG_DEFAULT(); // 

//DHT11
#define DHT11_PIN GPIO_NUM_27
uint32_t pm1p0_t, pm2p5_t, pm10_t;
uint32_t temperature, humidity;
uint64_t CO2;

//MQ135
#define DEFAULT_VREF    1100  
#define NO_OF_SAMPLES   64  
static esp_adc_cal_characteristics_t *adc_chars;
#if CONFIG_IDF_TARGET_ESP32
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12; 
#endif
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1; 
uint32_t adc_reading = 0;
uint32_t voltage =0;

//web
#define WEB_SERVER "api.thingspeak.com"
#define WEB_PORT "80"
#define WEB_PATH "/"
static const char *TAG = "Connect";
char REQUEST[512];

//tft
TaskHandle_t updateScreenTask_handle = NULL;
SemaphoreHandle_t updateScreen_semaphore = NULL;
struct label_st label_to_display;

void dht11_readData();
void mq135_readData(int temperature,int humidity);

static void check_efuse(void)
{
#if CONFIG_IDF_TARGET_ESP32
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
#endif
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

void mq135Check_task(void *pvParameter)
{
	printf( "Starting MQ135 Task\n\n");
	while (1) 
    {
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, width, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        //Convert adc_reading to voltage in mV
        voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        vTaskDelay( 2000 / portTICK_RATE_MS );
    }
}

void sensorRead_task(void *pvParameters)
{   
    gpio_set_direction(DHT11_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DHT11_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(2000));
 
    while (1)
    {
        //ESP_ERROR_CHECK_WITHOUT_ABORT(pms7003_initUart(&pms_uart_config));
        pms7003_readData(indoor, &pm1p0_t, &pm2p5_t, &pm10_t); //!= ESP_OK;
        dht11_readData();
        temperature = dht11_get_temperature();
        humidity = dht11_get_humidity();
        mq135_readData(temperature,humidity);
		vTaskDelay( 3000 / portTICK_RATE_MS );
    }
    
}

void httpPush_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[64];
    while(1)
    {
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }


        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));
        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        };

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            continue;
        }

        ESP_LOGI(TAG, "Data pushed to sever");
        freeaddrinfo(res);
        sprintf(REQUEST, "GET https://api.thingspeak.com/update?api_key=VLKYGVR2LAQKZ2E9&field1=%d&field2=%d&field3=%d&field4=%d&field5=%d&field6=%lld\n\n",pm1p0_t,pm2p5_t,pm10_t,humidity,temperature,CO2);
        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG, "... socket send failed");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }
        } while(r > 0);

        close(s);
        vTaskDelay(5000/portTICK_RATE_MS);
    }
}

void updateScreen_task(void *parameters){   
    while(1){
         tft_updateScreen(pm1p0_t,pm2p5_t,pm10_t,humidity,temperature,CO2);
         lv_task_handler();
         vTaskDelay(pdMS_TO_TICKS(3000));
     }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());
    ESP_ERROR_CHECK_WITHOUT_ABORT(pms7003_initUart(&pms_uart_config));
    ESP_ERROR_CHECK_WITHOUT_ABORT(tft_initialize());
    tft_initScreen();
    lv_task_handler();
    vTaskDelay(pdMS_TO_TICKS(10));
    check_efuse();
    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);
    xTaskCreate(mq135Check_task, "Read_voltage", 2048, NULL, 25, NULL );
    xTaskCreate(sensorRead_task,"Sensor_Read",10*1024,NULL,(UBaseType_t)25,NULL); 
    xTaskCreate(httpPush_task,"Http_connect",3*1024,NULL,(UBaseType_t)5,NULL);
    xTaskCreate(updateScreen_task, "Update data on screen", (1024*16), NULL, (BaseType_t)9, &updateScreenTask_handle);
}



void dht11_readData(){
    int ret = dht11_read(DHT11_PIN);
    if (ret == DHT11_OK) {
        ESP_LOGI(__func__, "Temperature: %d°C\tHumidity: %d%%", dht11_get_temperature(), dht11_get_humidity());
    } else {
        printf("Failed to read data from DHT11 sensor\n");
    }
}

void mq135_readData(int temperature,int humidity){
    CO2 = getCorrectedPPM(temperature,humidity,voltage);
    //printf( "CO2 :%f ppm\n",CO2);
    ESP_LOGI(__func__,"CO2: %llddppm\n",CO2);
}
