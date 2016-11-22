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

#ifndef _WEBSOCKETD_DEBUG_H__
#define _WEBSOCKETD_DEBUG_H__

#include <stdarg.h>
#include "libmsglog.h"

#define MOD_NAME_WEBSOCKETD "websocketd"
#define WEBSOCKETD_DEBUG


typedef enum _DEBUG_LEVEL
{
	MSG_EXCESS = 0,
	MSG_DEBUG,
	MSG_INFO,
	MSG_NOTICE,
	MSG_WARNING,
	MSG_ERROR,
	MSG_FATAL,
	MSG_MAX
} DEBUG_LEVEL;

#ifdef	WEBSOCKETD_DEBUG
#define	WEBSOCKETD_DBPRINTF(dbg_level, fmt...)										\
	do {																	\
		if (((int)dbg_level > (int)LOG_PRIO_WARNING) && ((int)dbg_level < (int)LOG_PRIO_MAX))	\
		{																	\
			msglog(dbg_level, MOD_NAME_WEBSOCKETD, fmt);							\
		}																	\
		websocketd_printf((int)dbg_level, __FUNCTION__, __LINE__, fmt);					\
	}while(0)
#else
#define	WEBSOCKETD_DBPRINTF(dbg_level, fmt...)										\
	do {																	\
		if ((dbg_level > LOG_PRIO_WARNING) && (dbg_level < LOG_PRIO_MAX))	\
		{																	\
			msglog(dbg_level, MOD_NAME_WEBSOCKETD, fmt);							\
		}																	\
	}while(0)
#endif


void websocketd_printf(int level,const char *func, int line,  char *fmt, ...);

#endif	/* end of __SOAP_DEBUG_H__ */

