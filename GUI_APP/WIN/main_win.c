#include "main_win.h"
#include "gui_app.h"
#include "login_win.h"
#include "set_win.h"

#include "BLL_MainFsm.h"


//��������
#define POINT_COUNT   20  //ÿ�������������е����ݵ����


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



//��������
static void event_handler(lv_obj_t * obj,lv_event_t event);


//�¼��ص�����
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



//��ҳѡ���ʼ��
void home_tab_init(lv_obj_t * parent)
{
	
	//����ledָʾ��
	lv_style_copy(&led_ng_style,&lv_style_plain_color);
	led_ng_style.body.radius = LV_RADIUS_CIRCLE;//���ư�Բ��
	led_ng_style.body.main_color = LV_COLOR_RED;
	led_ng_style.body.grad_color = LV_COLOR_RED;
	
	lv_style_copy(&led_ok_style,&led_ng_style);
	led_ok_style.body.main_color = LV_COLOR_LIME;
	led_ok_style.body.grad_color = LV_COLOR_LIME;
	

	led_ng = lv_led_create(parent, NULL);
	lv_obj_set_size(led_ng,25,25);//���ô�С
	lv_obj_set_style(led_ng, &led_ng_style);//������ʽ
	lv_obj_align(led_ng, NULL, LV_ALIGN_IN_TOP_RIGHT, -400, 110);
	
	led_ok = lv_led_create(parent, led_ng);
	lv_obj_set_style(led_ok, &led_ok_style);//������ʽ
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


	
	//����ͼ��
	lv_style_copy(&chart_style,&lv_style_pretty);
	chart_style.body.main_color = LV_COLOR_WHITE;//������Ϊ����ɫ
	chart_style.body.grad_color = chart_style.body.main_color;
	chart_style.body.border.color = LV_COLOR_BLACK;//�߿����ɫ
	chart_style.body.border.width = 3;//�߿�Ŀ��
	chart_style.body.border.opa = LV_OPA_COVER;
	chart_style.body.radius = 0;
	chart_style.line.color = LV_COLOR_GRAY;//�ָ��ߺͿ̶��ߵ���ɫ
	chart_style.text.color = LV_COLOR_BLUE;//���̶ȱ������ɫ
	
	
	chart = lv_chart_create(parent,NULL);
	lv_obj_set_size(chart,400,270);//����ͼ��Ĵ�С
	lv_obj_align(chart,NULL,LV_ALIGN_CENTER,240,55);//���ö��뷽ʽ
	lv_chart_set_type(chart,LV_CHART_TYPE_LINE);//����Ϊɢ������ߵ����
	lv_chart_set_series_opa(chart,LV_OPA_80);//���������ߵ�͸����,�����õĻ�,��LV_OPA_COVER��Ĭ��ֵ
	lv_chart_set_series_width(chart,4);//���������ߵĿ��
	lv_chart_set_series_darking(chart,LV_OPA_80);//���������ߵĺ���ӰЧ��
	lv_chart_set_style(chart,LV_CHART_STYLE_MAIN,&chart_style);//������ʽ
	lv_chart_set_point_count(chart,POINT_COUNT);//����ÿ�������������е����ݵ����,��������õĻ�,��Ĭ��ֵ��10
	lv_chart_set_div_line_count(chart,4,0);//����ˮƽ�ʹ�ֱ�ָ���
	lv_chart_set_range(chart,0,100);//����y�����ֵ��Χ,[0,100]Ҳ��Ĭ��ֵ
	lv_chart_set_y_tick_length(chart,10,3);//����y������̶��߳��Ⱥʹο̶��߳���
	lv_chart_set_y_tick_texts(chart,"100\n80\n60\n40\n20\n0",2,LV_CHART_AXIS_DRAW_LAST_TICK);
//	lv_chart_set_x_tick_length(chart,10,3);//����x������̶��߳��Ⱥʹο̶��߳���
//	lv_chart_set_x_tick_texts(chart,"0\n4\n8\n12\n16\n20",2,LV_CHART_AXIS_DRAW_LAST_TICK);//����x��Ŀ̶��������̶ȱ���
	lv_chart_set_margin(chart,40);//���ÿ̶�����ĸ߶�
	
	//������ť
	start_btn = lv_btn_create(parent, NULL);
	stop_btn = lv_btn_create(parent, NULL);
	lv_obj_set_size(start_btn,130,60);//���ô�С
	lv_obj_set_size(stop_btn,130,60);//���ô�С
	lv_obj_align(start_btn, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 70, -90);//���ö��뷽ʽ
	lv_obj_align(stop_btn, start_btn, LV_ALIGN_OUT_RIGHT_BOTTOM, 100, 0);//���ö��뷽ʽ
	lv_obj_t * start_btn_lable = lv_label_create(start_btn,NULL);//start_btn��label�Ӷ���
	lv_obj_t * stop_btn_lable = lv_label_create(stop_btn,NULL);//stop_btn��label�Ӷ���
	lv_label_set_text(start_btn_lable,"START");
	lv_label_set_text(stop_btn_lable,"STOP");
	lv_btn_set_layout(start_btn,LV_LAYOUT_CENTER);//���ð�ť�Ĳ��ַ�ʽ,ʹlabel�������м�,��Ȼ����������õĻ�,Ĭ��Ҳ�����м��
	lv_btn_set_layout(stop_btn,LV_LAYOUT_CENTER);
	lv_obj_set_event_cb(start_btn,event_handler);//�����¼��ص�����
	lv_obj_set_event_cb(stop_btn,event_handler);
	
	//����label��ǩ
	lv_style_copy(&label_press_style, &lv_style_plain_color);
	label_press_style.text.color = LV_COLOR_BLACK;
	label_press_style.text.font = &lv_font_roboto_22;
	label_press_style.body.main_color = LV_COLOR_WHITE;
	label_press_style.body.grad_color = LV_COLOR_WHITE;
	
	label_gagePress = lv_label_create(parent,NULL);//������ǩ
	lv_label_set_long_mode(label_gagePress,LV_LABEL_LONG_CROP);//���ó��ı�ģʽ
	lv_obj_set_size(label_gagePress, 150, 24);//���ÿ��,һ���÷���lv_label_set_long_mode�ĺ���,���������õ�
	lv_label_set_text(label_gagePress,"GAGE_PRESS:");//�����ı�
	lv_label_set_align(label_gagePress,LV_LABEL_ALIGN_LEFT);//�ı��������
	lv_label_set_style(label_gagePress,LV_LABEL_STYLE_MAIN,&label_press_style);//������������ʽ
	//ע��:��������Ļ�Ķ��뷽ʽ,����ӿ�Ҳ�Ǹ�����λ���й�ϵ��,��÷ŵ��������,��Ϊ�ŵ�̫ǰ���õĻ�,
	//����ʱ��ǩ����Ĵ�С���ܻ���δ֪��,��ʱlv_obj_align�ӿھ�û�취�������֮�������,�Ӷ�Ҳ��
	//�ﲻ��������Ҫ�Ķ���Ч��LV_COLOR_BLACK
	lv_obj_align(label_gagePress,NULL,LV_ALIGN_IN_LEFT_MID,70,-80);
	
	label_diffPress = lv_label_create(parent,label_gagePress);//������ǩ
	lv_label_set_long_mode(label_diffPress,LV_LABEL_LONG_CROP);//���ó��ı�ģʽ
	lv_obj_set_size(label_diffPress, 150, 24);//���ÿ��,һ���÷���lv_label_set_long_mode�ĺ���,���������õ�
	lv_label_set_text(label_diffPress,"DIFF_PRESS:");//�����ı�
	lv_label_set_align(label_diffPress,LV_LABEL_ALIGN_LEFT);//�ı��������
	lv_obj_align(label_diffPress,label_gagePress,LV_ALIGN_OUT_BOTTOM_LEFT,0,60);
	
	label_state = lv_label_create(parent,label_gagePress);//������ǩ
	lv_label_set_long_mode(label_state,LV_LABEL_LONG_CROP);//���ó��ı�ģʽ
	lv_obj_set_size(label_state, 150, 24);//���ÿ��,һ���÷���lv_label_set_long_mode�ĺ���,���������õ�
	lv_label_set_recolor(label_state, true);
	lv_label_set_text(label_state,"#ff0000 STANDBY#");//�����ı�
	lv_label_set_align(label_state,LV_LABEL_ALIGN_LEFT);//�ı��������
	lv_obj_align(label_state,label_gagePress,LV_ALIGN_OUT_TOP_MID,40,-60);
	
	
	gagePress = lv_label_create(parent,label_gagePress);//������ǩ
	lv_label_set_long_mode(gagePress,LV_LABEL_LONG_EXPAND);//���ó��ı�ģʽ
//	lv_obj_set_width(gagePress, 160);//���ÿ��,һ���÷���lv_label_set_long_mode�ĺ���,���������õ�
	lv_label_set_text(gagePress," ");//�����ı�
	lv_label_set_align(gagePress,LV_LABEL_ALIGN_LEFT);//�ı��������
	lv_label_set_style(gagePress,LV_LABEL_STYLE_MAIN,&label_press_style);//������������ʽ
	lv_label_set_body_draw(gagePress, true);//ʹ�ܱ�������
	lv_obj_align(gagePress,label_gagePress,LV_ALIGN_OUT_RIGHT_MID,0,0);
	
	diffPress = lv_label_create(parent,label_gagePress);//������ǩ
	lv_label_set_long_mode(diffPress,LV_LABEL_LONG_EXPAND);//���ó��ı�ģʽ
//	lv_obj_set_width(diffPress, 160);//���ÿ��,һ���÷���lv_label_set_long_mode�ĺ���,���������õ�
	lv_label_set_text(diffPress," ");//�����ı�
	lv_label_set_align(diffPress,LV_LABEL_ALIGN_LEFT);//�ı��������
	lv_label_set_style(diffPress,LV_LABEL_STYLE_MAIN,&label_press_style);//������������ʽ
	lv_label_set_body_draw(diffPress, true);//ʹ�ܱ�������
	lv_obj_align(diffPress,label_diffPress,LV_ALIGN_OUT_RIGHT_MID,0,0);
	
	label_ng = lv_label_create(parent,label_gagePress);//������ǩ
	lv_label_set_long_mode(label_ng,LV_LABEL_LONG_CROP);//���ó��ı�ģʽ
	lv_obj_set_size(label_ng, 60, 20);//���ÿ��,һ���÷���lv_label_set_long_mode�ĺ���,���������õ�
	lv_label_set_text(label_ng,"+NG");//�����ı�
	lv_label_set_align(label_ng,LV_LABEL_ALIGN_CENTER);//�ı��������
	lv_obj_align(label_ng,led_ng,LV_ALIGN_OUT_TOP_MID,0,-5);
	
	label_ok = lv_label_create(parent,label_gagePress);//������ǩ
	lv_label_set_long_mode(label_ok,LV_LABEL_LONG_CROP);//���ó��ı�ģʽ
	lv_obj_set_size(label_ok, 60, 20);//���ÿ��,һ���÷���lv_label_set_long_mode�ĺ���,���������õ�
	lv_label_set_text(label_ok,"OK");//�����ı�
	lv_label_set_align(label_ok,LV_LABEL_ALIGN_CENTER);//�ı��������
	lv_obj_align(label_ok,led_ok,LV_ALIGN_OUT_TOP_MID,0,-5);
	
	label_minus_ng = lv_label_create(parent,label_gagePress);//������ǩ
	lv_label_set_long_mode(label_minus_ng,LV_LABEL_LONG_CROP);//���ó��ı�ģʽ
	lv_obj_set_size(label_minus_ng, 60, 20);//���ÿ��,һ���÷���lv_label_set_long_mode�ĺ���,���������õ�
	lv_label_set_text(label_minus_ng,"-NG");//�����ı�
	lv_label_set_align(label_minus_ng,LV_LABEL_ALIGN_CENTER);//�ı��������
	lv_obj_align(label_minus_ng,led_minus_ng,LV_ALIGN_OUT_TOP_MID,0,-5);
	
	label_p_ng = lv_label_create(parent,label_gagePress);//������ǩ
	lv_label_set_long_mode(label_p_ng,LV_LABEL_LONG_CROP);//���ó��ı�ģʽ
	lv_obj_set_size(label_p_ng, 60, 20);//���ÿ��,һ���÷���lv_label_set_long_mode�ĺ���,���������õ�
	lv_label_set_text(label_p_ng,"P.NG");//�����ı�
	lv_label_set_align(label_p_ng,LV_LABEL_ALIGN_CENTER);//�ı��������
	lv_obj_align(label_p_ng,led_p_ng,LV_ALIGN_OUT_TOP_MID,0,-5);
	

	
}





