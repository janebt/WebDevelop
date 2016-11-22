/******************************************************************************
*
* Copyright (c) 1996 -2016 TP-LINK Technologies CO.,LTD.
* All rights reserved.
*
* FILE NAME  :   websocketd-utils.h
* VERSION    :   1.0
* DESCRIPTION:   supply string handle api for websocketd...
*
* AUTHOR     :   baitao <baitao@tp-link.net>
* CREATE DATE:   2016/8/19
*
* HISTORY    :
* 01   2016/8/19  baitao     Create.
*
******************************************************************************/
#include <websocketd-utils.h>

//utils
/*
 * ------------------------------------------------------
 * 取得秘钥
 * ------------------------------------------------------
 */
int getSeckey(const char* base, const char* key, char* result)
{
	if(0 > getSubstr(base, key, "\r\n", result))
	{
		return -1;
	}
	return 0;
}

/*
 * ------------------------------------------------------
 * 取得Protocol
 * ------------------------------------------------------
 */
int getProtocol(const char* base, char* result)
{
	// GET /chat HTTP/1.1
	if(0 > getSubstr(base, "/", "\r\n", result))
	{
		return -1;
	}

	return 0;
}

 /*
 * ------------------------------------------------------
 * 取出指定文字
 * ------------------------------------------------------
 */

int getSubstr(const char* base, const char* from_str, const char* to_str, char* result)
{
	char *searchStart = NULL;
	char *searchEnd = NULL;

	//起始字符检索
	searchStart = strstr(base, from_str);
	if(NULL != searchStart)
	{
		searchStart += strlen(from_str);
	}
	else
	{
		return -1;
	}

	//结尾字符检索
	searchEnd = strstr(searchStart, to_str);
	if(NULL != searchEnd && searchEnd > searchStart)
	{
		//取searchStart——searchEnd-1为目标
		strncpy(result, searchStart, searchEnd-searchStart+1);
	}
	else
	{
		return -2;
	}

	//去除前后空白
	if(trim(result) < 0)
	{
		return -3;
	}

	return 0;
}

/*
 * ------------------------------------------------------
 * 去除前后空白
 * ------------------------------------------------------
 */
int trim(char* str)
{
	char *searchStart = str;
	char *searchEnd = str + strlen(str) - 1;
    char *tempStr = (char *)malloc(strlen(str));
    memset(tempStr, 0, strlen(str));

	while(0 != isspace(*(searchStart++)));
	searchStart--;

	while(0 != isspace(*(searchEnd--)));
	searchEnd++;

	if (searchStart < searchEnd)
	{
		strncpy(tempStr, searchStart, (int)(searchEnd-searchStart)+1);
        memset(str, 0, strlen(str));
        strcpy(str, tempStr);
	}
	else
	{
		return -1;
	}

    if(tempStr)
    {
        free(tempStr);
    }

	return 0;
}

//intLib
int tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c + 'a' - 'A';
    }
    else
    {
        return c;
    }
}

int htoi(const char s[],int start,int len)
{
  int i,j;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X')) //判断是否有前导0x或者0X
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    i+=start;
    j=0;
    for (; (s[i] >= '0' && s[i] <= '9')
     || (s[i] >= 'a' && s[i] <= 'f') || (s[i] >='A' && s[i] <= 'F');++i)
    {
        if(j>=len)
  {
    break;
  }
        if (tolower(s[i]) > '9')
        {
            n = 16 * n + (10 + tolower(s[i]) - 'a');
        }
        else
        {
            n = 16 * n + (tolower(s[i]) - '0');
        }
  j++;
    }
    return n;
}
