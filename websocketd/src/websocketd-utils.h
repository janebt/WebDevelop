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
#ifndef _WEBSOCKETD_UTILS_
#define _WEBSOCKETD_UTILS_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//utils
int getSeckey(const char* base, const char* key, char* result);
int getProtocol(const char* base, char* result);
int getSubstr(const char* base, const char* from_str, const char* to_str, char* result);
int trim(char* str);

//intLib
//int tolower(int c);
int htoi(const char s[],int start,int len);

#endif
