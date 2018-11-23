#ifndef __POWER_H__
#define __POWER_H__

void Test_SlowMode(void);
void Test_HoldMode(void);
void Test_StopMode(void);
void Test_SLIdleMode(void);
void Test_IdleMode(void);
void Test_PLL(void);

void EINT4567Int(void)   __attribute__ ((interrupt ("IRQ")));
void STEINT4567Int(void) __attribute__ ((interrupt ("IRQ")));
void SLWEINT4567Int(void)__attribute__ ((interrupt ("IRQ")));
void STAlarmInt(void)    __attribute__ ((interrupt ("IRQ")));
void Eint0Int(void)      __attribute__ ((interrupt ("IRQ")));
void Timer0Int(void)     __attribute__ ((interrupt ("IRQ")));
void Timer1Int(void)     __attribute__ ((interrupt ("IRQ")));
void AlarmInt(void)      __attribute__ ((interrupt ("IRQ")));
void SlAlarmInt(void)    __attribute__ ((interrupt ("IRQ")));
void STEint0Int(void)    __attribute__ ((interrupt ("IRQ")));
void STAlarmInt(void)    __attribute__ ((interrupt ("IRQ")));
#endif /*__POWER_H__*/
