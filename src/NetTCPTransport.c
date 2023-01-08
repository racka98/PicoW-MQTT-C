
#include "NetTCPTransport.h"

#include <errno.h>
#include <pico/stdlib.h>
#include <stdlib.h>

#define DEBUG_LINE 25

/**
 * Only used in this file
 */
/***
 * Connect to socket previously stored ip address and port number
 * @return true if socket openned
 */
static bool _transConnect();

/***
 * Call back function for the DNS lookup
 * @param name - server name
 * @param ipaddr - resulting IP address
 * @param callback_arg - poiter to TCPTransport object
 */
static void _dnsCB(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

/***
 * Called when DNS is returned
 * @param name - server name
 * @param ipaddr - ip address of server
 * @param callback_arg - this TCPtransport object
 */
static void _dnsFound(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

// Socket number
int _xSock = 0;

// Port to connect to
uint16_t _xPort = 80;

// Remote server IP to connect to
ip_addr_t _xHost;

// Remote server name to connect to
char _xHostName[80];

// Semaphore used to wait on DNS responses
SemaphoreHandle_t _xHostDNSFound;

static bool _transConnect() {
    struct sockaddr_in serv_addr;

    _xSock = socket(AF_INET, SOCK_STREAM, 0);
    if (_xSock < 0) {
        LogError(("ERROR opening socket\n"));
        return false;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(_xPort);
    memcpy(&serv_addr.sin_addr.s_addr, &_xHost, sizeof(_xHost));

    int res = connect(_xSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (res < 0) {
        char *s = ipaddr_ntoa(&_xHost);
        LogError(("ERROR connecting %d to %s port %d\n", res, s, _xPort));
        return false;
    }

    int nonblock = 1;
    ioctlsocket(_xSock, FIONBIO, &nonblock);

    LogInfo(("Connect success\n"));
    return true;
}

static void _dnsCB(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    _dnsFound(name, ipaddr, callback_arg);
}

static void _dnsFound(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    memcpy(&_xHost, ipaddr, sizeof(_xHost));

    LogInfo(("DNS Found %s copied to xHost %s\n", ipaddr_ntoa(ipaddr), ipaddr_ntoa(&_xHost)));
    xSemaphoreGiveFromISR(_xHostDNSFound, NULL);
}

/// All public mehtods

void tcp_setup() {
    _xHostDNSFound = xSemaphoreCreateBinary();
}

void tcp_destruct() {
    _xHostDNSFound = NULL;
    _xSock = 0;
}

/***
 * Connect to remote TCP Socket
 * @param host - Host address
 * @param port - Port number
 * @return true on success
 */
bool tcp_transConnect(const char *host, uint16_t port) {
    err_t res = dns_gethostbyname(host, &_xHost, _dnsCB, NULL);

    strcpy(_xHostName, host);
    _xPort = port;

    if (xSemaphoreTake(_xHostDNSFound, TCP_TRANSPORT_WAIT) != pdTRUE) {
        LogError(("DNS Timeout on Connect: %s, %d", host, res));
        // return false;
    }

    return _transConnect();
}

int tcp_status() {
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(_xSock, SOL_SOCKET, SO_ERROR, &error, &len);
    return error;
}

bool tcp_transClose() {
    closesocket(_xSock);
    return true;
}

int32_t tcp_transSend(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend) {
    uint32_t dataOut;
    // tcp_debugPrintBuffer("TCPTransport::transSend", pBuffer, bytesToSend);
    dataOut = write(_xSock, (uint8_t *)pBuffer, bytesToSend);
    if (dataOut != bytesToSend) {
        LogError(("Send failed %d\n", dataOut));
    }
    return dataOut;
}

int32_t tcp_transRead(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv) {
    int32_t dataIn = 0;
    dataIn = read(_xSock, (uint8_t *)pBuffer, bytesToRecv);
    if (dataIn < 0) {
        if (errno == 0) {
            dataIn = 0;
        }
    }
    // printf("Read(%d)=%d\n", bytesToRecv, dataIn);
    // printf("transRead(%d)=%d\n", bytesToRecv, dataIn);
    return dataIn;
}

uint32_t tcp_getCurrentTime() {
    return to_ms_since_boot(get_absolute_time());
}

// this was directly translated from C++, will have to check its significance
static int32_t tcp_staticSend(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend) {
    return tcp_transSend(pNetworkContext, pBuffer, bytesToSend);
}

static int32_t tcp_staticRead(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv) {
    return tcp_transRead(pNetworkContext, pBuffer, bytesToRecv);
}

void tcp_debugPrintBuffer(const char *title, const void *pBuffer, size_t bytes) {
    size_t count = 0;
    size_t lineEnd = 0;
    const uint8_t *pBuf = (uint8_t *)pBuffer;

    printf("DEBUG: %s of size %d\n", title, bytes);

    while (count < bytes) {
        lineEnd = count + DEBUG_LINE;
        if (lineEnd > bytes) {
            lineEnd = bytes;
        }

        // Print HEX DUMP
        for (size_t i = count; i < lineEnd; i++) {
            if (pBuf[i] <= 0x0F) {
                printf("0%X ", pBuf[i]);
            } else {
                printf("%X ", pBuf[i]);
            }
        }

        // Pad for short lines
        size_t pad = (DEBUG_LINE - (lineEnd - count)) * 3;
        for (size_t i = 0; i < pad; i++) {
            printf(" ");
        }

        // Print Plain Text
        for (size_t i = count; i < lineEnd; i++) {
            if ((pBuf[i] >= 0x20) && (pBuf[i] <= 0x7e)) {
                printf("%c", pBuf[i]);
            } else {
                printf(".");
            }
        }

        printf("\n");

        count = lineEnd;
    }
}
