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
        IIC)

set(include_dirs
        LED
        KEY
        UART
        ESPTIMER
        GPTIMER
        SW_PWM
        XL9555
        IIC)

set(requires
        driver
        esp_timer)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)
