#include <stdio.h>
#include <esp_task_wdt.h>
#include "driver/gpio.h"
#include "notes.h"
#include "sampler.h"
#include "pins.h"


#define Button_PIN 18
#define PUSH_TIME_US 250000 // 250 ms
#define ESP_INTR_FLAG_DEFAULT 0


static volatile int buttonCounter = 1;
static volatile uint64_t lastPush = PUSH_TIME_US;


static void buttonPress_handler(void *arg)
{
    //if enough time has passed since the button was last pressed
    uint64_t now = esp_timer_get_time();

    if ((now - lastPush) > PUSH_TIME_US)
    {
        if(buttonCounter < 6){
            // go to the next note
            buttonCounter++; 
        }
        
        else if (buttonCounter == 6) //vi har bara 6 Note..
        {
            buttonCounter = 1; //all strings have been tuned --> start over!
        }

        lastPush = now;
    }

}


//idean är att skriva en metod som tar emot freqvensen och jämförar frq med tabellen för att få fram vilken note från tabellen.
//logiken på spelet ska också skrivas in i metoden, alltså när lamporna ska släkas och tändas ifall något note är större eller mindre 

void game(float fr1, float notefreq1){
    //För 3:e fallet, ifall de är plus/minus 3 Hz
    float delta = abs(fr1 - notefreq1);
    
    if(fr1 > notefreq1){

        setLED(12,1);
        setLED(14,0);

    }
    else if(fr1 < notefreq1){
        setLED(12,0);
        setLED(14,1);
    }
   else if(fr1 == notefreq1 || delta < 3.0 || delta > 3.0 ){
        setLED(12,1);
        setLED(14,1);
    }

}

void app_main()
{

    // init LEDs and button pins
    initLEDPin(12);
    initLEDPin(14);
    initButtonPin(Button_PIN);

    // signal the user that the game is start
    stratFlash();

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(Button_PIN, buttonPress_handler, NULL);

    while (1)
    {
        printf("--- Start!\n");
        startSampling();
        vTaskDelay(pdMS_TO_TICKS(5000));
        stopSampling();
        float fr = getFrequency();
        printf("Frequency is %.2f\n", fr);

    
        //char note[] = "***";//när vi inte har någon freq som motsvarar en note
        //freq2note(fr, note);
        //printf("Note is %s\n", note);


        switch (buttonCounter)
        {
        case 1 /* Note E4 329.63 Hz */:
            game(fr, 329.63);
            printf("Note E4  %.2f\n", 329.63);
            break;
        
        case 2:
            game(fr, 246.94);
            printf("Note B3  %.2f\n", 246.94);
            break;

        case 3 /* Note G3 */:
            game(fr, 196.00);
            printf("Note G3 %.2f\n", 196.00);
            break;

        case 4 /* Note D3 */:
            game(fr, 146.83);
            printf("Note D3  %.2f\n", 146.83);
            break;

        case 5/* Note A2 */:
            game(fr, 110.00);
            printf("Note A2  %.2f\n", 110.00);
            break;

        case 6/* Note E2 */:
            game(fr, 82.41);
            printf("Note E2  %.2f\n", 82.41);
            break;
        }

    }
}



/*void app_main()
{

    while (1)
    {
        printf("--- Start!\n");
        // sampling at 6kHz, max detectable freq = 3kHz
        startSampling();
        // wait 1 second
        vTaskDelay(pdMS_TO_TICKS(1000)); // alternative 500 som motsvarar 500ms
        stopSampling();
        printf("--- Stop!\n");
        float fr = getFrequency();
        printf("Frequency is %.2f\n", fr);
    }
}*/
/*void app_main()
{

    while (1)
    {
        printf("--- Start!\n");
        startSampling();
        vTaskDelay(pdMS_TO_TICKS(5000));
        stopSampling();
        float fr = getFrequency();
        printf("Frequency is %.2f\n", fr);
        char note[] = "***";
        freq2note(fr, note);
        // printf("Note is %s\n", note);
    }
}*/