#include "main_win.h"
#include "gui_app.h"
#include "login_win.h"
#include "set_win.h"

#include "BLL_MainFsm.h"


//变量定义
#define POINT_COUNT   20  //每条数据线所具有的数据点个数


static lv_style_t led_ng_style;
static lv_style_t led_ok_style;
static lv_style_t chart_style;
static lv_style_t label_press_style;

lv_obj_t *  main_win;
lv_obj_t * home_tab;
lv_obj_t * setting_tab;

lv_obj_t * led_ng;
lv_obj_t * led_ok;
lv_obj_t * led_minus_ng;
lv_obj_t * led_p_ng;
lv_obj_t * chart;

lv_obj_t * label_state;
static lv_obj_t * start_btn;
static lv_obj_t * stop_btn;
static lv_obj_t * label_gagePress;
static lv_obj_t * label_diffPress;
lv_obj_t * gagePress;
lv_obj_t * diffPress;
static lv_obj_t * label_ng;
static lv_obj_t * label_ok;
static lv_obj_t * label_minus_ng;
static lv_obj_t * label_p_ng;



//函数申明
static void event_handler(lv_obj_t * obj,lv_event_t event);


//事件回调函数
static void event_handler(lv_obj_t * obj,lv_event_t event)
{
	AppMessage appMsg;
	FiniteStateMachine * me = FiniteStateMachine_create();
	
	if(obj == start_btn)
	{
		if(event==LV_EVENT_RELEASED)
		{
			appMsg.dataType = EV_START;
			appMsg.pVoid = 0;
			me->sendEvent(&appMsg);
		}
	}
	else if(obj == stop_btn)
	{
		if(event==LV_EVENT_RELEASED)
		{
			appMsg.dataType = EV_STOP;
			appMsg.pVoid = 0;
			me->sendEvent(&appMsg);
		}
	}

}



