#include "main_win.h"
#include "gui_app.h"
#include "login_win.h"
#include "gui_function.h"

#include "led.h"
#include "userApp.h"
#include "BLL_MainFsm.h"


//#define SPLIT_LINE_WIDTH		200 //ˮƽ�ָ��ߵĿ��
//const lv_point_t SPLIT_LINE_POINTS[2] = {{0,0},{SPLIT_LINE_WIDTH,0}};
//const char * const MBOX_MAP[] ={"Yes","No",""};

const char * c_settingPara[12] = {NULL};

/* ��ʽ */
static lv_style_t label_style;
static lv_style_t ta_style;

/* ��ǩ�ؼ� */
static lv_obj_t * label_readyTime;
static lv_obj_t * label_delayTime;
static lv_obj_t * label_chargeTime;
static lv_obj_t * label_balanceTime;
static lv_obj_t * label_detectTime;
static lv_obj_t * label_exhaustTime;
static lv_obj_t * label_time_unit;
static lv_obj_t * label_bal_work_limit;
static lv_obj_t * label_bal_crit_limit;
static lv_obj_t * label_leak_work_limit;
static lv_obj_t * label_leak_crit_limit;
static lv_obj_t * label_press_max_limit;
static lv_obj_t * label_press_min_limit;
static lv_obj_t * label_press_unit;


/* �ı���ؼ� */
static lv_obj_t * ta1_readyTime;
static lv_obj_t * ta1_delayTime;
static lv_obj_t * ta1_balanceTime;
static lv_obj_t * ta1_chargeTime;
static lv_obj_t * ta1_detectTime;
static lv_obj_t * ta1_exhaustTime;
static lv_obj_t * ta1_bal_work;
static lv_obj_t * ta1_bal_crit;
static lv_obj_t * ta1_lea_work;
static lv_obj_t * ta1_lea_crit;
static lv_obj_t * ta1_pres_max;
static lv_obj_t * ta1_pres_min;
						
static lv_obj_t * apply_btn;
static lv_obj_t * cancel_btn;
							
static lv_obj_t * kb = NULL;

//static lv_obj_t * ta1_IP1;
//static lv_obj_t * ta1_IP2;
//static lv_obj_t * ta1_IP3;
//static lv_obj_t * ta1_IP4;

//static lv_obj_t * ta1_year;
//static lv_obj_t * ta1_month;
//static lv_obj_t * ta1_day;
//static lv_obj_t * ta1_hour;
//static lv_obj_t * ta1_minute;
//static lv_obj_t * ta1_second;


//static lv_obj_t * quit_btn;
//static lv_obj_t * quit_mbox = NULL;

/* �������� */
static void settingParaApply(const char *p_array[12]);
static void kb_create(lv_obj_t * bind_ta);
static void event_handler(lv_obj_t * obj,lv_event_t event);
//static void quit_mbox_create(lv_obj_t * parent);

static void settingParaApply(const char *p_array[])
{
	p_array[0] = lv_ta_get_text(ta1_readyTime);
	p_array[1] = lv_ta_get_text(ta1_delayTime);
	p_array[2] = lv_ta_get_text(ta1_chargeTime);
	p_array[3] = lv_ta_get_text(ta1_balanceTime);
	p_array[4] = lv_ta_get_text(ta1_detectTime);
	p_array[5] = lv_ta_get_text(ta1_exhaustTime);
	p_array[6] = lv_ta_get_text(ta1_bal_work);
	p_array[7] = lv_ta_get_text(ta1_bal_crit);
	p_array[8] = lv_ta_get_text(ta1_lea_work);
	p_array[9] = lv_ta_get_text(ta1_lea_crit);
	p_array[10] = lv_ta_get_text(ta1_pres_max);
	p_array[11] = lv_ta_get_text(ta1_pres_min);
}
	
static void kb_create(lv_obj_t * bind_ta)
{
	if(kb == NULL)
	{
		kb = lv_kb_create(main_win,NULL);
		lv_kb_set_cursor_manage(kb,true);//�Թ����й���
		lv_obj_set_event_cb(kb,event_handler);//�����¼��ص�����
	}
	lv_kb_set_mode(kb,LV_KB_MODE_NUM);//�򵯳����ּ���
	lv_kb_set_ta(kb,bind_ta);//�������
}

