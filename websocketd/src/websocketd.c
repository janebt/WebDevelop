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
/* server.c */
#include "websocketd.h"
#include "websocketd-utils.h"
#include "websocketd-msg.h"
#include "base64.h"
#include "sha1.h"
#include "websocketd-debug.h"

static const int field_max=60;
static const int buf_len=256;
static const int mid_len=512;
static const int max_len=2048;
static const int unused=-1;

static const int port_no=8880;

//handyshake
/**
 ** HTTP/1.1 101 Switching Protocols
 ** Upgrade: websocket
 ** Connection: Upgrade
 ** Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
 ** Sec-WebSocket-Protocol: chat
 **/
static const char *response_handyshake =
        "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n";

static const char *response_notfound =
        "HTTP/1.1 404 Not Found\r\n";

static const char *GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

static long transaction_number = 0;
//static time_t _date;

static int curr_sock;
static int fork_sock;

/****************************************************************************
 * Function   :     main
 * Description:     启动server
 * Input      :     N/A
 * Output     :     N/A
 * Return     :     返回启动结果
****************************************************************************/
int main(int argc, char *argv[])
{
    init();
    initSocket();
    initSignal();
    mainLoop();
    //closeSocket();
    return 0;
}

/****************************************************************************
 * Function   :     init
 * Description:     更新两个套接字状态
 * Input      :     N/A
 * Output     :     N/A
 * Return     :     N/A
****************************************************************************/
void init()
{
    curr_sock = unused;
    fork_sock = unused;
}

/****************************************************************************
 * Function   :     initSocket
 * Description:     创建Socket，绑定port_no端口
 * Input      :     N/A
 * Output     :     N/A
 * Return     :     N/A
****************************************************************************/
void initSocket()
{
    char hostname[buf_len];
    struct sockaddr_in saddr;
    int opt;

    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"Begining\nInitialize Socket\n");

    if(-1 == gethostname(hostname, buf_len))
    {
        perror("gethostname");
        WEBSOCKETD_DBPRINTF(MSG_ERROR ,">> Exit by error : Don't get a hostname. <<\n");
        exit(-1);
    }
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"hostname : %s\n",hostname);


    if(-1 == (curr_sock = socket(AF_INET, SOCK_STREAM, 0)))
    {
        perror("socket");
        WEBSOCKETD_DBPRINTF(MSG_ERROR ,">> Exit by error : It's fail to make a socket for new connection. <<\n");
        exit(-1);
    }
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"curr_sock : socket is %d\n", curr_sock);

    opt = 1;
    if(0 != setsockopt(curr_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)))
    {
        perror("setsockopt");
        WEBSOCKETD_DBPRINTF(MSG_ERROR ,">> Exit by error : Cannot set a socket options. <<\n");
        exit(-1);
    }
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"curr_sock : setsockopt\n");

    memset((char*)&saddr, 0, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port_no);

    //bind
    if(-1 == (bind(curr_sock, (struct sockaddr *)&saddr, sizeof(saddr))))
    {
        perror("bind");
        WEBSOCKETD_DBPRINTF(MSG_ERROR ,">> Exit by error : Don't bind a socket. <<\n");
        exit(-1);
    }
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"curr_sock : bind\n");
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,">> Ending \"Initialize Socket\" <<\n");
}

/****************************************************************************
 * Function   :     closeSocket
 * Description:     关闭Socket，修改两个socket状态
 * Input      :     N/A
 * Output     :     N/A
 * Return     :     N/A
****************************************************************************/
void closeSocket()
{
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,">> Begining \"Close Socket\" <<\n");

    if(unused != curr_sock)
    {
        close(curr_sock);
        WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"curr_sock : close\n");
        curr_sock = unused;
    }

    if(unused != fork_sock)
    {
        close(fork_sock);
        WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"fork_sock : close\n");
        fork_sock = unused;
    }

    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,">> Ending \"Close Socket\" <<\n");
    exit(0);
}

/****************************************************************************
 * Function   :     killChild
 * Description:     捕捉子进程结束信号
 * Input      :     sig     : 信号量
 * Output     :     N/A
 * Return     :     N/A
****************************************************************************/
void killChild(int sig)
{
    while(0 < waitpid(-1, NULL, WNOHANG));
    signal(SIGCHLD, killChild);
}

