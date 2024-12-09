//
// Created by Administrator on 2024/11/29.
//

#include "gt1151.h"
#include "IIC.h"
#include <driver/i2c.h>
#include <string.h>

/* 注意: 除了GT9271支持10点触摸之外, 其他触摸芯片只支持 5点触摸 */
uint8_t g_gt_tnum = 5;      /* 默认支持的触摸屏点数(5点触摸) */

/**
 * @brief   向GTXXXX 写入数据
 * @param   reg:起始寄存器地址
 * @param   buf:数据缓冲区
 * @param   len:写入数据长度
 * @retval  esp_err_t:0成功，1,失败*/
esp_err_t gt9xxx_wr_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, GT9XXX_CMD_WR, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, reg >> 8, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, reg & 0xff, I2C_MASTER_ACK);
    i2c_master_write(cmd, buf, len, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief   从GTXXXX读取数据
 * @param   reg:地址
 * @param   buf:数据缓冲区
 * @param   len:读取数据的长度
 * @retval  esp_err_t:0成功，1,失败
 * */
esp_err_t gt9xxx_rd_reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, GT9XXX_CMD_WR,I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, reg >> 8, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, reg & 0xff, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000);
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, GT9XXX_CMD_RD, I2C_MASTER_ACK);
    if ( len > 1 )
    {
        i2c_master_read(cmd, buf, len - 1 , I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, buf + len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    ret |= i2c_master_cmd_begin(GT9XXX_IIC_PORT, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief   初始化触摸屏
 * @param   无
 * @retval  0,初始化成功,1,初始化失败
 * */
uint8_t gt9xxx_init(void )
{
    uint8_t temp[5];
    gpio_config_t  pGPIOConfig;
    pGPIOConfig.mode = GPIO_MODE_INPUT;                         /*输入模式*/
    pGPIOConfig.pin_bit_mask = (1ull << GT9XXX_INT_GPIO_PIN);   /*配置INT引脚*/
    pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig.intr_type = GPIO_INTR_DISABLE;                  /*失能引脚中断*/
    gpio_config(&pGPIOConfig);

    /*初始化IIC Parama*/
    i2c_config_t  i2c_conf = {0};
    i2c_conf.mode = I2C_MODE_MASTER;
    i2c_conf.sda_io_num = GT9XXX_IIC_SDA;
    i2c_conf.scl_io_num = GT9XXX_IIC_SCL;
    i2c_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_conf.master.clk_speed = GT9XXX_IIC_FREQ;
    i2c_param_config(GT9XXX_IIC_PORT, &i2c_conf);

    /*激活IIC控制器驱动*/
    i2c_driver_install(GT9XXX_IIC_PORT, i2c_conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

    for ( int i = 2; i > 0; i--)
    {
        CT_RST(0);
        vTaskDelay( pdTICKS_TO_MS(200));
        CT_RST(1);
        vTaskDelay( pdTICKS_TO_MS(200));
    }

    gt9xxx_rd_reg(GT9XXX_PID_REG, temp, 4);     /*读取产品ID*/
    temp[4] = 0;
    /*判断一下是否是特定的触摸屏*/
    if ( strcmp((char*)temp, "1158") )
    {
        return 1;
    }
    printf("CTP ID:%s\r\n", temp);

    /*软复位*/
    temp[0] = 0x02;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp, 1);

    /*结束复位，读取坐标状态*/
    temp[1] = 0x00;
    gt9xxx_wr_reg(GT9XXX_CTRL_REG, temp ,1);

    return 0;
}

/* GT9XXX 10个触摸点(最多) 对应的寄存器表 */
const uint16_t GT9XXX_TPX_TBL[10] =
{
    GT9XXX_TP1_REG, GT9XXX_TP2_REG, GT9XXX_TP3_REG, GT9XXX_TP4_REG, GT9XXX_TP5_REG,
    GT9XXX_TP6_REG, GT9XXX_TP7_REG, GT9XXX_TP8_REG, GT9XXX_TP9_REG, GT9XXX_TP10_REG,
};

/**
 * @brief       扫描触摸屏(采用查询方式)
 * @param       mode : 电容屏未用到次参数, 为了兼容电阻屏
 * @retval      当前触屏状态
 *   @arg       0, 触屏无触摸;
 *   @arg       1, 触屏有触摸;
 */
uint8_t gt9xxx_scan(uint8_t mode)
{
    uint8_t buf[4];
    uint8_t i = 0;
    uint8_t res = 0;
    uint16_t temp;
    uint16_t  tempsta;
    static uint8_t t = 0;                                           /*控制查询间隔,降低CPU占用率*/
    t++;
    if ((t % 10) == 0 || t < 10)                                    /*空闲时，每进入10次CTP_Scan函数就检1次，从而节省CPU使用率*/
    {
        gt9xxx_rd_reg(GT9XXX_GSTID_REG, &mode, 1);    /*读取触摸点的状态*/
        if ((mode && 0x80) && ((mode & 0xf) <= g_gt_tnum))
        {
            i = 0;
            gt9xxx_wr_reg(GT9XXX_GSTID_REG, &i, 1);     /*清除标志，这个函数到现在目前是没任何用处*/
        }
        /*读取触摸点的个数*/
        if ((mode & 0xf) && ((mode & 0xf) <= g_gt_tnum))
        {
            temp = 0xfff << (mode & 0xf);                           /*奖触摸点的个数转为1的位数*/
            tempsta = tp_dev.sta;
            tp_dev.sta = ~(temp) | TP_PRES_DOWN | TP_CATH_PRES;
            tp_dev.x[g_gt_tnum - 1] = tp_dev.x[0];                  /*保存触点0的数据，保存在最后一个上*/
            tp_dev.y[g_gt_tnum - 1] = tp_dev.y[0];

            /*开始读取触点0的数据*/
            for (i = 0; i < g_gt_tnum; ++i) {
                if (tp_dev.sta & (1 << i))
                {
                    gt9xxx_rd_reg(GT9XXX_TPX_TBL[i], buf, 4);
                    if (tp_dev.touchtype & 0x01)
                    {
                        tp_dev.x[i] = ((uint16_t)buf[1] << 8) + buf[0];
                        tp_dev.y[i] = ((uint16_t)buf[3] << 8) + buf[2];
                    } else{
                        tp_dev.x[i] = ltdcdev.width - (((uint16_t)buf[3] << 8) + buf[2]);
                        tp_dev.y[i] = ((uint16_t)buf[1] << 8) + buf[0];
                    }
                }
            }
            /*ok 数据读完了*/
            res = 1;
            if (tp_dev.x[0] > ltdcdev.width || tp_dev.y[0] > ltdcdev.height)    /*数据超出了范围*/
            {
                if ((mode & 0xf) > 1)   /*则复制下一个数据点到这*/
                {
                    tp_dev.x[0] = tp_dev.x[1];
                    tp_dev.y[0] = tp_dev.y[1];
                    t = 0;                                                      /*出发10次则会至少连续监测10次*/
                } else{                                                         /*非法数据，则还原原来的*/
                    tp_dev.x[0] = tp_dev.x[g_gt_tnum -1];
                    tp_dev.y[0] = tp_dev.y[g_gt_tnum -1];
                    mode = 0x80;
                    tp_dev.sta = tempsta;
                }
            } else{
                t   =   0;                                                      /*出发10次则会至少连续监测10次*/
            }
        }
    }

    if ((mode & 0x8f) == 0x80)                                                  /*如果没有触摸点被按下*/
    {
        if (tp_dev.sta & TP_PRES_DOWN)                                         /*清除按下标记*/
        {
            tp_dev.sta &= ~TP_PRES_DOWN;
        } else{                                                                 /*之前就没有被按下*/
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
            tp_dev.sta &= 0XE000;                                               /* 清除点有效标记 */
        }
    }
    if (t > 240)
    {
        t = 10;                                                             /* 重新从10开始计数 */
    }
    return res;
}

/**
 * @brief       清空屏幕并在右上角显示"RST"
 * @param       无
 * @retval      无
 */
void load_draw_dialog(void)
{
    ltdc_clear(WHITE);                                                  /* 清屏 */
    ltdc_show_string(ltdcdev.width - 24, 0, 200, 16, 16, "RST", BLUE);  /* 显示清屏区域 */
}

/**
 * @brief       画粗线,这是RGBLCD中需要新增的函数
 * @param       x1,y1:起点坐标
 * @param       x2,y2:终点坐标
 * @param       size :线条粗细程度
 * @param       color:线的颜色
 * @retval      无
 */
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;

    if (x1 < size || x2 < size || y1 < size || y2 < size)
    {
        return;
    }

    delta_x = x2 - x1;                              /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)
    {
        incx = 1;                                   /* 设置单步方向 */
    }
    else if (delta_x == 0)
    {
        incx = 0;                                   /* 垂直线 */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;                                   /* 水平线 */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
    {
        distance = delta_x;                         /* 选取基本增量坐标轴 */
    }
    else
    {
        distance = delta_y;
    }

    for (t = 0; t <= distance + 1; t++)            /* 画线输出 */
    {
        ltdc_draw_circle(row, col, size, color);    /* 画点 */
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}