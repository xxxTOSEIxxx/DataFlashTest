/*
 * LogApi.h
 *
 *  Created on: 2020/10/30
 *      Author: 99990045
 */

#ifndef LOGAPI_H_
#define LOGAPI_H_

#include "r_rtc_rx_if.h"								// RTC I/F
#include "r_flash_rx_if.h"								// FLASH I/F
#include "semphr.h"

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define LOGAPI_DATA_SIZE_MAX				( 128 )
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define	LOGAPI_TM_SIZE						( sizeof(tm_t) )
#define LOGAPI_FUNC_NAME_SIZE				( 32 )


//------------------------------------------------
// ログヘッダー情報構造体
//------------------------------------------------
typedef struct
{
	tm_t								tTm;										// 発生日時
	uint8_t								szFuncName[LOGAPI_FUNC_NAME_SIZE + 1];		// 関数名
	uint32_t							Line;										// 行数

} LOGAPI_HEADER_INFO_TABLE;


//------------------------------------------------
// ログ情報構造体
//------------------------------------------------
#pragma pack
typedef struct
{
	LOGAPI_HEADER_INFO_TABLE			tHeaderInfo;															// ログヘッダー情報
	uint8_t								szLog[((LOGAPI_DATA_SIZE_MAX - sizeof(LOGAPI_HEADER_INFO_TABLE))+1)];	// ログ

} LOGAPI_INFO_TABLE;
#pragma unpack


typedef struct
{
	QueueHandle_t						MutexHandle;						// LOGAPI用ミューテックスハンドル
	LOGAPI_INFO_TABLE					tLogInfo;							// ログ情報(書込み用)

} LOGAPI_GLOBAL_TABLE;


#endif /* LOGAPI_H_ */
