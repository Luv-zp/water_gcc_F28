#ifndef _F28_FUNC_H
#define _F28_FUNC_H
#include "typedef.h"
#include "F28_MbTcpProtocol.h"


#define F28_FAIL 						-1
#define F28_OK 							0

#define F28_OFFLINE						0
#define F28_CONNECTED					1

typedef enum 
{
	LEAK_PA,
	LEAK_PASEC,
	LEAK_PA_HR,
	LEAK_PASEC_HR,
	
	LEAK_CAL_PA,
	LEAK_CAL_PASEC,
	
	LEAK_CCMIN,
	LEAK_CCSEC,
	LEAK_CCH,
	LEAK_MM3SEC,
	LEAK_CM3_MIN,
	LEAK_CM3_SEC,
	LEAK_CM3_H,
	
	LEAK_ML_SEC,
	LEAK_ML_MIN,
	LEAK_ML_H,
	//USA---------------
	LEAK_INCH3_SEC,
	LEAK_INCH3_MIN,
	LEAK_INCH3_H,
	LEAK_FT3_SEC,
	LEAK_FT3_MIN,
	LEAK_FT3_H,
	LEAK_MMCE,
	LEAK_MMCE_SEC,
	LEAK_SCCM,
	LEAK_POINTS,
	//V1.500
	LEAK_KPA,
	LEAK_MPA,
	LEAK_BAR,
	LEAK_mBAR,
	LEAK_PSI,
	LEAK_L_MIN,
	LEAK_CM_H2O,
	LEAK_UG_H2O,

	NMAX_LEAK_UNITS,

	LEAK_JET_CHECK	= 0xff		// F28 check jet unit
}F28_LEAK_UNITS;	

typedef enum
{
	PRESS_PA,
	PRESS_KPA,
	PRESS_MPA,
	PRESS_BAR,
	PRESS_mBAR,
	PRESS_PSI,
	PRESS_POINTS,
	NMAX_PRESS_UNITS
}F28_PRESS_UNITS;

typedef enum 
{
	VOLUME_CM3,			
	VOLUME_MM3,
	VOLUME_ML,
	VOLUME_LITRE,
	VOLUME_INCH3,
	VOLUME_FT3,
	NMAX_VOLUME_UNITS
}F28_VOLUME_UNIT;

typedef	enum 
{
	READY,
	FILL_STEP,
	STAB_STEP,
	TEST_STEP,
	DUMP_STEP,
	FILL_VOLUME_STEP,
	TRANSFERT_STEP
}F28_STEP_CODE;

typedef enum 
{
	LEAK_TEST,
	SEALED_COMPONENT,
	DESENSITIZED_MODE_TEST
}F28_TEST_TYPE;

typedef enum 
{
	STD_FILL_MODE,
	AUTOFILL_MODE,
	//V1.400
	INSTRUCTION_MODE, 	// Electronic regulator option 
	RAMP_MODE, 			// Electronic regulator option 
	RAMP_CONTROL,
	EASY,
	EASY_AUTO,
	NMAX_FILL_MODE
}F28_FILL_MODE;
	
