#include "key.h"
#include "proj.h"

st_key_info_t gstKeyInfo;
u8 enterStandByModeFlag = 0;
extern u8 gubMode;
// ***********************************************************************
//	@ 函数名称： Key_Init(void)
//	@ 函数功能： Key的初始化，主要是变量的初始化
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Key_Init(void)
{
    MemSetU8((u8 *)&gstKeyInfo, 0x00, sizeof(st_key_info_t));
}

// ***********************************************************************
//	@ 函数名称： Key_Read(u8 ubKeyNum)
//	@ 函数功能： 读取单个Key的结果
//	@ 函数输入： 按键号（从0开始）
//	@ 函数输出： IO的电平
//	@ 注意事项： 无
// ***********************************************************************
u8 Key_Read(u8 ubKeyNum)
{
    switch (ubKeyNum)
    {
        case 0:
            return KEY_0 == KEY_LEVEL_TOUCH;
            // break;

        default:
            return KEY_LEVEL_NO_TOUCH;
            // break;
    }
}

// ***********************************************************************
//	@ 函数名称： Key_Get_Statue(void)
//	@ 函数功能： 获取按键的状态
//	@ 函数输入： 无
//	@ 函数输出： 按键输出的结果
//	@ 注意事项： 无
// ***********************************************************************
void Key_GetStatue(void)
{
    MemCpyU8 (gstKeyInfo.pubKeyState, gstKeyInfo.pubKeyStaBak,   KEY_NUM);  // 备份状态
    MemCpyU16(gstKeyInfo.pwbKeyTimer, gstKeyInfo.pwbKeyTimerBak, KEY_NUM);

    for (u8 i = 0; i < KEY_NUM; i++)                                        // 获取当前的状态
    {
        gstKeyInfo.pubKeyState[i] = Key_Read(i);
        if (gstKeyInfo.pubKeyState[i])
        {
            if (gstKeyInfo.pwbKeyTimer[i] < U16_MAX)
            {
                gstKeyInfo.pwbKeyTimer[i] += 1;
            }
        }
        else
        {
            gstKeyInfo.pwbKeyTimer[i] = 0;
        }
    }

    for (u8 i = 0; i < KEY_NUM; i++)                                        // 根据状态设置Flag
    {
        if (gstKeyInfo.pubKeyState[i] && !gstKeyInfo.pubKeyStaBak[i])       // 刚刚按下的时刻
        {
            gstKeyInfo.pubKeyJustTouch[i] = TRUE;
        }
        else
        {
            gstKeyInfo.pubKeyJustTouch[i] = FALSE;
        }

        if (!gstKeyInfo.pubKeyState[i] && gstKeyInfo.pubKeyStaBak[i])       // 刚刚松开的时刻
        {
            gstKeyInfo.pubKeyJustRelease[i] = TRUE;
        }
        else
        {
            gstKeyInfo.pubKeyJustRelease[i] = FALSE;
        }

        if ((!gstKeyInfo.pwbKeyTimer[i]) &&                                 // 长按松开的时刻
            (gstKeyInfo.pwbKeyTimerBak[i] >= KEY_LONG_THD))
        {
            gstKeyInfo.pubKeyLongTohchRel[i] = TRUE;
        }
        else
        {
            gstKeyInfo.pubKeyLongTohchRel[i] = FALSE;
        }

        if (gstKeyInfo.pwbKeyTimer[i] >= KEY_LONG_THD)                      // 是否长按
        {
            gstKeyInfo.pubKeyLongTouching[i] = TRUE;
        }
        else
        {
            gstKeyInfo.pubKeyLongTouching[i] = FALSE;
        }
    }
}

// ***********************************************************************
//	@ 函数名称： Key_Get_Statue(void)
//	@ 函数功能： 获取按键的状态
//	@ 函数输入： 无
//	@ 函数输出： 按键输出的结果
//	@ 注意事项： gpubKeyRst[0/1]分别是短按/长按的结果，对应的bit表示对应的Key
//              读完了以后要清除掉，否则下次仍然会读到
// ***********************************************************************
void Key_Process(void)
{
    Key_GetStatue();

    if (CalSumU8(gstKeyInfo.pubKeyJustTouch, KEY_NUM))
    {
        // KEY_PRINTF("Key just touched!\r\n");
    }

    if (CalSumU8(gstKeyInfo.pubKeyJustRelease, KEY_NUM))
    {
        // KEY_PRINTF("Key just release!\r\n");
        if (gstKeyInfo.pubKeyJustRelease[0]
            && (gstKeyInfo.pwbKeyTimerBak[0] < KEY_LONG_THD))                    // 触摸按键短按：调整一级菜单下的不同图案
        {
            gubMode += 1;
            if(gubMode >= 9)
            {
                gubMode = 0;
            }
            // printf("modify picture\r\n");
        }
    }

    if (CalSumU8(gstKeyInfo.pubKeyLongTouching, KEY_NUM))
    {
        // KEY_PRINTF("Key long touching!\r\n");
    }


    if (CalSumU8(gstKeyInfo.pubKeyLongTohchRel, KEY_NUM))
    {
        // KEY_PRINTF("Key releaseed after long touch!\r\n");
        if (gstKeyInfo.pubKeyLongTohchRel[0])                                   // 顶部按键长按：进入低功耗模式
        {
            enterStandByModeFlag = 1;
        }
    }
}
