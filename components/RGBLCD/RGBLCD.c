//
// Created by Administrator on 2024/10/18.
//

#include "RGBLCD.h"
#include "ltdcfont.h"


static const char  *TAG = "ltdc";
esp_lcd_panel_handle_t panel_handle = NULL;                         /** RGBLCD句柄 */
static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;     /**定义portMUX_TYPE类型的自旋锁变量，用于临界区保护*/
uint32_t g_back_color = 0xffff;                                     /**背景色*/

/**管理ltdc的重要参数*/
_ltdc_dev ltdcdev;

/**
 * @brief   读取面板ID
 * @note    利用LCD RGB线的最高位(R7,G7,B7)来是被面板ID
 *          PG6 = R7(M0); PI2 = (M1); PI7 = B7(M2);
 *          M2:M1:M0
 *          0 :0: 0     4.3 寸480*272 RGB屏幕, ID = 0x4342
 *          1 :0: 0     4.3 寸800*480 RGB屏幕, ID = 0x4384
 * @param   无
 * @retval  0, 非法
 *          其他,LCD ID
 **/
uint16_t ltdc_panelid_read(void )
{
    uint8_t idx = 0;
    gpio_config_t gpio_init_struct;

    gpio_init_struct.intr_type    = GPIO_INTR_DISABLE;     /**失能引脚中断*/
    gpio_init_struct.mode         = GPIO_MODE_INPUT;       /**输入输出模式*/
    gpio_init_struct.pull_up_en   = GPIO_PULLUP_ENABLE;    /**使能上拉电阻*/
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE; /**失能下拉电阻*/
    gpio_init_struct.pin_bit_mask = 1ull << GPIO_LCD_B7;
    gpio_config( &gpio_init_struct ); /**配置GPIO*/

    gpio_init_struct.pin_bit_mask = 1ull << GPIO_LCD_R7 | GPIO_LCD_G7;
    gpio_config( &gpio_init_struct );

    idx = (uint8_t)gpio_get_level(GPIO_LCD_R7);         /**读取M0*/
    idx |= (uint8_t)gpio_get_level(GPIO_LCD_G7)  << 1;   /**读取M1*/
    idx |= (uint8_t)gpio_get_level(GPIO_LCD_B7) << 2;    /**读取M2*/

    switch ( idx )
    {
        case 0:
            return 0x4342;
        case 4:
            return 0x4384;
        default:
            return 1;
    }
}

/**
 * @brief   初始化ltdc
 * @param   无
 * @retval  无
 * */
void ltdc_init(void )
{
    panel_handle = NULL;
    ltdcdev.id = ltdc_panelid_read();               /**读取LCD面板ID*/
    switch ( ltdcdev.id )
    {
        case 0x4342:
            printf("屏幕的分辨率是 480*272\n");
            break;
        case 0x4384:
            printf("屏幕的分辨率是 800*480\n");
            break;
        case 0:
            printf("没有识别到屏幕\n");
            break;
    }
    /**本试验用的屏幕ID是0x4384*/
    ltdcdev.hbp = 88;                   /**HSYNC Back Porch*/
    ltdcdev.hfp = 40;                   /**HSYNC Front Porch*/
    ltdcdev.hsw = 48;                   /**HSYNC Pulse Width*/
    ltdcdev.vbp = 32;                   /** VSYNC Back Porch */
    ltdcdev.vfp = 13;                   /**VSYNC Front Porch*/
    ltdcdev.vsw = 3;                    /**VSYNC Pulse Width*/
    ltdcdev.pheight = 480;               /**Vertical dissplay area*/
    ltdcdev.pwidth = 800;               /**Horizontal display area*/
    ltdcdev.pclk_hz = 18 * 1000 * 1000;  /**PLCK Frequency*/

    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,                           /**RGB565 in parallel mode, thus 16bit in width*/
        .psram_trans_align = 64,                    /**在PSRAM中分配的缓冲区的对齐*/
        .clk_src = LCD_CLK_SRC_PLL160M,             /**RGBLCD的时钟源*/
        .disp_gpio_num = GPIO_NUM_NC,               /**用于显示控制信号,不使用设为-1*/
        .pclk_gpio_num = GPIO_LCD_CLK,              /**PCLK引脚定义*/
        .vsync_gpio_num = GPIO_LCD_VSYNC,           /**VSYNC引脚定义,DE模式不使用*/
        .hsync_gpio_num = GPIO_LCD_HSYNC,           /**HSYNC引脚定义,DE模式不使用*/
        .de_gpio_num = GPIO_LCD_DE,                 /**DE引脚定义*/
        .data_gpio_nums = {
        GPIO_LCD_B3, GPIO_LCD_B4, GPIO_LCD_B5, GPIO_LCD_B6, GPIO_LCD_B7,
        GPIO_LCD_G2, GPIO_LCD_G3, GPIO_LCD_G4, GPIO_LCD_G5, GPIO_LCD_G6 ,GPIO_LCD_G7,
        GPIO_LCD_R3, GPIO_LCD_R4, GPIO_LCD_R5, GPIO_LCD_R6, GPIO_LCD_R7,
        },
        .timings = {
            .pclk_hz = ltdcdev.pclk_hz,
        .h_res = ltdcdev.pwidth,
        .v_res = ltdcdev.pheight,
        .hsync_back_porch= ltdcdev.hbp,
        .hsync_front_porch = ltdcdev.hfp,
        .hsync_pulse_width = ltdcdev.hsw,
        .vsync_back_porch = ltdcdev.vbp,
        .vsync_front_porch = ltdcdev.vfp,
        .vsync_pulse_width = ltdcdev.vsw,
        .flags.pclk_active_neg = true,                  /**RGB数据在下降沿处计时*/
        },
        .flags.fb_in_psram = true,                      /**在PSRAM中分配帧缓冲*/
        .bounce_buffer_size_px = 480 * 10,              /**解决写spiflash时，抖动的问题*/

    };
    /**创建RGB panel对象*/
    ESP_ERROR_CHECK( esp_lcd_new_rgb_panel(&panel_config,&panel_handle));
    ESP_LOGI(TAG, "Initialize RGB LCD panel");
    /**复位RGB屏幕*/
    ESP_ERROR_CHECK( esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK( esp_lcd_panel_init(panel_handle));

    /**设置屏幕显示方向, example下并未对此做出设置*/
    ltdc_display_dir(1);                            /**横屏显示*/
    /**显示白色*/
    ltdc_clear(RED);
    /**打开背光*/
    LCD_BL(1);
}

