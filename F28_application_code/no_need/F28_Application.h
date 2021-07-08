#ifndef F28_APP_H
#define F28_APP_H
#include "typedef.h"
#include "mbTcp_protocol.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define F28_FAIL 						-1
#define F28_OK 							0

#define F28_OFFLINE						0
#define F28_CONNECTED					1

enum F28_LEAK_UNITS
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
};

enum F28_PRESS_UNITS
{
	PRESS_PA,
	PRESS_KPA,
	PRESS_MPA,
	PRESS_BAR,
	PRESS_mBAR,
	PRESS_PSI,
	PRESS_POINTS,
	NMAX_PRESS_UNITS
};

enum F28_VOLUME_UNIT
{
	VOLUME_CM3,			
	VOLUME_MM3,
	VOLUME_ML,
	VOLUME_LITRE,
	VOLUME_INCH3,
	VOLUME_FT3,
	NMAX_VOLUME_UNITS
};

enum F28_TEST_TYPE
{
	LEAK_TEST,
	SEALED_COMPONENT,
	DESENSITIZED_MODE_TEST
};

enum F28_FILL_MODE
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
};

enum F28_STEP_CODE
{
	READY,
	FILL_STEP,
	STAB_STEP,
	TEST_STEP,
	DUMP_STEP,
	FILL_VOLUME_STEP,
	TRANSFERT_STEP
};

enum F28_RESULT_STATUS_CODE				// Obsolete, use X28_RESULT_STATUS_CODE
{
	STATUS_GOOD_PART,
	STATUS_TEST_FAIL_PART,
	STATUS_REF_FAIL_PART,
	STATUS_ALARM_EEEE,
	STATUS_ALARM_MMMM,
	STATUS_ALARM_PPPP,
	STATUS_ALARM_MPPP,
	STATUS_ALARM_OFFD_LEAK,
	STATUS_ALARM_OFFD_PRESSURE,
	STATUS_ALARM_PST,
	STATUS_ALARM_MPST,
	STATUS_ALARM_CS_VOLUME_LOW,
	STATUS_ALARM_CS_VOLUME_HIGH,
	STATUS_ALARM_ERROR_PRESS_CALIBRATION,
	STATUS_ALARM_ERROR_LEAK_CALIBRATION,
	STATUS_ALARM_ERROR_LINE_PRESS_CALIBRATION,
	STATUS_ALARM_APPR_REG_ELEC_ERROR,
	STATUS_ALARM_TEST_PART_LARGE_LEAK,
	STATUS_ALARM_REF_SIDE_LARGE_LEAK,
	STATUS_ALARM_P_TOO_LARGE_FILL,
	STATUS_ALARM_P_TOO_LOW_FILL,
	STATUS_ALARM_JET_CHECK_FAIL,
	STATUS_ALARM_JET_CHECK_PASS,
	NMAX_STATUS_CODE
};


typedef struct F28_PARAMETERS
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
		float	fTempSTD;			//Temperature standard condition (en °C)
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
	
typedef struct F28_DATE
{
	WORD wYear;
	WORD wMonth;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
}F28_DATE;

typedef struct F28_LAST_RESULT
{
	UCHAR ucStatus;
	float fPressureValue;
	UCHAR ucUnitPressure;
	float fLeakValue;
	UCHAR ucUnitLeak;
	float fPatm;
	float fInternalTemperature;
}F28_LAST_RESULT;

typedef struct F28_REALTIME_CYCLE
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

typedef struct F28_CYCLE_STATISTICS
{
	DWORD dwTotalCycles;
	DWORD dwFailCycles;
	DWORD dwSuccessCycles;
}F28_CYCLE_STATISTICS;

typedef struct F28_COMMUNICATION_STATISTICS
{
	DWORD dwTransmited;
	DWORD dwReceived;
	DWORD dwErrors;
}F28_COMMUNICATION_STATISTICS;
	
//-------------------------------------------------
//Connection
//-------------------------------------------------
SHORT F28_Init(void);
SHORT F28_OpenChannel(void);								
void F28_Close(void);		

//-------------------------------------------------
// Create/Search F28 modules 
//-------------------------------------------------
SHORT F28_ReconnectModule(SHORT sModuleID);
SHORT F28_RemoveModule(SHORT sModuleID);
SHORT F28_RemoveAllModules(void);
SHORT F28_ResetEthernetModule(SHORT sModuleID);
SHORT F28_IsModuleConnected(SHORT sModuleID);
	
//-------------------------------------------------
// Edit programs/Parameters
//-------------------------------------------------
SHORT F28_GetModuleParameters(SHORT sModuleID, F28_PARAMETERS* pPara);
//SHORT F28_SetModuleParameters(SHORT sModuleID, F28_PARAMETERS* pPara);
SHORT F28_SetModuleParameters(MBTCP_PROTOCOL_FRAME* pFrame, F28_PARAMETERS* pPara);

//-------------------------------------------------
// Real Time cycle 
//-------------------------------------------------
//SHORT F28_StartCycle(SHORT sModuleID);	
SHORT F28_StartCycle(MBTCP_PROTOCOL_FRAME* pFrame);
SHORT F28_StopCycle(MBTCP_PROTOCOL_FRAME* pFrame);		
//SHORT F28_GetRealTimeData(SHORT sModuleID, F28_REALTIME_CYCLE* pCycle);	
SHORT F28_GetRealTimeData(MBTCP_PROTOCOL_FRAME* pFrame, F28_REALTIME_CYCLE* pCycle);
SHORT F28_Send_ReadRealTimeDataFrame(MBTCP_PROTOCOL_FRAME* pFrame);

//-------------------------------------------------
// Results - FIFO
//-------------------------------------------------
SHORT F28_ClearFIFOResults(SHORT sModuleID);							
WORD F28_GetResultsCount(SHORT sModuleID);								
//SHORT F28_GetNextResult(SHORT sModuleID, F28_RESULT* pResult);			
//SHORT F28_GetLastResult(SHORT sModuleID, F28_RESULT* pResult);
SHORT F28_GetLastResult(MBTCP_PROTOCOL_FRAME* pFrame, F28_LAST_RESULT* pResult);

//-------------------------------------------------
// Special cycle
//-------------------------------------------------
SHORT F28_StartAutoZeroPressure(SHORT sModuleID, float fDumpTime);				
SHORT F28_StartRegulatorAdjust(SHORT sModuleID);									
SHORT F28_StartLearningRegulator(SHORT sModuleID, float fDumpTime);				
SHORT F28_StartJetCheck(SHORT sModuleID);		


//-------------------------------------------------
// Statistics/Counters
//-------------------------------------------------
SHORT F28_GetCycleStatistics(SHORT sModuleID, F28_CYCLE_STATISTICS* pInfo);					
SHORT F28_GetCommunicationStatistics(SHORT sModuleID, F28_COMMUNICATION_STATISTICS* pInfo);

//-------------------------------------------------
// Auto-Cal V1.304
//-------------------------------------------------
SHORT F28_StartAutoCalOffsetOnly(SHORT sModuleID, WORD wNbCycles, WORD wInterCycleTime, float fOffsetMax);										
SHORT F28_StartAutoCalOffset(SHORT sModuleID, WORD wNbCycles, WORD wInterCycleTime, float fOffsetMax);											
SHORT F28_StartAutoCalVolume(SHORT sModuleID, WORD wNbCycles, WORD wInterCycleTime, float fLeak, float fPressure, float fVolMin, float fVolMax);
SHORT F28_StopAutoCal(SHORT sModuleID);

#ifdef __cplusplus
}
#endif

#endif

