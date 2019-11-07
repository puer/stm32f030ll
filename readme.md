## T-SET 10K POT 计算

10K 电阻的调整是线性的，但是窜连了 1K 电阻之后，分压的就不线性了。根据分压公式 计算出电阻的值，为用户保持线性感。

在 20 到 80 之间可调

R = U / (3.3-U) (Kohms)

0 - 10K => 20 ~ 80 C

Target = 60 \* READING / 4096 + 293.15 (K)

## NTC 电阻电压温度的计算

B = 3950, (实际测定 B = 4100??)

R = 4700U / (3.3-U) (Kohms)

R0 = 10K, T0 = 273.15 + 25 = 298.15

T = 1 / ((ln(R) - ln(R0))/ B + 1/T0)

// T = 4100 / (ln(U/(3.3-U)) + 12.9964448)

T = B / (ln(R) - ln(R0) + B/T0)
= B / (ln(4700U/(3.3-U)) -ln(R0) + B/T0)
= B / (ln(adc/(4096-adc)) + ln(4700) - ln(R0) + B/T0)
= 3950 / (ln(adc/(4096-adc)) + 12.49334233271006)

U = READING \* 3.3 / 4096;

T = 4100 / (ln(adc/(4096-adc)) + 12.9964448)

if T > Target:
Roll the fan duty = 0.3 + (target - T) / 30
if target - 5 > T : (margin)
stop the fan
else:
do nothing, keep the fan rolling at previous speed.