/**
 * @brief   清除屏幕
 * @param   color:清除的颜色
 * @retval  无
 * */
void ltdc_clear(uint16_t color)
{
    uint16_t  *buffer = heap_caps_malloc(ltdcdev.width * sizeof( uint16_t ), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if ( buffer == NULL )
    {
        ESP_LOGE(TAG, "Memory for bit map is not enough");
    }
    else
    {
        for ( uint16_t x = 0; x < ltdcdev.width; ++x )
        {
            buffer[x] = color;
        }
        /**这个只是我个人想法*/
//        esp_lcd_panel_draw_bitmap(panel_handle,0,0,ltdcdev.width,ltdcdev.height,buffer);
        for ( uint16_t y = 0; y < ltdcdev.height; ++y )
        {
            /**使用taskENTER_CRITICAL()和taskEXIT_CRITICAL()保护画点过程，禁止任务调度*/
            taskENTER_CRITICAL(&my_spinlock);               /**屏蔽中断*/
            esp_lcd_panel_draw_bitmap(panel_handle,0,y,ltdcdev.width,y + 1,buffer);
            taskEXIT_CRITICAL(&my_spinlock);                /**重新使能中断*/
        }
        heap_caps_free(buffer);
    }
}

/**
 * @brief   RGB888转RGB565
 * @note    8个位的RGB都取最高位
 * @param   r:红色
 * @param   g:绿色
 * @param   b:蓝色
 * @retval  返回RGB565的颜色值
 * */
uint16_t ltdc_rgv888_to_565(uint8_t r, uint16_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xfc) << 3) | (b >> 3);
}

/**
 * @brief   LTDC显示方向设置
 * @param   dir:0,竖屏幕 1:横屏幕
 * @retval  无
 * */
void ltdc_display_dir(uint8_t dir)
{
    ltdcdev.dir = dir;                  /**保存显示方向*/
    if ( ltdcdev.dir == 0 )             /**竖屏幕显示*/
    {
        ltdcdev.width = ltdcdev.pheight;
        ltdcdev.height = ltdcdev.pwidth;
        esp_lcd_panel_swap_xy(panel_handle, true);                  /**交换x和y轴*/
        esp_lcd_panel_mirror(panel_handle, false, true);       /**对屏幕的Y轴进行镜像处理*/
    }
    else if ( ltdcdev.dir == 1 )
    {
        ltdcdev.width = ltdcdev.pwidth;
        ltdcdev.height = ltdcdev.pheight;
        esp_lcd_panel_swap_xy(panel_handle, false);                   /**不需要交换x轴和y轴*/
        esp_lcd_panel_mirror(panel_handle, false, false);       /**对屏幕的xy轴不作镜像处理*/
    }
}

/**
 * @brief   LTDC画点函数
 * @param   x,y :写入坐标
 * @param   color:  颜色值
 * @retval  无
 * */
void ltdc_draw_point(uint16_t x, uint16_t y,uint16_t color)
{
    taskENTER_CRITICAL(&my_spinlock);
    esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 1, y + 1,&color);
    taskEXIT_CRITICAL(&my_spinlock);
}

