#pragma once

#define TCP_TRANSPORT_WAIT 10000

#include "MQTTConfig.h"
#include "core_mqtt.h"
// #include "core_mqtt_agent.h"

extern "C" {
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include "lwip/dns.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip_addr.h"
#include "lwip/sockets.h"
}

class TCPTransport {
   public:
    /***
     * Constructor
     */
    TCPTransport();

    /***
     * Destructor
     */
    virtual ~TCPTransport();

    /***
     * Connect to remote TCP Socket
     * @param host - Host address
     * @param port - Port number
     * @return true on success
     */
    bool transConnect(const char *host, uint16_t port);

    /***
     * Get status of the socket
     * @return int <0 is error
     */
    int status();

    /***
     * Close the socket
     * @return true on success
     */
    bool transClose();

    /***
     * Send bytes through socket
     * @param pNetworkContext - Network context object from MQTT
     * @param pBuffer - Buffer to send from
     * @param bytesToSend - number of bytes to send
     * @return number of bytes sent
     */
    int32_t transSend(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend);

    /***
     * Send
     * @param pNetworkContext
     * @param pBuffer
     * @param bytesToRecv
     * @return
     */
    int32_t transRead(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv);

    /***
     * returns current time, as time in ms since boot
     * Required for MQTT Agent library
     * @return
     */
    static uint32_t getCurrentTime();

    /***
     * Static function to send data through socket from buffer
     * @param pNetworkContext - Used to locate the TCPTransport object to use
     * @param pBuffer - Buffer of data to send
     * @param bytesToSend - number of bytes to send
     * @return number of bytes sent
     */
    static int32_t staticSend(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend);

    /***
     * Read data from network socket. Non blocking returns 0 if no data
     * @param pNetworkContext - Used to locate the TCPTransport object to use
     * @param pBuffer - Buffer to read into
     * @param bytesToRecv - Maximum number of bytes to read
     * @return number of bytes read. May be 0 as non blocking
     * Negative number indicates error
     */
    static int32_t staticRead(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv);

    /***
     * Print the buffer in hex and plain text for debugging
     */
    void debugPrintBuffer(const char *title, const void *pBuffer, size_t bytes);

   private:
    /***
     * Connect to socket previously stored ip address and port number
     * @return true if socket openned
     */
    bool transConnect();

    /***
     * Call back function for the DNS lookup
     * @param name - server name
     * @param ipaddr - resulting IP address
     * @param callback_arg - poiter to TCPTransport object
     */
    static void dnsCB(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

    /***
     * Called when DNS is returned
     * @param name - server name
     * @param ipaddr - ip address of server
     * @param callback_arg - this TCPtransport object
     */
    void dnsFound(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

    // Socket number
    int xSock = 0;

    // Port to connect to
    uint16_t xPort = 80;

    // Remote server IP to connect to
    ip_addr_t xHost;

    // Remote server name to connect to
    char xHostName[80];

    // Semaphore used to wait on DNS responses
    SemaphoreHandle_t xHostDNSFound;
};
