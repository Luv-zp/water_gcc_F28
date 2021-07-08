#ifndef __MAIN_WIN_H__
#define __MAIN_WIN_H__
#include "sys.h"
#include "lvgl.h"


//变量申明
extern lv_obj_t * main_win;
extern lv_obj_t * home_tab;
extern lv_obj_t * setting_tab;
extern lv_obj_t * label_state;
extern lv_obj_t * gagePress;
extern lv_obj_t * diffPress;
extern lv_obj_t * led_ng;
extern lv_obj_t * led_ok;
extern lv_obj_t * led_minus_ng;
extern lv_obj_t * led_p_ng;


//函数申明
void main_win_create(lv_obj_t * parent);

#endif
