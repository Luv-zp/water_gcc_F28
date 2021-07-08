#include "BLL_F28Comm.h"


static void init(CF28CommHandler * me);
static int32_t SendDataStructPack(CF28CommHandler * me, uint8_t *u8SendBuff, MBTCP_PROTOCOL_FRAME protFrame_send); 
static int32_t ReceDataStructUnpack(CF28CommHandler * me, uint8_t *u8ReceBuff, MBTCP_PROTOCOL_FRAME protFrame_rece);
