#ifndef _FML_EXIT_GRATING_H
#define _FML_EXIT_GRATING_H
#include "FML_ExtValve.h"


typedef struct _GratDev GratDev;
struct _GratDev
{
	void (* init)(GratDev * me);
	void (* trigOn)(GratDev * me, ValveIODev * pValveIODev);	//��դ����
	void (* trigOff)(GratDev * me, ValveIODev * pValveIODev);	//��դ�������
	void (* updateCylPos)(GratDev * me, u8 uCyl, u8 dCyl, u8 fCyl, u8 bCyl);	//��������״̬
	u8 (* isTrig)(GratDev * me);	//��դ�Ƿ񴥷�
	
	u8 uCylPos;
	u8 dCylPos;
	u8 fCylPos;
	u8 bCylPos;
	u8 isTrigOr;
};

GratDev * GratDev_create(void);

#endif