/****************************************************************************
 * Function   :     initSignal
 * Description:     初始化信号，绑定killChild()
 * Input      :     N/A
 * Output     :     N/A
 * Return     :     N/A
****************************************************************************/
void initSignal()
{
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,">> Begining \"Innitialize Signal\" <<\n");

    signal(SIGCHLD, killChild);

    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,">> Ending \"Innitialize Signal\" <<\n");
}

/****************************************************************************
 * Function   :     mainLoop
 * Description:     主循环函数，监听端口并建立子进程与各个请求通信
 * Input      :     N/A
 * Output     :     N/A
 * Return     :     N/A
****************************************************************************/
void mainLoop()
{
    struct sockaddr_in caddr;
    socklen_t len;
    int sresp;
    int width, pid;

    fd_set readfds;
    struct timeval timeout;

    printf(">> Begining \"Main Loop\" <<\n");
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,">> Begining \"Main Loop\" <<\n");
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"curr_sock is %d\n", curr_sock);

    //printf("listen : %s")

    if(0 > (listen(curr_sock, SOMAXCONN)))
    {   
        printf(">> Exit by error : Cannot listen. <<\n");
        perror("listen");
        exit(-1);
    }

    printf("curr_sock : listen\n");
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"curr_sock : listen\n");

    while(1)
    {
        FD_ZERO(&readfds);
        FD_SET(curr_sock, &readfds);
        width = curr_sock + 1;

        //5s监听
        timeout.tv_sec=5;
        timeout.tv_usec=0;

        sresp = select(width, &readfds, NULL, NULL, &timeout);
        if(sresp && sresp != -1)
        {
            if(FD_ISSET(curr_sock, &readfds))
            {
                printf("curr_sock : reading\n");
                WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"curr_sock : reading\n");

                len = sizeof(caddr);
                fork_sock = accept(curr_sock, (struct sockaddr *)&caddr, &len);
                if(0 > fork_sock)
                {
                    if(EINTR == errno)
                    {
                        continue;
                    }

                    perror("accept");
                    continue;
                }

                if(0 == (pid = fork()))
                {
                    close(curr_sock);
                    curr_sock=unused;

                    transaction_number++;
                    startChild();

                    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,">> Ending \"Main Loop\" <<\n");
                    exit(0);
                }

                setpgid(pid, getpid());
                close(fork_sock);
                fork_sock = unused;
            } //<--if(FD_ISSET())
        } //<--if(select())
    } //<--while(1)
}

/****************************************************************************
 * Function   :     startChild
 * Description:     建立子进程，对每个请求根据请求报文进行握手或通信
 * Input      :     N/A
 * Output     :     N/A
 * Return     :     N/A
****************************************************************************/
void startChild()
{
    char protocol[max_len];
    memset(protocol, 0, sizeof(protocol));
    char *secWebSocketKey;

    int isHandshake = 0;

    while(1)
    {
        int msglen = 0;
        char data[max_len];
        memset(data, 0, sizeof(data));

        msglen = read(fork_sock, data, max_len-1);
        if(msglen < 1)
        {
            sleep(100);
        }
        else
        {
            WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"\n!!!<REQUEST>!!!\n%s\n", data);

            if(!isHandshake)
            {
                //握手
                isHandshake = (0 == shakeHand(protocol, data)) ? 0 : 1;
                //printf("isHandshake = %d\n", isHandshake);
                continue;
            }
            else //if(0 == strcmp(protocol, "chat"))//???
            {
                char payload[max_len];
                char temp[max_len];
                int opcode, i;
                int senddata_len;
                unsigned char senddata[max_len];
                memset(senddata,0,sizeof(senddata));

                WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00001:\n");
                //接收数据解码
                decodeReceiveData(data, &opcode, payload);
                WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00002:\n");
                WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"payload:\n%s\n", payload);
                switch(opcode)
                {
                    case 1: //text frame
                        //接收的文本做了记号送回
                        WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00003:\n");
                        if(0 > handleTextMsg(payload, temp))
                        {
                            return;
                        }
                        memset(payload, 0 , sizeof(payload));
                        strcpy(payload, temp);

                        WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"payload:\n%s\n", payload);
                        senddata_len = encodeSendData(senddata, opcode, payload, 0);
                    break;
                    case 2: //BINARY: 2,二进制数据，通常用来保存图片
                    break;
                    case 8: //close frame

                    break;
                    //PING: 9, 心跳检测。ping
                    //PONG: 10,心跳检测。pong
                    default:
                    break;
                }
                WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00004:\n");
                WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"senddata_len after encode:%d\n", senddata_len);

                if(0 >= senddata_len)
                {
                    exit(1);
                }

                for(i = 0; i < senddata_len; i++)
                {
                    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"%02x ", senddata[i]);
                }
                WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00005:\n");

                write(fork_sock,senddata,senddata_len);
                //write(fork_sock,&response[0],response.length());
            }
            /*
            else
            {
                //??
                memset(response, 0, sizeof(response));
                strcpy(response, response_notfound);
                write(fork_sock, &response[0], strlen(response));

            }
            */
        }
    }
}

