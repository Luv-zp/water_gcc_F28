#ifndef _FML_EXIT_GRATING_H
#define _FML_EXIT_GRATING_H
#include "FML_ExtValve.h"


typedef struct _GratDev GratDev;
struct _GratDev
{
	void (* init)(GratDev * me);
	void (* trigOn)(GratDev * me, ValveIODev * pValveIODev);	//光栅触发
	void (* trigOff)(GratDev * me, ValveIODev * pValveIODev);	//光栅解除触发
	void (* updateCylPos)(GratDev * me, u8 uCyl, u8 dCyl, u8 fCyl, u8 bCyl);	//更新气缸状态
	u8 (* isTrig)(GratDev * me);	//光栅是否触发
	
	u8 uCylPos;
	u8 dCylPos;
	u8 fCylPos;
	u8 bCylPos;
	u8 isTrigOr;
};

GratDev * GratDev_create(void);

#endif