/**
 * @brief       在指定区域内填充指定颜色块
 * @note        此函数仅支持uint16_t,RGB565格式的颜色数组填充.
 *              (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex - sx + 1) * (ey - sy + 1)
 *              注意:sx,ex,不能大于ltdcdev.width - 1; sy,ey,不能大于ltdcdev.height - 1
 * @param       sx,sy:起始坐标
 * @param       ex,ey:结束坐标
 * @param       color:填充的颜色数组首地址
 * @retval      无
 */
 void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    if(sx < 0 || sy < 0 || ex > ltdcdev.width || ey > ltdcdev.height)
    {
        return;
    }

    if ( sx > ex || sy > ey )
    {
        return ;
    }

    sx = fmax(0, sx);
    sy = fmax(0, sy);
    ex = fmin(ltdcdev.width - 1, ex);
    ey = fmin(ltdcdev.height - 1, ey);

    /**开始填充颜色*/
    for ( int i = 0; i <= ex; ++i )
    {
        for ( int j = 0; j <= ey; ++j )
        {
            ltdc_draw_point(i ,j ,color);
        }
    }
}

/**
 * @brief   显示单色图标
 * @param   x,y,width,height:坐标及尺寸
 * @param   iconbase:点整位置
 * @param   color:画点的颜色
 * @param   bkcolor:背景色
 * @retval  无
 * */
void ltdc_app_show_mono_icos(uint16_t x, uint16_t y,uint8_t width, uint8_t height, uint8_t *iconbase, uint16_t color, uint16_t bkcolor)
{
    uint16_t resize;                               /**用来存储一行有多少个字节*/
    uint16_t x0 = x;                               /**保存x的位置，切换到下一行使用*/
    uint16_t i, j;                               /**循环用*/
    uint8_t t = 0;                               /**用于判断一行是否刷新完毕*/
    uint8_t temp;
    resize = width/8 + (width % 8) ? 1 : 0;     /**每行的字节数*/

    for ( i = 0; i < resize * height; i++ )     /**总共有resize * height个字节的数据*/
    {
        temp = iconbase[i];
        for ( j = 0; j < 8; j++ )
        {
            if ( temp & 0x80 )
            {
                ltdc_draw_point(x, y, color);
            }
            else
            {
                ltdc_draw_point(x, y, bkcolor);
            }
            temp <<= 1;
            x++;
            t++;
            if ( t == width )
            {
                x = x0;
                y++;
                t = 0;
                break;
            }
        }
    }
}

/**
 * @brief   画线
 * @param   x1,y1:起点坐标
 * @param   x2,y2:终点坐标
 * @param   color:线的颜色
 * @retval  无
 * */
void ltdc_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row = x1, col = y1;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    if ( delta_x > 0 )
        incx = 1;
    else if ( delta_x == 0 )
        incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if ( delta_y > 0 )
        incy = 1;
    else if ( delta_y == 0 )
        incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y )
    {
        distance = delta_x;
    }
    else
    {
        distance = delta_y;
    }

    for ( t = 0; t <= distance ; ++t )
    {
        ltdc_draw_point(row, col, color);
        xerr += delta_x;
        yerr += delta_y;
        if ( xerr > distance )
        {
            xerr -= distance;
            row += incx;
        }
        if ( yerr > distance )
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief       画圆
 * @param       x0,y0:圆中心坐标
 * @param       r    :半径
 * @param       color:圆的颜色
 * @retval      无
 */
void ltdc_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       /* 判断下个点位置的标志 */

    while (a <= b)
    {
        ltdc_draw_point(x0 + a, y0 - b, color);  /* 5 */
        ltdc_draw_point(x0 + b, y0 - a, color);  /* 0 */
        ltdc_draw_point(x0 + b, y0 + a, color);  /* 4 */
        ltdc_draw_point(x0 + a, y0 + b, color);  /* 6 */
        ltdc_draw_point(x0 - a, y0 + b, color);  /* 1 */
        ltdc_draw_point(x0 - b, y0 + a, color);
        ltdc_draw_point(x0 - a, y0 - b, color);  /* 2 */
        ltdc_draw_point(x0 - b, y0 - a, color);  /* 7 */
        a++;

        /* 使用Bresenham算法画圆 */
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * @brief   画矩形
 * @param   x1,y1:起始点坐标
 * @param   x2,y2:终点坐标
 * @retval  无
 * */
void ltdc_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    ltdc_draw_line(x1, y1, x2, y1, color);
    ltdc_draw_line(x2, y1, x2, y2, color);
    ltdc_draw_line(x2, y2, x1, y2, color);
    ltdc_draw_line(x1, y2, x1, y1, color);
}

/**
 * @brief   在指定位置显示一个字符
 * @param   x,y:    坐标
 * @param   chr:    要显示的字符
 * @param   size:   字体大小 12/16/24/32
 * @param   mode:   :叠加方式(1),非叠加方式(2)
 * @param   color:  字体颜色
 * @retval  无
 * */
void ltdc_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);     /**区分采用那个字体*/
    chr = (char)chr - ' ';

    switch ( size )
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[(uint8_t)chr];
            break;
        case 16:
            pfont = (uint8_t *)asc2_1608[(uint8_t)chr];
            break;
        case 24:
            pfont = (uint8_t *)asc2_2412[(uint8_t)chr];
            break;
        case 32:
            pfont = (uint8_t *)asc2_3216[(uint8_t)chr];
            break;
        default:
            return;
    }

    for ( t = 0; t < csize; ++t )
    {
        temp = pfont[t];
        for ( t1 = 0; t1 < 8; ++t1 )
        {
            if ( temp & 0x80 )
            {
                ltdc_draw_point(x, y, color);
            }
            else
            {
                ltdc_draw_point(x, y, g_back_color);
            }

            temp <<= 1;
            y++;
            if ( (y - y0) ==  size)
            {
                y = y0;
                x++;

                if ( x >= ltdcdev.width)
                {
                    return;
                }
                break;
            }
        }
    }
}