/****************************************************************************
 * Function   :     shakeHand
 * Description:     进行握手，过程为：取得通信秘钥，加上幻数后进行sha1加密，
                    然后进行编码处理，再加上报头返回。
 * Input      :     protocol    : 获得通信协议，判断是否是第一次通信
                    data        : 应用层得到的请求报文
 * Output     :     N/A
 * Return     :     luci返回的json消息
****************************************************************************/
int shakeHand(char* protocol, const char *data)
{

    char response[max_len];
    memset(response, 0, sizeof(response));

    char seckey[max_len];
    memset(seckey, 0, sizeof(seckey));

    char sec_websocket_protocol[max_len];
    memset(sec_websocket_protocol, 0, sizeof(sec_websocket_protocol));

    char sec_websocket_accept[max_len];
    memset(sec_websocket_accept, 0, sizeof(sec_websocket_accept));

    char sha1DataHashed[max_len];
    memset(sha1DataHashed, 0, sizeof(sha1DataHashed));

    char *sha1Data = NULL;

    char serverKey[max_len];
    memset(serverKey, 0, sizeof(serverKey));

    int n, i;
    int isHandshake=-1;

    //printf("\nshakeHand\n<REQUEST>\n%s\n", data);

    //取得被发送的协议名
    if(0 > getProtocol(data, protocol))
    {
        WEBSOCKETD_DBPRINTF(MSG_ERROR ,"ERROR:getProtocol\n");
        exit(-1);
    }

    //取得秘钥
    // Sec-WebSocket-Key: wh9PtKqHD/xPmaOlIAS7nQ==
    if(0 > getSeckey(data, "Sec-WebSocket-Key:", seckey))
    {
        WEBSOCKETD_DBPRINTF(MSG_ERROR ,"ERROR:getSeckey\n");
        exit(-1);
    }
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"shakeHand1 getSeckey:%s\n",seckey);
    strcat(seckey, GUID);

    if(0 >= sha1_hash(seckey, sha1DataHashed))
    {
        WEBSOCKETD_DBPRINTF(MSG_ERROR ,"ERROR:sha1_hash\n");
        exit(-1);
    }
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"shakeHand1 sha1_hash:%s\n",sha1DataHashed);

    n = strlen(sha1DataHashed);
    sha1Data = (char *)malloc(n/2+1);
    memset(sha1Data, 0, n/2+1);

    for(i = 0; i < n; i += 2)
    {
        sha1Data[i/2] = htoi(sha1DataHashed, i, 2);
    }

    base64_encode(sha1Data, strlen(sha1Data), serverKey);
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"shakeHand1 base64_encode:%s\n",serverKey);
    //返回客户端
    strcpy(sec_websocket_accept, "Sec-WebSocket-Accept: ");
    strcat(sec_websocket_accept, serverKey);
    strcat(sec_websocket_accept, "\r\n");

    strcpy(sec_websocket_protocol, "Sec-WebSocket-Protocol: ");
    strcat(sec_websocket_protocol, protocol);
    strcat(sec_websocket_protocol, "\r\n");

    //组包response报文
    strcpy(response, response_handyshake);
    strcat(response, sec_websocket_accept);
    strcat(response, "\r\n");

    //debug print
    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"<RESPONSE>\n%s", response);

    isHandshake = write(fork_sock, response, strlen(response));

    //printf("<SIZE>\n%d\n<WRITE>\n%d\n", strlen(response), isHandshake);

    if(sha1Data)
    {
        free(sha1Data);
    }

    return isHandshake;
}

