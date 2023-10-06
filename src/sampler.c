#include <stdio.h>
#include <driver/adc.h>
#include <esp_task_wdt.h>
#include <driver/timer.h>
#include <soc/adc_channel.h>
#include "esp_timer.h"
#include "sampler.h"

// The average value is 1250.
#define averageValue 1250

volatile unsigned int counter = 0;
float startTime = 0;
float stopTime = 0;
volatile int lastSample = 0;
volatile int currSample = 0;
volatile int prevSample = 0;

bool initialized = 0;

void IRAM_ATTR timerISR()
{
    // Note, the following 2 instructions should be called at the beginning of the ISR!

    /* 1: read what caused the interrupt
        interrupts may be grouped, therefore it is possible that another timer
        may have cause the interrupt. Check that it timer 0 the one that caused
        the interrupt.
        Notice that we are accessing registers directly because we do not have access to the ESP-IDF here.
    */
    uint32_t is_t0 = TIMERG0.int_st_timers.t0;

    if (is_t0)
    {
        /* 2: "Clear" the interrupt.
            Generally, you clear it when you enter the routine,
            indicating that you are going to handle whatever is causing the interrupt
            from that moment in time forward.  If another interrupt event occurs during
            your servicing of the initial interrupt you don't want to lose that
            interrupt notification.  If you clear it after you have serviced the interrupt
            and another interrupt event occured in the mean time, you would
            lose that interrupt notification.
            To do that, set the TIMERGN.int_clr_timers.tM structure
        */
        TIMERG0.int_clr_timers.t0 = 1;

        // oldSample gets the old value of newSample
        lastSample = currSample;
        // newSample gets the raw value from adc channel 32 for the esp32
        currSample = adc1_get_raw(ADC1_GPIO32_CHANNEL);

        /* A crossing is easily computed by detecting when the
        previous sample is below the average value and the next sample is above the average (or vice versa).*/

        if (currSample > averageValue && lastSample < averageValue)
        {
            counter++;
        }
        // going from low to high
        if (currSample < averageValue && lastSample > averageValue)
        {
            counter++;
        }

        // ebug with ets_printf()

        // TIMERG0.int_ena.t0 = 1;

        /* Re-enable the alarm:
            After the alarm has been triggered we need enable it again,
            so it is triggered the next time.
        */
        TIMERG0.hw_timer[0]
            .config.alarm_en = TIMER_ALARM_EN;
    }
}

void startSampling()
{

    if (initialized == 0)
    {
        // start ADC
        adc_power_acquire();

        // configure pin
        // adc_gpio_init(ADC_UNIT_1, ADC1_GPIO32_CHANNEL);

        // use full 12 bits width
        adc1_config_width(ADC_WIDTH_BIT_12);

        // set attenuation to support full scale voltage
        adc1_config_channel_atten(ADC1_GPIO32_CHANNEL, ADC_ATTEN_DB_11);

        // let's setup a pull-up on that pin
        // this must be called after initialisation of the ADC or it will be ignored
        gpio_pullup_en(32);

        timer_config_t timerConfig;
        // auto-reload the timer = resets the timer when the counter is reached
        timerConfig.auto_reload = TIMER_AUTORELOAD_EN;
        // timer is stopped
        timerConfig.counter_en = TIMER_PAUSE;
        // timer counts up
        timerConfig.counter_dir = TIMER_COUNT_UP;
        // divisor of the incoming 80 MHz APB_CLK clock
        // max is 65536 , esp-32 har ADC som kan klarar av max 65536. vilket är 16-bitar omvandlare
        // 2KHz kommer när vi dividerar 80MHz med  timerConfig.divider= 40000
        timerConfig.divider = 1000; // expected freq = 2KHz som motsvarar frekvensen för hela signalen
        // timer interrupt level
        timerConfig.intr_type = TIMER_INTR_LEVEL;
        // enable the alarm
        timerConfig.alarm_en = TIMER_ALARM_EN;

        timer_init(TIMER_GROUP_0, TIMER_0, &timerConfig);

        // set the initial value of the counter to 0
        timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);

        // using a counter of 2000, the alarm should be raised at 1 Hz
        timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 4);

        // enable interrupts
        timer_enable_intr(TIMER_GROUP_0, TIMER_0);

        // register the ISR
        timer_isr_register(TIMER_GROUP_0, TIMER_0, timerISR, NULL, ESP_INTR_FLAG_IRAM, NULL);
        // in flags, you can use ESP_INTR_FLAG_IRAM if you want to use on-chip RAM
        // that's faster, but, you will lose access to the rest of the ESP-IDF
        // you can also pass arguments to the ISR, but we are not doing it here

        // start!
        timer_start(TIMER_GROUP_0, TIMER_0);

        initialized = 1;
    }
    else
    {
        timer_start(TIMER_GROUP_0, TIMER_0);
    }

    // needs to get frequency
    startTime = esp_timer_get_time();

    counter = 0;
}

float getFrequency()
{
    // printf("starttimer %.2f\n", startTime);
    // printf("stoptimer is %.2f\n", stopTime);

    float samplingTime = ((stopTime - startTime) / 1000); // /1 000 000 to convert from MHz to Hz
    // printf("samplingtime is %.2f\n", samplingTime);
    //printf("counter is %d\n", counter);
    // enheten är sampel / sekund villket betyder att counter är antal crossing och (2* signal duration)
    float frequency = (counter * 1000) / (2 * samplingTime);

    // printf("Frequency2 is %.2f\n", frequency);

    return frequency;
}

void stopSampling()
{
    timer_pause(TIMER_GROUP_0, TIMER_0);
    // stop ADC
    // adc_power_release();
    // adc_power_off();
    //  needs to get frequency
    stopTime = esp_timer_get_time();
}

