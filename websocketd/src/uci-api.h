/******************************************************************************
*
* Copyright (c) 1996 -2016 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   uci-api.h
* VERSION    :   1.0
* DESCRIPTION:   get/set uci interface..
*
* AUTHOR     :   wangfuyu <wangfuyu@tp-link.net>
* CREATE DATE:   12/23/2014
*
* HISTORY    :
* 01   12/23/2014  wangfuyu     Create.
*
******************************************************************************/
#ifndef _UCI_API_H_
#define _UCI_API_H_

#include <uci.h>


typedef struct uci_context UCI_CONTEXT;
typedef struct uci_package UCI_PACKAGE;
typedef struct uci_element UCI_ELEMENT;
typedef struct uci_section UCI_SECTION;
typedef struct uci_ptr UCI_PTR;

/* alloc uci context */
UCI_CONTEXT *alloc_uci_ctx(void);

/* load uci package */
UCI_PACKAGE *load_uci_package(UCI_CONTEXT *ctx, const char *package);

/* get uci section */
UCI_SECTION *get_uci_section(UCI_CONTEXT *ctx, const char *package, const char *section);

/* lookup uci option string */
char *lookup_uci_option_str(UCI_CONTEXT *ctx, UCI_SECTION *section, char *option);

/* free uci context */
int free_uci_ctx(UCI_CONTEXT *ctx);

#endif

