#include <stdint.h>
#include "stm32f401re_gpio.h"
#include "stm32f401re_rcc.h"

#define GPIO_PIN_SET        1
#define GPIO_PIN_RESET      0

// LED & Buzzer
#define LED_RED_PORT        GPIOA
#define LED_RED_PIN         GPIO_Pin_1

#define LED_BLUE_PORT       GPIOA
#define LED_BLUE_PIN        GPIO_Pin_10

#define BUZZER_PORT         GPIOC
#define BUZZER_PIN          GPIO_Pin_9

// Nút
#define BUTTON_B3_PORT      GPIOA
#define BUTTON_B3_PIN       GPIO_Pin_4

#define BUTTON_B4_PORT      GPIOB
#define BUTTON_B4_PIN       GPIO_Pin_0

void delay(uint32_t time) {
    for (volatile uint32_t i = 0; i < time; i++);
}

void GPIO_Set(GPIO_TypeDef *port, uint16_t pin, uint8_t status) {
    if (status == GPIO_PIN_SET) {
        port->BSRRL = pin;
    } else {
        port->BSRRH = pin;
    }
}

uint8_t Button_Read(GPIO_TypeDef *port, uint16_t pin) {
    return (port->IDR & pin) ? 1 : 0;
}

void GPIO_Init_All(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable clock for GPIO ports A, B, C
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // LED & Buzzer: Output
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    // PA1, PA10, PC9
    GPIO_InitStructure.GPIO_Pin = LED_RED_PIN;
    GPIO_Init(LED_RED_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LED_BLUE_PIN;
    GPIO_Init(LED_BLUE_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);

    // Buttons: Input Pull-Up
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_InitStructure.GPIO_Pin = BUTTON_B3_PIN;
    GPIO_Init(BUTTON_B3_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BUTTON_B4_PIN;
    GPIO_Init(BUTTON_B4_PORT, &GPIO_InitStructure);
}

void Blink_LED(GPIO_TypeDef *port, uint16_t pin, int times) {
    for (int i = 0; i < times; i++) {
        GPIO_Set(port, pin, GPIO_PIN_SET);
        delay(500000);
        GPIO_Set(port, pin, GPIO_PIN_RESET);
        delay(500000);
    }
}

void BeepAndBlink(GPIO_TypeDef *led_port, uint16_t led_pin, GPIO_TypeDef *buzzer_port, uint16_t buzzer_pin, int times) {
    for (int i = 0; i < times; i++) {
        GPIO_Set(led_port, led_pin, GPIO_PIN_SET);
        GPIO_Set(buzzer_port, buzzer_pin, GPIO_PIN_SET);
        delay(500000);
        GPIO_Set(led_port, led_pin, GPIO_PIN_RESET);
        GPIO_Set(buzzer_port, buzzer_pin, GPIO_PIN_RESET);
        delay(500000);
    }
}

int main(void) {
    GPIO_Init_All();

    // LED RED nháy 2 lần khi khởi động
    Blink_LED(LED_RED_PORT, LED_RED_PIN, 2);

    uint8_t B4_last_state = 1;
    int B4_press_count = 0;

    int B3_hold_time = 0;

    while (1) {
    	// Kiểm tra nút B4 nhấn 2 lần
    	        uint8_t B4_state = Button_Read(BUTTON_B4_PORT, BUTTON_B4_PIN);
    	        if (B4_state == 0 && B4_last_state == 1) { // Phát hiện nhấn mới
    	            B4_press_count++;
    	            delay(100000); // chống dội phím
    	        }
    	        B4_last_state = B4_state;

    	        if (B4_press_count == 2) {
    	            BeepAndBlink(LED_BLUE_PORT, LED_BLUE_PIN, BUZZER_PORT, BUZZER_PIN, 2);
    	            B4_press_count = 0;
    	        }

    	        // Kiểm tra giữ nút B3
    	        if (Button_Read(BUTTON_B3_PORT, BUTTON_B3_PIN) == 0) {
    	            B3_hold_time++;
    	            delay(10000);
    	            if (B3_hold_time > 50) { // Tương đương ~500ms
    	                GPIO_Set(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_SET);
    	            }
    	        } else {
    	            B3_hold_time = 0;
    	            GPIO_Set(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
    	        }
    	    }

    	    return 0;
    }
