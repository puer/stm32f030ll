#include <math.h>
#include <stdio.h>
#include "adc.h"
#include "stm32f0xx_ll_tim.h"
#include "fan.h"

static uint16_t ave[3];

// Generated according to NTC formula Rt = R0 * exp( B* (1/Tt - 1/T0)).
// R0 = 10000, T = 298.15 (Kelvin), B = 3950
// Elimated using of exp or log in math.h to reduce prog. size.
static const uint16_t ntc_3950_tbl[NTC_TBL_CNT] = {
    3594, 3570, 3545, 3520, 3494, 3467, 3440, 3412, 3382, 3353,
    3322, 3291, 3259, 3226, 3193, 3159, 3124, 3089, 3053, 3016,
    2979, 2941, 2903, 2865, 2826, 2786, 2747, 2707, 2666, 2625,
    2585, 2543, 2502, 2461, 2419, 2378, 2337, 2295, 2254, 2212,
    2171, 2130, 2089, 2049, 2008, 1968, 1929, 1889, 1850, 1812,
    1773, 1735, 1698, 1661, 1625, 1589, 1553, 1519, 1484, 1450,
    1417, 1384, 1352, 1321, 1290, 1259, 1229, 1200, 1171, 1143,
    1116, 1089, 1063, 1037, 1011, 987, 963, 939, 916, 894,
    872, 850, 829, 809, 789, 769, 750, 732, 714, 696,
    679, 662, 646, 630, 614, 599, 584, 570, 556, 543,
    529, 516, 504, 492, 480, 468, 457, 446, 435, 425};

static uint16_t ADC_Value[ADBufferSize];

static uint16_t adc_to_celsius(uint16_t av)
{
    for (size_t i = 0; i < NTC_TBL_CNT; i++)
    {
        if (av > ntc_3950_tbl[i])
        {
            return i;
        }
    }
    return 0xffff;
}
// //====================================================================================
// // Description: Bubble sort min to max
// //====================================================================================
// void Sort_values(uint16_t A[], uint8_t L)
// {
//     uint8_t i = 0;
//     uint8_t status = 1;

//     while (status == 1)
//     {
//         status = 0;
//         for (i = 0; i < L - 1; i++)
//         {
//             if (A[i] > A[i + 1])
//             {
//                 A[i] ^= A[i + 1];
//                 A[i + 1] ^= A[i];
//                 A[i] ^= A[i + 1];
//                 status = 1;
//             }
//         }
//     }
// }
uint16_t tmp;
void qsort(uint16_t a[], int lo0, int hi0)
{
    int lo = lo0;
    int hi = hi0;
    int mid;
    if (hi0 > lo0)
    {
        mid = a[(hi0 + lo0) / 2];
        while (lo <= hi)
        {
            while ((lo < hi0) && (a[lo] < mid))
                ++lo;

            while ((hi > lo0) && (a[hi] > mid))
                --hi;

            if (lo <= hi)
            {
                // swap(a, lo, hi);
                // a[lo] ^= a[hi];
                // a[hi] ^= a[lo];
                // a[lo] ^= a[hi];
                tmp = a[lo];
                a[lo] = a[hi];
                a[hi] = tmp;
                ++lo;
                --hi;
            }
        }
        if (lo0 < hi)
        {
            qsort(a, lo0, hi);
        }
        if (lo < hi0)
        {
            qsort(a, lo, hi0);
        }
    }
    return;
}

static uint8_t startup = 0;

int FAN_Adjust_PWM(void)
{
    static uint16_t print_throttle = 0;
    ave[0] = 0;
    ave[1] = 0;
    ave[2] = 0;

    // todo: sort ADC and exclude n mins and n maxs
    size_t count = ADBufferSize / 3;
    size_t samples = count / 2;

    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            ADC_Value[j * count + i] = ADC_ConvertedValue[3 * i + j];
        }
    }

    // Sort_values(ADC_Value, count);
    // Sort_values(&ADC_Value[count], count);
    // Sort_values(&ADC_Value[2 * count], count);
    qsort(ADC_Value, 0, count - 1);
    qsort(&ADC_Value[count], 0, count - 1);
    qsort(&ADC_Value[count * 2], 0, count - 1);

    // do average and pass average to next handler;
    for (int i = samples / 2; i < count - samples / 2; i++)
    {
        ave[0] += ADC_Value[i];
        ave[1] += ADC_Value[i + count];
        ave[2] += ADC_Value[i + 2 * count];
    }

    for (int i = 0; i < 3; i++)
    {
        ave[i] /= samples;
        ;
    }

    // ave[0]  NTC ave[1] SET ave[2] FB
    uint16_t ntc = adc_to_celsius(ave[0]);
    // double set = SET_RANGE * ave[1] / 4096.0 + SET_MIN;
    uint16_t set = SET_RANGE * ave[1] / 4096 + SET_MIN;

    if (print_throttle-- == 0)
    {
        print_throttle = 256;
        // printf("NTC - [%d]  SET - [%d]", (int)ntc, (int)set);
        // printf("NTC - [%dC/%d]  SET - [%dC/%d] PWR - [%ld%%/%lu] FB - [%d%%/%d] \n",
        //        ntc, ave[0],
        //        (int)set, ave[1],
        //        LL_TIM_OC_GetCompareCH1(TIM3) * 100 / 256, LL_TIM_OC_GetCompareCH1(TIM3),
        //        ave[2] * 100 / 4096, ave[2]);
        extern int _write(int file, char *ptr, int len);
        _write(0, "__LL_FAN_", 9);
        _write(0, (char *)&ntc, sizeof(ntc));
        _write(0, (char *)&ave[0], sizeof(ave[0]));
        _write(0, (char *)&set, sizeof(set));
        _write(0, (char *)&ave[1], sizeof(ave[1]));
        // printf("A");

        startup = 0;
    }

    if (startup == 1)
    {
        // skip adj
        return SUCCESS;
    }

    if (ntc > set)
    {
        // adj 77 is base vale of 30% power of fan (256 * 30% = 77).
        // B8025B24H
        // 256 * 0.7 / 30, linear in 30 celsius
        // uint16_t power = (ntc - set) * (256 * (1 - POWER_MIN) / POWER_LINEAR_RANGE) + 256 * POWER_MIN;
        // elimate float computation
        uint16_t power = (ntc - set) * 6 + 102;
        if (power > 255)
        {
            power = 255;
        }

        if (LL_TIM_OC_GetCompareCH1(TIM3) == 0)
        {
            LL_TIM_OC_SetCompareCH1(TIM3, 255);
            //full speed start up
            startup = 1;
        }
        else
        {
            LL_TIM_OC_SetCompareCH1(TIM3, power);
        }
    }
    else if (set - 5 > ntc)
    {
        // Stop the fan
        LL_TIM_OC_SetCompareCH1(TIM3, 0);
    }

    return SUCCESS;
}