//����������
void main_win_create(lv_obj_t * parent)
{
	static lv_style_t scr_bg_style;
	lv_style_copy(&scr_bg_style,&lv_style_plain_color);
	scr_bg_style.body.main_color = LV_COLOR_CYAN;
	scr_bg_style.body.grad_color = scr_bg_style.body.main_color;
//	lv_tabview_set_style(main_win,LV_TABVIEW_STYLE_BG,&scr_bg_style);
	//Ϊ�˼��ٸ�����,���������ֱ�Ӱ�tabview��Ϊmain_win������
	main_win = lv_tabview_create(parent,NULL);
	lv_tabview_set_btns_pos(main_win,LV_TABVIEW_BTNS_POS_BOTTOM);//ѡ���ťλ�ڵײ�
	lv_tabview_set_style(main_win,LV_TABVIEW_STYLE_BG,&scr_bg_style); //���ñ�����ʽ ����Ļ����һ����ɫ�ı���,�����ǵ�ͼ����ʾ������,ͬʱ���Խ��ɫ������
	
	lv_tabview_set_anim_time(main_win, 0);
	//���tab1ѡ�
	home_tab = lv_tabview_add_tab(main_win,LV_SYMBOL_HOME" Home");
	home_tab_init(home_tab);
	//���tab1ѡ�
	setting_tab = lv_tabview_add_tab(main_win,LV_SYMBOL_SETTINGS" Setting");
	setting_tab_init(setting_tab);
	
	
}










