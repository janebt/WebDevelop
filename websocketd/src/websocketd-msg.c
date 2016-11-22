/******************************************************************************
*
* Copyright (c) 1996 -2016 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   websocketd-msg.h
* VERSION    :   1.0
* DESCRIPTION:   deal with browser request msg ...
*
* AUTHOR     :   baitao <baitao@tp-link.net>
* CREATE DATE:   2016/8/19
*
* HISTORY    :
* 01   2016/8/19  baitao     Create.
*
******************************************************************************/
#include "websocketd-debug.h"
#include "websocketd-msg.h"
#include "uci-api.h"
/******************************************************************************
* FUNCTION		: handleTextMsg
* AUTHOR		: baitao <baitao@tp-link.net>
* DESCRIPTION	: alloc uci contex.
* INPUT			:
* OUTPUT		:
* RETURN		:
******************************************************************************/
int handleTextMsg(const char* msg, char* resp)
{
	if(NULL == msg)
	{
		return -1;
	}

	json_object* jsonRecv = NULL;
	json_tokener* jsonData = NULL;
	char *method = NULL;
	char *data = NULL;
	int status = 1, errorcode = 0;

	jsonRecv = json_tokener_parse(msg);
	if(is_error(jsonRecv))
	{
		WEBSOCKETD_DBPRINTF(MSG_ERROR ,"error parse json: %s", json_tokener_errors[-(unsigned long)jsonRecv]);
		return -1;
	}

	method = json_object_get_string(json_object_object_get(jsonRecv, "method"));
	data = json_object_get_string(json_object_object_get(json_object_object_get(jsonRecv, "motion_detection"),"name"));



	printf("\n\nhandleTextMsg 1\n");
	if(0 == strcmp(method, "get"))
	{
		printf("\n\nhandleTextMsg 2\n");
		if(0 == strcmp(data, "motion_status"))
		{
			WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"\n\nhandleTextMsg 3\n");
			//listen(status);
//        {"motion_detection": {"motion_status": { "motion_exception_status": "0" } }，"error_code": 0}
			sprintf(resp, "{\"motion_detection\":{\"motion_status\":{\"motion_exception_status\": \"%d\"}},\"error_code\": %d}",status,errorcode);
			WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"send msg:%s\n",resp);
		}
	}
	WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"\n\nhandleTextMsg 4%s\n");

	return 0;
}

/****************************************************************************
 * Function   : 	jsonGetOSDSettings
 * Description:     获取OSD配置
 * Input      : 	N/A
 * Output     : 	N/A
 * Return     :	    luci返回的json消息
****************************************************************************/

void listen(int *status)
{
		status = (int)system("uci get motion_detection.motion_status.motion_exception_status");
		WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"listen Status:%d",status);
		sleep(1000);
}


/****************************************************************************
 * Function   : 	jsonGetOSDSettings
 * Description:     获取OSD配置
 * Input      : 	N/A
 * Output     : 	N/A
 * Return     :	    luci返回的json消息
****************************************************************************/

static int get_exception_status()
{
	UCI_SECTION *uci_section = NULL;
	UCI_CONTEXT *ctx = NULL;

	ctx = alloc_uci_ctx();
	if(!ctx)
	{
		WEBSOCKETD_DBPRINTF(MSG_ERROR ,"uci alloc context failed.\n");
		return -1;
	}

	/*get uci section: msg.exception*/
	//if()

}
