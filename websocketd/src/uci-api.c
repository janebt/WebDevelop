/******************************************************************************
*
* Copyright (c) 1996 -2014 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   uci-api.c
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
#include "websocketd-debug.h"
#include "uci-api.h"

/******************************************************************************
* FUNCTION		: alloc_uci_ctx
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: alloc uci contex. 
* INPUT			: 
* OUTPUT		: 
* RETURN		: 
******************************************************************************/
UCI_CONTEXT *alloc_uci_ctx(void)
{
	return uci_alloc_context();
}

/******************************************************************************
* FUNCTION		: load_uci_package()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: load package. 
* INPUT			: 
* OUTPUT		: 
* RETURN		: 
******************************************************************************/
UCI_PACKAGE *load_uci_package(UCI_CONTEXT *ctx, const char *package)
{
	UCI_PACKAGE *uci_pkg = NULL;

	if (!ctx || !package)
	{
		WEBSOCKETD_DBPRINTF(MSG_ERROR ,"ctx or package is NULL!\n");
		return NULL;
	}
	
	uci_pkg = uci_lookup_package(ctx, package);	
	if (uci_pkg)
	{
		WEBSOCKETD_DBPRINTF(MSG_ERROR ,"uci_unload package %s\n", package);
		uci_unload(ctx, uci_pkg);
	}

	/* uci_load return 0 means successful, else mean failed. */
	if (uci_load(ctx, package, &uci_pkg))
	{
		WEBSOCKETD_DBPRINTF(MSG_ERROR ,"uci_load package[%s] failed\n", package);
		return NULL;
	}

	WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"init config package [%s] successful.\n", package);
	return uci_pkg;
}

/******************************************************************************
* FUNCTION		: get_uci_section()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: get uci section from config and section type. 
* INPUT			: 
* OUTPUT		: 
* RETURN		: 
******************************************************************************/
UCI_SECTION *get_uci_section(UCI_CONTEXT *ctx, const char *package, const char *section) 
{
	UCI_PACKAGE *uci_pkg = NULL;

	if (NULL == ctx || NULL == package || NULL == section)
	{
		WEBSOCKETD_DBPRINTF(MSG_ERROR ,"ctx or package or section is NULL!\n");
		return NULL;
	}

	uci_pkg = load_uci_package(ctx, package);
	if (!uci_pkg)
	{
		WEBSOCKETD_DBPRINTF(MSG_ERROR ,"uci_pkg is NULL!\n");
		return NULL;
	}
	
	return uci_lookup_section(ctx, uci_pkg, section);
}
	
/******************************************************************************
* FUNCTION		: lookup_uci_option_str()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: lookup option name from given section. 
* INPUT			: 
* OUTPUT		: 
* RETURN		: 
******************************************************************************/
char *lookup_uci_option_str(UCI_CONTEXT *ctx, UCI_SECTION *section, char *option)
{
	if (!ctx || !section || !option)
	{
		WEBSOCKETD_DBPRINTF(MSG_ERROR ,"ctx(%p) or section(%p) or option(%p) is null ptr.\n", ctx, section, option);
		return NULL;
	}

	return (char*)uci_lookup_option_string(ctx, section, option);
}

/******************************************************************************
* FUNCTION		: free_uci_ctx
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: free uci resource. 
* INPUT			: 
* OUTPUT		: 
* RETURN		: 0:success; -1:failed
******************************************************************************/
int free_uci_ctx(UCI_CONTEXT *ctx)
{
	if (ctx)
	{
		uci_free_context(ctx);
	}
	
	return 0;
}

