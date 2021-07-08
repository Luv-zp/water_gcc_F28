#include "BLL_Alarm.h"
#include "universalType.h"
#include "BLL_Modbus_server.h"
#include "userApp.h"

FlagHand ErrHand;	//错误处理对象
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

//错误处理对象初始化，创建时自动调用
static void errHand_init(void)
{
	err_code = 0;
}

//对错误状态进行置位
static void errHand_setErrBit(ERROR_t type)
{
	AppMessage msg;
	//只有在发现需要置位的标志位没有置1时才给MODBUS发送消息，减少线程负担
	err_code |= type;
	msg.pVoid = &err_code;
	msg.dataType = MB_UPDATE_ERR;
	xQueueSend(modbusQ, &msg, 10);
	//有错误发生
	errHand_setVbStatus(EX_ERR_S);
}

//清除错误状态位
static void errHand_clearErrBit(ERROR_t type)
{
	AppMessage msg;
	//只有在发现指定的标志位置1时才给MODBUS发送消息，减少线程负担
	if(err_code & type)
	{
		err_code &= ~type;
		msg.pVoid = &err_code;
		msg.dataType = MB_UPDATE_ERR;
		xQueueSend(modbusQ, &msg, 10);
		//当没有错误时，清除VB_STATUS中的错误标识，并给MODBUS发送消息，更新VB_STATUS
		if((err_code == 0) && (vb_status == EX_ERR_S))
			errHand_clearVbStatus(EX_ERR_S);
	}
}

//获取所有错误状态位
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
	//当没有错误时，清除掉VB_STATUS的错误标识，并给MODBUS发送消息，更新VB_STATUS
	if(vb_status & EX_ERR_S)
		errHand_clearVbStatus(EX_ERR_S);
}

//设置设备状态
static void errHand_setVbStatus(VB_STATUS_t type)
{
	AppMessage msg;
	switch(type)
	{
		case EX_READY_S:
			//处于需要干燥的状态时，不响应READY信号
			if(!(vb_status & EX_DRY_S))
			{
				//收到准备完毕信号， 应该清除RUN,DEBUG,OUTDRY等信号
				vb_status &= ~(EX_RUN_S | EX_DEBUG_S | EX_DRY_S | EX_END_S);
				vb_status |= type;
			}
			break;
		case EX_RUN_S:
			//收到RUN信号，表示设备开始动作，因此需要清除RUN, DEBUG, OUTDRY信号
			vb_status &= ~(EX_READY_S | EX_END_S);
			vb_status |= type;
			break;
		case EX_DEBUG_S:
			//收到DEBUG信号，表示设备进入调试状态，因此需要清除READY, END信号
			vb_status &= ~(EX_READY_S | EX_END_S);
			vb_status |= type;
			break;
		case EX_DRY_S:
			//收到干燥信号
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
	//给MODBUS发送消息更新状态标志位
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
			//清除错误状态时，如果系统当前状态只有错误时，则可以置位READY状态
			if(vb_status == EX_ERR_S)
				vb_status = EX_READY_S;
			else 
				vb_status &= ~type;
			break;
		default:
			vb_status &= ~type;
			break;
			
	}
	//给MODBUS发送消息更新状态标志位
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

//设置测试结果
static void errHand_setVbRes(VB_RES_t type)
{
	AppMessage msg;
	vb_res |= type;
	msg.dataType = MB_UPDATE_RES;
	msg.pVoid = &vb_res;
	xQueueSend(modbusQ, &msg, 10);
}

//清除测试结果
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
