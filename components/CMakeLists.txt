##源文件路径
#set(src_dirs
#            LED)
##设置头文件路径
#set(include_dirs
#            LED)
##设置依赖库
#set(requires
#            driver)
#
#idf_component_register(SRC_DIRS ${src_dirs}
#                    INCLUDE_DIRS ${include_dris} REQUIRES ${requires})
#
#component_compile_options(--ffast-math -O3 -Wno-error=format=-Wno-format)

set(src_dirs
        LED
        KEY
        UART
        ESPTIMER
        GPTIMER
        SW_PWM
        XL9555
        IIC
        24CXX
        ADC
        AP3216C
        SERNSOR
        WIFI_USER
        RGBLCD
        UDP)

set(include_dirs
        LED
        KEY
        UART
        ESPTIMER
        GPTIMER
        SW_PWM
        XL9555
        IIC
        24CXX
        ADC
        AP3216C
        SERNSOR
        WIFI_USER
        RGBLCD
        UDP)

set(requires
        driver
        esp_timer
        esp_adc
        nvs_flash
        esp_wifi
        esp_lcd)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)
