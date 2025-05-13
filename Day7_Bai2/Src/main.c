#include <stdint.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_rcc.h>

#define LOW                 0
#define BTN_PRESS           LOW

#define GPIO_PIN_SET        1
#define GPIO_PIN_RESET      0
#define GPIO_PIN_LOW        0
#define GPIO_PIN_HIGH       1

// SỬA LẠI: Dùng số chân (5, 13), KHÔNG dùng GPIO_Pin_X (bit mask)
#define LED_GPIO_PORT       GPIOA
#define LED_GPIO_PIN        5
#define LEDControl_Clock    RCC_AHB1Periph_GPIOA

#define BUTTON_GPIO_PORT    GPIOC
#define BUTTON_GPIO_PIN     13
#define BUTTONControl_Clock RCC_AHB1Periph_GPIOC

void delay() {
    for (volatile uint32_t i = 0; i < 500000; i++);
}

static void Led_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(LEDControl_Clock, ENABLE);

    GPIO_InitStructure.GPIO_Pin = (1 << LED_GPIO_PIN);   // Sử dụng bit mask
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
}

static void Button_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(BUTTONControl_Clock, ENABLE);

    GPIO_InitStructure.GPIO_Pin = (1 << BUTTON_GPIO_PIN);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStructure);
}

static void LedControl_SetStatus(GPIO_TypeDef *GPIOx, uint8_t pin_number, uint8_t Status) {
    if (Status == GPIO_PIN_SET) {
        GPIOx->BSRRL = (1 << pin_number); // Set pin
    } else {
        GPIOx->BSRRH = (1 << pin_number); // Reset pin
    }
}

static uint8_t ButtonRead_Status(GPIO_TypeDef *GPIOx, uint8_t pin_number) {
    uint32_t Read_Pin = (GPIOx->IDR >> pin_number) & 0x01;
    return (uint8_t)Read_Pin;
}

int main(void) {
    uint8_t status = 1;
    Led_init();
    Button_init();

    while (1) {
        if (ButtonRead_Status(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN) == BTN_PRESS) {
            delay(); // debounce
            while (ButtonRead_Status(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN) == BTN_PRESS); // đợi nhả nút
            status++;
        }

        if (status % 2 == 0) {
            LedControl_SetStatus(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_HIGH);
        } else {
            LedControl_SetStatus(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_LOW);
        }
    }
    return 0;
}