static void event_handler(lv_obj_t * obj,lv_event_t event)
{
	AppMessage appMsg;
	int i = 0;
	static float f_paraSetiing[12];
	FiniteStateMachine * me = FiniteStateMachine_create();
	if(obj==ta1_readyTime||obj==ta1_delayTime||obj==ta1_balanceTime||obj==ta1_chargeTime||obj==ta1_detectTime||obj==ta1_exhaustTime||
		obj==ta1_bal_work||obj==ta1_bal_crit||obj==ta1_lea_work||obj==ta1_lea_crit||obj==ta1_pres_max||obj==ta1_pres_min)
	{
		if(event==LV_EVENT_RELEASED)//�����¼�
		{
			lv_ta_set_cursor_type(ta1_readyTime,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_delayTime,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_balanceTime,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_chargeTime,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_detectTime,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_exhaustTime,LV_CURSOR_HIDDEN);//�����ع��
			
			lv_ta_set_cursor_type(ta1_bal_work,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_bal_crit,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_lea_work,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_lea_crit,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_pres_max,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_pres_min,LV_CURSOR_HIDDEN);//�����ع��
			
			lv_ta_set_cursor_type(obj,LV_CURSOR_LINE);//�ȴ򿪹��
			kb_create(obj);//��������
		}
	}
	else if(obj==kb)
	{
		lv_kb_def_event_cb(kb,event);//���ü��̵�Ĭ���¼��ص�����
		if(event==LV_EVENT_APPLY||event==LV_EVENT_CANCEL)//����˼����ϵġ̻��ߡ�����
		{
			lv_obj_del(kb);//ɾ������
			kb = NULL;
			lv_ta_set_cursor_type(ta1_readyTime,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_delayTime,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_balanceTime,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_chargeTime,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_detectTime,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_exhaustTime,LV_CURSOR_HIDDEN);//�����ع��
			
			lv_ta_set_cursor_type(ta1_bal_work,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_bal_crit,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_lea_work,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_lea_crit,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_pres_max,LV_CURSOR_HIDDEN);//�����ع��
			lv_ta_set_cursor_type(ta1_pres_min,LV_CURSOR_HIDDEN);//�����ع��
		}
	}
	else if(obj == apply_btn)
	{
		if(event==LV_EVENT_RELEASED)//�����¼�
		{
			settingParaApply(c_settingPara);
			for(i = 0; i < 12; i++)
			{
				f_paraSetiing[i] = str2float(c_settingPara[i]);
			}
			appMsg.dataType = EV_SET_PARA;
			appMsg.pVoid = f_paraSetiing;
			me->sendEvent(&appMsg);
		}
	}
	else if(obj == cancel_btn)
	{
		if(event==LV_EVENT_RELEASED)//�����¼�
		{
		}
	}
//	else if(obj == main_win && event==LV_EVENT_VALUE_CHANGED)
//	{
//		uint16_t page_id = lv_tabview_get_tab_act(main_win);
//		if(page_id == 0)
//		{
//			
//		}	
//	}
}

//�����˳�ȷ�ϵ���Ϣ�Ի���
/*
static void quit_mbox_create(lv_obj_t * parent)
{
	if(quit_mbox)//��Ҫ�ظ�����
		return;
	quit_mbox = lv_mbox_create(parent,NULL);
	lv_obj_set_size(quit_mbox,lv_obj_get_width(parent)*0.7f,lv_obj_get_height(parent)/2);
	lv_mbox_set_text(quit_mbox,"Tip\nAre you to quit?");
	lv_mbox_add_btns(quit_mbox,(const char**)MBOX_MAP);
	lv_obj_set_drag(quit_mbox,true);//���öԻ�����Ա���ק
	lv_obj_align(quit_mbox,NULL,LV_ALIGN_CENTER,0,0);
	lv_obj_set_event_cb(quit_mbox,event_handler);
}*/

