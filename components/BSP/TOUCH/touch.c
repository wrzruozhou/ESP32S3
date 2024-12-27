//
// Created by Administrator on 2024/11/29.
//

#include "touch.h"

_m_tp_dev tp_dev = {
        {tp_init,
         0,
         0,
         0,
         0,
         0,
        }
};

/**
 * @brief       触摸屏初始化
 * @param       无
 * @retval      0,触摸屏初始化成功
 *              1,触摸屏有问题
 */
uint8_t tp_init(void)
{
    tp_dev.touchtype = 0;                                   /* 默认设置(电阻屏 & 竖屏) */
    tp_dev.touchtype |= ltdcdev.dir & 0X01;                 /* 根据LCD判定是横屏还是竖屏 */

    if (ltdcdev.id == 0X4342 || ltdcdev.id == 0X4384)       /* 电容触摸屏,4.3寸屏 */
    {
        gt9xxx_init();
        tp_dev.scan = gt9xxx_scan;                          /* 扫描函数指向GT触摸屏扫描 */
        tp_dev.touchtype |= 0X80;                           /* 电容屏 */
        return 0;
    }
    return 1;
}
