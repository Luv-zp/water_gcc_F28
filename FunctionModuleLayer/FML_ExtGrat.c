#include "FML_ExtGrat.h"

static void init(GratDev * me);
static void trigOn(GratDev * me, ValveIODev * pValveIODev);
static void trigOff(GratDev * me, ValveIODev * pValveIODev);
static void updateCylPos(GratDev * me, u8 uCyl, u8 dCyl, u8 fCyl, u8 bCyl);
static u8 isTrig(GratDev * me);

GratDev gratDev;

GratDev * GratDev_create(void)
{
	static GratDev * me = 0;
	if(me == 0)
	{
		me = &gratDev;
		me->init = init;
		me->trigOff = trigOff;
		me->trigOn = trigOn;
		me->updateCylPos = updateCylPos;
		me->isTrig = isTrig;
		
		me->uCylPos = 0;
		me->dCylPos = 0;
		me->fCylPos = 0;
		me->bCylPos = 0;
		me->isTrigOr = 0;
		
		me->init(me);
	}
	return me;
}

static void init(GratDev * me)
{
}

//��դ��������ر��������ײ��ұ��ֵ�ǰ����״̬
static void trigOn(GratDev * me, ValveIODev * pValveIODev)
{
	me->uCylPos = pValveIODev->getValState(pValveIODev, CYL_UP_V);
	me->dCylPos = pValveIODev->getValState(pValveIODev, CYL_DOWN_V);
	me->fCylPos = pValveIODev->getValState(pValveIODev, CYL_FRONT_V);
	me->bCylPos = pValveIODev->getValState(pValveIODev, CYL_BACK_V);
	
	pValveIODev->setValState(pValveIODev, CYL_UP_V, 0);
	pValveIODev->setValState(pValveIODev, CYL_DOWN_V, 0);
	pValveIODev->setValState(pValveIODev, CYL_FRONT_V, 0);
	pValveIODev->setValState(pValveIODev, CYL_BACK_V, 0);
	
	me->isTrigOr = 1;
}

//��դ����������Ὣ���׿�������Ϊ�����ֵ�״̬
static void trigOff(GratDev * me, ValveIODev * pValveIODev)
{
	me->isTrigOr = 0;
	pValveIODev->setValState(pValveIODev, CYL_UP_V, me->uCylPos);
	pValveIODev->setValState(pValveIODev, CYL_DOWN_V, me->dCylPos);
	pValveIODev->setValState(pValveIODev, CYL_FRONT_V, me->fCylPos);
	pValveIODev->setValState(pValveIODev, CYL_BACK_V, me->bCylPos);
	me->uCylPos = 0;
	me->dCylPos = 0;
	me->fCylPos = 0;
	me->bCylPos = 0;
}

//��������λ�ã�ֻ���ڹ�դ������ʱ��Ż����
static void updateCylPos(GratDev * me, u8 uCyl, u8 dCyl, u8 fCyl, u8 bCyl)
{
	me->uCylPos = uCyl;
	me->dCylPos = dCyl;
	me->fCylPos = fCyl;
	me->bCylPos = bCyl;
}

static u8 isTrig(GratDev * me)
{
	return me->isTrigOr;
}

