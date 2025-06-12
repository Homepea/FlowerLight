#ifndef __FLASH_H__
#define __FLASH_H__

#include "main.h"
#include "common.h"
#include "stm32g0xx.h"

//#pragma         pack(1)                   // STM32G0是M0内核，如果字节不对齐会触发硬件错误
#define         FLASH_RECORD_USE_UUID       0                               // 是否记录和使用UUID(0: 记录UUID到Flash，1：从对比UUID是否合格)
#define         FLASH_SERIAL_DEBUG          0                               // 串口打印Flash中间数据的宏
#define         FLASH_FIRST_DEBUG           0                               // 第一次将一些信息写入Flash

#define         STM32_EEPROM_PAGE           15                              // STM32G070Cxx有128K的flash，一页2k，最后一页为第63页
#define         STM32_EEPROM_BASE           (STM32_FLASH_BASE + ( STM32_EEPROM_PAGE << CONST_11))
#define         STM32_FLASH_END             (STM32_FLASH_BASE + ((STM32_EEPROM_PAGE + 1) << CONST_11) - 1)
#define         STM32_FLASH_BASE            0x08000000                      // 首页Flash的起始地址
#define         FLASH_WAITETIME             3                               // Flash等待的次数
#define         CONFIG_LENGTH               (sizeof(st_flash_info))         // 配置的长度
#define         gubFlashBuffer              ((u8 *)(&gstFlashInfo))         // 配置的内容
#define         gubFlashData                ((u8 *)STM32_EEPROM_BASE)       // Flash的内容

typedef struct stFlashInfo{
//  Type        Name                                                        // Addd Info
    char        ubPID[4];                                                   // PID信息
    u8          ubFWVersion[4];                                             // 固件版本
    char        ubBurnInData[4];                                            // 烧录日期
    u32         puiUuid[3];                                                 // 存储UUID
    u32         uiSystemRunHour;                                            // 系统运行的小时
    u8          ubShowMode;                                                 // 显示模式
    u8          ubFreqHopingMode;                                           // 设置跳动模式
    u16         uwFreqSetTonal;                                             // 频谱模式设置色调
    u8          ubFreqSetBright;                                            // 频谱模式设置亮度
    u8          ubFreqSetGain;                                              // 频谱模式设置增益
    u8          ubLightHopMode;                                             // 小夜灯设置跳动模式
    u16         uwLightSetTonal;                                            // 小夜灯设置色调
    u8          ubLightSetBright;                                           // 小夜灯设置亮度
    u8          ubEffectsHopMode;                                           // 特效灯的模式
    u8          boVirtualSoundFlag          : 1;                            // 是否开启虚拟的声音
    u8          RES                         : 7;
    u8          ubRes[4];
    u8          ubChkSum;                                                   // 校验和
} st_flash_info;

extern st_flash_info gstFlashInfo;

u32 STMFLASH_ReadWord(u32 uiFlashAddr);
void STMFLASH_Read(u32 uiReadAddr, u32 *puiBuffer, u32 NumToRead);
void STMFLASH_Write(u32 WriteAddr, u32 *pBuffer, u32 NumToWrite);
void Config_DelaySave(void);
void Config_Save(void);
void Config_Init(void);

#endif
