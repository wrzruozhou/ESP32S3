//
// Created by Administrator on 2024/10/9.
//

#include "qma6100a.h"

i2c_obj_t qma6100p_i2c_master;

#define QMA6100P_INT    gpio_get_level(qma6100p_self.qma_int)
#define I2C_MASTER_TIMEOUT_MS   1000
#define M_PI                    3.141582653589793f
#define M_G                     9.80665f
#define RAD_TO_DEG              (180.0f / M_PI)

/**
 * @brief   读取qma6100p的数据
 * @param   reg_addr:   要读取的寄存器地址
 * @param   data    :   读取的数据
 * @param   len     :   数据的大小
 * @retval  错误值   :     0:成功，其他值:失败
 * */
esp_err_t qma6100p_register_read(const uint8_t reg,uint8_t *data, const size_t len)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0] = reg;

    i2c_buf_t bufs[2] = {
        {.len = 1, .buf = memaddr_buf},
        {.len = len, .buf = data},
    };
    i2c_transfer(&qma6100p_i2c_master, QMA6100P_ADDR, 2, bufs, I2C_FLAG_WRITE | I2C_FLAG_READ | I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief   向qma6100p寄存器写数据
 * @param   reg_addr    :要写入的寄存器地址
 * @param   data        :要写入的数据
 * @retval  错误值         0:成功,其他值:错误
 * */
static esp_err_t qma6100p_register_write_byte(uint8_t reg, uint8_t data)
{
    uint8_t memaddr_buf[1];
    memaddr_buf[0] = reg;

    i2c_buf_t bufs[2] = {
        {.len = 1, .buf = memaddr_buf},
        {.len = 1, .buf = &data}
    };

    i2c_transfer(&qma6100p_i2c_master, QMA6100P_ADDR, 2, bufs, I2C_FLAG_STOP);
    return ESP_OK;
}

uint8_t xyz_data[6] = {0};
short raw_data[3] = {0};
float accl_data[3] = {0};
float acc_normal;

/**
 * @brief   读三轴数据(原始数据、加速度、俯仰角、翻滚角)
 * @param   rawdata: qma6100p数据的结构体
 * @retval  无
 * */
void qma6100p_read_rawdata(qma6100p_rawdata_t *rawdata)
{
    qma6100p_register_read(QMA6100P_REG_XOUTL, xyz_data, 6);

    raw_data[0] = (short )((xyz_data[1] << 8) | (xyz_data[0]));
    raw_data[1] = (short )((xyz_data[3] << 8) | (xyz_data[2]));
    raw_data[2] = (short )((xyz_data[5] << 8) | (xyz_data[4]));

    rawdata->acc_x = (float )((raw_data[0] >> 2) * M_G) / (1024);
    rawdata->acc_y = (float )((raw_data[1] >> 2) * M_G) / (1024);
    rawdata->acc_z = (float )((raw_data[2] >> 2) * M_G) / (1024);

    rawdata->acc_g = sqrt(rawdata->acc_x * rawdata->acc_x + rawdata->acc_y * rawdata->acc_y + rawdata->acc_z * rawdata->acc_z);

    acc_normal = sqrtf(rawdata->acc_x * rawdata->acc_x + rawdata->acc_y * rawdata->acc_y + rawdata->acc_z * rawdata->acc_z);
    accl_data[0] = rawdata->acc_x / acc_normal;
    accl_data[1] = rawdata->acc_y / acc_normal;
    accl_data[2] = rawdata->acc_z /acc_normal;

    rawdata->pitch = -atan2f(rawdata->acc_x, rawdata->acc_z) * RAD_TO_DEG;

    acc_normal = sqrtf(accl_data[0] * accl_data[0] + accl_data[1] * accl_data[1] + accl_data[2] * accl_data[2]);

    rawdata->roll = asinf((accl_data[1]/acc_normal)) * RAD_TO_DEG ;
}

/**
 * @brief   配置中断函数
 * @param   int_map:中断模式
 * @param   enable:使能
 * @retval  mp_const_none:初始化成功
 * */
void qma6100p_step_int_config(int int_map, int enable)
{
    uint8_t reg_16 = 0;
    uint8_t reg_19 = 0;
    uint8_t reg_1b = 0;

    qma6100p_register_read(0x16, &reg_16, 1);
    qma6100p_register_read(0x19, &reg_19, 1);
    qma6100p_register_read(0x1b, &reg_1b, 1);

    if ( enable )
    {
        reg_16 |= 0x08;
        reg_19 |= 0x08;
        reg_1b |= 0x08;

        qma6100p_register_write_byte(0x16, reg_16);
        if ( int_map == QMA6100P_MAP_INT1 )
        {
            qma6100p_register_write_byte(0x19, reg_19);
        }
        else if ( int_map == QMA6100P_MAP_INT2 )
        {
            qma6100p_register_write_byte(0x1b, reg_1b);
        }
    }
    else
    {
        reg_16 &= (~0x08);
        reg_19 &= (~0x08);
        reg_1b &= (~0x08);

        qma6100p_register_write_byte(0x16, reg_16);
        qma6100p_register_write_byte(0x19, reg_19);
        qma6100p_register_write_byte(0x1b, reg_1b);
    }
}

/**
 * @brief   初始化QMA6100p
 * @param   无
 * @retval  0,成功
 *          1,失败
 * */
uint8_t qma6100p_config(void )
{
    static uint8_t id_data[2];
    qma6100p_register_read(QMA6100P_REG_CHIP_ID,id_data,1);     /**读取设备ID，正常是0x90*/

    /**qma6100p的初始化序列,请看手册"6.3 Initial sequence"章节*/
    qma6100p_register_write_byte(QMA6100P_REG_RESET, QMA6100P_RESET);
    vTaskDelay(5);
    qma6100p_register_write_byte(QMA6100P_REG_RESET, QMA6100P_RESET_END);
    vTaskDelay(10);

    qma6100p_register_read(QMA6100P_REG_CHIP_ID,id_data,1);     /**读取设备ID，正常是0x90*/

    qma6100p_register_write_byte(0x11, 0x80);
    qma6100p_register_write_byte(0x11, 0x84);
    qma6100p_register_write_byte(0x4a, 0x20);
    qma6100p_register_write_byte(0x56, 0x01);
    qma6100p_register_write_byte(0x5f, 0x80);
    vTaskDelay(1);
    qma6100p_register_write_byte(0x5f, 0x00);
    vTaskDelay(10);

    qma6100p_register_write_byte(QMA6100P_REG_RANGE, QMA6100P_RANGE_8G);
    qma6100p_register_write_byte(QMA6100P_REG_BW_ODR, QMA6100P_BW_100);
    qma6100p_register_write_byte(QMA6100P_REG_POWER_MANAGE, QMA6100P_MCLK_51_2K | 0x80);

    qma6100p_register_write_byte(0x21, 0x03);               /**default 0x1c, step latch mode*/

    qma6100p_step_int_config(QMA6100P_MAP_INT1, 1);

    if ( id_data[0] == 0x90 )
    {
        ESP_LOGE("qma6100p", "qma6100p success!!!");
        return 0;
    }
    else
    {
        ESP_LOGE("qma6100p", "qma6100p fail!!!");
        return 1;
    }
}

/**
 * @brief   qma6100p的初始化
 * @param   无
 * @param   无
 * */
void qma6100p_init(i2c_obj_t self)
{
    if ( self.init_flag == ESP_FAIL )
    {
        iic_init(I2C_NUM_0);        /**初始化IIC*/
    }
    qma6100p_i2c_master = self;
    while ( qma6100p_config() )     /**检测不到qma6100p*/
    {
        ESP_LOGE("qma6100p", "qma6100p init fail!!!");
        vTaskDelay(500);
    }
}