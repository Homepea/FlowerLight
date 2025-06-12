#ifndef __KEY_H__
#define __KEY_H__

#include "common.h"
#include "gpio.h"


// 首先定义电容按键的宏

#define     KEY_NUM                 1
#define     KEY_LEVEL_NO_TOUCH      0                                   // 没有按下的时候默认电平
#define     KEY_LEVEL_TOUCH         (!KEY_LEVEL_NO_TOUCH)

#define     KEY_0                   HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)
#define     KEY_LONG_THD            255                                // 长按的时间 = 周期 * THD

typedef struct
{
    u16 pwbKeyTimer[KEY_NUM];                                            // 按键按下的计时
    u16 pwbKeyTimerBak[KEY_NUM];                                         // 上一次按键的计时
    u8  pubKeyState[KEY_NUM];                                            // 按键此时的状态，是否按下和松开
    u8  pubKeyStaBak[KEY_NUM];                                           // 上一次的状态，是否按下和松开
    u8  pubKeyJustTouch[KEY_NUM];                                        // 刚刚按下的时刻
    u8  pubKeyJustRelease[KEY_NUM];                                      // 刚刚松开的时刻
    u8  pubKeyLongTouching[KEY_NUM];                                     // 正在长按
    u8  pubKeyLongTohchRel[KEY_NUM];                                     // 长按松开的时刻
} st_key_info_t;

void Key_Init(void);
void Key_Process(void);

#endif
