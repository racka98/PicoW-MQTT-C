
#include "WiFiHelper.h"

#include <FreeRTOS.h>
#include <pico/cyw43_arch.h>
#include <pico/util/datetime.h>
#include <task.h>

bool wifi_init() {
    int res = cyw43_arch_init();
    if (res) return false;
    // cyw43_wifi_pm(&cyw43_state, CYW43_PERFORMANCE_PM);
    return true;
}

bool wifi_join(const char *sid, const char *password, uint8_t retries) {
    cyw43_arch_enable_sta_mode();
    printf("Connecting to WiFi... %s \n", sid);

    // Loop trying to connect to wifi
    int r = -1;
    uint8_t attempts = 0;
    while (r < 0) {
        attempts++;
        r = cyw43_arch_wifi_connect_timeout_ms(sid, password, CYW43_AUTH_WPA2_AES_PSK, 60000);
        if (r) {
            printf("Failed to join AP\n");
            if (attempts >= retries) return false;
            vTaskDelay(2000);
        }
    }
    return true;
}

bool wifi_isJoined() {
    int res = cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
    return res >= 0;
}

bool wifi_getIPAddress(uint8_t *ip) {
    memcpy(ip, netif_ip4_addr(&cyw43_state.netif[0]), 4);
    return true;
}

bool wifi_getIPAddressStr(char *ips) {
    char *s = ipaddr_ntoa(netif_ip4_addr(&cyw43_state.netif[0]));
    strcpy(ips, s);
    return true;
}

bool wifi_getGWAddress(uint8_t *ip) {
    memcpy(ip, netif_ip4_gw(&cyw43_state.netif[0]), 4);
    return true;
}

bool wifi_getGWAddressStr(char *ips) {
    char *s = ipaddr_ntoa(netif_ip4_gw(&cyw43_state.netif[0]));
    strcpy(ips, s);
    return true;
}

bool wifi_getNetMask(uint8_t *ip) {
    memcpy(ip, netif_ip4_netmask(&cyw43_state.netif[0]), 4);
    return true;
}

bool wifi_getNetMaskStr(char *ips) {
    char *s = ipaddr_ntoa(netif_ip4_netmask(&cyw43_state.netif[0]));
    strcpy(ips, s);
    return true;
}

bool wifi_getMACAddressStr(char *macStr) {
    uint8_t mac[6];
    int r = cyw43_wifi_get_mac(&cyw43_state, CYW43_ITF_STA, mac);

    if (r == 0) {
        for (uint8_t i = 0; i < 6; i++) {
            if (mac[i] < 16) {
                sprintf(&macStr[i * 2], "0%X", mac[i]);
            } else {
                sprintf(&macStr[i * 2], "%X", mac[i]);
            }
        }
        macStr[13] = 0;
        return true;
    }
    return false;
}