//首页选项卡初始化
void home_tab_init(lv_obj_t * parent)
{
	
	//创建led指示灯
	lv_style_copy(&led_ng_style,&lv_style_plain_color);
	led_ng_style.body.radius = LV_RADIUS_CIRCLE;//绘制半圆角
	led_ng_style.body.main_color = LV_COLOR_RED;
	led_ng_style.body.grad_color = LV_COLOR_RED;
	
	lv_style_copy(&led_ok_style,&led_ng_style);
	led_ok_style.body.main_color = LV_COLOR_LIME;
	led_ok_style.body.grad_color = LV_COLOR_LIME;
	

	led_ng = lv_led_create(parent, NULL);
	lv_obj_set_size(led_ng,25,25);//设置大小
	lv_obj_set_style(led_ng, &led_ng_style);//设置样式
	lv_obj_align(led_ng, NULL, LV_ALIGN_IN_TOP_RIGHT, -400, 110);
	
	led_ok = lv_led_create(parent, led_ng);
	lv_obj_set_style(led_ok, &led_ok_style);//设置样式
	lv_obj_align(led_ok, led_ng, LV_ALIGN_OUT_RIGHT_TOP, 70, 0);
	
	led_minus_ng = lv_led_create(parent, led_ng);
	lv_obj_align(led_minus_ng, led_ok, LV_ALIGN_OUT_RIGHT_TOP, 70, 0);

	led_p_ng = lv_led_create(parent, led_ng);
	lv_obj_align(led_p_ng, led_minus_ng, LV_ALIGN_OUT_RIGHT_TOP, 70, 0);
	
	lv_led_off(led_ng);
	lv_led_off(led_ok);
	lv_led_off(led_minus_ng);
	lv_led_off(led_p_ng);

//	lv_led_on(led_ng);
//	lv_led_on(led_ok);
//	lv_led_on(led_minus_ng);
//	lv_led_on(led_p_ng);


	
	//创建图表
	lv_style_copy(&chart_style,&lv_style_pretty);
	chart_style.body.main_color = LV_COLOR_WHITE;//主背景为纯白色
	chart_style.body.grad_color = chart_style.body.main_color;
	chart_style.body.border.color = LV_COLOR_BLACK;//边框的颜色
	chart_style.body.border.width = 3;//边框的宽度
	chart_style.body.border.opa = LV_OPA_COVER;
	chart_style.body.radius = 0;
	chart_style.line.color = LV_COLOR_GRAY;//分割线和刻度线的颜色
	chart_style.text.color = LV_COLOR_BLUE;//主刻度标题的颜色
	
	
	chart = lv_chart_create(parent,NULL);
	lv_obj_set_size(chart,400,270);//设置图表的大小
	lv_obj_align(chart,NULL,LV_ALIGN_CENTER,240,55);//设置对齐方式
	lv_chart_set_type(chart,LV_CHART_TYPE_LINE);//设置为散点和折线的组合
	lv_chart_set_series_opa(chart,LV_OPA_80);//设置数据线的透明度,不设置的话,则LV_OPA_COVER是默认值
	lv_chart_set_series_width(chart,4);//设置数据线的宽度
	lv_chart_set_series_darking(chart,LV_OPA_80);//设置数据线的黑阴影效果
	lv_chart_set_style(chart,LV_CHART_STYLE_MAIN,&chart_style);//设置样式
	lv_chart_set_point_count(chart,POINT_COUNT);//设置每条数据线所具有的数据点个数,如果不设置的话,则默认值是10
	lv_chart_set_div_line_count(chart,4,0);//设置水平和垂直分割线
	lv_chart_set_range(chart,0,100);//设置y轴的数值范围,[0,100]也是默认值
	lv_chart_set_y_tick_length(chart,10,3);//设置y轴的主刻度线长度和次刻度线长度
	lv_chart_set_y_tick_texts(chart,"100\n80\n60\n40\n20\n0",2,LV_CHART_AXIS_DRAW_LAST_TICK);
//	lv_chart_set_x_tick_length(chart,10,3);//设置x轴的主刻度线长度和次刻度线长度
//	lv_chart_set_x_tick_texts(chart,"0\n4\n8\n12\n16\n20",2,LV_CHART_AXIS_DRAW_LAST_TICK);//设置x轴的刻度数和主刻度标题
	lv_chart_set_margin(chart,40);//设置刻度区域的高度
	
	//创建按钮
	start_btn = lv_btn_create(parent, NULL);
	stop_btn = lv_btn_create(parent, NULL);
	lv_obj_set_size(start_btn,130,60);//设置大小
	lv_obj_set_size(stop_btn,130,60);//设置大小
	lv_obj_align(start_btn, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 70, -90);//设置对齐方式
	lv_obj_align(stop_btn, start_btn, LV_ALIGN_OUT_RIGHT_BOTTOM, 100, 0);//设置对齐方式
	lv_obj_t * start_btn_lable = lv_label_create(start_btn,NULL);//start_btn加label子对象
	lv_obj_t * stop_btn_lable = lv_label_create(stop_btn,NULL);//stop_btn加label子对象
	lv_label_set_text(start_btn_lable,"START");
	lv_label_set_text(stop_btn_lable,"STOP");
	lv_btn_set_layout(start_btn,LV_LAYOUT_CENTER);//设置按钮的布局方式,使label处于正中间,当然了如果不设置的话,默认也是正中间的
	lv_btn_set_layout(stop_btn,LV_LAYOUT_CENTER);
	lv_obj_set_event_cb(start_btn,event_handler);//设置事件回调函数
	lv_obj_set_event_cb(stop_btn,event_handler);
	
	//创建label标签
	lv_style_copy(&label_press_style, &lv_style_plain_color);
	label_press_style.text.color = LV_COLOR_BLACK;
	label_press_style.text.font = &lv_font_roboto_22;
	label_press_style.body.main_color = LV_COLOR_WHITE;
	label_press_style.body.grad_color = LV_COLOR_WHITE;
	
	label_gagePress = lv_label_create(parent,NULL);//创建标签
	lv_label_set_long_mode(label_gagePress,LV_LABEL_LONG_CROP);//设置长文本模式
	lv_obj_set_size(label_gagePress, 150, 24);//设置宽度,一定得放在lv_label_set_long_mode的后面,否则不起作用的
	lv_label_set_text(label_gagePress,"GAGE_PRESS:");//设置文本
	lv_label_set_align(label_gagePress,LV_LABEL_ALIGN_LEFT);//文本居左对齐
	lv_label_set_style(label_gagePress,LV_LABEL_STYLE_MAIN,&label_press_style);//设置主背景样式
	//注意:设置与屏幕的对齐方式,这个接口也是跟调用位置有关系的,最好放到后面调用,因为放的太前调用的话,
	//在那时标签对象的大小可能还是未知的,此时lv_obj_align接口就没办法算出对齐之后的坐标,从而也就
	//达不到我们所要的对齐效果LV_COLOR_BLACK
	lv_obj_align(label_gagePress,NULL,LV_ALIGN_IN_LEFT_MID,70,-80);
	
	label_diffPress = lv_label_create(parent,label_gagePress);//创建标签
	lv_label_set_long_mode(label_diffPress,LV_LABEL_LONG_CROP);//设置长文本模式
	lv_obj_set_size(label_diffPress, 150, 24);//设置宽度,一定得放在lv_label_set_long_mode的后面,否则不起作用的
	lv_label_set_text(label_diffPress,"DIFF_PRESS:");//设置文本
	lv_label_set_align(label_diffPress,LV_LABEL_ALIGN_LEFT);//文本居左对齐
	lv_obj_align(label_diffPress,label_gagePress,LV_ALIGN_OUT_BOTTOM_LEFT,0,60);
	
	label_state = lv_label_create(parent,label_gagePress);//创建标签
	lv_label_set_long_mode(label_state,LV_LABEL_LONG_CROP);//设置长文本模式
	lv_obj_set_size(label_state, 150, 24);//设置宽度,一定得放在lv_label_set_long_mode的后面,否则不起作用的
	lv_label_set_recolor(label_state, true);
	lv_label_set_text(label_state,"#ff0000 STANDBY#");//设置文本
	lv_label_set_align(label_state,LV_LABEL_ALIGN_LEFT);//文本居左对齐
	lv_obj_align(label_state,label_gagePress,LV_ALIGN_OUT_TOP_MID,40,-60);
	
	
	gagePress = lv_label_create(parent,label_gagePress);//创建标签
	lv_label_set_long_mode(gagePress,LV_LABEL_LONG_EXPAND);//设置长文本模式
//	lv_obj_set_width(gagePress, 160);//设置宽度,一定得放在lv_label_set_long_mode的后面,否则不起作用的
	lv_label_set_text(gagePress," ");//设置文本
	lv_label_set_align(gagePress,LV_LABEL_ALIGN_LEFT);//文本居左对齐
	lv_label_set_style(gagePress,LV_LABEL_STYLE_MAIN,&label_press_style);//设置主背景样式
	lv_label_set_body_draw(gagePress, true);//使能背景绘制
	lv_obj_align(gagePress,label_gagePress,LV_ALIGN_OUT_RIGHT_MID,0,0);
	
	diffPress = lv_label_create(parent,label_gagePress);//创建标签
	lv_label_set_long_mode(diffPress,LV_LABEL_LONG_EXPAND);//设置长文本模式
//	lv_obj_set_width(diffPress, 160);//设置宽度,一定得放在lv_label_set_long_mode的后面,否则不起作用的
	lv_label_set_text(diffPress," ");//设置文本
	lv_label_set_align(diffPress,LV_LABEL_ALIGN_LEFT);//文本居左对齐
	lv_label_set_style(diffPress,LV_LABEL_STYLE_MAIN,&label_press_style);//设置主背景样式
	lv_label_set_body_draw(diffPress, true);//使能背景绘制
	lv_obj_align(diffPress,label_diffPress,LV_ALIGN_OUT_RIGHT_MID,0,0);
	
	label_ng = lv_label_create(parent,label_gagePress);//创建标签
	lv_label_set_long_mode(label_ng,LV_LABEL_LONG_CROP);//设置长文本模式
	lv_obj_set_size(label_ng, 60, 20);//设置宽度,一定得放在lv_label_set_long_mode的后面,否则不起作用的
	lv_label_set_text(label_ng,"+NG");//设置文本
	lv_label_set_align(label_ng,LV_LABEL_ALIGN_CENTER);//文本居左对齐
	lv_obj_align(label_ng,led_ng,LV_ALIGN_OUT_TOP_MID,0,-5);
	
	label_ok = lv_label_create(parent,label_gagePress);//创建标签
	lv_label_set_long_mode(label_ok,LV_LABEL_LONG_CROP);//设置长文本模式
	lv_obj_set_size(label_ok, 60, 20);//设置宽度,一定得放在lv_label_set_long_mode的后面,否则不起作用的
	lv_label_set_text(label_ok,"OK");//设置文本
	lv_label_set_align(label_ok,LV_LABEL_ALIGN_CENTER);//文本居左对齐
	lv_obj_align(label_ok,led_ok,LV_ALIGN_OUT_TOP_MID,0,-5);
	
	label_minus_ng = lv_label_create(parent,label_gagePress);//创建标签
	lv_label_set_long_mode(label_minus_ng,LV_LABEL_LONG_CROP);//设置长文本模式
	lv_obj_set_size(label_minus_ng, 60, 20);//设置宽度,一定得放在lv_label_set_long_mode的后面,否则不起作用的
	lv_label_set_text(label_minus_ng,"-NG");//设置文本
	lv_label_set_align(label_minus_ng,LV_LABEL_ALIGN_CENTER);//文本居左对齐
	lv_obj_align(label_minus_ng,led_minus_ng,LV_ALIGN_OUT_TOP_MID,0,-5);
	
	label_p_ng = lv_label_create(parent,label_gagePress);//创建标签
	lv_label_set_long_mode(label_p_ng,LV_LABEL_LONG_CROP);//设置长文本模式
	lv_obj_set_size(label_p_ng, 60, 20);//设置宽度,一定得放在lv_label_set_long_mode的后面,否则不起作用的
	lv_label_set_text(label_p_ng,"P.NG");//设置文本
	lv_label_set_align(label_p_ng,LV_LABEL_ALIGN_CENTER);//文本居左对齐
	lv_obj_align(label_p_ng,led_p_ng,LV_ALIGN_OUT_TOP_MID,0,-5);
	

	
}





