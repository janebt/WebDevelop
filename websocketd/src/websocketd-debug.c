/******************************************************************************
*
* Copyright (c) 1996 -2016 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   websocketd-debug.h
* VERSION    :   1.0
* DESCRIPTION:   add print log method for websocketd ...
*
* AUTHOR     :   baitao <baitao@tp-link.net>
* CREATE DATE:   2016/8/19
*
* HISTORY    :
* 01   2016/8/19  baitao     Create.
*
******************************************************************************/

#include <stdio.h>
#include <stdarg.h>

#include "libmsglog.h"
#include "tp_type.h"
#include "websocketd-debug.h"


LOCAL int debug_level = MSG_ERROR;


/****************************************************************************
 * Function   : 	websocketd_printf
 * Description: 	打印调试信息

 * Input      : 	level : 信息等级

 * Output     :
 * Return     :	N/A
****************************************************************************/
void websocketd_printf(int level,const char *func, int line,  char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	if (level >= debug_level && level < MSG_MAX)
	{
		printf("[%s:%d]: ", func, line);
		vprintf(fmt, args);
        if(level >= MSG_ERROR)
        {
            printf("\n");
        }
	}
    va_end(args);
}


