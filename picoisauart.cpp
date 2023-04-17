#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"


// uart
#define UART_ID uart0
#define BAUD_RATE 115200
//#define UART_RX_PIN -1

// pins
#define IOW_PIN 4
#define IOR_PIN 5
#define AD0_PIN 6
#define AD1_PIN 7
#define AD2_PIN 8
#define AD3_PIN 9
#define AD4_PIN 10
#define AD5_PIN 11
#define AD6_PIN 12
#define AD7_PIN 13
#define A8_PIN 14
#define A9_PIN 15

#define DACK_PIN 19

#define IRQ_PIN 21

#define IOCHRDY_PIN 26
#define ADS_PIN 27
#define UART_TX_PIN 28


int main() {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    //gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_puts(UART_ID, " Hello, UART!\n");

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_set_dir(ADS_PIN, GPIO_OUT);

    gpio_set_dir(AD0_PIN, GPIO_IN);
    gpio_set_dir(AD1_PIN, GPIO_IN);
    gpio_set_dir(AD2_PIN, GPIO_IN);
    gpio_set_dir(AD3_PIN, GPIO_IN);
    gpio_set_dir(AD4_PIN, GPIO_IN);
    gpio_set_dir(AD5_PIN, GPIO_IN);
    gpio_set_dir(AD6_PIN, GPIO_IN);
    gpio_set_dir(AD7_PIN, GPIO_IN);
    gpio_set_dir(A8_PIN, GPIO_IN);
    gpio_set_dir(A9_PIN, GPIO_IN);

    gpio_set_dir(IOW_PIN, GPIO_IN);
    gpio_set_dir(IOR_PIN, GPIO_IN);

    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);

        gpio_put(ADS_PIN, 0);
        uint a0 = gpio_get(AD0_PIN);
        uint a1 = gpio_get(AD1_PIN);
        uint a2 = gpio_get(AD2_PIN);
        uint a3 = gpio_get(AD3_PIN);
        uint a4 = gpio_get(AD4_PIN);
        uint a5 = gpio_get(AD5_PIN);
        uint a6 = gpio_get(AD6_PIN);
        uint a7 = gpio_get(AD7_PIN);
        uint a8 = gpio_get(A8_PIN);
        uint a9 = gpio_get(A9_PIN);
        gpio_put(ADS_PIN, 1);
        uint d0 = gpio_get(AD0_PIN);
        uint d1 = gpio_get(AD1_PIN);
        uint d2 = gpio_get(AD2_PIN);
        uint d3 = gpio_get(AD3_PIN);
        uint d4 = gpio_get(AD4_PIN);
        uint d5 = gpio_get(AD5_PIN);
        uint d6 = gpio_get(AD6_PIN);
        uint d7 = gpio_get(AD7_PIN);
        char buf[500] = "";
        sprintf(buf, "A0-9:[%d:%d:%d:%d:%d:%d:%d:%d:%d:%d] D0-7:[%d:%d:%d:%d:%d:%d:%d:%d]\n", a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, d0, d1, d2, d3, d4, d5, d6, d7);
        uart_puts(UART_ID, buf);
        printf(".");

        sleep_ms(1000);
    }

    return 0;
}