/**
 * @brief   平方函数,m^n
 * @param   m:  底数
 * @param   n:  指数
 * @retval  m的n次方
 * */
static uint32_t ltdc_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while ( n-- )
    {
        result *= m;
    }
    return result;
}

/**
 * @brief   显示len个数字
 * @param   x,y     :起始坐标
 * @param   num     :数值(0 ~ 2^32)
 * @param   len     :显示数字的位数
 * @param   size    :选择的字体
 * @param   color   :颜色
 * @retval  无
 * */
void ltdc_show_num(uint16_t x,uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for ( t = 0; t < len; ++t )                                 /* 按总显示位数循环 */
    {
        temp = (num / ltdc_pow(10, len - t -1)) % 10;
        if ( enshow == 0 && t < (len - 1) )                     /* 没有使能显示,且还有位要显示 */
        {
            if ( temp == 0 )
            {
                ltdc_show_char(x + (size) / 2 * t, y, ' ', size, 0, color);     /* 显示空格,占位 */
                continue;                                                                      /* 继续下个一位 */
            }
            else
            {
                enshow = 1;                                                                     /* 使能显示 */
            }
        }
        ltdc_show_char(x + (size / 2) * t, y, temp + '0', size, 0, color);
    }
}


/**
 * @brief       扩展显示len个数字(高位是0也显示)
 * @param       x,y     :起始坐标
 * @param       num     :数值(0 ~ 2^32)
 * @param       len     :显示数字的位数
 * @param       size    :选择字体 12/16/24/32
 * @param       mode    :显示模式
 *              [7]:0,不填充;1,填充0.
 *              [6:1]:保留
 *              [0]:0,非叠加显示;1,叠加显示.
 * @param       color   :字体颜色
 * @retval      无
 */
 void ltdc_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
 {
     uint8_t t, temp;
     uint8_t enshow = 0;

     for ( t = 0; t < len; ++t )                                 /* 按总显示位数循环 */
     {
         temp = (num / ltdc_pow(10, len - t -1)) % 10;
         if ( enshow == 0 && t < (len - 1) )                     /* 没有使能显示,且还有位要显示 */
         {
             if ( temp == 0 )
             {
                 if ( mode & 0x80 )
                 {
                     ltdc_show_char(x + (size) / 2 * t, y, '0', size, 0, color);     /* 显示空格,占位 */
                 }
                 else/* 继续下个一位 */
                 {
                     ltdc_show_char(x + (size) / 2 * t, y, ' ', size, 0, color);     /* 显示空格,占位 */
                     continue;
                 }
             }
             else
             {
                 enshow = 1;                                                                     /* 使能显示 */
             }
         }
         ltdc_show_char(x + (size / 2) * t, y, temp + '0', size, 0, color);
     }
 }

 /**
  * @brief  显示字符串
  * @param  x,y             :起始坐标
  * @param  width,height    :区域大小
  * @param  size            :选择字体 12/16/24/32
  * @param  p               :字符串首地址
  * @param  color           :字体颜色
  * @retval 无
  * */
 void ltdc_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
 {
     uint8_t x0 = x;
     width += x;
     height += y;

     /**判断是不是非法字符*/
     while ( *p )
     {
         if((*p <= '~') || (*p >= ' '))
         {
             if ( x >= width )
             {
                 x = x0;
                 y += size;
             }
             if ( y >= width )
                 break;
             ltdc_show_char(x, y, *p, size, 1, color);
             x += size / 2;
             p++;
         }
     }
 }