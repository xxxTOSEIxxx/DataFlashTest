/*
 * RtcApi.c
 *
 *  Created on: 2020/10/30
 *      Author: 99990045
 */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "RtcApi.h"


// RtcAPIグローバル変数
RTCAPI_GLOBAL_TABLE						g_tRtcApi;


//============================================================================================================
// RtcApI オープン
//============================================================================================================
int8_t RtcApi_Open(void)
{
	rtc_err_t					eRetResult = RTC_SUCCESS;


	// 既にオープン済みの場合
	if (g_tRtcApi.bOpen == true)
	{
		return 0;
	}

	memset(&g_tRtcApi.tRtcInit,0x00,sizeof(rtc_init_t));
	g_tRtcApi.tRtcInit.set_time = false;									// RTCの初期化および日時設定(true：実行, false:スキップ)
	g_tRtcApi.tRtcInit.output_freq = RTC_OUTPUT_OFF;						// クロック出力の周波数(set_time = false指定時：設定値無効)
	g_tRtcApi.tRtcInit.








}




