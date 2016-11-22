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
#ifndef _WEBSOCKETD_
#define _WEBSOCKETD_

#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/param.h>
#include <sys/uio.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

void mainLoop(void);
void init(void);
void initSocket(void);
void initSignal(void);
void closeSocket(void);
void startChild(void);
void killChild(int);
void mainLoop(void);
int shakeHand(char*, const char*);
int decodeReceiveData(const char*,  int*, char*);
int encodeSendData(unsigned char*, const int, const char*, const int);
#endif