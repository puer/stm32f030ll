#if !defined(__FAN_H)
#define __FAN_H

#define NTC_TBL_CNT 110
#define SET_MIN 20
#define SET_RANGE 60

// 30% power
#define POWER_MIN_PCT 40
#define POWER_LINEAR_RANGE 30
#define POWER_MIN (256 * POWER_MIN_PCT / 100)
#define POWER_LINEAR_FACTOR (256 * (100 - POWER_MIN_PCT) / 100 / POWER_LINEAR_RANGE)

#endif // __FAN_H
