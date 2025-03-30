#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;
const int BTN_PIN_G = 26;
const int LED_PIN_G = 6;

volatile bool flag_f_r = false;
volatile bool flag_f_g = false;
volatile bool flag_btn_red = false;
volatile bool flag_btn_green = false;

repeating_timer_t timer_0;
repeating_timer_t timer_1;

// Seta Flag -> Indicar Que Sinal Ocorreu
bool timer_0_callback(repeating_timer_t *rt) {
    flag_f_r = true;
    return true;
}

bool timer_1_callback(repeating_timer_t *rt) {
    flag_f_g = true;
    return true;
}

// Seta Flag -> Button Fall Ocorreu ou Não
void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        if(gpio == BTN_PIN_R)
            flag_btn_red = true;
        else if (gpio == BTN_PIN_G)
            flag_btn_green = true;
    }
}

int main() {

    bool contando_r = false;
    bool contando_g = false;

    stdio_init_all();

    // Setup LED
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0);

    // Setup Botão
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    while (true) {

        // Trata o Botão
        if (flag_btn_red) {

            flag_btn_red = false; 

            if (contando_r) {
                cancel_repeating_timer(&timer_0);
                gpio_put(LED_PIN_R, 0); 
            } else {
                add_repeating_timer_ms(500, timer_0_callback, NULL, &timer_0);                
            }

            contando_r = !contando_r;
        }

        if (flag_btn_green) {

            flag_btn_green = false; 

            if (contando_g) {
                cancel_repeating_timer(&timer_1);
                gpio_put(LED_PIN_G, 0); 
            } else {
                add_repeating_timer_ms(250, timer_1_callback, NULL, &timer_1);                
            }

            contando_g = !contando_g;    
        }


        if (flag_f_r) {
            flag_f_r = false; 
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R)); 
        }

        if (flag_f_g) {
            flag_f_g = false; 
            gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G)); 
        }
    }
}