//创建主界面
void main_win_create(lv_obj_t * parent)
{
	static lv_style_t scr_bg_style;
	lv_style_copy(&scr_bg_style,&lv_style_plain_color);
	scr_bg_style.body.main_color = LV_COLOR_CYAN;
	scr_bg_style.body.grad_color = scr_bg_style.body.main_color;
//	lv_tabview_set_style(main_win,LV_TABVIEW_STYLE_BG,&scr_bg_style);
	//为了减少复杂性,我们这里就直接把tabview作为main_win窗体了
	main_win = lv_tabview_create(parent,NULL);
	lv_tabview_set_btns_pos(main_win,LV_TABVIEW_BTNS_POS_BOTTOM);//选项卡按钮位于底部
	lv_tabview_set_style(main_win,LV_TABVIEW_STYLE_BG,&scr_bg_style); //设置背景样式 给屏幕设置一个纯色的背景,让我们的图表显示更明显,同时可以解决色点现象
	
	lv_tabview_set_anim_time(main_win, 0);
	//添加tab1选项卡
	home_tab = lv_tabview_add_tab(main_win,LV_SYMBOL_HOME" Home");
	home_tab_init(home_tab);
	//添加tab1选项卡
	setting_tab = lv_tabview_add_tab(main_win,LV_SYMBOL_SETTINGS" Setting");
	setting_tab_init(setting_tab);
	
	
}










