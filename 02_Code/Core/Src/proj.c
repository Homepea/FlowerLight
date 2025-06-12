#include "proj.h"
u8 gubMode = 0, ubLongFlag = 0, ubShortFlag = 0;
extern u8 enterStandByModeFlag;

// 进入睡眠模式函数
void Enter_Sleep_Mode(void)
{
    // 设置SLEEPDEEP位=0 (选择睡眠模式)
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

    // 执行WFI指令进入睡眠
    __WFI();
}

// ***********************************************************************
//	@ 函数名称： Proj_Init(void)
//	@ 函数功能： Proj的初始化，全部流程的初始化都在这里
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Proj_Init(void)
{
    HAL_Delay(200); // 延迟一会儿让其余的设备稳定
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

    HAL_Delay(5);
    HAL_TIM_Base_Start_IT(&htim16);
}

// ***********************************************************************
//	@ 函数名称： Effect_Blue_Breath(void)
//	@ 函数功能： 蓝色呼吸效果（星空）
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Effect_Blue_Breath(void)
{
    static u8 ubDuty = 255;
    static u8 ubFadeDir = 1;     // 呼吸方向: 1=渐亮, 0=渐暗
    static u8 ubCount= 0;
    u8 ubFlag = 0;

    ubCount += 1;
    if(ubCount >= 1)
    {
        ubCount = 0;
        ubFlag = 1; // 每10次循环执行一次
    }

    if(ubFlag)
    {
        if(ubFadeDir)
        {
            ubDuty -= 1;
            if(ubDuty <= 0)
            {
                ubDuty = 0;
                ubFadeDir = 0;
            }
        }
        else
        {
            ubDuty += 1;
            if(ubDuty >= U8_MAX)
            {
                ubDuty = U8_MAX;
                ubFadeDir = 1;
            }
        }
    }

    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, ubDuty);
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, ubDuty);
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, ubDuty);
}

// ***********************************************************************
//	@ 函数名称： Effect_Green_Breath(void)
//	@ 函数功能： 绿色呼吸效果（萤火虫）
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Effect_Green_Breath(void)
{
    static u8 ubDuty = 255;
    static u8 ubFadeDir = 1;     // 呼吸方向: 1=渐亮, 0=渐暗
    static u8 ubCount= 0;
    u8 ubFlag = 0;

    ubCount += 1;
    if(ubCount >= 1)
    {
        ubCount = 0;
        ubFlag = 1; // 每10次循环执行一次
    }

    if(ubFlag)
    {
        if(ubFadeDir)
        {
            ubDuty -= 1;
            if(ubDuty <= 0)
            {
                ubDuty = 0;
                ubFadeDir = 0;
            }
        }
        else
        {
            ubDuty += 1;
            if(ubDuty >= U8_MAX)
            {
                ubDuty = U8_MAX;
                ubFadeDir = 1;
            }
        }
    }

    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, ubDuty);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, ubDuty);
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, ubDuty);
}

// ***********************************************************************
//	@ 函数名称： Effect_Flash(void)
//	@ 函数功能： 闪烁效果
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Effect_Flash(void)
{
    static u8 ubCount= 0;
    static u8 ubFlag = 0;

    ubCount += 1;
    if(ubCount >= 100)
    {
        ubCount = 0;
        ubFlag = !ubFlag;
    }

    if(ubFlag)
    {
        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 0);
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 0);
        __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 255);
    }
    else
    {
        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 255);
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 255);
        __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 0);
    }
}

// ***********************************************************************
//	@ 函数名称： Effect_InnerFlash(void)
//	@ 函数功能： 内圈闪烁效果
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Effect_InnerFlash(void)
{
    static u8 ubCount= 0;
    static u8 ubFlag = 0;

    ubCount += 1;
    if(ubCount >= 100)
    {
        ubCount = 0;
        ubFlag = !ubFlag;
    }

    if(ubFlag)
    {
        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 0);
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, 255);
    }
    else
    {
        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 255);
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, 0);
    }
    // __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 0);
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 0);
}

// ***********************************************************************
//	@ 函数名称： Effect_StaticFlower1(void)
//	@ 函数功能： 静态樱花效果
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Effect_StaticFlower1(void)
{
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 0);
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 100);
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 80);
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 50);
}

// ***********************************************************************
//	@ 函数名称： Effect_StaticFlower2(void)
//	@ 函数功能： 静态樱花效果
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Effect_StaticFlower2(void)
{
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, 0);
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 100);
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 80);
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 50);

}

// ***********************************************************************
//	@ 函数名称： Effect_Red_Outer(void)
//	@ 函数功能： 外圈红色效果
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Effect_Red_Outer(void)
{
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);
}

// ***********************************************************************
//	@ 函数名称： Effect_Green_Outer(void)
//	@ 函数功能： 外圈绿色效果
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Effect_Green_Outer(void)
{
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 0);
}

// ***********************************************************************
//	@ 函数名称： Effect_Blue_Outer(void)
//	@ 函数功能： 外圈蓝色效果
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Effect_Blue_Outer(void)
{
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 0);
}

// ***********************************************************************
//	@ 函数名称： Proj_Proc(void)
//	@ 函数功能： 运行的进程函数
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 无
// ***********************************************************************
void Proj_Proc(void)
{
    // 熄灭灯珠
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 255);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, 255);
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, 255);

    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 255);
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 255);
    __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 255);

    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 255);
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 255);
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 255);

    switch (gubMode)
    {
    case 0:
        Effect_Blue_Breath();
        break;
    case 1:
        Effect_Green_Breath();
        break;

    case 2:
        Effect_Flash();
        break;

    case 3:
        Effect_StaticFlower1();
        break;

    case 4:
        Effect_StaticFlower2();
        break;

    case 5:
        Effect_InnerFlash();
        break;

    case 6:
        Effect_Red_Outer();
        break;

    case 7:
        Effect_Green_Outer();
        break;

    case 8:
        Effect_Blue_Outer();
        break;

    default:
        break;
    }

    if (enterStandByModeFlag)
    {
        // 即将进入待机模式
        // printf("Entering StandBy Mode\r\n");
        // 熄灭灯珠
        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 255);               // 内圈红色
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, 255);               // 内圈绿色
        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, 255);               // 内圈蓝色

        __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 255);               // 内圈红色
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3, 255);               // 内圈绿色
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2, 255);               // 内圈蓝色

        __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 255);               // 外圈红色
        __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4, 255);               // 外圈绿色
        __HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 255);               // 外圈蓝色

        /*清除WU状态标志位*/
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF);

        /* 使能WKUP引脚的唤醒功能 ，使能PA0*/
        HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

        //暂停滴答时钟，防止�?�过滴答时钟中断唤醒
        HAL_SuspendTick();
        /* 进入待机模式 */
        HAL_PWR_EnterSTANDBYMode();
    }

    HAL_Delay(2);
}
