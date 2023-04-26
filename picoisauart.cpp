#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"


// commands
const int LCD_CLEARDISPLAY = 0x01;
const int LCD_RETURNHOME = 0x02;
const int LCD_ENTRYMODESET = 0x04;
const int LCD_DISPLAYCONTROL = 0x08;
const int LCD_CURSORSHIFT = 0x10;
const int LCD_FUNCTIONSET = 0x20;
const int LCD_SETCGRAMADDR = 0x40;
const int LCD_SETDDRAMADDR = 0x80;

// flags for display entry mode
const int LCD_ENTRYSHIFTINCREMENT = 0x01;
const int LCD_ENTRYLEFT = 0x02;

// flags for display and cursor control
const int LCD_BLINKON = 0x01;
const int LCD_CURSORON = 0x02;
const int LCD_DISPLAYON = 0x04;

// flags for display and cursor shift
const int LCD_MOVERIGHT = 0x04;
const int LCD_DISPLAYMOVE = 0x08;

// flags for function set
const int LCD_5x10DOTS = 0x04;
const int LCD_2LINE = 0x08;
const int LCD_8BITMODE = 0x10;

// flag for backlight control
const int LCD_BACKLIGHT = 0x08;

const int LCD_ENABLE_BIT = 0x04;

// By default these LCD display drivers are on bus address 0x27
static int addr = 0x27;

// Modes for lcd_send_byte
#define LCD_CHARACTER  1
#define LCD_COMMAND    0

#define MAX_LINES      2
#define MAX_CHARS      16

/* Quick helper function for single byte transfers */
void i2c_write_byte(uint8_t val) {
#ifdef i2c_default
    i2c_write_blocking(i2c_default, addr, &val, 1, false);
#endif
}

void lcd_toggle_enable(uint8_t val) {
    // Toggle enable pin on LCD display
    // We cannot do this too quickly or things don't work
#define DELAY_US 600
    sleep_us(DELAY_US);
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
    i2c_write_byte(val & ~LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
}

// The display is sent a byte as two separate nibble transfers
void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}

void lcd_clear(void) {
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

// go to location on LCD
void lcd_set_cursor(int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND);
}

static void inline lcd_char(char val) {
    lcd_send_byte(val, LCD_CHARACTER);
}

void lcd_string(const char *s) {
    while (*s) {
        lcd_char(*s++);
    }
}

void lcd_init() {
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear();
}




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

#define SDA_PIN 17
#define SCL_PIN 16

int main() {
	uart_init(UART_ID, BAUD_RATE);
	gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
	//gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);


	i2c_init(i2c_default, 100 * 1000);

	gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(SDA_PIN);
	gpio_pull_up(SCL_PIN);
	// Make the I2C pins available to picotool
	bi_decl(bi_2pins_with_func(SDA_PIN, SCL_PIN, GPIO_FUNC_I2C));

	lcd_init();
	//lcd_set_cursor(0, 0);
	//lcd_string("START");

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
	uint address, last_address, all, data = 0;
	uint last_iow = 0;
	uint line = 0;

	gpio_put(ADS_PIN, 0);
	sleep_us(1);
	gpio_put(ADS_PIN, 1);
	sleep_us(1);
	gpio_put(ADS_PIN, 0);

	uart_puts(UART_ID, "==START==\r\n");

	gpio_put(LED_PIN, 1);
	last_iow = gpio_get(IOW_PIN);
	gpio_put(ADS_PIN, 0);
	while (true) {
		last_iow = iow;
		iow = gpio_get(IOW_PIN);
		if (last_iow == 1 && iow == 0) {	// falling IOW
			all = gpio_get_all();
			address = (all >> 6) & 0xFF;

			//sprintf(buf, "FALLING IOR [%02X]\r\n", address);
			//uart_puts(UART_ID, buf);

			gpio_put(ADS_PIN, 1);
			gpio_put(ADS_PIN, 1);
			gpio_put(ADS_PIN, 1);
			all = gpio_get_all();
			data = (all >> 6) & 0xFF;

			gpio_put(ADS_PIN, 0);

			//if (last_address != address) {
			if (address == 0x80 && data != 0x80) {
				sprintf(buf, "D[%02X]\r\n", data);
				//lcd_set_cursor(line, 0);
				//lcd_string(buf);
				uart_puts(UART_ID, buf);
				//line = (line + 1) % 2;
			}

			//sleep_ms(500);
		}
	}

	return 0;
}
