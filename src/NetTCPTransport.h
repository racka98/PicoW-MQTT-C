#pragma once

#define TCP_TRANSPORT_WAIT 10000

#include <FreeRTOS.h>
#include <MQTTConfig.h>
#include <core_mqtt.h>
#include <lwip/dns.h>
#include <lwip/ip4_addr.h>
#include <lwip/ip_addr.h>
#include <lwip/sockets.h>
#include <semphr.h>
#include <task.h>

/**
 * Setup all required variables
 */
void tcp_setup();

/**
 * Clear everything to default state
 */
void tcp_destruct();

/***
 * Connect to remote TCP Socket
 * @param host - Host address
 * @param port - Port number
 * @return true on success
 */
bool tcp_transConnect(const char *host, uint16_t port);

/***
 * Get status of the socket
 * @return int <0 is error
 */
int tcp_status();

/***
 * Close the socket
 * @return true on success
 */
bool tcp_transClose();

/***
 * Send bytes through socket
 * @param pNetworkContext - Network context object from MQTT
 * @param pBuffer - Buffer to send from
 * @param bytesToSend - number of bytes to send
 * @return number of bytes sent
 */
int32_t tcp_transSend(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend);

/***
 * Send
 * @param pNetworkContext
 * @param pBuffer
 * @param bytesToRecv
 * @return
 */
int32_t tcp_transRead(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv);

/***
 * returns current time, as time in ms since boot
 * Required for MQTT Agent library
 * @return
 */
uint32_t tcp_getCurrentTime();

/***
 * Static function to send data through socket from buffer
 * @param pNetworkContext - Used to locate the TCPTransport object to use
 * @param pBuffer - Buffer of data to send
 * @param bytesToSend - number of bytes to send
 * @return number of bytes sent
 */
static int32_t tcp_staticSend(NetworkContext_t *pNetworkContext, const void *pBuffer, size_t bytesToSend);

/***
 * Read data from network socket. Non blocking returns 0 if no data
 * @param pNetworkContext - Used to locate the TCPTransport object to use
 * @param pBuffer - Buffer to read into
 * @param bytesToRecv - Maximum number of bytes to read
 * @return number of bytes read. May be 0 as non blocking
 * Negative number indicates error
 */
static int32_t tcp_staticRead(NetworkContext_t *pNetworkContext, void *pBuffer, size_t bytesToRecv);

/***
 * Print the buffer in hex and plain text for debugging
 */
void tcp_debugPrintBuffer(const char *title, const void *pBuffer, size_t bytes);