typedef struct _F28_PARAMETERS
{
	WORD	wTypeTest;		// STANDARD or LARGE LEAK (Sealed) or LARGE LEAK TEST (sealed + test)
	WORD	wTpsFillVol;	// Fill time for volume transfer in 0.01 sec
	WORD	wTpsTransfert;	// Transfer time in 0.01 sec
	WORD	wTpsFill;		// Fill time in 0.01 sec 
	WORD	wTpsStab;		// Stabilization time in 0.01 sec
	WORD	wTpsTest;		// Test time in 0.01 sec
	WORD	wTpsDump;		// Dump time in 0.01 sec 
	WORD	wPress1Unit;	// See F28_PRESS_UNITS
	float	fPress1Min;		// Minimum pressure value
	float	fPress1Max;		// Maximum pressure value
	float	fSetFillP1;			//consigne mode auto-fill
	float	fRatioMax;			//LARGE LEAK mode only
	float	fRatioMin;			//LARGE LEAK mode only
	WORD	wFillMode;			//STD_FILL_MODE / AUTOFILL_MODE
	WORD	wLeakUnit;			// See F28_LEAK_UNITS
	
	WORD	wRejectCalc;		// Pa or Pa/s
	
	WORD	wVolumeUnit;		// See F28_VOLUME_UNIT 
	float	fVolume;			// Volume value
	float	fRejectMin;
	float	fRejectMax;
	
	float   fCoeffAutoFill;		//
	
	WORD	wOptions;			// SIGNE(NO/YES)
	//V1.200
	float	fPatmSTD;			//Patm  standard condition (hPa)
	float	fTempSTD;			//Temperature standard condition (en ∞C)
	WORD	fFilterTime;		//  en (s)
	//V1.300
	float	fOffsetLeak;		//Offset sur la fuite
	
	
	float	fVolumeRef;			//Nouveau parametre pour le C28
	WORD	wTpsTestCompTemp;
	WORD	wPourcCompTemp;
	WORD	wTpsWaitingTime;
	//F28_LIGHT_V104
	WORD	wLastConsigneDacEasy;
	float	fNominalValue;
	float	fCoeffMin;
	float	fCoeffMax;
	/*		//V1.500
	WORD	wIndirectFillMode;	//STD_FILL_MODE / AUTOFILL_MODE
	WORD	wTpsPartFill;
	WORD	wIndirectUnitePression;
	float	fIndirectPressMin;
	float	fIndirectPressMax;
	float	fIndirectSetFill;	//consigne mode indirect
*/
}F28_PARAMETERS;	
	
typedef struct _F28_REALTIME_CYCLE
{
	WORD  wEndCycle;
	WORD  wStatus;
	float fPressureValue;
	WORD  wUnitPressure;
	float fLeakValue;
	WORD  wUnitLeak;
	float fPatm;
	float fInternalTemperature;
}F28_REALTIME_CYCLE;

typedef struct _F28_LAST_RESULT
{
	WORD  wStatus;
	float fPressureValue;
	WORD  wUnitPressure;
	float fLeakValue;
	WORD  wUnitLeak;
	float fPatm;
	float fInternalTemperature;
}F28_LAST_RESULT;

typedef struct _CF28Light CF28Light;		//F28Lignt C∞Ê £®F28 LPV£©
struct _CF28Light
{
	SHORT (* Init)(CF28Light * me);
	SHORT (* Open)(CF28Light * me);	
	void (* Close)(CF28Light * me);
	SHORT (* IsConnected)(CF28Light * me);
	
	SHORT (* StartCycle)(CF28Light * me, MBTCP_PROTOCOL_FRAME * pFrame);
	SHORT (* StopCycle)(CF28Light * me, MBTCP_PROTOCOL_FRAME * pFrame);	
	
	SHORT (* SetModuleParameters)(CF28Light * me, MBTCP_PROTOCOL_FRAME * pFrame);
//	SHORT (* Send_ReadModuleParametersFrame)(CF28Light * me, MBTCP_PROTOCOL_FRAME * pFrame);
//	SHORT (* GetModuleParameters)(CF28Light * me, MBTCP_PROTOCOL_FRAME * pFrame, F28_PARAMETERS* pPara);  //‘› ±≤ª”√

	SHORT (* Send_ReadRealTimeDataFrame)(CF28Light * me, MBTCP_PROTOCOL_FRAME * pFrame);
	SHORT (* GetRealTimeData)(CF28Light * me, MBTCP_PROTOCOL_FRAME * pFrame);
	
	SHORT (* Send_ReadLastResultFrame)(CF28Light * me, MBTCP_PROTOCOL_FRAME * pFrame);
	SHORT (* GetLastResult)(CF28Light * me, MBTCP_PROTOCOL_FRAME * pFrame);
	
	F28_PARAMETERS * parametes;
	F28_REALTIME_CYCLE * realTimeData;
	F28_LAST_RESULT * lastResult;
};
CF28Light * CF28Light_create(void);


#endif

