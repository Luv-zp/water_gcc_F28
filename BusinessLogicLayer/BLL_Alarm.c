#include "BLL_Alarm.h"
#include "universalType.h"
#include "BLL_Modbus_server.h"
#include "userApp.h"

FlagHand ErrHand;	//���������
static u32 err_code;
static u16 vb_status;
static u16 vb_res;
u8 testCondition;

static void errHand_init(void);	

static void errHand_setErrBit(ERROR_t type);
static void errHand_clearErrBit(ERROR_t type);
static u32 errHand_getAllErr(void);
static void errHand_clearAllErr(void);

static void errHand_setVbStatus(VB_STATUS_t type);
static void errHand_clearVbStatus(VB_STATUS_t type);
static u16 errHand_getAllVbStatus(void);
static void errHand_clearAllVbStatus(void);

static void errHand_setVbRes(VB_RES_t type);
static void errHand_clearVbRes(VB_RES_t type);
static u16 errHand_getAllVbRes(void);
static void errHand_clearAllVbRes(void);

static void errHand_threadRun(void);


FlagHand * errHand_create(void)
{
	static FlagHand * me = 0;
	if(me == 0)
	{
		me = &ErrHand;
		me->init = errHand_init;
		me->setErrBit = errHand_setErrBit;
		me->clearErrBit = errHand_clearErrBit;
		me->getAllErr = errHand_getAllErr;
		me->clearAllErr = errHand_clearAllErr;
		
		me->setVbStatus = errHand_setVbStatus;
		me->clearVbStatus = errHand_clearVbStatus;
		me->getAllVbStatus = errHand_getAllVbStatus;
		me->clearAllVbStatus = errHand_clearAllVbStatus;
		
		me->setVbRes = errHand_setVbRes;
		me->clearVbRes = errHand_clearVbRes;
		me->getAllVbRes = errHand_getAllVbRes;
		me->clearAllVbRes = errHand_clearAllVbRes;
		
		me->threadRun = errHand_threadRun;
		
		me->init();
	}
	return me;
}

//����������ʼ��������ʱ�Զ�����
static void errHand_init(void)
{
	err_code = 0;
}

//�Դ���״̬������λ
static void errHand_setErrBit(ERROR_t type)
{
	AppMessage msg;
	//ֻ���ڷ�����Ҫ��λ�ı�־λû����1ʱ�Ÿ�MODBUS������Ϣ�������̸߳���
	err_code |= type;
	msg.pVoid = &err_code;
	msg.dataType = MB_UPDATE_ERR;
	xQueueSend(modbusQ, &msg, 10);
	//�д�����
	errHand_setVbStatus(EX_ERR_S);
}

//�������״̬λ
static void errHand_clearErrBit(ERROR_t type)
{
	AppMessage msg;
	//ֻ���ڷ���ָ���ı�־λ��1ʱ�Ÿ�MODBUS������Ϣ�������̸߳���
	if(err_code & type)
	{
		err_code &= ~type;
		msg.pVoid = &err_code;
		msg.dataType = MB_UPDATE_ERR;
		xQueueSend(modbusQ, &msg, 10);
		//��û�д���ʱ�����VB_STATUS�еĴ����ʶ������MODBUS������Ϣ������VB_STATUS
		if((err_code == 0) && (vb_status == EX_ERR_S))
			errHand_clearVbStatus(EX_ERR_S);
	}
}

//��ȡ���д���״̬λ
static u32 errHand_getAllErr(void)
{
	return err_code;
}

static void errHand_clearAllErr(void)
{
	AppMessage msg;
	err_code = 0;
	msg.dataType = MB_UPDATE_ERR;
	msg.pVoid = &err_code;
	xQueueSend(modbusQ, &msg, 10);
	//��û�д���ʱ�������VB_STATUS�Ĵ����ʶ������MODBUS������Ϣ������VB_STATUS
	if(vb_status & EX_ERR_S)
		errHand_clearVbStatus(EX_ERR_S);
}

//�����豸״̬
static void errHand_setVbStatus(VB_STATUS_t type)
{
	AppMessage msg;
	switch(type)
	{
		case EX_READY_S:
			//������Ҫ�����״̬ʱ������ӦREADY�ź�
			if(!(vb_status & EX_DRY_S))
			{
				//�յ�׼������źţ� Ӧ�����RUN,DEBUG,OUTDRY���ź�
				vb_status &= ~(EX_RUN_S | EX_DEBUG_S | EX_DRY_S | EX_END_S);
				vb_status |= type;
			}
			break;
		case EX_RUN_S:
			//�յ�RUN�źţ���ʾ�豸��ʼ�����������Ҫ���RUN, DEBUG, OUTDRY�ź�
			vb_status &= ~(EX_READY_S | EX_END_S);
			vb_status |= type;
			break;
		case EX_DEBUG_S:
			//�յ�DEBUG�źţ���ʾ�豸�������״̬�������Ҫ���READY, END�ź�
			vb_status &= ~(EX_READY_S | EX_END_S);
			vb_status |= type;
			break;
		case EX_DRY_S:
			//�յ������ź�
			vb_status |= type;
			break;
		case EX_END_S:
			vb_status &= ~(EX_READY_S | EX_RUN_S);
			vb_status |= type;
			break;
		case EX_ERR_S:
			vb_status &= ~EX_READY_S;
			break;
		default:
			vb_status |= type;
			break;	
	}
	//��MODBUS������Ϣ����״̬��־λ
	msg.pVoid = &vb_status;
	msg.dataType = MB_UPDATE_VB_STATUS;
	xQueueSend(modbusQ, &msg, 10);
}

static void errHand_clearVbStatus(VB_STATUS_t type)
{
	AppMessage msg;
	switch(type)
	{
		case EX_ERR_S:
			//�������״̬ʱ�����ϵͳ��ǰ״ֻ̬�д���ʱ���������λREADY״̬
			if(vb_status == EX_ERR_S)
				vb_status = EX_READY_S;
			else 
				vb_status &= ~type;
			break;
		default:
			vb_status &= ~type;
			break;
			
	}
	//��MODBUS������Ϣ����״̬��־λ
	msg.pVoid = &vb_status;
	msg.dataType = MB_UPDATE_VB_STATUS;
	xQueueSend(modbusQ, &msg, 10);
}

static u16 errHand_getAllVbStatus(void)
{
	return vb_status;
}

static void errHand_clearAllVbStatus(void)
{
	AppMessage msg;
	vb_status = 0;
	msg.pVoid = &vb_status;
	msg.dataType = MB_UPDATE_VB_STATUS;
	xQueueSend(modbusQ, &msg, 10);
}

//���ò��Խ��
static void errHand_setVbRes(VB_RES_t type)
{
	AppMessage msg;
	vb_res |= type;
	msg.dataType = MB_UPDATE_RES;
	msg.pVoid = &vb_res;
	xQueueSend(modbusQ, &msg, 10);
}

//������Խ��
static void errHand_clearVbRes(VB_RES_t type)
{
	AppMessage msg;
	vb_res &= ~type;
	msg.dataType = MB_UPDATE_RES;
	msg.pVoid = &vb_res;
	xQueueSend(modbusQ, &msg, 10);
}

static u16 errHand_getAllVbRes(void)
{
	return vb_res;
}

static void errHand_clearAllVbRes(void)
{
	AppMessage msg;
	vb_res = 0;
	msg.dataType = MB_UPDATE_RES;
	msg.pVoid = &vb_res;
	xQueueSend(modbusQ, &msg, 10);
}

static void errHand_threadRun(void)
{
}