//����ѡ���ʼ��
void setting_tab_init(lv_obj_t * parent)
{
	//������ǩ��ʽ
	lv_style_copy(&label_style, &lv_style_plain_color);
	label_style.text.color = LV_COLOR_BLACK;
	label_style.text.font = &lv_font_roboto_22;
	label_style.body.main_color = LV_COLOR_CYAN;
	label_style.body.grad_color = LV_COLOR_CYAN;
	
	//�����ı�����ʽ
	lv_style_copy(&ta_style, &lv_style_plain_color);
	ta_style.body.main_color = LV_COLOR_ORANGE;
	ta_style.body.grad_color = LV_COLOR_ORANGE;
	ta_style.text.font = &lv_font_roboto_22;
	ta_style.text.color = LV_COLOR_BLACK;
	
	label_readyTime = lv_label_create(parent,NULL);//������ǩ
	lv_label_set_long_mode(label_readyTime,LV_LABEL_LONG_CROP);//���ó��ı�ģʽ
	lv_obj_set_width(label_readyTime, 120);
	lv_label_set_text(label_readyTime,"REA_TIME:  ");//�����ı�
	lv_label_set_align(label_readyTime,LV_LABEL_ALIGN_LEFT);//�ı��������

	lv_label_set_style(label_readyTime,LV_LABEL_STYLE_MAIN,&label_style);//������������ʽ
	lv_label_set_body_draw(label_readyTime, true);//ʹ�ܱ�������
	lv_obj_align(label_readyTime,NULL,LV_ALIGN_IN_TOP_LEFT,120,70);
	
	label_delayTime = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_text(label_delayTime,"DEL_TIME:");//�����ı�
	lv_obj_align(label_delayTime,label_readyTime,LV_ALIGN_OUT_BOTTOM_LEFT,0,40);
	
	label_chargeTime = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_text(label_chargeTime,"CHA_TIME:");//�����ı�
	lv_obj_align(label_chargeTime,label_delayTime,LV_ALIGN_OUT_BOTTOM_LEFT,0,40);
	
	label_balanceTime = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_text(label_balanceTime,"BAL_TIME:");//�����ı�
	lv_obj_align(label_balanceTime,label_chargeTime,LV_ALIGN_OUT_BOTTOM_LEFT,0,40);
	
	label_detectTime = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_text(label_detectTime,"DET_TIME:");//�����ı�
	lv_obj_align(label_detectTime,label_balanceTime,LV_ALIGN_OUT_BOTTOM_LEFT,0,40);
	
	label_exhaustTime = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_text(label_exhaustTime,"EXH_TIME:");//�����ı�
	lv_obj_align(label_exhaustTime,label_detectTime,LV_ALIGN_OUT_BOTTOM_LEFT,0,40);
	
	
	ta1_readyTime = lv_ta_create(parent, NULL);
	lv_ta_set_style(ta1_readyTime, LV_TA_STYLE_BG, &ta_style);
	lv_obj_set_width(ta1_readyTime, 150);//�����ı���Ĵ�С
	lv_ta_set_cursor_type(ta1_readyTime,LV_CURSOR_LINE|LV_CURSOR_HIDDEN);//���ù�������Ϊ��ֱ��
	lv_ta_set_text(ta1_readyTime,"");//�����ı�����
	lv_ta_set_one_line(ta1_readyTime,true);//ʹ�ܵ���ģʽ,Ĭ�Ͼ��ǲ�ʹ�ܵ�
	lv_ta_set_text_align(ta1_readyTime,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_readyTime,label_readyTime,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_readyTime,event_handler);//�����¼��ص�����
	
	ta1_delayTime = lv_ta_create(parent, ta1_readyTime);
	lv_ta_set_text_align(ta1_delayTime,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_delayTime,label_delayTime,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_delayTime,event_handler);//�����¼��ص�����
	
	ta1_chargeTime = lv_ta_create(parent, ta1_readyTime);
	lv_ta_set_text_align(ta1_chargeTime,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_chargeTime,label_chargeTime,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_chargeTime,event_handler);//�����¼��ص�����
	
	ta1_balanceTime = lv_ta_create(parent, ta1_readyTime);
	lv_ta_set_text_align(ta1_balanceTime,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_balanceTime,label_balanceTime,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_balanceTime,event_handler);//�����¼��ص�����
	
	ta1_detectTime = lv_ta_create(parent, ta1_readyTime);
	lv_ta_set_text_align(ta1_detectTime,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_detectTime,label_detectTime,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_detectTime,event_handler);//�����¼��ص�����
	
	ta1_exhaustTime = lv_ta_create(parent, ta1_readyTime);
	lv_ta_set_text_align(ta1_exhaustTime,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_exhaustTime,label_exhaustTime,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_exhaustTime,event_handler);//�����¼��ص�����
	
	
    label_time_unit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_long_mode(label_time_unit,LV_LABEL_LONG_EXPAND);//���ó��ı�ģʽ
	lv_label_set_text(label_time_unit,"S");//�����ı�
	lv_obj_align(label_time_unit,ta1_readyTime,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	label_time_unit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_long_mode(label_time_unit,LV_LABEL_LONG_EXPAND);//���ó��ı�ģʽ
	lv_label_set_text(label_time_unit,"S");//�����ı�
	lv_obj_align(label_time_unit,ta1_delayTime,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	
	label_time_unit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_long_mode(label_time_unit,LV_LABEL_LONG_EXPAND);//���ó��ı�ģʽ
	lv_label_set_text(label_time_unit,"S");//�����ı�
	lv_obj_align(label_time_unit,ta1_chargeTime,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	label_time_unit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_long_mode(label_time_unit,LV_LABEL_LONG_EXPAND);//���ó��ı�ģʽ
	lv_label_set_text(label_time_unit,"S");//�����ı�
	lv_obj_align(label_time_unit,ta1_balanceTime,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	label_time_unit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_long_mode(label_time_unit,LV_LABEL_LONG_EXPAND);//���ó��ı�ģʽ
	lv_label_set_text(label_time_unit,"S");//�����ı�
	lv_obj_align(label_time_unit,ta1_detectTime,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	label_time_unit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_long_mode(label_time_unit,LV_LABEL_LONG_EXPAND);//���ó��ı�ģʽ
	lv_label_set_text(label_time_unit,"S");//�����ı�
	lv_obj_align(label_time_unit,ta1_exhaustTime,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	
	label_bal_work_limit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_text(label_bal_work_limit,"BAL_WORK:");//�����ı�
	lv_obj_align(label_bal_work_limit,label_readyTime,LV_ALIGN_OUT_RIGHT_MID,340 ,0);
	
	label_bal_crit_limit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_text(label_bal_crit_limit,"BAL_CRIT:");//�����ı�
	lv_obj_align(label_bal_crit_limit,label_bal_work_limit,LV_ALIGN_OUT_BOTTOM_LEFT,0 ,40);
	
	label_leak_work_limit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_text(label_leak_work_limit,"LEA_WORK:");//�����ı�
	lv_obj_align(label_leak_work_limit,label_bal_crit_limit,LV_ALIGN_OUT_BOTTOM_LEFT,0 ,40);
	
	label_leak_crit_limit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_text(label_leak_crit_limit,"LEA_CRIT:");//�����ı�
	lv_obj_align(label_leak_crit_limit,label_leak_work_limit,LV_ALIGN_OUT_BOTTOM_LEFT,0 ,40);
	
	label_press_max_limit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_text(label_press_max_limit,"PRES_MAX:");//�����ı�
	lv_obj_align(label_press_max_limit,label_leak_crit_limit,LV_ALIGN_OUT_BOTTOM_LEFT,0 ,40);
	
	label_press_min_limit = lv_label_create(parent,label_readyTime);//������ǩ
	lv_label_set_text(label_press_min_limit,"PRES_MIN:");//�����ı�
	lv_obj_align(label_press_min_limit,label_press_max_limit,LV_ALIGN_OUT_BOTTOM_LEFT,0 ,40);
	
	
	ta1_bal_work = lv_ta_create(parent, ta1_readyTime);
	lv_ta_set_text_align(ta1_bal_work,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_bal_work,label_bal_work_limit,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_bal_work,event_handler);//�����¼��ص�����
	
	ta1_bal_crit = lv_ta_create(parent, ta1_readyTime);
	lv_ta_set_text_align(ta1_bal_crit,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_bal_crit,label_bal_crit_limit,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_bal_crit,event_handler);//�����¼��ص�����
	
	ta1_lea_work = lv_ta_create(parent, ta1_readyTime);
	lv_ta_set_text_align(ta1_lea_work,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_lea_work,label_leak_work_limit,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_lea_work,event_handler);//�����¼��ص�����
	
	ta1_lea_crit = lv_ta_create(parent, ta1_readyTime);
	lv_ta_set_text_align(ta1_lea_crit,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_lea_crit,label_leak_crit_limit,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_lea_crit,event_handler);//�����¼��ص�����
	
	ta1_pres_max = lv_ta_create(parent, ta1_readyTime);
	lv_ta_set_text_align(ta1_pres_max,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_pres_max,label_press_max_limit,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_pres_max,event_handler);//�����¼��ص�����
	
	ta1_pres_min = lv_ta_create(parent, ta1_readyTime);
	lv_ta_set_text_align(ta1_pres_min,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
	lv_obj_align(ta1_pres_min,label_press_min_limit,LV_ALIGN_OUT_RIGHT_MID,10,0);//����Ļ���ж���
	lv_obj_set_event_cb(ta1_pres_min,event_handler);//�����¼��ص�����
	
	
	label_press_unit =  lv_label_create(parent,label_time_unit);//������ǩ
	lv_label_set_text(label_press_unit,"Pa");//�����ı�
	lv_obj_align(label_press_unit,ta1_bal_work,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	label_press_unit =  lv_label_create(parent,label_time_unit);//������ǩ
	lv_label_set_text(label_press_unit,"Pa");//�����ı�
	lv_obj_align(label_press_unit,ta1_bal_crit,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	label_press_unit =  lv_label_create(parent,label_time_unit);//������ǩ
	lv_label_set_text(label_press_unit,"Pa");//�����ı�
	lv_obj_align(label_press_unit,ta1_lea_work,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	label_press_unit =  lv_label_create(parent,label_time_unit);//������ǩ
	lv_label_set_text(label_press_unit,"Pa");//�����ı�
	lv_obj_align(label_press_unit,ta1_lea_crit,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	label_press_unit =  lv_label_create(parent,label_time_unit);//������ǩ
	lv_label_set_text(label_press_unit,"kPa");//�����ı�
	lv_obj_align(label_press_unit,ta1_pres_max,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	label_press_unit =  lv_label_create(parent,label_time_unit);//������ǩ
	lv_label_set_text(label_press_unit,"kPa");//�����ı�
	lv_obj_align(label_press_unit,ta1_pres_min,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
	//������ť
	apply_btn = lv_btn_create(parent, NULL);
	cancel_btn = lv_btn_create(parent, NULL);
	lv_obj_set_size(apply_btn,130,60);//���ô�С
	lv_obj_set_size(cancel_btn,130,60);//���ô�С
	lv_obj_align(apply_btn, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 276, -50);//���ö��뷽ʽ
	lv_obj_align(cancel_btn, apply_btn, LV_ALIGN_OUT_RIGHT_MID, 200, 0);//���ö��뷽ʽ
	lv_obj_t * apply_btn_lable = lv_label_create(apply_btn,NULL);//start_btn��label�Ӷ���
	lv_obj_t * cancel_btn_lable = lv_label_create(cancel_btn,NULL);//stop_btn��label�Ӷ���
	lv_label_set_text(apply_btn_lable,"APPLY");
	lv_label_set_text(cancel_btn_lable,"CANCEL");
	lv_btn_set_layout(apply_btn,LV_LAYOUT_CENTER);//���ð�ť�Ĳ��ַ�ʽ,ʹlabel�������м�,��Ȼ����������õĻ�,Ĭ��Ҳ�����м��
	lv_btn_set_layout(cancel_btn,LV_LAYOUT_CENTER);
	lv_obj_set_event_cb(apply_btn,event_handler);//�����¼��ص�����
	lv_obj_set_event_cb(cancel_btn,event_handler);
	
	

//	lv_obj_t * label_remoteIP = lv_label_create(parent,label_readyTime);//������ǩ
//	lv_label_set_text(label_remoteIP,"REM_IP:  192. 168. ");//�����ı�
//	lv_obj_align(label_remoteIP,NULL,LV_ALIGN_IN_TOP_RIGHT,-200,50);

//	lv_obj_t * label_localIP = lv_label_create(parent,label_readyTime);//������ǩ
//	lv_label_set_text(label_localIP,"LOC_IP:  192. 168. ");//�����ı�
//	lv_obj_align(label_localIP,label_remoteIP,LV_ALIGN_OUT_BOTTOM_RIGHT,0,40);

	//�������ڱ�ǩ
//	lv_obj_t * label_date = lv_label_create(parent,label_readyTime);//������ǩ
//	lv_label_set_text(label_date,"DATE:");//�����ı�
//	lv_obj_align(label_date,label_localIP,LV_ALIGN_OUT_BOTTOM_LEFT,0,40);
	
//	lv_obj_t * label_time = lv_label_create(parent,label_readyTime);//������ǩ
//	lv_label_set_text(label_time,"TIME:");//�����ı�
//	lv_obj_align(label_time,label_date,LV_ALIGN_OUT_BOTTOM_LEFT,0,40);

//	ta1_IP1 = lv_ta_create(parent, ta1_readyTime);
//	lv_ta_set_text_align(ta1_IP1,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
//	lv_obj_set_width(ta1_IP1, parent_width*0.06f);//�����ı���Ĵ�С
//	lv_obj_align(ta1_IP1,label_remoteIP,LV_ALIGN_OUT_RIGHT_MID,0,0);//����Ļ���ж���
//	lv_obj_set_event_cb(ta1_IP1,event_handler);//�����¼��ص�����

	//������ǩ��
//	lv_obj_t * label_dot1 = lv_label_create(parent,label_readyTime);//������ǩ
//	lv_label_set_text(label_dot1,".");//�����ı�
//	lv_obj_align(label_dot1,ta1_IP1,LV_ALIGN_OUT_RIGHT_MID,0,0);

	//�����ı���
//	ta1_IP2 = lv_ta_create(parent, ta1_readyTime);
//	lv_ta_set_text_align(ta1_IP2,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
//	lv_obj_set_width(ta1_IP2, parent_width*0.06f);//�����ı���Ĵ�С
//	lv_obj_align(ta1_IP2,label_dot1,LV_ALIGN_OUT_RIGHT_MID,0,0);//����Ļ���ж���
//	lv_obj_set_event_cb(ta1_IP2,event_handler);//�����¼��ص�����
	
//	ta1_IP3 = lv_ta_create(parent, ta1_readyTime);
//	lv_ta_set_text_align(ta1_IP3,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
//	lv_obj_set_width(ta1_IP3, parent_width*0.06f);//�����ı���Ĵ�С
//	lv_obj_align(ta1_IP3,label_localIP,LV_ALIGN_OUT_RIGHT_MID,0,0);//����Ļ���ж���
//	lv_obj_set_event_cb(ta1_IP3,event_handler);//�����¼��ص�����

	//������ǩ��
//	lv_obj_t * label_dot2 = lv_label_create(parent,label_readyTime);//������ǩ
//	lv_label_set_text(label_dot2,".");//�����ı�
//	lv_obj_align(label_dot2,ta1_IP3,LV_ALIGN_OUT_RIGHT_MID,0,0);

	//�����ı���
//	ta1_IP4 = lv_ta_create(parent, ta1_readyTime);
//	lv_ta_set_text_align(ta1_IP4,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
//	lv_obj_set_width(ta1_IP4, parent_width*0.06f);//�����ı���Ĵ�С
//	lv_obj_align(ta1_IP4,label_dot2,LV_ALIGN_OUT_RIGHT_MID,0,0);//����Ļ���ж���
//	lv_obj_set_event_cb(ta1_IP4,event_handler);//�����¼��ص�����
	
//	ta1_year = lv_ta_create(parent, ta1_readyTime);
//	lv_ta_set_text_align(ta1_year,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
//	lv_obj_set_width(ta1_year, parent_width*0.08f);//�����ı���Ĵ�С
//	lv_obj_align(ta1_year,label_date,LV_ALIGN_OUT_RIGHT_MID,15,0);//����Ļ���ж���
//	lv_obj_set_event_cb(ta1_year,event_handler);//�����¼��ص�����
	
	//����"/"
//	lv_obj_t * label_date_sep1 = lv_label_create(parent,label_readyTime);//������ǩ
//	lv_label_set_text(label_date_sep1,"/");//�����ı�
//	lv_obj_align(label_date_sep1,ta1_year,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
//	ta1_month = lv_ta_create(parent, ta1_readyTime);
//	lv_ta_set_text_align(ta1_month,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
//	lv_obj_set_width(ta1_month, parent_width*0.06f);//�����ı���Ĵ�С
//	lv_obj_align(ta1_month,label_date_sep1,LV_ALIGN_OUT_RIGHT_MID,5,0);//����Ļ���ж���
//	lv_obj_set_event_cb(ta1_month,event_handler);//�����¼��ص�����
	
//	lv_obj_t * label_date_sep2 = lv_label_create(parent,label_readyTime);//������ǩ
//	lv_label_set_text(label_date_sep2,"/");//�����ı�
//	lv_obj_align(label_date_sep2,ta1_month,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
//	ta1_day = lv_ta_create(parent, ta1_readyTime);
//	lv_ta_set_text_align(ta1_day,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
//	lv_obj_set_width(ta1_day, parent_width*0.06f);//�����ı���Ĵ�С
//	lv_obj_align(ta1_day,label_date_sep2,LV_ALIGN_OUT_RIGHT_MID,5,0);//����Ļ���ж���
//	lv_obj_set_event_cb(ta1_day,event_handler);//�����¼��ص�����
	
//	lv_obj_t * ta1_hour = lv_ta_create(parent, ta1_readyTime);
//	lv_ta_set_text_align(ta1_hour,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
//	lv_obj_set_width(ta1_hour, parent_width*0.08f);//�����ı���Ĵ�С
//	lv_obj_align(ta1_hour,label_time,LV_ALIGN_OUT_RIGHT_MID,15,0);//����Ļ���ж���
//	lv_obj_set_event_cb(ta1_hour,event_handler);//�����¼��ص�����
	
	//����":"
//	lv_obj_t * label_time_sep1 = lv_label_create(parent,label_readyTime);//������ǩ
//	lv_label_set_text(label_time_sep1,":");//�����ı�
//	lv_obj_align(label_time_sep1,ta1_hour,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
//	ta1_minute = lv_ta_create(parent, ta1_month);
//	lv_ta_set_text_align(ta1_minute,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
//	lv_obj_set_width(ta1_minute, parent_width*0.06f);//�����ı���Ĵ�С
//	lv_obj_align(ta1_minute,label_time_sep1,LV_ALIGN_OUT_RIGHT_MID,5,0);//����Ļ���ж���
//	lv_obj_set_event_cb(ta1_minute,event_handler);//�����¼��ص�����
	
//	lv_obj_t * label_time_sep2 = lv_label_create(parent,label_readyTime);//������ǩ
//	lv_label_set_text(label_time_sep2,":");//�����ı�
//	lv_obj_align(label_time_sep2,ta1_minute,LV_ALIGN_OUT_RIGHT_MID,5,0);
	
//	ta1_second = lv_ta_create(parent, ta1_readyTime);
//	lv_ta_set_text_align(ta1_second,LV_LABEL_ALIGN_RIGHT);//�����ı��������,Ĭ�Ͼ��Ǿ������
//	lv_obj_set_width(ta1_second, parent_width*0.06f);//�����ı���Ĵ�С
//	lv_obj_align(ta1_second,label_time_sep2,LV_ALIGN_OUT_RIGHT_MID,5,0);//����Ļ���ж���
//	lv_obj_set_event_cb(ta1_second,event_handler);//�����¼��ص�����

	
	

//	//�����û�����id
//	lv_obj_t * name_id_label = lv_label_create(parent,NULL);
//	lv_label_set_recolor(name_id_label,true);
//	lv_label_set_text(name_id_label,"#FFFFFF Xiong jia yu#\n#BBBBBB ID:15727652280#");
//	lv_obj_align(name_id_label,head_img,LV_ALIGN_OUT_RIGHT_MID,20,0);
	//����һ��ˮƽ�ָ����
//	lv_obj_t * split_line = lv_line_create(parent,NULL);
//	lv_obj_set_size(split_line,SPLIT_LINE_WIDTH,2);
//	lv_line_set_points(split_line,SPLIT_LINE_POINTS,2);
//	lv_obj_align(split_line,NULL,LV_ALIGN_IN_TOP_MID,0,100);
//	//����һ���˳���ť
//	quit_btn = lv_btn_create(parent,NULL);
//	lv_obj_set_width(quit_btn,parent_width/2);
//	lv_obj_align(quit_btn,table,LV_ALIGN_OUT_BOTTOM_MID,0,30);
//	lv_obj_t * quit_label = lv_label_create(quit_btn,NULL);
//	lv_label_set_recolor(quit_label,true);
//	lv_label_set_text(quit_label,"#FF0000 Quit#");
//	lv_obj_set_event_cb(quit_btn,event_handler);
	
}

