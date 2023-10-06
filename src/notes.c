#include <stdio.h>
#include "notes.h"
#include <math.h>
#include <string.h>

void freq2note(float freq, char *stringbuff)
{

    int n = abs(lround(12 * log2(freq / 440)));
    n = n % 12; //Vi har 12 notes
    printf(" n: %d\n", n);


    if (freq >= 440)
    {

        if (n == 0)
        {
            stringbuff = "A\0";
        }

        if (n == 1)
        {
            stringbuff = "A#\0";
        }

        if (n == 2)
        {
            stringbuff = "B\0";
        }
    }

    else
    {

        if (n == 1 )
        {
            stringbuff = "G#\0";
        }
        else if (n == 2)
        {
            stringbuff = "G\0";
        }
        else if (n==3)
        {
            stringbuff = "F#\0";
        }
        else if (n == 4)
        {
            stringbuff = "F\0";
        }
        else if (n == 5)
        {
            stringbuff = "E\0";
        }
         else if (n == 6)
        {
            stringbuff = "D#\0";
        }

         else if (n == 7)
        {
            stringbuff = "D\0";
        }
         else if (n == 8)
        {
            stringbuff = "C#\0";
        }
         else if (n == 9)
        {
            stringbuff = "C\0";
        }
    }
    printf("The note is %s\n", stringbuff);

}