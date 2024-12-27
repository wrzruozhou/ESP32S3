//
// Created by Administrator on 2024/12/27.
//

#ifndef LVGL_MYTEST_H
#define LVGL_MYTEST_H

#include "lvgl.h"

typedef struct {
    char name[32];
    int age;
}USER_DATA;

void task_test_start(void );
void key_handled(void);

#endif //LVGL_MYTEST_H
