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
#include "base64.h"

const char basestr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static char find_pos(char ch)
{
    char *ptr = (char*)strrchr(basestr, ch);//the last position (the only) in base[]
    return (ptr - basestr);
}

int base64_encode(const char* data, int data_len, char* result)
{
    //int data_len = strlen(data);
    int prepare = 0;
    int ret_len;
    int temp = 0;
    char *f = NULL;
    int tmp = 0;
    char changed[4];
    int i = 0;
    ret_len = data_len / 3;
    temp = data_len % 3;

    if (0 < temp)
    {
        ret_len += 1;
    }

    ret_len = ret_len * 4 + 1;

    f = result;

    while (tmp < data_len)
    {
        temp = 0;
        prepare = 0;
        memset(changed, '\0', 4);
        while (temp < 3)
        {
            //printf("tmp = %d\n", tmp);
            if (tmp >= data_len)
            {
                break;
            }
            prepare = ((prepare << 8) | (data[tmp] & 0xFF));
            tmp++;
            temp++;
        }
        prepare = (prepare<<((3-temp)*8));
        //printf("before for : temp = %d, prepare = %d\n", temp, prepare);
        for (i = 0; i < 4 ;i++ )
        {
            if (temp < i)
            {
                changed[i] = 0x40;
            }
            else
            {
                changed[i] = (prepare >> ((3-i)*6)) & 0x3F;
            }
            *f = basestr[(int)changed[i]];
            //printf("%.2X", changed[i]);
            f++;
        }
    }
    *f = '\0';

    return 0;
}

int base64_decode(const char *data, int data_len, char* result)
{
    int ret_len = (data_len / 4) * 3;
    int equal_count = 0;
    char *f = NULL;
    int tmp = 0;
    int temp = 0;
    char need[3];
    int prepare = 0;
    int i = 0;
    if (*(data + data_len - 1) == '=')
    {
        equal_count += 1;
    }
    if (*(data + data_len - 2) == '=')
    {
        equal_count += 1;
    }
    if (*(data + data_len - 3) == '=')
    {//seems impossible
        equal_count += 1;
    }
    switch (equal_count)
    {
    case 0:
        ret_len += 4;//3 + 1 [1 for NULL]
        break;
    case 1:
        ret_len += 4;//Ceil((6*3)/8)+1
        break;
    case 2:
        ret_len += 3;//Ceil((6*2)/8)+1
        break;
    case 3:
        ret_len += 2;//Ceil((6*1)/8)+1
        break;
    }

    f = result;
    while (tmp < (data_len - equal_count))
    {
        temp = 0;
        prepare = 0;
        memset(need, 0, 4);
        while (temp < 4)
        {
            if (tmp >= (data_len - equal_count))
            {
                break;
            }
            prepare = (prepare << 6) | (find_pos(data[tmp]));
            temp++;
            tmp++;
        }
        prepare = prepare << ((4-temp) * 6);
        for (i=0; i<3 ;i++ )
        {
            if (i == temp)
            {
                break;
            }
            *f = (char)((prepare>>((2-i)*8)) & 0xFF);
            f++;
        }
    }
    *f = '\0';
    return 0;
}
