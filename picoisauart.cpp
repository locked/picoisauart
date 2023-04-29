#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/pio.h"
#include "isa.pio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"


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

#define UART_TX_PIN 28

#define SDA_PIN 17
#define SCL_PIN 16


char buf[50] = "";
uint address, last_address, last_data, all, data = 0;
uint last_io, io = 0;
uint code_index = 0;
uint codes[200] = {};
uint sm;


void iow_isr(void) {
	uint32_t iow_read = pio_sm_get(pio0, sm);
	uint16_t port = (iow_read >> 8) & 0x00FF;
	if (port == 0x80) {
		uint16_t data = iow_read & 0x00FF;
		codes[code_index] = data;
		code_index += 1;
	}

    // pio_interrupt_clear(pio0, pio_intr_sm0_rxnempty_lsb);
    irq_clear(PIO0_IRQ_0);
}


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

	const uint LED_PIN = PICO_DEFAULT_LED_PIN;
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	uart_puts(UART_ID, "==START==\r\n");

	gpio_put(LED_PIN, 1);

	// waggle ADS to set BUSOE latch
    gpio_init(ADS_PIN);
    gpio_set_dir(ADS_PIN, GPIO_OUT);
    gpio_put(ADS_PIN, 1);
    busy_wait_ms(10);
    gpio_put(ADS_PIN, 0);

    gpio_set_slew_rate(ADS_PIN, GPIO_SLEW_RATE_FAST);

   	uint64_t last_display = to_us_since_boot(get_absolute_time());
	uint64_t now = 0;
	uint i, j = 0;
	uint last_index = 0;

    uint offset = pio_add_program(pio0, &isa_program);
    sm = pio_claim_unused_sm(pio0, true);
    isa_program_init(pio0, sm, offset);

    irq_set_enabled(PIO0_IRQ_0, false);
    pio_set_irq0_source_enabled(pio0, pis_sm0_rx_fifo_not_empty, true);
    irq_set_priority(PIO0_IRQ_0, PICO_HIGHEST_IRQ_PRIORITY);
    irq_set_exclusive_handler(PIO0_IRQ_0, iow_isr);
    irq_set_enabled(PIO0_IRQ_0, true);


	while (true) {
		i += 1;
		if (i > 10000) {
			i = 0;
			now = to_us_since_boot(get_absolute_time());
			if (now - last_display > 1000000) {
				for (j=last_index; j<code_index; j++) {
					sprintf(buf, "D[%02X]\r\n", codes[j]);
					uart_puts(UART_ID, buf);
				}
				last_index = j;

				if (code_index >= 3) {
					sprintf(buf, "%02X => %02X => %02X ", codes[code_index-3], codes[code_index-2], codes[code_index-1]);
					lcd_set_cursor(0, 0);
					lcd_string(buf);
				}
			}
		}
	}

	/*
	 * Slow method
	uint io_pin = IOW_PIN;
	last_io = gpio_get(io_pin);
	while (true) {
		last_io = io;
		io = gpio_get(io_pin);
		if (last_io == 1 && io == 0) {	// falling IOW
			all = gpio_get_all();
			address = (all >> 6) & 0xFF;

			gpio_put(ADS_PIN, 1);
			gpio_put(ADS_PIN, 1);
			gpio_put(ADS_PIN, 1);
			gpio_put(ADS_PIN, 1);
			all = gpio_get_all();
			data = (all >> 6) & 0xFF;

			gpio_put(ADS_PIN, 0);

			if (address == 0x80 && data != 0x80) {
				sprintf(buf, "D[%02X] D[%02X]\r\n", last_data, data);
				uart_puts(UART_ID, buf);
				//line = (line + 1) % 2;
				last_data = data;
			}
		}
		i += 1;
		if (i > 50000) {
			i = 0;
			now = to_us_since_boot(get_absolute_time());
			if (now - last_display > 20000000) {
				lcd_set_cursor(0, 0);
				lcd_string(buf);
				last_display = now;
			}
		}
	}
	*/

	return 0;
}
