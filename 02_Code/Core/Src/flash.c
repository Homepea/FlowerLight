#include "flash.h"
#include "proj.h"

st_flash_info gstFlashInfo;
u16 guwFlashTimer = 0;

// ***********************************************************************
// @ 函数名称： STMFLASH_Read
// @ 函数功能： 获取Flash的数据连续读取
// @ 函数输入： uiReadAddr(读取的起始地址), uiReadNum4Byte(数据长度)
// @ 函数输出： Flash地址的数据
// @ 注意事项： 返回数据是u32的
// ***********************************************************************
void STMFLASH_Read(u32 uiReadAddr, u32 *puiBuffer, u32 uiReadNum4Byte)       // 读取4个byte
{
    for(u32 i = 0; i < uiReadNum4Byte; i++)
    {
        vu32 uiTemp = *(vu32 *)uiReadAddr;
        MemCpyU8((u8 *)&uiTemp, (u8 *)&puiBuffer[i], 4);
        uiReadAddr += CONST_4;                                              // 偏移4个Byte
    }
}

// ***********************************************************************
// @ 函数名称： STMFLASH_Write
// @ 函数功能： 获取Flash的数据连续读取
// @ 函数输入： uiReadAddr(读取的起始地址), uiReadNum8Byte(数据长度, 8Byte*n)
// @ 函数输出： Flash地址的数据
// @ 注意事项： 返回数据是u32的
// ***********************************************************************
void STMFLASH_Write(u32 uiWriteAddr, u32 *puiBuffer, u32 uiReadNum8Byte)
{
    FLASH_EraseInitTypeDef stFlashEraseInit;
    u32 uiPageError = 0;
    u32 uiEndAddr = uiWriteAddr + uiReadNum8Byte * 8;                       // 写入的结束地址;
    if( (uiWriteAddr < STM32_EEPROM_BASE) ||                                // 小于允许操作的起始地址
        (uiWriteAddr >= STM32_FLASH_END ) ||                                // 大于等于允许操作的最大地址
        (uiWriteAddr % 4)   )                                               // 地址不对
    {
#if FLASH_SERIAL_DEBUG
        printf("Flash addr error!\r\n");
#endif
        return;                                                             // 非法地址
    }
    else
    {
#if FLASH_SERIAL_DEBUG
        printf("Flash addr OK!\r\n");
#endif

    }

    HAL_FLASH_Unlock();                                                     // 解锁
#if FLASH_SERIAL_DEBUG
    printf("Flash unlock\r\n");
#endif

    stFlashEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;                     // 擦除类型，页擦除
    stFlashEraseInit.Page = STM32_EEPROM_PAGE;                              // 从哪页开始擦除
    stFlashEraseInit.NbPages = 1;                                           // 擦除一页
    if (HAL_OK != HAL_FLASHEx_Erase(&stFlashEraseInit, &uiPageError))
    {
#if FLASH_SERIAL_DEBUG
        printf("Flash Erase error!\r\n");
#endif
    }

    if (HAL_OK == FLASH_WaitForLastOperation(FLASH_WAITETIME))              // 等待上次操作完成
    {
        while (uiWriteAddr < uiEndAddr)
        {
            u64 temp;
            MemCpyU8((u8 *)puiBuffer, (u8 *)&temp, 8);
            if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, uiWriteAddr, temp)) //写入数据
            {
#if FLASH_SERIAL_DEBUG
        printf("Flash Write error!\r\n");
#endif
                break;                                                      // 写入异常
            }
            uiWriteAddr += CONST_8;                                         // 每次写入8byte数据，地址偏移8
            puiBuffer   += CONST_2;                                         // 这里puiBuffer是32位指针，加2代表偏移8个字节
        }
    }
    HAL_FLASH_Lock();                                                       // 上锁
}

// ***********************************************************************
// @ 函数名称： ChipUuid_Read
// @ 函数功能： 读取UUID并且写到Flash
// @ 函数输入： 无
// @ 函数输出： 无
// @ 注意事项： 无
// ***********************************************************************
void ChipUuid_Read(void)
{
#if FLASH_SERIAL_DEBUG
    printf("Chip uuid: ");
#endif
    for (u8 i = 0; i < 3; i++)
    {
        gstFlashInfo.puiUuid[i] = (*(vu16*)(UID_BASE + (i << 2)));
#if FLASH_SERIAL_DEBUG
        printf("0x%x ", gstFlashInfo.puiUuid[i]);
#endif
    }
#if FLASH_SERIAL_DEBUG
        printf("\r\n");
#endif

	Config_Save();					// 最后需要存储到Flash
}

