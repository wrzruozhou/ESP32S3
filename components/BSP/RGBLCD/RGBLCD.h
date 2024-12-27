//
// Created by Administrator on 2024/10/18.
//

#ifndef WRZ_RGBLCD_H
#define WRZ_RGBLCD_H

#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "XL9555.h"



/**背光引脚*/
#define LCD_BL(x) do{x? \
                        xl9555_pin_write(LCD_BL_IO, 1): \
                        xl9555_pin_write(LCD_BL_IO, 0);}while(0)

/** RGB LCD引脚 */
#define GPIO_LCD_DE     (GPIO_NUM_4)
#define GPIO_LCD_VSYNC  (GPIO_NUM_NC)
#define GPIO_LCD_HSYNC  (GPIO_NUM_NC)
#define GPIO_LCD_CLK    (GPIO_NUM_5)

#define GPIO_LCD_R3     (GPIO_NUM_45)
#define GPIO_LCD_R4     (GPIO_NUM_48)
#define GPIO_LCD_R5     (GPIO_NUM_47)
#define GPIO_LCD_R6     (GPIO_NUM_21)
#define GPIO_LCD_R7     (GPIO_NUM_14)

#define GPIO_LCD_G2     (GPIO_NUM_10)
#define GPIO_LCD_G3     (GPIO_NUM_9)
#define GPIO_LCD_G4     (GPIO_NUM_46)
#define GPIO_LCD_G5     (GPIO_NUM_3)
#define GPIO_LCD_G6     (GPIO_NUM_8)
#define GPIO_LCD_G7     (GPIO_NUM_18)

#define GPIO_LCD_B3     (GPIO_NUM_17)
#define GPIO_LCD_B4     (GPIO_NUM_16)
#define GPIO_LCD_B5     (GPIO_NUM_15)
#define GPIO_LCD_B6     (GPIO_NUM_7)
#define GPIO_LCD_B7     (GPIO_NUM_6)

/* 常用画笔颜色 */
#define WHITE           0xFFFF      /* 白色 */
#define BLACK           0x0000      /* 黑色 */
#define RED             0xF800      /* 红色 */
#define GREEN           0x07E0      /* 绿色 */
#define BLUE            0x001F      /* 蓝色 */
#define MAGENTA         0xF81F      /* 品红色/紫红色 = BLUE + RED */
#define YELLOW          0xFFE0      /* 黄色 = GREEN + RED */
#define CYAN            0x07FF      /* 青色 = GREEN + BLUE */

/**设定LCD的重要参数集*/
typedef struct
{
    uint32_t pwidth;        /** LTDC面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入 */
    uint32_t pheight;       /** LTDC面板的高度,固定参数,不随显示方向改变 */
    uint16_t hsw;           /**水平同步宽度*/
    uint16_t vsw;           /**垂直同步宽度*/
    uint16_t hbp;           /**水平后廊*/
    uint16_t vbp;           /**垂直后廊*/
    uint16_t hfp;           /* 水平前廊 */
    uint16_t vfp;           /* 垂直前廊 */
    uint8_t activelayer;    /**当前层编号:0/1*/
    uint8_t dir;            /**0,竖屏幕，1,横屏*/
    uint16_t id;            /* LTDC ID */
    uint32_t pclk_hz;       /* 设置像素时钟 */
    uint16_t width;         /* LTDC宽度 */
    uint16_t height;        /* LTDC高度 */
}_ltdc_dev;
extern _ltdc_dev ltdcdev;

extern esp_lcd_panel_handle_t panel_handle;             /*LVGL 新增部分*/

void ltdc_init(void );
void ltdc_display_dir(uint8_t dir);
void ltdc_clear(uint16_t color);

/**画图像的API函数*/
void ltdc_draw_point(uint16_t x, uint16_t y,uint16_t color);
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void ltdc_app_show_mono_icos(uint16_t x, uint16_t y,uint8_t width, uint8_t height, uint8_t *iconbase, uint16_t color, uint16_t bkcolor);
void ltdc_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ltdc_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ltdc_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);
void ltdc_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void ltdc_show_num(uint16_t x,uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);
void ltdc_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);
void ltdc_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);

#endif   // WRZ_RGBLCD_H