#pragma once

#include <pico/stdlib.h>
#include <stdlib.h>

#ifndef WIFI_RETRIES
#define WIFI_RETRIES 3
#endif

/***
 * Initialise the controller
 * @return true if successful
 */
bool wifi_init();

/***
 * Get IP address of unit
 * @param ip - output uint8_t[4]
 * @return - true if IP addres assigned
 */
bool wifi_getIPAddress(uint8_t *ip);

/***
 * Get IP address of unit
 * @param ips - output char * up to 16 chars
 * @return - true if IP addres assigned
 */
bool wifi_getIPAddressStr(char *ips);

/***
 * Get Gateway address
 * @param ip - output uint8_t[4]
 */
bool wifi_getGWAddress(uint8_t *ip);

/***
 * Get Gateway address
 * @param ips - output char * up to 16 chars
 * @return - true if IP addres assigned
 */
bool wifi_getGWAddressStr(char *ips);

/***
 * Get Net Mask address
 * @param ip - output uint8_t[4]
 */
bool wifi_getNetMask(uint8_t *ip);

/***
 * Get Net Mask
 * @param ips - output char * up to 16 chars
 * @return - true if IP addres assigned
 */
bool wifi_getNetMaskStr(char *ips);

/***
 * Get the mac address as a string
 * @param macStr: pointer to string of at least 14 characters
 * @return true if successful
 */
bool wifi_getMACAddressStr(char *macStr);

/***
 *  Join a Wifi Network
 * @param sid - string of the SID
 * @param password - Password for network
 * @param retries - Number of times to retry, defalt defined in WIFI_RETRIES.
 * @return true if successful
 */
bool wifi_join(const char *sid, const char *password, uint8_t retries);

/***
 * Returns if joined to the network and we have a link
 * @return true if joined.
 */
bool wifi_isJoined();
