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

    gpio_init(ADS_PIN);
    gpio_set_dir(ADS_PIN, GPIO_OUT);

    gpio_init(AD0_PIN);
    gpio_init(AD1_PIN);
    gpio_init(AD2_PIN);
    gpio_init(AD3_PIN);
    gpio_init(AD4_PIN);
    gpio_init(AD5_PIN);
    gpio_init(AD6_PIN);
    gpio_init(AD7_PIN);
    gpio_init(A8_PIN);
    gpio_init(A9_PIN);
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

    gpio_init(IOW_PIN);
    gpio_set_dir(IOW_PIN, GPIO_IN);
    gpio_init(IOR_PIN);
    gpio_set_dir(IOR_PIN, GPIO_IN);

    char buf[500] = "";
    uint a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, d0, d1, d2, d3, d4, d5, d6, d7, iow, ior;

    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);

        gpio_put(ADS_PIN, 0);
        a0 = gpio_get(AD0_PIN);
        a1 = gpio_get(AD1_PIN);
        a2 = gpio_get(AD2_PIN);
        a3 = gpio_get(AD3_PIN);
        a4 = gpio_get(AD4_PIN);
        a5 = gpio_get(AD5_PIN);
        a6 = gpio_get(AD6_PIN);
        a7 = gpio_get(AD7_PIN);
        a8 = gpio_get(A8_PIN);
        a9 = gpio_get(A9_PIN);

        gpio_put(ADS_PIN, 1);
        d0 = gpio_get(AD0_PIN);
        d1 = gpio_get(AD1_PIN);
        d2 = gpio_get(AD2_PIN);
        d3 = gpio_get(AD3_PIN);
        d4 = gpio_get(AD4_PIN);
        d5 = gpio_get(AD5_PIN);
        d6 = gpio_get(AD6_PIN);
        d7 = gpio_get(AD7_PIN);

        iow = gpio_get(IOW_PIN);
        ior = gpio_get(IOR_PIN);

        sprintf(buf, "A0-9:[%u:%u:%u:%u:%u:%u:%u:%u:%u:%u] D0-7:[%u:%u:%u:%u:%u:%u:%u:%u] IOW/R:[%u/%u]\r\n", a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, d0, d1, d2, d3, d4, d5, d6, d7, iow, ior);
        uart_puts(UART_ID, buf);
        printf(".");

        sleep_ms(500);
    }

    return 0;
}