/*
    * websocket 通信协议
    *
      0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+
    *
    *  说明：
    *  FIN：1位，是否是消息的结束帧(分片)
    RSV1, RSV2, RSV3: 分别都是1位, 预留，用于约定自定义协议。 如果双方之间没
        有约定自定义协议，那么这几位的值都必须为0,否则必须断掉WebSocket连接；
    Opcode:4位操作码，定义有效负载数据，如果收到了一个未知的操作码，连接也必
        须断掉，以下是定义的操作码：
            %x0 表示连续消息分片
            %x1 表示文本消息分片
            %x2 表未二进制消息分片
            %x3-7 为将来的非控制消息片断保留的操作码
            %x8 表示连接关闭
            %x9 表示心跳检查的ping
            %xA 表示心跳检查的pong
            %xB-F 为将来的控制消息片断的保留操作码
    Mask: 定义传输的数据是否有加掩码,如果设置为1,掩码键必须放在masking-key区
        域，客户端发送给服务端的所有消息，此位的值都是1；
    Payload length: 传输数据的长度，以字节的形式表示：7位、7+16位、或者7+64
        位。如果这个值以字节表示是0-125这个范围，那这个值就表示传输数据的长
        度；如果这个值是126，则随后的两个字节表示的是一个16进制无符号数，用
        来表示传输数据的长度；如果这个值是127,则随后的是8个字节表示的一个64
        位无符合数，这个数用来表示传输数据的长度。多字节长度的数量是以网络字
        节的顺序表示。负载数据的长度为扩展数据及应用数据之和，扩展数据的长度
        可能为0,因而此时负载数据的长度就为应用数据的长度。注意Payload length
        不包括Masking-key在内。
    Masking-key: 0或4个字节，客户端发送给服务端的数据，都是通过内嵌的一个32
        位值作为掩码的；掩码键只有在掩码位设置为1的时候存在。 数据Mask方法
        是，第 i byte 数据 = orig-data ^ (i % 4) .
    Payload data: (x+y)位，负载数据为扩展数据及应用数据长度之和。
    Extension data:x位，如果客户端与服务端之间没有特殊约定，那么扩展数据的长
        度始终为0，任何的扩展都必须指定扩展数据的长度，或者长度的计算方式，
        以及在握手时如何确定正确的握手方式。如果存在扩展数据，则扩展数据就会
        包括在负载数据的长度之内。
    Application data:y位，任意的应用数据，放在扩展数据之后，应用数据的长度=
        负载数据的长度-扩展数据的长度。
    *
*/

/*
 * ------------------------------------------------------
 * req报文解码
 * ------------------------------------------------------
 */
/****************************************************************************
 * Function   :     decodeReceiveData
 * Description:     解码请求报文， 将结果返回给opcode和payload
 * Input      :     data    : 应用层得到的请求报文
                    opcode  : 引用，返回4位的操作码
                    payload : 传输的有效数据
 * Output     :      
 * Return     :     payload长度
****************************************************************************/
int decodeReceiveData(const char *data,  int *opcode, char *payload)
{
    int mask;
    char mask_key[4];
    int payload_len;
    int payload_data_len;
    int payload_offset;

    int i;

    //opcode
    *opcode = data[0] & 0x0f;
    //mask
    mask = (data[1] & 0x80) >> 7;
    //payload length
    payload_len = data[1] & 0x7f;

    if (126 == payload_len)
    {
        //masking
        for (i = 0; i < 4; i++){
            mask_key[i] = data[i+4];
        }
        //payload的起始位置
        payload_offset = 8;
        //payload长度
        payload_data_len = data[2]<<8 | data[3];
    }
    else if (127 == payload_len)
    {
        //masking
        for (i = 0; i < 4; i++){
            mask_key[i] = data[i+10];
        }
        //payload的起始位置
        payload_offset = 14;
        //payload长度
        payload_data_len = 0;
        //for(i = 0; i < 8; i++){ //64位协议使用此??）
        for(i = 0; i < 4; i++)
        {
            payload_data_len |= data[10-i] << (i * 8);
        }
    }
    else
    {
        //masking
        for (i = 0; i < 4; i++){
            mask_key[i] = data[i+2];
        }
        //payload的起始位置
        payload_offset = 6;
        //payload长度
        payload_data_len = payload_len;
    }

    //payloadデータをセット
    for(i = 0; i < payload_data_len; i++){
        payload[i] = data[i+payload_offset];
        if(1 == mask)
        {
            payload[i] = payload[i] ^ mask_key[i%4];
        }
    }
    payload[i] = 0;

    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"<DECODE>\nopcode:%d\nmask:%d\npayload_len:%d\npayload_data_len:%d\npayload_offset:%d\npayload:%s\n",
            *opcode, mask, payload_len, payload_data_len, payload_offset, payload);

    return i;
}

