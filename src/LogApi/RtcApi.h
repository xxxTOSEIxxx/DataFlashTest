/*
 * RtcApi.h
 *
 *  Created on: 2020/10/30
 *      Author: 99990045
 */

#ifndef RTCAPI_H_
#define RTCAPI_H_
#include "r_rtc_rx_if.h"								// RTC I/F
#include "semphr.h"


typedef struct
{
	QueueHandle_t						MutexHandle;						// RtcAPI用ミューテックスハンドル
	rtc_init_t							tRtcInit;							// RTC初期設定構造体
	bool								bOpen;								// オープンフラグ(false:未オープン, true：オープン)

	tm_t								tCurrentTime;						// 現在日時
	tm_t								tAlarmTime;							// アラーム日時

} RTCAPI_GLOBAL_TABLE;


#endif /* RTCAPI_H_ */
