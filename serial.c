#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "ssd1306.h" // Biblioteca para o display
#include "ws2812.h"   // Biblioteca para os LEDs WS2812
#include "Modulos/led.h"


ssd1306_t display;
volatile bool led_green_on = false;
volatile bool led_blue_on = false;

// I2C defines
// This example will use I2C0 on GPI14 (SDA) and GPI15 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define BUTTON_A 5
#define BUTTON_B 6
#define LED_R 11
#define LED_G 12
#define LED_B 13
#define WS2812_PIN 7
#define I2C_SDA 14
#define I2C_SCL 15

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

void button_a_irq_handler(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A) {
        led_green_on = !led_green_on;
        gpio_put(LED_G, led_green_on);
        printf("Botão A pressionado: LED Verde %s\n", led_green_on ? "Ligado" : "Desligado");
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, led_green_on ? "LED Verde ON" : "LED Verde OFF");
        ssd1306_show(&display);
    }
}

void button_b_irq_handler(uint gpio, uint32_t events) {
    if (gpio == BUTTON_B) {
        led_blue_on = !led_blue_on;
        gpio_put(LED_B, led_blue_on);
        printf("Botão B pressionado: LED Azul %s\n", led_blue_on ? "Ligado" : "Desligado");
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, led_blue_on ? "LED Azul ON" : "LED Azul OFF");
        ssd1306_show(&display);
    }
}

void setup() {
    stdio_init_all();
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_a_irq_handler);
    
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &button_b_irq_handler);
    
    gpio_init(LED_R);
    gpio_init(LED_G);
    gpio_init(LED_B);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_set_dir(LED_B, GPIO_OUT);
    
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_init(&display, i2c0, 0x3C, 128, 64);
    ssd1306_clear(&display);
    
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
}

void loop() {
    if (uart_is_readable(uart0)) {
        char c = uart_getc(uart0);
        printf("Recebido: %c\n", c);
        ssd1306_clear(&display);
        ssd1306_draw_char(&display, 0, 0, c);
        ssd1306_show(&display);
        
        if (c >= '0' && c <= '9') {
            int num = c - '0';
            ws2812_show_digit(num, WS2812_PIN);
        }
    }
    sleep_ms(100);
}

int main() {
    setup();
    while (true) {
        loop();
    }
}
