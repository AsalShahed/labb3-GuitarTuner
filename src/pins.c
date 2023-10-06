#include "pins.h"
#include "driver/gpio.h"
#include <esp_task_wdt.h>

/* initialises the LED pin */
void initLEDPin(uint8_t pinN)
{
    gpio_config_t configLed;
    configLed.pin_bit_mask = (u_int64_t)1 << pinN;
    configLed.mode = GPIO_MODE_OUTPUT;
    configLed.pull_down_en = 0;
    configLed.pull_up_en = 0;
    gpio_config(&configLed);
}

/* initialises the button pin */
void initButtonPin(uint8_t pinN)
{
    gpio_config_t configButton;
    configButton.pin_bit_mask = (u_int64_t)1 << pinN;
    configButton.mode = GPIO_MODE_INPUT;
    // configure the pin with the internal pullup
    configButton.pull_up_en = 1;
    // enable interrupts.
    configButton.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&configButton);
}

/* switches LED on if level!=0 or off if level==0*/
void setLED(uint8_t pinN, uint8_t level)
{
    if (level)
    {
        gpio_set_level(pinN, 1);
    }
    else
    {
        gpio_set_level(pinN, 0);
    }
}

void stratFlash()
{
    setLED(14, 1);
    setLED(12, 0);
    vTaskDelay(50);
    setLED(14, 0);
    setLED(12, 1);
    vTaskDelay(50);
    setLED(14, 1);
    setLED(12, 0);
    vTaskDelay(50);
    setLED(14, 0);
    setLED(12, 1);
    vTaskDelay(50);
    setLED(14, 1);
    setLED(12, 0);
    vTaskDelay(50);
    setLED(14, 0);
    setLED(12, 1);
    vTaskDelay(50);
}