/*
 * ------------------------------------------------------
 * resp报文编码
 * ------------------------------------------------------
 */
/****************************************************************************
 * Function   :     encodeSendData
 * Description:     编码响应报文， 将结果返回给opcode和payload
 * Input      :     frame   : 响应报文段
                    opcode  : 4位操作码
                    payload : 传输的有效数据
                    mask    : 掩码
 * Output     :      
 * Return     :     payload长度
****************************************************************************/
int encodeSendData(unsigned char *frame, const int opcode, const char *payload,
     const int mask)
{
    unsigned char *frameHeader = NULL;
    int frameHeaderSize;
    char mask_key[4];
    int payload_data_len;
    int i,j;

    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00100:\n");
    payload_data_len = strlen(payload);

    if(65535 < payload_data_len)
    {
        /*
         * [0]     fin|rsv1|rsv2|rsv3|opcode(4)
         * [1]     mask|payload_len(7)
         * [2-9]   extended payload length(8*8)
         * [10-13] mask key(4*8)
         */
        frameHeaderSize = 10;
    }else if(125 < payload_data_len)
    {
        /*
         * [0]     fin|rsv1|rsv2|rsv3|opcode(4)
         * [1]     mask|payload_len(7)
         * [2-3]   extended payload length(8*2)
         * [4-7]   mask key(4*8)
         */
        frameHeaderSize = 4;
    }else
    {
        /*
         * [0]     fin|rsv1|rsv2|rsv3|opcode(4)
         * [1]     mask|payload_len(7)
         * [2-5]   mask key(4*8)
         */
        frameHeaderSize = 2;
    }
    if(1 == mask)
    {
        frameHeaderSize += 4;
    }

    frameHeader = (unsigned char*)realloc(frameHeader, sizeof(unsigned char)*frameHeaderSize);

    //opcode
    switch(opcode){
        case 1:
            /*
             * 1000 0001(fin=1,opcode=1)
             */
            frameHeader[0] = 0x81;
            break;
        default:
            frameHeader[0] = 0x80;
            break;
    }

    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00101:\n");
    if(65535 < payload_data_len)
    {
        WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00102:\n");
        /*
         * 1111 1111(mask=1,payload_len=127)
         * 0111 1111(mask=0,payload_len=127)
         */
        frameHeader[1] = (mask << 7) + 127;

        /*
         * フレーム[2-9]
         * %x0000000000000000-7FFFFFFFFFFFFFFF
         */
        for(i = 0; i < 8; i++)
        {
            if(3 < i)
            {
                frameHeader[9-i] = 0x00;
            } //32bit以上(int型)
            frameHeader[9-i] = (payload_data_len & (0xff << (i*8))) >> (i*8);
        }
    }
    else if(125 < payload_data_len)
    {
        WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00103:\n");
        /*
         * 1111 1110(mask=1,payload_len=126)
         * 0111 1110(mask=0,payload_len=126)
         */
        frameHeader[1] = (mask << 7) + 126;

        /*
         * [2-3]
         * %x0000-FFFF
         */
        for(i = 0; i < 2; i++)
        {
            frameHeader[3-i] = (payload_data_len & (0xff << (i*8))) >> (i*8);
        }
    }
    else
    {
        WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00104:\n");
        frameHeader[1] = (mask << 7) + payload_data_len;
    }

    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00105:\n");

    //mask_key
    if(1 == mask)
    {
        for(i = 0; i < 4; i++)
        {
            mask_key[i] = rand() % 255; //0-255
            frameHeader[frameHeaderSize-4+i] = mask_key[i];
        }
    }

    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00106:\n");

    for(i = 0; i < frameHeaderSize; i++)
    {
        frame[i] = frameHeader[i];
    }

    for(i = 0; i < payload_data_len; i++)
    {
        j = i + frameHeaderSize;
        frame[j] = payload[i] & 0xff; //1byte
        if(mask==1)
        {
            frame[j] = frame[j] ^ mask_key[i%4];
        }
    }
    free(frameHeader);

    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"00107:\n");
    frame[frameHeaderSize+payload_data_len] = 0;

    WEBSOCKETD_DBPRINTF(MSG_DEBUG ,"<ENCODE>\nopcode:%d\nmask:%d\npayload_data_len:%d\npayload_offset:%d\npayload:%s\n",
            opcode, mask, payload_data_len, frameHeaderSize, payload);

    return frameHeaderSize + payload_data_len;
}
