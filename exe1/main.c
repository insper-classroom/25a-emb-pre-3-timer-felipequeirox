#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile bool flag_f_r = false;
volatile bool flag_btn_fall = false;

repeating_timer_t timer_0;

// Seta Flag -> Indicar Que Sinal Ocorreu
bool timer_0_callback(repeating_timer_t *rt) {
    flag_f_r = true;
    return true;
}

// Seta Flag -> Button Fall Ocorreu ou Não
void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        flag_btn_fall = true;
    }
}

int main() {

    bool contando = false;

    stdio_init_all();

    // Setup LED
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    // Setup Botão
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    while (true) {

        // Trata o Botão
        if (flag_btn_fall) {

            flag_btn_fall = false; 

            contando = !contando;

            if (contando) {
                add_repeating_timer_ms(500, timer_0_callback, NULL, &timer_0);
            } else {
                cancel_repeating_timer(&timer_0);
                gpio_put(LED_PIN_R, 0); 
            }
        }

        if (flag_f_r) {
            flag_f_r = false; 
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R)); 
        }
    }
}