// ***********************************************************************
// @ 函数名称： ChipUuid_Check
// @ 函数功能： 验证UUID
// @ 函数输入： 无
// @ 函数输出： 无
// @ 注意事项： 无
// ***********************************************************************
u8 ChipUuid_Check(void)
{
    for (u8 i = 0; i < 3; i++)
    {
        u16 uwChipId = (*(vu16*)(UID_BASE + (i << 2)));

        if (uwChipId != gstFlashInfo.puiUuid[i])
        {
            return FALSE;
        }
    }
    return TRUE;
}

// ***********************************************************************
// @ 函数名称： Config_Init
// @ 函数功能： 读取Flash的内容到RAM
// @ 函数输入： 无
// @ 函数输出： 无
// @ 注意事项： 无
// ***********************************************************************
void Config_Init(void)
{
    if (CONFIG_LENGTH % 8)                                                  // Config 的长度必须为8的倍数，如果不够的话需要再后面加一些补充的
    {
        while(1)
        {
            printf("Config length error, data: %d\r\n", CONFIG_LENGTH);
            HAL_Delay(200);
        }
    }
    else
    {
        printf("Config length: %d\r\n", CONFIG_LENGTH);
    }

    STMFLASH_Read(STM32_EEPROM_BASE, (u32 *)gubFlashBuffer, CONFIG_LENGTH >> 2);

#if FLASH_FIRST_DEBUG

    gstFlashInfo.ubPID[0]        = '8';                                     // PID
    gstFlashInfo.ubPID[1]        = 'W';
    gstFlashInfo.ubPID[2]        = '3';
    gstFlashInfo.ubPID[3]        = '0';
    gstFlashInfo.ubFWVersion[0]  =  1 ;                                     // 大版本号
    gstFlashInfo.ubFWVersion[1]  =  0 ;                                     // 小版本号0
    gstFlashInfo.ubFWVersion[2]  =  0 ;                                     // 小版本号1
    gstFlashInfo.ubFWVersion[3]  =  0 ;                                     // 客户ID(0表示公版)
    gstFlashInfo.ubBurnInData[0] = '2' ;                                    // 烧录日期：2022/11/11
    gstFlashInfo.ubBurnInData[1] = '0' ;
    gstFlashInfo.ubBurnInData[2] = '2' ;
    gstFlashInfo.ubBurnInData[3] = '2' ;
    gstFlashInfo.ubBurnInData[4] = '1' ;
    gstFlashInfo.ubBurnInData[5] = '1' ;
    gstFlashInfo.ubBurnInData[6] = '1' ;
    gstFlashInfo.ubBurnInData[7] = '1' ;

    Config_Save();
#endif

#if (0 == FLASH_RECORD_USE_UUID)                // 0：记录UUID到Flash
    ChipUuid_Read();
#else                                           // 1: 准备进行UUID校验
    if (FALSE == ChipUuid_Check())
    {
        while(1)
        {
#if FLASH_SERIAL_DEBUG
            printf("Chip check error!!!\r\n");
            HAL_Delay(500);
#endif
        }
    }
    else
    {
        printf("Chip check OK!");
    }
#endif

}

// ***********************************************************************
// @ 函数名称： Config_Save
// @ 函数功能： 将Ram的数据保存到Flash
// @ 函数输入： 无
// @ 函数输出： 无
// @ 注意事项： 双word写入，因此一次写入8个Byte，但是结构体的长度不一定是8的倍数，因此可能会加一
// ***********************************************************************
void Config_Save(void)
{
    gstFlashInfo.ubChkSum = ChkSumU8(gubFlashBuffer, CONFIG_LENGTH - 1);  // ChkSum
    STMFLASH_Write(STM32_EEPROM_BASE, (u32 *)&gstFlashInfo.ubPID[0], (CONFIG_LENGTH >> 3) + ((CONFIG_LENGTH & 0x07) > 0));
#if FLASH_SERIAL_DEBUG
    printf("Config saved!\r\n");
#endif
}

// ***********************************************************************
//	@ 函数名称： Config_DelaySave(void)
//	@ 函数功能： 8W30的初始化
//	@ 函数输入： 无
//	@ 函数输出： 无
//	@ 注意事项： 放在While1中运行
// ***********************************************************************
void Config_DelaySave(void)
{
    if (guwFlashTimer > 0)                                      // 有计数了才开始累加
    {
        if (1 == guwFlashTimer)
        {
            // 设置stFlashInfo的值
            Config_Save();
        }
        guwFlashTimer -= CONST_1;
    }
}
