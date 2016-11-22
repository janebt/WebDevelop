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
#ifndef _WEBSOCKETD_BASE_64_
#define _WEBSOCKETD_BASE_64_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <ctype.h>

//base_64
int base64_encode(const char* data, int data_len, char* result);
int base64_decode(const char* data, int data_len, char* result);
//char find_pos(char ch);

#endif
