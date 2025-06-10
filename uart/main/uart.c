#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "hal/uart_types.h"
#include "portmacro.h"

static void IRAM_ATTR uart_task(void* pvParameters)
{
    uint8_t data[6];
    for (;;) {
        data[0] = 20;
        data[1] = 40;
        data[2] = 50;
        data[3] = 60;
        data[4] = 50;
        data[5] = 00;
        printf("Pos 1\n");
        uart_write_bytes(UART_NUM_0, (const char*)data, 6);
        //vTaskDelay(1000/portTICK_PERIOD_MS);
        data[0] = 50;
        data[1] = 20;
        data[2] = 70;
        data[3] = 20;
        data[4] = 40;
        data[5] = 40;
        printf("Pos 2\n");
        uart_write_bytes(UART_NUM_0, (const char*)data, 6);
        //vTaskDelay(1000/portTICK_PERIOD_MS);
    }

}
static void uart_init() {
    uart_port_t uart_num = UART_NUM_0;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    
    // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(uart_num, 1, 3, -1, -1));

    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, \
                                            uart_buffer_size, 10, &uart_queue, 0));
    xTaskCreate(uart_task, "uart_motor", 1024, NULL, 5, NULL);
}
void app_main(void)
{
    //uart_init();
    for(;;)
    {
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}
