/*
 * LogApi.c
 *
 *  Created on: 2020/10/30
 *      Author: 99990045
 */
#include "LogApi.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"


// LogApiグローバル変数
LOGAPI_GLOBAL_TABLE				g_tLogApi;


// プロトタイプ宣言
static int8_t LogApi_Write(LOGAPI_INFO_TABLE* ptLogInfo);
static int8_t LogApi_SetLogAddress(uint32_t LogAddress);
static int8_t LogApi_AllClear(void);
static int8_t LogApi_Clear(uint32_t LogAddress);



//============================================================================================================
// ログ出力
//============================================================================================================
void LogApi_Output(const uint8_t *pszFuncName, uint32_t Line, const uint8_t *format,...)
{
	va_list						ap;


	// 引数チェック
	if (format == NULL)
	{
		return;
	}

	// ▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼▽▼
	xSemaphoreTake(g_tLogApi.MutexHandle,portMAX_DELAY);

	// ログ情報を初期化
	memset(&g_tLogApi.tLogInfo, 0x00, sizeof(g_tLogApi.tLogInfo));

	// 関数名
	if (pszFuncName != NULL)
	{
		strncpy(g_tLogApi.tLogInfo.tHeaderInfo.szFuncName, pszFuncName, LOGAPI_FUNC_NAME_SIZE);
	}

	// ログ出力日時をRTCから取得

	// ログ
	va_start(ap, format);
	vsnprintf(g_tLogApi.tLogInfo.szLog, (LOGAPI_DATA_SIZE_MAX - sizeof(LOGAPI_HEADER_INFO_TABLE)),ap);
	va_end(ap);

	// ログ書込み
	LogApi_Write(&g_tLogApi.tLogInfo);

	xSemaphoreGive(g_tLogApi.MutexHandle);
	// ▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲△▲

	return;
}


//============================================================================================================
// ログ書込み
//============================================================================================================
static int8_t LogApi_Write(LOGAPI_INFO_TABLE* ptLogInfo)
{
	int8_t						Result = 0;
	flash_err_t					eFlashResult = FLASH_SUCCESS;
	uint32_t*					pLogAddress = FLASH_DF_BLOCK_0;
	uint32_t					LogAddress = 0;


	// 引数チェック
	if (ptLogInfo == NULL)
	{
		return -1;
	}

	// FLASH API オープン
	eFlashResult = R_FLASH_Open();
	if (eFlashResult != FLASH_SUCCESS)
	{
		// 既にオープンしている以外のエラーの場合
		if (eFlashResult != FLASH_ERR_ALREADY_OPEN)
		{
			printf("R_FLASH_Open Error. [eFlashResult:%d]\n",eFlashResult);
			return -1;
		}
	}

	// ログ書込み位置取得(データフラッシュの先頭ブロックのアドレス)
	LogAddress = *pLogAddress;
	if ((g_tLogApi.LogAddress >= FLASH_DF_BLOCK_1) && (g_tLogApi.LogAddress < FLASH_DF_BLOCK_INVALID))
	{
		if ((g_tLogApi.LogAddress % LOGAPI_DATA_SIZE_MAX) != 0)
		{
			LogAddress = FLASH_DF_BLOCK_1;
			LogApi_AllClear();
		}
	}
	else
	{
		LogAddress = FLASH_DF_BLOCK_1;
		LogApi_AllClear();
	}

	// 指定位置のログ情報をクリアする
	Result = LogApi_Clear(LogAddress);
	if (Result != 0)
	{
		goto LogApi_Write_End_Lable;
	}

	// ログ書込み
	eFlashResult = R_FLASH_Write((uint32_t)ptLogInfo,LogAddress,sizeof(LOGAPI_INFO_TABLE));
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("R_FLASH_Write Error. [eFlashResult:%d]\n",eFlashResult);
		Result = -1;
		goto LogApi_Write_End_Lable;
	}

	//　ログ出力先アドレスを書込む
	LogAddress += LOGAPI_DATA_SIZE_MAX;
	if (LogAddress >= FLASH_DF_BLOCK_INVALID)
	{
		LogAddress = FLASH_DF_BLOCK_1;
	}
	Result = LogApi_SetLogAddress(g_tLogApi.LogAddress);
	if (Result != 0)
	{
		goto LogApi_Write_End_Lable;
	}

	Result = 0;

LogApi_Write_End_Lable:

	// FLASH API クローズ
	eFlashResult = R_FLASH_Close();
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("R_FLASH_Close Error. [eFlashResult:%d]\n",eFlashResult);
		Result = -1;
	}

	return Result;
}


//============================================================================================================
//　ログ出力先アドレスを書込む
//============================================================================================================
static int8_t LogApi_SetLogAddress(uint32_t LogAddress)
{
	int8_t						Result = 0;
	flash_err_t					eFlashResult = FLASH_SUCCESS;


	// 書込み先をイレース
	Result = LogApi_Clear(FLASH_DF_BLOCK_0);
	if (Result != 0)
	{
		return -1;
	}

	// ログ出力先アドレスを書込む
	eFlashResult = R_FLASH_Write(&LogAddress,FLASH_DF_BLOCK_0,sizeof(LogAddress));
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("R_FLASH_Write Error. [eFlashResult:%d]\n",eFlashResult);
		return -1;
	}

	return 0;
}


//============================================================================================================
// ログ全クリア
//============================================================================================================
static int8_t LogApi_AllClear(void)
{
	flash_err_t					eFlashResult = FLASH_SUCCESS;
	uint32_t					NumBlocks = ((FLASH_DF_BLOCK_INVALID - FLASH_DF_BLOCK_0) / 64);


	// ログを全てクリア
	eFlashResult = R_FLASH_Erase(FLASH_DF_BLOCK_0, NumBlocks);
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("R_FLASH_Erase Error. [eFlashResult:%d]\n",eFlashResult);
		return -1;
	}

	return 0;
}


//============================================================================================================
// 指定位置のログ情報をクリアする
//============================================================================================================
static int8_t LogApi_Clear(uint32_t LogAddress)
{
	flash_err_t					eFlashResult = FLASH_SUCCESS;
	uint32_t					NumBlocks = LOGAPI_DATA_SIZE_MAX / 64;


	// 指定位置のログ情報をクリア
	eFlashResult = R_FLASH_Erase(LogAddress, NumBlocks);
	if (eFlashResult != FLASH_SUCCESS)
	{
		printf("R_FLASH_Erase Error. [eFlashResult:%d]\n",eFlashResult);
		return -1;
	}

	return 0;
}




