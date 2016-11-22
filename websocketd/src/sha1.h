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
#ifndef _WEBSOCKETD_SHA_1_
#define _WEBSOCKETD_SHA_1_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <ctype.h>

//SHA1 struct
typedef struct SHA1Context{
  unsigned Message_Digest[5];
  unsigned Length_Low;
  unsigned Length_High;
  unsigned char Message_Block[64];
  int Message_Block_Index;
  int Computed;
  int Corrupted;
} SHA1Context;

#define SHA1CircularShift(bits,word) ((((word) << (bits)) & 0xFFFFFFFF) | ((word) >> (32-(bits))))

void SHA1Reset(SHA1Context *);
int SHA1Result(SHA1Context *);
void SHA1Input( SHA1Context *,const char *, unsigned);
int sha1_hash(const char *source, char* buf);
void SHA1ProcessMessageBlock(SHA1Context *);
void SHA1PadMessage(SHA1Context *);

#endif
